
Simple UEFI Snake Game.

![example](example.png)

# Setup
1. install gnu-efi
2. install Qemu   (optional)
3. Get UEFI Firmware (for example EDK II's OVMF)  (optional)
4. Adjust the `LIB` and `EFILIB` variables in the Makefile for your system, if necessary.

# Build
`make`  should be enough.<br/>
`make clean` to remove build files.

# Run
Put this onto your /EFI Partition and start with uefi shell / add to grub / ...<br/>
Or use QEMU - Example for x86_64:<br/>
`qemu-system-x86_64 -m 1G -net none -bios path/to/uefi/firmware.fd -kernel build/snake.efi`

# Make it available as boot-option in Grub
Add something like this to `/etc/grub.d/40_custom`  and do a `update-grub`.
```
menuentry 'Snake'  $menuentry_id_option 'play Snake' {
	insmod part_gpt
	insmod fat
	search --no-floppy --fs-uuid --set=root <uuid of the .efi location, e.g. your EFI partition>
	chainloader /EFI/snake.efi
}
```
