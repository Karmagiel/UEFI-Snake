#include "efi_graphics.h"
#include "efi_utils.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = 0;

const efi_gfx_color efi_gfx_BLACK = {.red=0, .green=0, .blue=0,};
const efi_gfx_color efi_gfx_WHITE = {.red=0xff, .green=0xff, .blue=0xff};
const efi_gfx_color efi_gfx_RED = {.red=0xff, .green=0, .blue=0};
const efi_gfx_color efi_gfx_GREEN = {.red=0,  .green=0xff, .blue=0};
const efi_gfx_color efi_gfx_BLUE = {.red=0,  .green=0, .blue=0xff};


//Mode Stuff Adopted from https://wiki.osdev.org/GOP

void efi_gfx_init(){
    EFI_STATUS err;

    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    err = wrap(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    efi_handle_error(err, L"Graphics Locate failed!");


    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo;
    err = wrap(gop->QueryMode, 4, gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &info);
    if (err == EFI_NOT_STARTED){
        Print(L"EFI Graphics not started... Setting Default Mode to Mode 0.\n");
        err = wrap(gop->SetMode, 2, gop, 0);
    }
    efi_handle_error(err, L"Unable to get native mode");
}



UINT32 efi_gfx_get_mode_count(){
    return gop->Mode->MaxMode;
}

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* efi_gfx_get_mode(UINT32 index){
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo;
    wrap(gop->QueryMode, 4, gop, index, &SizeOfInfo, &info);
    return info;
}

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* efi_gfx_get_current_mode(){
    return gop->Mode->Info;
}

UINT32 efi_gfx_get_current_mode_index(){
    return gop->Mode->Mode;
}

void efi_gfx_set_mode(UINT32 index){
    efi_handle_error(wrap(gop->SetMode, 2, gop, index), L"Unable to set mode");
}

UINTN efi_gfx_get_framebuffer_size(){
    return gop->Mode->FrameBufferSize;
}

EFI_PHYSICAL_ADDRESS efi_gfx_get_framebuffer_address(){
    return gop->Mode->FrameBufferBase;
}

void efi_gfx_print_modes(){
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
     for (UINT32 i = 0; i < efi_gfx_get_mode_count(); i++) {
        info = efi_gfx_get_mode(i);
        Print(L"mode %03d width %d height %d format %x%s\n",
            i,
            info->HorizontalResolution,
            info->VerticalResolution,
            info->PixelFormat,
            i == efi_gfx_get_current_mode_index() ? L"(current)" : L""
        );
    }
}


void efi_gfx_draw_pixel(int x, int y, efi_gfx_color c){
   *((UINT32*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = *(UINT32*)&c;
}

void efi_gfx_fill_rect(int x, int y, int width, int height, efi_gfx_color c){
    for(int xi = 0; xi < width; ++xi){
        for(int yi = 0; yi < height; ++yi){
            efi_gfx_draw_pixel(x+xi, y+yi, c);
        }
    }
    
}

void efi_gfx_clear(efi_gfx_color c){
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode = efi_gfx_get_current_mode();
    efi_gfx_fill_rect(0, 0, mode->HorizontalResolution, mode->VerticalResolution, c);
}