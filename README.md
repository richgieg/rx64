# Rx64

Experimental operating system for x86-64 PCs.

## Initial Setup

Run the following in the root directory of the project:
```
git clone https://git.code.sf.net/p/gnu-efi/code gnu-efi/gnu-efi-code
```

## Post-Build Events

The "loader" and "kernel" projects each have a post-build event configured which copies the output file to the specified location.

To adjust, find the following in "loader" and "kernel" project properties:

Build Events -> Post-Build Event -> Command Line 
