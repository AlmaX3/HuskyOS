#include <acpi.h>
#include <hkStdio.h>
#include <pci.h>
#include <serial.h>
bool isRSDT = true;

void *FindTable(char *Table, void *RootSDT) {
    if (isRSDT) {
        RSDT *rsdt = (RSDT *)RootSDT;
        int entries = (rsdt->header.Length - sizeof(rsdt->header)) / 4;
        for (int i = 0; i < entries; i++) {
            SDTHeader *h = (SDTHeader *)rsdt->PointerSDT[i];
            if (!strncmp((char *)h->Signature, Table, 4)) {
                return (void *)h;
            }
        }

        return nullptr;
    } else {
        XSDT *xsdt = (XSDT *)RootSDT;
        int entries = (xsdt->header.Length - sizeof(xsdt->header)) / 4;
        for (int i = 0; i < entries; i++) {
            SDTHeader *h = (SDTHeader *)xsdt->PointerSDT[i];
            if (!strncmp((char *)h->Signature, Table, 4)) {
                return (void *)h;
            }
        }

        return nullptr;
    }
}

void PrepareACPI(stivale2_struct_tag_rsdp *rsdp) {
    RSDP2 *NewRSDP = (RSDP2 *)rsdp->rsdp;
    if (NewRSDP->XSDTAddress != 0x0) {
        HuskyStandardOutput.kprint("XSDT: %p\n", NewRSDP->XSDTAddress);
        debug("FOUND XSDP.\n");
        isRSDT = false;
        XSDT *xsdt = (XSDT *)NewRSDP->XSDTAddress;
        MCFGHeader *mcfg = (MCFGHeader *)FindTable("MCFG", (void *)xsdt);

        HuskyStandardOutput.kprint("%s\n", mcfg->Header.Signature);

        EnumeratePCI(mcfg);

        APIC *Apic = (APIC *)FindTable("APIC", (void *)xsdt);
        uint8_t lapic_ids[256] = {0}; // CPU core Local APIC IDs
        uint8_t numcore = 0;          // number of cores detected
        uint64_t lapic_ptr = 0;       // pointer to the Local APIC MMIO registers
        uint64_t ioapic_ptr = 0;      // pointer to the IO APIC MMIO registers

        uint8_t *ptr = (uint8_t *)Apic;
        lapic_ptr = (uint64_t)(((uint32_t *)(ptr + 0x24)));

        uint8_t *ptr2 = ptr + *((uint32_t *)(ptr + 4));
        for (ptr += 44; ptr < ptr2; ptr += ptr[1]) {
            switch (ptr[0]) {
            case 0:
                if (ptr[4] & 1)
                    lapic_ids[numcore++] = ptr[3];
                break; // found Processor Local APIC
            case 1:
                ioapic_ptr = (uint64_t) * ((uint32_t *)(ptr + 4));
                break; // found IOAPIC
            case 5:
                lapic_ptr = *((uint64_t *)(ptr + 4));
                break; // found 64 bit LAPIC
            }
        }

        HuskyStandardOutput.kprint("cores: %d IOAPIC %p LAPIC %p Processors IDs:\n", numcore, ioapic_ptr, lapic_ptr);
        for (int i = 0; i < numcore; i++)
            HuskyStandardOutput.kprint(" %d", lapic_ids[i]);
        HuskyStandardOutput.kprint("\n");

    } else {
        debug("XSDP NOT FOUND. USING RSDT INSTEAD\n");
        RSDT *rsdt = (RSDT *)NewRSDP->RSDTAddress;
        MCFGHeader *mcfg = (MCFGHeader *)FindTable("MCFG", (void *)rsdt);

        HuskyStandardOutput.kprint("%s\n", mcfg->Header.Signature);

        EnumeratePCI(mcfg);


        APIC *Apic = (APIC *)FindTable("APIC", (void *)rsdt);
        uint8_t lapic_ids[256] = {0}; // CPU core Local APIC IDs
        uint8_t numcore = 0;          // number of cores detected
        uint64_t lapic_ptr = 0;       // pointer to the Local APIC MMIO registers
        uint64_t ioapic_ptr = 0;      // pointer to the IO APIC MMIO registers

        uint8_t *ptr = (uint8_t *)Apic;
        lapic_ptr = (uint64_t)(((uint32_t *)(ptr + 0x24)));

        uint8_t *ptr2 = ptr + *((uint32_t *)(ptr + 4));
        for (ptr += 44; ptr < ptr2; ptr += ptr[1]) {
            switch (ptr[0]) {
            case 0:
                if (ptr[4] & 1)
                    lapic_ids[numcore++] = ptr[3];
                break; // found Processor Local APIC
            case 1:
                ioapic_ptr = (uint64_t) * ((uint32_t *)(ptr + 4));
                break; // found IOAPIC
            case 5:
                lapic_ptr = *((uint64_t *)(ptr + 4));
                break; // found 64 bit LAPIC
            }
        }

        HuskyStandardOutput.kprint("cores: %d IOAPIC %p LAPIC %p Processors IDs:\n", numcore, ioapic_ptr, lapic_ptr);
        for (int i = 0; i < numcore; i++)
            HuskyStandardOutput.kprint(" %d", lapic_ids[i]);
        HuskyStandardOutput.kprint("\n");
    }
}