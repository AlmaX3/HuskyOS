#include <PageTableManager.h>
#include <ahci.h>
#include <heap.h>
#include <hkStdio.h>
#include <pageFrameAllocator.h>

#define HBA_PORT_DEV_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE 0x1

#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_ATA 0x00000101
#define SATA_SIG_SEMB 0xC33C0101
#define SATA_SIG_PM 0x96690101

#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x4000

PortType CheckPortType(HBAPort *port) {
    uint32_t sataStatus = port->sataStatus;
    uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
    uint8_t deviceDetection = sataStatus & 0b111;

    if (deviceDetection != HBA_PORT_DEV_PRESENT)
        return PortType::None;
    if (interfacePowerManagement != HBA_PORT_IPM_ACTIVE)
        return PortType::None;
    switch (port->signature) {
    case SATA_SIG_ATAPI:
        return PortType::SATAPI;
    case SATA_SIG_ATA:
        return PortType::SATA;
    case SATA_SIG_PM:
        return PortType::SEMB;
    case SATA_SIG_SEMB:
        return PortType::SEMB;
    default:
        return PortType::None;
    }
}

void AHCIDriver::ProbePorts() {
    uint32_t portsImplemented = ABAR->portsImplemented;
    for (size_t i = 0; i < 32; i++) {
        if (portsImplemented & (1 << i)) {
            PortType portType = CheckPortType(&ABAR->ports[i]);
            if (portType == PortType::SATA || portType == PortType::SATAPI) {
                ports[portCount] = new Port();
                ports[portCount]->portType = portType;
                ports[portCount]->hbaPortPtr = &ABAR->ports[i];
                ports[portCount]->portNumber = portCount;
                portCount++;
            }
        }
    }
}

void Port::Configure() {
    stopCmd();

    void *newBase = GlobalAllocator.RequestPage();
    hbaPortPtr->commandListBase = (uint32_t)(uint64_t)newBase;
    hbaPortPtr->commandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
    memset((void *)hbaPortPtr->commandListBase, 0, 1024);

    void *FISBase = GlobalAllocator.RequestPage();
    hbaPortPtr->fisBaseAddress = (uint32_t)(uint64_t)FISBase;
    hbaPortPtr->fisBaseAddressUpper = (uint32_t)((uint64_t)FISBase >> 32);
    memset(FISBase, 0, 256);
    HBACommandHeader *commandHeader = (HBACommandHeader *)((uint64_t)hbaPortPtr->commandListBase + ((uint64_t)hbaPortPtr->commandListBaseUpper << 32));

    for (int i = 0; i < 32; i++) {
        commandHeader[i].prdtLength = 8;

        void *cmdTableAddress = GlobalAllocator.RequestPage();
        uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
        commandHeader[i].commandTableAddress = (uint32_t)address;
        commandHeader[i].commandTableAddressUpper = (uint32_t)((uint64_t)address >> 32);
        memset(cmdTableAddress, 0, 256);
    }

    startCmd();
}

void Port::stopCmd() {
    hbaPortPtr->cmdSts &= ~HBA_PxCMD_ST;
    hbaPortPtr->cmdSts &= ~HBA_PxCMD_FRE;
    while (true) {
        if (hbaPortPtr->cmdSts & HBA_PxCMD_FR)
            continue;
        if (hbaPortPtr->cmdSts & HBA_PxCMD_CR)
            continue;

        break;
    }
}

void Port::startCmd() {
    while (hbaPortPtr->cmdSts & HBA_PxCMD_CR)
        ;

    hbaPortPtr->cmdSts |= HBA_PxCMD_FRE;
    hbaPortPtr->cmdSts |= HBA_PxCMD_ST;
}

bool Port::Read(uint64_t sector, uint32_t sectorCount, void *buffer) {
    uint64_t spin = 0;
    while ((hbaPortPtr->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
        spin++;
    }
    if (spin == 1000000) {
        return false;
    }

    uint32_t SectorLow = (uint32_t)sector;
    uint32_t SectorHigh = (uint32_t)(sector >> 32);
    hbaPortPtr->interruptStatus = (uint32_t)-1;
    HBACommandHeader *cmdHeader = (HBACommandHeader *)hbaPortPtr->commandListBase;
    cmdHeader->commandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
    cmdHeader->write = 0;
    cmdHeader->prdtLength = 1;

    HBACommandTable *CmdTable = (HBACommandTable *)(cmdHeader->commandTableAddress);
    memset(CmdTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HBAPRDTENTRY));

    CmdTable->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)buffer;
    CmdTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
    CmdTable->prdtEntry[0].byteCount = (sectorCount << 9) - 1;
    CmdTable->prdtEntry[0].interruptOnCompletion = 1;

    FIS_REG_H2D *cmdFIS = (FIS_REG_H2D *)(&CmdTable->commandFIS);
    cmdFIS->fisType = FIS_TYPE_REG_H2D;
    cmdFIS->commandControl = 1;
    cmdFIS->command = ATA_CMD_READ_DMA_EX;

    cmdFIS->lba0 = (uint8_t)SectorLow;
    cmdFIS->lba1 = (uint8_t)(SectorLow >> 8);
    cmdFIS->lba2 = (uint8_t)(SectorLow >> 16);
    cmdFIS->lba3 = (uint8_t)SectorHigh;
    cmdFIS->lba4 = (uint8_t)(SectorHigh >> 8);
    cmdFIS->lba5 = (uint8_t)(SectorHigh >> 16);

    cmdFIS->deviceRegister = 1 << 6;
    cmdFIS->countLow = sectorCount & 0xFF;
    cmdFIS->countHigh = (sectorCount >> 8) & 0xFF;

    hbaPortPtr->commandIssue = 1;

    while (true) {
        if ((hbaPortPtr->commandIssue == 0))
            break;
        if (hbaPortPtr->interruptStatus & HBA_PxIS_TFES) {
            return false;
        }
    }
    return true;
}

AHCIDriver::AHCIDriver(PCIDeviceHeader *pciDeviceHeader) {
    this->PCIBaseAddress = pciDeviceHeader;

    ABAR = (HBAMemory *)((PCIHeader0 *)pciDeviceHeader)->BAR5;
    GlobalPageTableManager.MapMemory(ABAR, ABAR);

    ProbePorts();

    for (size_t i = 0; i < portCount; i++) {
        Port *port = ports[i];
        port->Configure();
        port->buffer = (uint8_t *)GlobalAllocator.RequestPage();
        memset(port->buffer, 0, 0x1000);

        port->Read(0, 4, port->buffer);
        for (size_t j = 0; j < 1024; j++) {
            HuskyStandardOutput.kprint("%c", port->buffer[j]);
        }
        HuskyStandardOutput.kprint("\n");
    
}

HuskyStandardOutput.kprint("AHCI DRIVER INSTANCE INITIALIZED.\n");
}

AHCIDriver::~AHCIDriver() {
}
