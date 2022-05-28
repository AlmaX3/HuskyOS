#include <ahci.h>
#include "hkStdio.h"


AHCIDriver::AHCIDriver(PCIDeviceHeader* pciDeviceHeader){
    this->PCIBaseAddress = pciDeviceHeader;
    HuskyStandardOutput.kprint("AHCI DRIVER INSTANCE INITIALIZED.\n");
}



AHCIDriver::~AHCIDriver() {

}

