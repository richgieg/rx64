#!/bin/bash

sudo mount -t drvfs e: /mnt/e
cp bin/main.efi /mnt/e
sudo umount /mnt/e
