#include <efi.h>
#include <efilib.h>
#include "recon.h"
#include "util.h"

VOID
PrintPml4Table (
    IN BOOLEAN SkipLargePages,
    IN BOOLEAN PauseAfterEntry
    )
{
    EFI_PHYSICAL_ADDRESS    Pml4TableAddress;
    UINT64                  *Pml4TableEntry;
    EFI_PHYSICAL_ADDRESS    PdpTableAddress;
    int                     i;

    Pml4TableAddress = GetPml4TableAddress();
    Print(L"\nPML4 Table Address: %x", Pml4TableAddress);
    Pml4TableEntry = (UINT64 *)Pml4TableAddress;
    // 512 PML4 table entries (4096 bytes).
    for (i = 0; i < 512; i++) {
        if (*Pml4TableEntry & 1) {
            PdpTableAddress = *Pml4TableEntry & 0x0000fffffffff000;
            Print(L"\nPML4E %d : { PDPT=%x } ", i, PdpTableAddress);
            if (*Pml4TableEntry & 0x8000000000000000) {
                Print (L"[ Execute-Disable ] ");
            }
            if (*Pml4TableEntry & 0x20) {
                Print(L"[ Accessed ] ");
            }
            if (*Pml4TableEntry & 0x10) {
                Print(L"[ Cache-Disable ] ");
            }
            if (*Pml4TableEntry & 0x8) {
                Print(L"[ Write-Through ] ");
            }
            if (*Pml4TableEntry & 0x4) {
                Print(L"[ User/Supervisor ] ");
            } else {
                Print(L"[ Supervisor-Only ] ");
            }
            if (*Pml4TableEntry & 0x2) {
                Print(L"[ Read/Write ] ");
            } else {
                Print(L"[ Read-Only ] ");
            }
            if (PauseAfterEntry) {
                WaitForKeyStroke(NULL);
            }
            PrintPdpTable(PdpTableAddress, SkipLargePages, PauseAfterEntry);
        }
        Pml4TableEntry++;
    }
}

VOID
PrintPdpTable (
    IN EFI_PHYSICAL_ADDRESS     PdpTableAddress,
    IN BOOLEAN                  SkipLargePages,
    IN BOOLEAN                  PauseAfterEntry
    )
{
    UINT64                  *PdpTableEntry;
    EFI_PHYSICAL_ADDRESS    PageDirectoryOrLargePageAddress;
    int                     i;
    BOOLEAN                 IsLargePage;

    PdpTableEntry = (UINT64 *)PdpTableAddress;
    // 512 PDP table entries (4096 bytes)
    for (i = 0; i < 512; i++) {
        IsLargePage = *PdpTableEntry & 0x80;
        if ((*PdpTableEntry & 1) && !(SkipLargePages && IsLargePage)) {
            PageDirectoryOrLargePageAddress = *PdpTableEntry & 0x0000fffffffff000;
            Print(L"\n    PDPTE %d : ", i);
            // Maps a 1 GB Page
            if (IsLargePage) {
                Print(L"{ 1 GB Page=%x } ", PageDirectoryOrLargePageAddress);
            // References page directory
            } else {
                Print(L"{ PD=%x } ", PageDirectoryOrLargePageAddress);
            }
            if (*PdpTableEntry & 0x8000000000000000) {
                Print (L"[ Execute-Disable ] ");
            }
            if (*PdpTableEntry & 0x40 && IsLargePage) {
                Print(L"[ Dirty ] ");
            }
            if (*PdpTableEntry & 0x20) {
                Print(L"[ Accessed ] ");
            }
            if (*PdpTableEntry & 0x10) {
                Print(L"[ Cache-Disable ] ");
            }
            if (*PdpTableEntry & 0x8) {
                Print(L"[ Write-Through ] ");
            }
            if (*PdpTableEntry & 0x4) {
                Print(L"[ User/Supervisor ] ");
            } else {
                Print(L"[ Supervisor-Only ] ");
            }
            if (*PdpTableEntry & 0x2) {
                Print(L"[ Read/Write ] ");
            } else {
                Print(L"[ Read-Only ] ");
            }
            if (PauseAfterEntry) {
                WaitForKeyStroke(NULL);
            }
            if (!IsLargePage) {
                PrintPageDirectory(PageDirectoryOrLargePageAddress, SkipLargePages, PauseAfterEntry);
            }
        }
        PdpTableEntry++;
    }
}

VOID
PrintPageDirectory (
    IN EFI_PHYSICAL_ADDRESS     PageDirectoryAddress,
    IN BOOLEAN                  SkipLargePages,
    IN BOOLEAN                  PauseAfterEntry
    )
{
    UINT64                  *PageDirectoryEntry;
    EFI_PHYSICAL_ADDRESS    PageTableOrLargePageAddress;
    int                     i;
    BOOLEAN                 IsLargePage;

    PageDirectoryEntry = (UINT64 *)PageDirectoryAddress;
    // 512 Page directory entries (4096 bytes)
    for (i = 0; i < 512; i++) {
        IsLargePage = *PageDirectoryEntry & 0x80;
        if ((*PageDirectoryEntry & 1) && !(SkipLargePages && IsLargePage)) {
            PageTableOrLargePageAddress = *PageDirectoryEntry & 0x0000fffffffff000;
            Print(L"\n        PDE %d : ", i);
            // Maps a 2 MB Page
            if (IsLargePage) {
                Print(L"{ 2 MB Page=%x } ", PageTableOrLargePageAddress);
            // References page directory
            } else {
                Print(L"{ PT=%x } ", PageTableOrLargePageAddress);
            }

            if (*PageDirectoryEntry & 0x8000000000000000) {
                Print (L"[ Execute-Disable ] ");
            }
            if (*PageDirectoryEntry & 0x40 && IsLargePage) {
                Print(L"[ Dirty ] ");
            }
            if (*PageDirectoryEntry & 0x20) {
                Print(L"[ Accessed ] ");
            }
            if (*PageDirectoryEntry & 0x10) {
                Print(L"[ Cache-Disable ] ");
            }
            if (*PageDirectoryEntry & 0x8) {
                Print(L"[ Write-Through ] ");
            }
            if (*PageDirectoryEntry & 0x4) {
                Print(L"[ User/Supervisor ] ");
            } else {
                Print(L"[ Supervisor-Only ] ");
            }
            if (*PageDirectoryEntry & 0x2) {
                Print(L"[ Read/Write ] ");
            } else {
                Print(L"[ Read-Only ] ");
            }
            if (PauseAfterEntry) {
                WaitForKeyStroke(NULL);
            }
            if (!IsLargePage) {
                PrintPageTable(PageTableOrLargePageAddress, PauseAfterEntry);
            }
        }
        PageDirectoryEntry++;
    }
}

VOID
PrintPageTable (
    IN EFI_PHYSICAL_ADDRESS     PageTableAddress,
    IN BOOLEAN                  PauseAfterEntry
    )
{
    UINT64                  *PageTableEntry;
    EFI_PHYSICAL_ADDRESS    PageAddress;
    int                     i;

    PageTableEntry = (UINT64 *)PageTableAddress;
    // 512 Page table entries (4096 bytes)
    for (i = 0; i < 512; i++) {
        if (*PageTableEntry & 1) {
            PageAddress = *PageTableEntry & 0x0000fffffffff000;
            Print(L"\n            PTE %d : { Page=%x } ", i, PageAddress);
            if (*PageTableEntry & 0x8000000000000000) {
                Print (L"[ Execute-Disable ] ");
            }
            if (*PageTableEntry & 0x40) {
                Print(L"[ Dirty ] ");
            }
            if (*PageTableEntry & 0x20) {
                Print(L"[ Accessed ] ");
            }
            if (*PageTableEntry & 0x10) {
                Print(L"[ Cache-Disable ] ");
            }
            if (*PageTableEntry & 0x8) {
                Print(L"[ Write-Through ] ");
            }
            if (*PageTableEntry & 0x4) {
                Print(L"[ User/Supervisor ] ");
            } else {
                Print(L"[ Supervisor-Only ] ");
            }
            if (*PageTableEntry & 0x2) {
                Print(L"[ Read/Write ] ");
            } else {
                Print(L"[ Read-Only ] ");
            }
            if (PauseAfterEntry) {
                WaitForKeyStroke(NULL);
            }
        }
        PageTableEntry++;
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
