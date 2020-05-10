#!/bin/bash

sudo mount -t drvfs e: /mnt/e
cp bin/kernel.bin bin/loader.efi /mnt/e
sudo umount /mnt/e
