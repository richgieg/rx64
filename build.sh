#!/bin/bash

mkdir -p bin

nasm -o bin/kernel.bin src/kernel/kernel.asm

gcc \
    -c \
    -fno-stack-protector \
    -fpic \
    -fshort-wchar \
    -mno-red-zone \
    -DEFI_FUNCTION_WRAPPER \
    -DHAVE_USE_MS_ABI \
    -I /usr/include/efi \
    -I /usr/include/efi/x86_64 \
    -o bin/loader.o \
    src/loader/loader.c

ld \
    bin/loader.o \
    /usr/lib/crt0-efi-x86_64.o \
    -nostdlib \
    -znocombreloc \
    -T /usr/lib/elf_x86_64_efi.lds \
    -shared \
    -Bsymbolic \
    -L /usr/lib \
    -l:libgnuefi.a \
    -l:libefi.a \
    -o bin/loader.so

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
    bin/loader.so \
    bin/loader.efi
