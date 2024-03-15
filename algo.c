//SET PATH=C:\mingw64\bin;%PATH%
//CD C:\Users\jules.botte-magalha\Documents\edt
//ls


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <math.h>

#define FPS 60
#define WIDTH 1200
#define HEIGHT 600
#define SIZE 200


typedef struct Box {
    char value[1024];
    int number;
    struct Box *next;
    int posX;
    int posY;
} Box;


bool isInsideRect(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}


int index = 0;

Box *head[] = {NULL, NULL, NULL, NULL, NULL, NULL};

int gap = 5, height_of_element = 50;
int espace(int number) {return height_of_element * number + gap * ++number;};

int box_height = 200;
void create_box (SDL_Renderer *rend, int x, int y) {
    SDL_Rect box = {x,y,box_height,box_height};
    SDL_SetRenderDrawColor(rend, 225, 27, 27, 255);
    SDL_RenderFillRect (rend, &box);
}

void create_element_of_box (int boite, char nom[]){
    Box *newBox = malloc(sizeof(Box));
    printf("c%s\n", nom);
    strcpy(newBox -> value, nom);
    printf("c%s\n", newBox->value);
    newBox -> next = NULL;
    if(head[boite] == NULL) {
        head[boite] = newBox;
    } else {
        Box* temp=head[boite];
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newBox;
    }
    
}
void display_element_of_box (SDL_Renderer *rend, int red, int blue, int green, int opacity, int number, int boite[2]) {
    int maxBox = box_height / espace(1);
    //maxBox 
    // printf("%-*d a\n", 20, maxBox);
    // box_height taille carré rouge
    // height_of_element taille carré noire
    // espace(var) taille var * (carré noire + gap)
    SDL_Rect element = {(espace(number)%(box_height-height_of_element+gap*3)) + boite[0],espace(espace(number) / (box_height-height_of_element) ) + boite[1],height_of_element,height_of_element};
    //SDL_Rect element = { x, y, height_of_element, height_of_element};
    SDL_SetRenderDrawColor (rend,red,blue,green,opacity);
    SDL_RenderFillRect (rend, &element);
}

void compare_for_view () {
    char key [1024][1024];
    int numberKey[1024] = {0};
    int keyIndex = 0;
    Box *checkElem = head[0];
    for(int nbBox = 0; checkElem != NULL; ++nbBox) {
        int debug = 0;
        for (int i = 0; i < keyIndex; i++)
        {
           if (strcmp(key[i], checkElem -> value) == 0) {
            ++numberKey[keyIndex];
            debug = 1;
            }
        }
        
        
        if (debug != 1) {
            strcpy(key[keyIndex], checkElem -> value);
            ++keyIndex;
        }
        checkElem = checkElem->next;
    }
    
    printf ("%s : %d\n", key[0], numberKey[0]);
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
        Box *newElem = head[0];
        int posBox[2] = {0, 0};
        for(int nbBox = 0; newElem != NULL; ++nbBox) {
            display_element_of_box (rend,0,0,0,255,nbBox, posBox);
            newElem = newElem->next;
        }

        // InfoBar
        SDL_Rect infobar = {900, 0, 300, 600};
        SDL_SetRenderDrawColor(rend, 32, 32, 32 , 255);
        SDL_RenderFillRect (rend, &infobar);




        //création du add button
        SDL_Rect add_button = {0, 540, 200, 75};
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderFillRect (rend, &add_button);

        compare_for_view();

        /*SDL_Rect delete_button = {650, 540, 100, 75};
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderFillRect (rend, &delete_button);*/


        
        TTF_Font *font = TTF_OpenFont("arial.ttf", 20);
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface *surface = TTF_RenderText_Solid(font, "Add Box", textColor);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);
        

        int text_width = surface->w;
        int text_height = surface->h;
        SDL_FreeSurface(surface);

        SDL_Rect recttext = { 50, 560, 100, 100};
        SDL_RenderFillRect(rend, &recttext);
        SDL_Rect text = recttext;
        text.w = text_width;
        text.h = text_height;
        SDL_RenderCopy(rend, texture, NULL, &text);
        SDL_DestroyTexture(texture);

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
                            create_element_of_box(0, "pomme");
                            create_element_of_box(0, "banana");
                            }
                        }
                        /*else if (isInsideRect(event.button.x, event.button.y, delete_button)){ 
                            
                        }else {
                            for (int i = 0; i < index; i++) {
                                if (isInsideRect(event.button.x, event.button.y, tasks[i].rect)) {
                                    tasks[i].isDragging = true;
                                    tasks[i].offsetX = event.button.x - tasks[i].rect.x;
                                    tasks[i].offsetY = event.button.y - tasks[i].rect.y;
                                }
                            }
                        }*/
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