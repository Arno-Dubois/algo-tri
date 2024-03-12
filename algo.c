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
    int number;
    struct Box *next;
} Box;


bool isInsideRect(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}


int index = 0;

Box *head = NULL;

int gap = 5, height_of_element = 50;
int espace(int number) {return height_of_element * number + gap * ++number;};

int box_height = 200;
void create_box (SDL_Renderer *rend, int x, int y) {
    SDL_Rect box = {x,y,box_height,box_height};
    SDL_SetRenderDrawColor(rend, 225, 27, 27, 255);
    SDL_RenderFillRect (rend, &box);
}

void create_element_of_box (){
    Box *newBox = malloc(sizeof(Box));
    newBox -> value = ++index, newBox -> next = NULL;
    if(head == NULL) {
        head = newBox;
    } else {
        Box* temp=head;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newBox;
    }
    
}
void display_element_of_box (SDL_Renderer *rend, int red, int blue, int green, int opacity, int number) {

    SDL_Rect element = {espace(number)%box_height,espace(espace(number) / (box_height-height_of_element) ),height_of_element,height_of_element};
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


        //creation des entrepots
        create_box (rend, 0,0);
        create_box (rend, 350,0);
        create_box (rend, 650,0);
        create_box (rend, 0,300);
        create_box (rend, 350,300);
        create_box (rend, 650,300);


        //affichage des boxs
        
        display_element_of_box (rend,0,0,0,255,1); 
        display_element_of_box (rend,0,0,0,255,2);
        display_element_of_box (rend,0,0,0,255,3);  
        display_element_of_box (rend,0,0,0,255,4);
        display_element_of_box (rend,0,0,0,255,5);
        display_element_of_box (rend,0,0,0,255,6);
        display_element_of_box (rend,0,0,0,255,7);
        display_element_of_box (rend,0,0,0,255,8);

        SDL_Rect add_button = {775, 540, 200, 75};
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderFillRect (rend, &add_button);

        SDL_Rect delete_button = {650, 540, 100, 75};
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderFillRect (rend, &delete_button);


        
        // TTF_Font *font = TTF_OpenFont("arial.ttf", 20);
        // SDL_Color textColor = {255, 255, 255, 255};
        // SDL_Surface *surface = TTF_RenderText_Solid(font, "Add Box", textColor);
        // SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);
        

        // int text_width = surface->w;
        // int text_height = surface->h;
        // SDL_FreeSurface(surface);

        // SDL_Rect renderQuad = {785, 550, text_width, text_height};
        // SDL_RenderCopy(rend, texture, NULL, &renderQuad);
        // SDL_DestroyTexture(texture);

        // SDL_Surface *surface2 = TTF_RenderText_Solid(font, "Delete Box", textColor);
        // SDL_Texture *texture2 = SDL_CreateTextureFromSurface(rend, surface2);
        

        // int text_width2 = surface2->w;
        // int text_height2 = surface2->h;
        // SDL_FreeSurface(surface2);

        // SDL_Rect renderQuad2 = {785, 460, text_width2, text_height2};
        // SDL_RenderCopy(rend, texture2, NULL, &renderQuad2);
        // SDL_DestroyTexture(texture2);


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

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (isInsideRect(event.button.x, event.button.y, add_button)) {
                            create_element_of_box();
                            }
                        }
                        else if (isInsideRect(event.button.x, event.button.y, delete_button)){ 
                            
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