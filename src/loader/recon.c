#include <efi.h>
#include <efilib.h>
#include "recon.h"
#include "util.h"

VOID
PrintPml4Entries ()
{
    EFI_PHYSICAL_ADDRESS Pml4TableAddress;
    UINT64 *Pml4TableEntry;
    UINT64 PdpTableAddress;
    int i;

    Pml4TableAddress = GetPml4TableAddress();
    Print(L"\nPML4 Table Address: %x\n", Pml4TableAddress);
    Pml4TableEntry = (UINT64 *)Pml4TableAddress;
    // 512 PML4 table entries (4096 bytes).
    for (i = 0; i < 512; i++) {
        if (*Pml4TableEntry & 1) {
            PdpTableAddress = *Pml4TableEntry & 0x0000fffffffff000;
            Print(L"\nPML4 Table Entry %d:\n", i);
            Print(L"PDP Table Address: %x\n", PdpTableAddress);
            if (*Pml4TableEntry & 0x20) {
                Print(L"Accessed  ");
            }
            if (*Pml4TableEntry & 0x10) {
                Print(L"Cache Disabled  ");
            }
            if (*Pml4TableEntry & 0x8) {
                Print(L"Write-Through  ");
            }
            if (*Pml4TableEntry & 0x4) {
                Print(L"User & Supervisor  ");
            } else {
                Print(L"Supervisor Only  ");
            }
            if (*Pml4TableEntry & 0x2) {
                Print(L"Read & Write  ");
            } else {
                Print(L"Readonly  ");
            }
            Print(L"\n");
        }
        Pml4TableEntry++;
    }
}

VOID
PrintControlRegisters ()
{
    UINT64 CR0;
    UINT64 CR3;
    UINT64 CR4;
    UINT32 IA32_EFER;

    asm(
        "mov %%cr0, %%rax;"
        "mov %%rax, %0;"
        "mov %%cr3, %%rax;"
        "mov %%rax, %1;"
        "mov %%cr4, %%rax;"
        "mov %%rax, %2;"
        "mov $0xC0000080, %%ecx;"
        "rdmsr;"
        "mov %%eax, %3;"

        :"=r"(CR0),
         "=r"(CR3),
         "=r"(CR4),
         "=r"(IA32_EFER)
        :
        :"%rax", "%rcx", "%rdx"
    );

    Print(L"\nCR0:        %x\n", CR0);
    Print(L"CR3:        %x\n", CR3);
    Print(L"CR4:        %x\n", CR4);
    Print(L"IA32_EFER:  %x\n", IA32_EFER);
}

VOID
PrintEnvironmentVariables (
    IN BOOLEAN  PauseAfterEntry
    )
{
    EFI_STATUS  Status;
    UINTN       VariableNameSize;
    CHAR16      VariableName[256];
    EFI_GUID    VendorGuid;
    UINT8       *VariableValue;

    Print(L"\nEnvironment Variables (press any key to cycle through):\n\n");
    VariableName[0] = 0x0000;
    VendorGuid = NullGuid;
    Print(L"GUID                                Variable Name        Value Address   \n");
    Print(L"=================================== ==================== ================\n");
    do {
        VariableNameSize = sizeof(VariableName);
        Status = RT->GetNextVariableName(&VariableNameSize, VariableName, &VendorGuid);
        if (Status == EFI_SUCCESS) {
            VariableValue = LibGetVariable(VariableName, &VendorGuid);
            if (VariableValue != NULL) {
                Print(L"%.-35g %.-20s %lX\n", &VendorGuid, VariableName, VariableValue);
                FreePool(VariableValue);
                if (PauseAfterEntry) {
                    WaitForKeyStroke(NULL);
                }
            }
        }
    } while (Status == EFI_SUCCESS);
    WaitForKeyStroke(L"\nPress any key to continue...\n");
}

VOID
PrintMemoryMap (
    IN BOOLEAN PauseAfterEntry
    )
{
    const CHAR16 *MemoryTypes[EfiMaxMemoryType] = {
        L"Reserved               ",
        L"Loader Code            ",
        L"Loader Data            ",
        L"Boot Services Code     ",
        L"Boot Services Data     ",
        L"Runtime Services Code  ",
        L"Runtime Services Data  ",
        L"Conventional           ",
        L"Unusable               ",
        L"ACPI Reclaim           ",
        L"ACPI NVS               ",
        L"Memory-Mapped I/O      ",
        L"Memory-Mapped I/O Ports",
        L"Pal Code               "
    };

    EFI_STATUS              Status;
    EFI_MEMORY_DESCRIPTOR   *MemoryMapEntry;
    UINTN                   NoEntries;
    UINTN                   MapKey;
    UINTN                   DescriptorSize;
    UINT32                  DescriptorVersion;
    UINTN                   i;

    MemoryMapEntry = LibMemoryMap(&NoEntries, &MapKey, &DescriptorSize, &DescriptorVersion);
    Print(L"\nMemory Descriptors (press any key to cycle through):\n\n");
    Print(L"Type                     Start Address     End Address       Attributes      \n");
    Print(L"=======================  ================  ================  ================\n");
    for (i = 0; i < NoEntries; i++) {
        Print(L"%s  %lX  %lX  %lX\n",
            MemoryTypes[MemoryMapEntry->Type],
            MemoryMapEntry->PhysicalStart,
            MemoryMapEntry->PhysicalStart +
                LShiftU64(MemoryMapEntry->NumberOfPages, EFI_PAGE_SHIFT) - 1,
            MemoryMapEntry->Attribute);
        MemoryMapEntry = NextMemoryDescriptor(MemoryMapEntry, DescriptorSize);
        if (PauseAfterEntry) {
            WaitForKeyStroke(NULL);
        }
    }
    WaitForKeyStroke(L"\nPress any key to continue...\n");
}
