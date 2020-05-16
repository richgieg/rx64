#!/bin/bash

mkdir -p bin/kernel
mkdir -p bin/loader

################################################################################
# Compile Kernel
################################################################################

# Kernel
gcc -c -fno-stack-protector -fshort-wchar -mno-red-zone \
    -o bin/kernel/kernel.o src/kernel/kernel.c

# Console
gcc -c -fno-stack-protector -fshort-wchar -mno-red-zone \
    -o bin/kernel/console.o src/kernel/console.c

# Graphics
gcc -c -fno-stack-protector -fshort-wchar -mno-red-zone \
    -o bin/kernel/graphics.o src/kernel/graphics.c

# Runtime
gcc -c -fno-stack-protector -fshort-wchar -mno-red-zone \
    -o bin/kernel/runtime.o src/kernel/runtime.c

################################################################################
# Link Kernel
################################################################################

ld \
    bin/kernel/kernel.o \
    bin/kernel/console.o \
    bin/kernel/graphics.o \
    bin/kernel/runtime.o \
    bin/kernel/util.o \
    --entry KernelEntry \
    -nostdlib \
    -static \
    -o bin/kernel.elf


################################################################################
# Compile Loader
################################################################################

# Loader
gcc -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER \
    -DHAVE_USE_MS_ABI -I /usr/include/efi -I /usr/include/efi/x86_64 \
    -o bin/loader/loader.o src/loader/loader.c

# Graphics
gcc -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER \
    -DHAVE_USE_MS_ABI -I /usr/include/efi -I /usr/include/efi/x86_64 \
    -o bin/loader/graphics.o src/loader/graphics.c

# Recon
gcc -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER \
    -DHAVE_USE_MS_ABI -I /usr/include/efi -I /usr/include/efi/x86_64 \
    -o bin/loader/recon.o src/loader/recon.c

# Util
gcc -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER \
    -DHAVE_USE_MS_ABI -I /usr/include/efi -I /usr/include/efi/x86_64 \
    -o bin/loader/util.o src/loader/util.c

################################################################################
# Link Loader
################################################################################

ld \
    bin/loader/loader.o \
    bin/loader/graphics.o \
    bin/loader/recon.o \
    bin/loader/util.o \
    /usr/lib/crt0-efi-x86_64.o \
    -nostdlib \
    -znocombreloc \
    -T /usr/lib/elf_x86_64_efi.lds \
    -shared \
    -Bsymbolic \
    -L /usr/lib \
    -l:libgnuefi.a \
    -l:libefi.a \
    -o bin/loader/loader.so

objcopy \
    -j .text \
    -j .sdata \
    -j .data \
    -j .dynamic \
    -j .dynsym \
    -j .rel \
    -j .rela \
    -j .reloc \
    --target=efi-app-x86_64 \
    bin/loader/loader.so \
    bin/loader.efi
