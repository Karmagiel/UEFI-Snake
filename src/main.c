//Author: Tom Maximilian von Allwörden

#include "efi_utils.h"
#include "efi_graphics.h"
#include "random.h"

// Constants
#define GRID_WIDTH 15
#define GRID_HEIGHT 15
#define MAX_LENGTH (GRID_WIDTH*GRID_HEIGHT)
#define NUM_APPLE 2
#define INITIAL_SNAKE_LENGTH 3
#define STALL_TIME 200  //aka time per tick
UINT32 CELL_SIZE;
UINT32 HOR_MARGIN; //for centering the playing field
UINT32 VER_MARGIN; //for centering the playing field

//colors
efi_gfx_color background_color = {.red=0, .green=0, .blue=0xff};
efi_gfx_color grid_color = {.red=0, .green = 0, .blue = 0};
efi_gfx_color apple_color = {.red=0xff, .green=0, .blue=0};
efi_gfx_color snake_head_color = {.red=0x7a, .green=0xcf, .blue=0x43};
efi_gfx_color snake_body_color = {.red=0, .green=0xff, .blue=0};
efi_gfx_color trail_color = {.red=0, .green=0x4f, .blue=0};

// Structs
struct{
    INT32 x;
    INT32 y;
} apple[NUM_APPLE];

typedef struct snake_node{
    INT32 x;
    INT32 y;
    struct snake_node* prev;
    struct snake_node* next;
} snake_node;

typedef enum{DIR_UP=0, DIR_RIGHT=1, DIR_DOWN=2, DIR_LEFT=3} direction;


// Game Vars
snake_node snake_buffer[MAX_LENGTH + 20];
UINT32 next_free = 0;

snake_node* head;
snake_node* tail;

direction dir = DIR_LEFT;


// Manage Snake Body
snake_node* allocate_snake_node(){
    snake_node* s = &snake_buffer[next_free++];
    s->next = s->prev = NULL;
    s->x = s->y = 0;
    return s;
}

BOOLEAN collides_with_snake(INT32 x, INT32 y, BOOLEAN include_head){
    if(include_head && head->x == x && head->y == y){
        return TRUE;
    }
    snake_node* cur = head->next;
    while(cur != NULL){
        if(cur->x == x && cur->y == y){
            return TRUE;
        }
        cur = cur->next;
    }
    return FALSE;
}

//draw cells
void draw_cell(INT32 x, INT32 y, efi_gfx_color c){
    //leave small borders of background behind
    efi_gfx_fill_rect(HOR_MARGIN + x*CELL_SIZE+2, VER_MARGIN + y*CELL_SIZE+2, CELL_SIZE-4, CELL_SIZE-4, c);
}

//Manage Apples
void place_apple(UINT32 apple_index){
    //Place apples on a free space or on top of another apple. Not on the snake.
    do{
        apple[apple_index].x = random_next_range_int64(0, GRID_WIDTH);
        apple[apple_index].y = random_next_range_int64(0, GRID_HEIGHT);
    }while(collides_with_snake(apple[apple_index].x, apple[apple_index].y, TRUE));
    draw_cell(apple[apple_index].x, apple[apple_index].y, apple_color);
}


void reset(){
    next_free = 0;
    head = tail = NULL;
    dir = DIR_LEFT;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info = efi_gfx_get_current_mode();
    UINT32 pxl_per_cell_hor = info->HorizontalResolution / GRID_WIDTH;
    UINT32 pxl_per_cell_vert = info->VerticalResolution / GRID_HEIGHT;
    if(pxl_per_cell_hor < pxl_per_cell_vert){
        CELL_SIZE = pxl_per_cell_hor;
        HOR_MARGIN = 0;
        VER_MARGIN = (info->VerticalResolution - (GRID_WIDTH*pxl_per_cell_hor)) / 2;
    }else{
        CELL_SIZE = pxl_per_cell_vert;
        HOR_MARGIN = (info->HorizontalResolution - (GRID_HEIGHT*pxl_per_cell_vert)) / 2;
        VER_MARGIN = 0;
    }

    //Draw Background and cells    
    efi_gfx_clear(background_color);
    for(int xi = 0; xi < GRID_WIDTH; ++xi){
        for(int yi = 0; yi < GRID_HEIGHT; ++yi){
            draw_cell(xi, yi, grid_color);
        }
    }
}

UINT32 game(){
    
    EFI_STATUS status;

    UINT32 snake_length = INITIAL_SNAKE_LENGTH;
    if(INITIAL_SNAKE_LENGTH < 2){
        Print(L"Error: did i program a false value? Snake Len must be >=2 for the game() function to work!\n");
        Pause();
        efi_shutdown();
    }

    //Init Snake.
    head = allocate_snake_node();
    head->x = GRID_WIDTH/2;
    head->y = GRID_HEIGHT/2;

    snake_node* cur = head;
    for(int i = 1; i < INITIAL_SNAKE_LENGTH; ++i){
        snake_node* new = allocate_snake_node();
        new->x = head->x;
        new->y = head->y;

        draw_cell(new->x, new->y, snake_body_color); //will be overdrawn directly below. keeping this in case i modify this loop.

        cur->next = new;
        new->prev = cur;
        cur = new;
    }
    tail = cur;
    draw_cell(head->x, head->y, snake_head_color);

    //Init apples
    for(int i = 0; i < NUM_APPLE; ++i){
        place_apple(i);
    }
    
    efi_stall(STALL_TIME);

    //Game Loop
    for(;;){
        //poll key.
        //wrap(ST->ConIn->Reset, 2, ST->ConIn, FALSE);
        EFI_INPUT_KEY key = {0};
        status = wrap(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
        if(status == EFI_SUCCESS){
            //Up Movement
            if(dir != DIR_UP && dir != DIR_DOWN && ((key.ScanCode==SCAN_NULL && key.UnicodeChar=='w') || key.ScanCode==SCAN_UP)){
                dir = DIR_UP;
            }
        
            //Down Movement
            if(dir != DIR_UP && dir != DIR_DOWN && ((key.ScanCode==SCAN_NULL && key.UnicodeChar=='s') || key.ScanCode==SCAN_DOWN)){
                dir = DIR_DOWN;
            }
            
            //Right Movement
            if(dir != DIR_LEFT && dir != DIR_RIGHT && ((key.ScanCode==SCAN_NULL && key.UnicodeChar=='d') || key.ScanCode==SCAN_RIGHT)){
                dir = DIR_RIGHT;
            }

            //Left Movement
            if(dir != DIR_LEFT && dir != DIR_RIGHT && ((key.ScanCode==SCAN_NULL && key.UnicodeChar=='a') || key.ScanCode==SCAN_LEFT)){
                dir = DIR_LEFT;
            }
        }

        //before move: clear snakes tail
        draw_cell(tail->x, tail->y, trail_color);
        draw_cell(head->x, head->y, snake_body_color); //head will become part of body shortly

        //move snake in direction.  
        //put tail-node to new position (in `dir` direction)
        INT32 x = head->x;
        INT32 y = head->y;

        tail->prev->next = NULL;
        tail->next = head;
        head->prev = tail;
        head = tail;
        tail = tail->prev;
        head->prev = NULL;
        
        switch(dir){
            case DIR_UP: head->x = x; head->y = y-1; break;
            case DIR_DOWN: head->x = x; head->y = y+1; break; 
            case DIR_RIGHT: head->x = x+1; head->y = y; break;
            case DIR_LEFT: head->x = x-1; head->y = y; break;
        }

        //wrap around screen
        if(head->x < 0)          head->x = GRID_WIDTH-1;
        if(head->x>=GRID_WIDTH)  head->x = 0;
        if(head->y < 0)          head->y = GRID_HEIGHT-1;
        if(head->y>=GRID_HEIGHT) head->y = 0;

        //update tail color
        draw_cell(tail->x, tail->y, snake_body_color);
        draw_cell(head->x, head->y, snake_head_color);


        //check collisions head <-> apple
        for(int i = 0; i < NUM_APPLE; ++i){
            if(collides_with_snake(apple[i].x, apple[i].y, TRUE)){
                ++snake_length;

                //check length for max length and end there
                if(snake_length >= MAX_LENGTH){
                    goto end;
                }

                //add new snake part
                snake_node* new = allocate_snake_node();
                new->x = tail->x;
                new->y = tail->y;
                tail->next = new;
                new->prev = tail;
                tail = new;

                //create new apple (on free space)
                place_apple(i);

            }
        }
            
        
        //check collisions head <-> rest of body
        if(collides_with_snake(head->x, head->y, FALSE)){
            goto end;
        }


        efi_stall(STALL_TIME);
    }

    end:
    return snake_length;
}


EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable){
    
    InitializeLib(ImageHandle, SystemTable);  //Macht den Systemtable überall über ST erreichbar
    random_set_seed(efi_get_time_ms());


    Print(L"Hello, world! (from UEFI %s)\n", ST->FirmwareVendor);
    Print(L"This is a snake Game. Press any Key to play...\n");
    Print(L"(If Graphics are not Supported by your UEFI Firmware, you will see an error.)\n");
    Pause();

    efi_gfx_init();

    UINT32 highscore = 0;

    for(;;){
        reset();
        UINT32 snake_length = game();
        if(snake_length > highscore){
            highscore = snake_length;
        }
        BOOLEAN won = snake_length >= MAX_LENGTH;
        Print(L"%s\nScore: %u. Highscore: %u\n\n\n", won?L"You cleared the game, hurrary :D" : L"Game Over.", snake_length, highscore);
        
        Print(L"Press Escape to quit, or any other key to play again.\n");
        EFI_INPUT_KEY k = efi_get_key_press();
        if(k.ScanCode == SCAN_ESC){
            Print(L"Thanks for playing :)");
            efi_stall(2000);
            break;
        }

    }

    
    efi_shutdown();
    return EFI_SUCCESS;  //unreachable
}

