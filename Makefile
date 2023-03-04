# Adapted from http://www.rodsbooks.com/efi-programming/index.html

# Get current Architecture  (e.g x86_64)
ARCH = $(shell uname -m | sed s,i[3456789]86,ia32,)

# Inputs, Target & Build-dir
SOURCEDIR = src
BUILDDIR = build

SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJS  = $(addprefix $(BUILDDIR)/, $(notdir $(SOURCES:.c=.o)))
DEPS = $(OBJS:.o=.d)
NAME = snake


# EFI Includes and Linker Script
EFIINC          = /usr/include/efi
EFIINCS         = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB             = /usr/lib
EFILIB          = /usr/lib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

# Flags
CFLAGS = $(EFIINCS) -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -Wall -Wextra -Werror -Wno-unused-function -MMD -MP
ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif
LDFLAGS = -nostdlib -znocombreloc -T $(EFI_LDS) -shared -Bsymbolic -L $(EFILIB) -L $(LIB) $(EFI_CRT_OBJS) 


# Build Directives

.PHONY: all, clean

all: $(BUILDDIR)/$(NAME).efi

# Include .d Deps to also rebuild objs on header change
-include $(DEPS)

$(BUILDDIR):
	mkdir -p $@

# create object files but create build-dir first
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# create shared lib
$(BUILDDIR)/$(NAME).so: ${OBJS}
	ld $(LDFLAGS) $^ -o $@ -lefi -lgnuefi

# create final .efi file
$(BUILDDIR)/$(NAME).efi: $(BUILDDIR)/$(NAME).so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .reloc --target=efi-app-$(ARCH) $^ $@



clean:
	rm -f $(BUILDDIR)/*.efi
	rm -f $(BUILDDIR)/*.so
	rm -f $(BUILDDIR)/*.o
	rm -f $(BUILDDIR)/*.d
	rmdir build