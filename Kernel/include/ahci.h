#pragma once
#include <stdint.h>
#include <pci.h>

class AHCIDriver {
    public:
    AHCIDriver(PCIDeviceHeader* pciDeviceHeader);
    ~AHCIDriver();
    PCIDeviceHeader* PCIBaseAddress;
};