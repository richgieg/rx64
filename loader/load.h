#pragma once

#include <efi.h>

typedef struct _IMAGE_FILE_HEADER {
    UINT16 Machine;
    UINT16 NumberOfSections;
    UINT32 TimeDateStamp;
    UINT32 PointerToSymbolTable;
    UINT32 NumberOfSymbols;
    UINT16 SizeOfOptionalHeader;
    UINT16 Characteristics;
} IMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
    UINT32 VirtualAddress;
    UINT32 Size;
} IMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

typedef struct _IMAGE_OPTIONAL_HEADER64 {
    UINT16                  Magic;
    UINT8                   MajorLinkerVersion;
    UINT8                   MinorLinkerVersion;
    UINT32                  SizeOfCode;
    UINT32                  SizeOfInitializedData;
    UINT32                  SizeOfUninitializedData;
    UINT32                  AddressOfEntryPoint;
    UINT32                  BaseOfCode;
    UINT64                  ImageBase;
    UINT32                  SectionAlignment;
    UINT32                  FileAlignment;
    UINT16                  MajorOperatingSystemVersion;
    UINT16                  MinorOperatingSystemVersion;
    UINT16                  MajorImageVersion;
    UINT16                  MinorImageVersion;
    UINT16                  MajorSubsystemVersion;
    UINT16                  MinorSubsystemVersion;
    UINT32                  Win32VersionValue;
    UINT32                  SizeOfImage;
    UINT32                  SizeOfHeaders;
    UINT32                  CheckSum;
    UINT16                  Subsystem;
    UINT16                  DllCharacteristics;
    UINT64                  SizeOfStackReserve;
    UINT64                  SizeOfStackCommit;
    UINT64                  SizeOfHeapReserve;
    UINT64                  SizeOfHeapCommit;
    UINT32                  LoaderFlags;
    UINT32                  NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY    DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS64 {
    UINT32                      Signature;
    IMAGE_FILE_HEADER           FileHeader;
    IMAGE_OPTIONAL_HEADER64     OptionalHeader;
} IMAGE_NT_HEADERS64;

typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    UINT16   e_magic;                     // Magic number
    UINT16   e_cblp;                      // Bytes on last page of file
    UINT16   e_cp;                        // Pages in file
    UINT16   e_crlc;                      // Relocations
    UINT16   e_cparhdr;                   // Size of header in paragraphs
    UINT16   e_minalloc;                  // Minimum extra paragraphs needed
    UINT16   e_maxalloc;                  // Maximum extra paragraphs needed
    UINT16   e_ss;                        // Initial (relative) SS value
    UINT16   e_sp;                        // Initial SP value
    UINT16   e_csum;                      // Checksum
    UINT16   e_ip;                        // Initial IP value
    UINT16   e_cs;                        // Initial (relative) CS value
    UINT16   e_lfarlc;                    // File address of relocation table
    UINT16   e_ovno;                      // Overlay number
    UINT16   e_res[4];                    // Reserved words
    UINT16   e_oemid;                     // OEM identifier (for e_oeminfo)
    UINT16   e_oeminfo;                   // OEM information; e_oemid specific
    UINT16   e_res2[10];                  // Reserved words
    UINT32   e_lfanew;                    // File address of new exe header
} IMAGE_DOS_HEADER;

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct _IMAGE_SECTION_HEADER {
    CHAR8   Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        UINT32 PhysicalAddress;
        UINT32 VirtualSize;
    } Misc;
    UINT32  VirtualAddress;
    UINT32  SizeOfRawData;
    UINT32  PointerToRawData;
    UINT32  PointerToRelocations;
    UINT32  PointerToLinenumbers;
    UINT16  NumberOfRelocations;
    UINT16  NumberOfLinenumbers;
    UINT32  Characteristics;
} IMAGE_SECTION_HEADER;

typedef struct _KERNEL_IMAGE_INFO {
    VOID (*kmain) (
        IN UINT64 FrameBufferBase,
        IN UINT32 HorizontalResolution,
        IN UINT32 VerticalResolution
        );
} KERNEL_IMAGE_INFO;

EFI_STATUS
LoadKernelImage (
    IN EFI_HANDLE           LoaderImageHandle,
    IN CHAR16               *FileName,
    OUT KERNEL_IMAGE_INFO   **KernelImageInfo
    );
