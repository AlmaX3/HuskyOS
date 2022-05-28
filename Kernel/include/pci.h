#pragma once
#include <acpi.h>
#include <stdint.h>
#include <vector.h>

struct PCIDeviceHeader {
    uint16_t DeviceID;
    uint16_t VendorID;
    uint16_t Status;
    uint16_t Command;
    uint8_t RevisionID;
    uint8_t ProgramIF;
    uint8_t SubClass;
    uint8_t Class;
    uint8_t CacheLineSize;
    uint8_t LatencyTimer;
    uint8_t HeaderType;
    uint8_t Bist;
};


struct PCIHeader0 {
    PCIDeviceHeader Header;
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t CardBusCISPtr;
    uint16_t SubsystemVendorID;
    uint16_t SubsystemID;
    uint32_t ExtensionROMBaseAddress;
    uint8_t CapabilitiesPtr;
    uint8_t rsv0;
    uint16_t rsv1;
    uint32_t rsv2;
    uint8_t InterruptLine;
    uint8_t InterruptPin;
    uint8_t MinGrant;
    uint8_t MaxLatency;
};

void EnumeratePCI(MCFGHeader* mcfg);
extern Vector<PCIDeviceHeader*> PCIDevices;
extern char* ClassTypes[];

char* getSubClass(uint8_t classCode, uint8_t subClassCode);