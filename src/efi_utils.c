#include "efi_utils.h"


void efi_stall(UINT32 ms){
    wrap(BS->Stall, 1, ms * 1000);
}

void efi_shutdown(){
    Print(L"Shutting Down...\n");
    wrap(ST->RuntimeServices->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    //or use  Exit(...)
}

EFI_INPUT_KEY efi_get_key_press(){
    EFI_INPUT_KEY key = {0};
    EFI_EVENT evt = {0};
    wrap(ST->ConIn->Reset, 2, ST->ConIn, FALSE);
    wrap(ST->BootServices->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &evt);
    wrap(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
    //Print(L"=> Key Pressed: Scan=%d Unicode=%c\n", key.ScanCode, key.UnicodeChar);
    return key;
}


void efi_handle_error(EFI_STATUS s, CHAR16* msg){
    if(EFI_ERROR(s)){
        if(msg){
            Print(L"Error (%r): %s\n", s, msg);
        }else{
            Print(L"Error occured! (%r)\n", s, msg);
        }
        
        Print(L"Press any key to exit...\n");
        Pause();
        efi_shutdown();
    }
}

EFI_TIME efi_get_datetime(){
    EFI_TIME time = {0};
    wrap(ST->RuntimeServices->GetTime, 2, &time, NULL);
    return time;
}

UINT64 efi_get_time_ms(){
    EFI_TIME t = efi_get_datetime();
    //approximated. ignoring that months have different day counts etc.
    return 1000ull*((UINT64)t.Second + 60ull*((UINT64)t.Minute + 60ull*((UINT64)t.Hour + 24ull*((UINT64)t.Day + 31ull*((UINT64)t.Month + 12ull*((UINT64)t.Year-1998))))));
}
