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


void EnumeratePCI(MCFGHeader* mcfg);
extern Vector<PCIDeviceHeader*> PCIDevices;
extern char* ClassTypes[];

char* getSubClass(uint8_t classCode, uint8_t subClassCode);