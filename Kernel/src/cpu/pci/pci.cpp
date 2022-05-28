#include <PageTableManager.h>
#include <Utils.h>
#include <hkStdio.h>
#include <pci.h>
#include <heap.h>

char *ClassTypes[] = {"Unclassified", "Mass Storage Device", "Network Controller", "Display Controller", "Multimedia Controller", "Memory Controller", "Bridge", "Simple Communication Controller", "Base System Peripheral", "Input Device Controller", "Docking Station", "Processor", "Serial Bus Controller", "Wireless Controller", "Intelligent Controller", "Encryption Controller", "signal Processing Controller", "Processing Accelerator", "Non-Essential Instrumentation", "Reserved"};
char *UnclassifiedSubClasses[] = {"Non-VGA-Compatible Unclassified Device", "VGA-Compatible Unclassified Device"};
char *MassStorageControllerSubClasses[] = {"SCSI Bus Controller", "IDE Controller", "Floppy Disk Controller", "IPI Bus Controller", "RAID Controller", "ATA Controller", "Serial ATA Controller", "Serial Attached SCSI Controller", "Non-Volatile Memory Controller"};

Vector<PCIDeviceHeader *> PCIDevices;

char *getSubClass(uint8_t classCode, uint8_t subClassCode) {
    switch (classCode) {
    case 0x0:
        return UnclassifiedSubClasses[subClassCode];
    case 0x1:
        return MassStorageControllerSubClasses[subClassCode];
    }
    return (char *)subClassCode;
}

void EnumerateFunction(uint64_t deviceAddress, uint64_t function) {
    uint64_t offset = function << 12;
    uint64_t functionAddress = deviceAddress + offset;
    GlobalPageTableManager.MapMemory((void *)functionAddress, (void *)functionAddress);

    PCIDeviceHeader *pciDeviceHeader = (PCIDeviceHeader *)functionAddress;
    if (pciDeviceHeader->DeviceID == 0)
        return;
    if (pciDeviceHeader->DeviceID == 0xFFFF)
        return;
    PCIDevices.Add(pciDeviceHeader);
}

void EnumerateDevice(uint64_t busAddress, uint64_t device) {
    uint64_t offset = device << 15;
    uint64_t deviceAddress = busAddress + offset;
    GlobalPageTableManager.MapMemory((void *)deviceAddress, (void *)deviceAddress);

    PCIDeviceHeader *pciDeviceHeader = (PCIDeviceHeader *)deviceAddress;
    if (pciDeviceHeader->DeviceID == 0)
        return;
    if (pciDeviceHeader->DeviceID == 0xFFFF)
        return;
    for (uint64_t func = 0; func < 8; func++) {
        EnumerateFunction(deviceAddress, func);
    }
}
void EnumerateBus(uint64_t baseAddress, uint64_t bus) {
    uint64_t offset = bus << 20;
    uint64_t busAddress = baseAddress + offset;
    GlobalPageTableManager.MapMemory((void *)busAddress, (void *)busAddress);

    PCIDeviceHeader *pciDeviceHeader = (PCIDeviceHeader *)busAddress;

    if (pciDeviceHeader->DeviceID == 0)
        return;
    if (pciDeviceHeader->DeviceID == 0xFFFF)
        return;
    for (uint64_t dev = 0; dev < 32; dev++) {

        EnumerateDevice(busAddress, dev);
    }
}


void EnumeratePCI(MCFGHeader *mcfg) {
    int entries = ((mcfg->Header.Length) - sizeof(MCFGHeader)) / sizeof(DeviceConfig);
    for (int i = 0; i < entries; i++) {
        DeviceConfig *NewDeviceConfig = (DeviceConfig *)((uint64_t)mcfg + sizeof(MCFGHeader) + (sizeof(DeviceConfig) * i));
        HuskyStandardOutput.kprint("Start Bus: %lld, end bus: %lld\n", NewDeviceConfig->StartBus, NewDeviceConfig->EndBus);
        for (uint64_t bus = NewDeviceConfig->StartBus; bus < NewDeviceConfig->EndBus; bus++) {

            EnumerateBus(NewDeviceConfig->BaseAddress, bus);
        }
        
    }
}

