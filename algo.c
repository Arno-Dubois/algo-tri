//SET PATH=C:\mingw64\bin;%PATH%
//CD C:\Users\jules.botte-magalha\Documents\edt
//ls


#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <math.h>

#define FPS 60
#define WIDTH 900
#define HEIGHT 600
#define SIZE 200


typedef struct Box {
    int value;
    struct Box *next;
    struct Box *previous;
    struct BoxList *boxList;
} Box;

Box *head = NULL;
Box *tail = NULL;

void create_box (SDL_Renderer *rend, int x, int y) {
    SDL_Rect box = {x,y,200,200};
    SDL_SetRenderDrawColor(rend, 225, 27, 27, 255);
    SDL_RenderFillRect (rend, &box);
}

void create_element_of_box (SDL_Renderer *rend, int x, int y, int red, int blue, int green, int opacity){
    Box *newBox = malloc(sizeof(Box));
    newBox -> value = 3, newBox -> next = NULL;
    if(tail == NULL) {
        newBox -> previous = NULL;
        head = newBox;
    }else {
        newBox -> previous = tail;
    }
    tail = newBox;
    SDL_Rect element = {x,y,75,75};
    SDL_SetRenderDrawColor (rend,red,blue,green,opacity);
    SDL_RenderFillRect (rend, &element);
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }
    if (TTF_Init() != 0) {
        printf("Error initializing SDL TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }

    SDL_Window *wind = SDL_CreateWindow("Algo tri dans ma boite",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WIDTH, HEIGHT, 0);
    if (!wind) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, render_flags);
    if (!rend) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 0;
    }

    bool running = true;
    SDL_Event event;


    while (running) {
    SDL_SetRenderDrawColor(rend, 203, 198,198 , 255);
    SDL_RenderClear(rend);


    //creation des boites
    create_box (rend, 0,0);
    create_box (rend, 350,0);
    create_box (rend, 650,0);
    create_box (rend, 0,300);
    create_box (rend, 350,300);
    create_box (rend, 650,300);

    create_element_of_box (rend,0,0,0,0,0,255); 






        while (SDL_PollEvent(&event))  {
            switch (event.type)  {
                case SDL_WINDOWEVENT:
                    switch (event.window.event)  {
                        
                        // Handle quit event
                        case SDL_WINDOWEVENT_CLOSE:
                            event.type = SDL_QUIT;
                            SDL_PushEvent(&event);
                        break;
                        
                    }
                break;
                
                case SDL_QUIT:  
                    running = false;
                break;
                
            }
            
        }


    SDL_RenderPresent(rend);
    SDL_Delay(1000/FPS);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(wind);    
    TTF_Quit();
    SDL_Quit();
    return 0;
}