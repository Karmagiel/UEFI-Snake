#ifndef EFI_UTILS_H
#define EFI_UTILS_H

#include <efi.h>
#include <efilib.h>

#define wrap uefi_call_wrapper


void efi_stall(UINT32 ms);

void efi_shutdown();

EFI_INPUT_KEY efi_get_key_press();

void efi_handle_error(EFI_STATUS s, CHAR16* msg);

EFI_TIME efi_get_datetime();

UINT64 efi_get_time_ms();


#endif