#!/bin/bash

sudo mount -t drvfs e: /mnt/e
cp bin/kernel.elf bin/loader.efi /mnt/e
sudo umount /mnt/e
