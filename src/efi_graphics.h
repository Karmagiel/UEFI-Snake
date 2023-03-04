#ifndef EFI_GRAPHICS_H
#define EFI_GRAPHICS_H
#include <efi.h>

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;


typedef struct __attribute__((__packed__)){
    UINT8 blue;
    UINT8 green;
    UINT8 red;
    UINT8 padding;
} efi_gfx_color;

extern const efi_gfx_color efi_gfx_BLACK, efi_gfx_WHITE, efi_gfx_RED, efi_gfx_GREEN, efi_gfx_BLUE;


void efi_gfx_init();


//Modes & Framebuffer
UINT32 efi_gfx_get_mode_count();

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* efi_gfx_get_mode(UINT32 index);

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* efi_gfx_get_current_mode();

UINT32 efi_gfx_get_current_mode_index();

void efi_gfx_set_mode(UINT32 index);

UINTN efi_gfx_get_framebuffer_size();

EFI_PHYSICAL_ADDRESS efi_gfx_get_framebuffer_address();

void efi_gfx_print_modes();


//Drawing

void efi_gfx_draw_pixel(int x, int y, efi_gfx_color c);

void efi_gfx_fill_rect(int x, int y, int width, int height, efi_gfx_color c);

void efi_gfx_clear(efi_gfx_color c);


#endif
