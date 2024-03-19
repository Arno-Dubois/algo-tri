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


int index[6] = {0,0,0,0,0,0};

int compteur_fruits [1024] = {0};

char buffer[100];

Box *head[] = {NULL, NULL, NULL, NULL, NULL, NULL};

int gap = 5, height_of_element = 50;
int espace(int number) {return height_of_element * number + gap * ++number;};
int maxBox;




char sorting = 'A';

int box_height = 200;
void create_box (SDL_Renderer *rend, int x, int y) {
    SDL_Rect box = {x,y,box_height,box_height};
    SDL_SetRenderDrawColor(rend, 90, 94, 107, 255);
    SDL_RenderFillRect (rend, &box);
}

void create_element_of_box (int boite, char nom[]){
    maxBox = box_height / espace(1);
    if(index[boite] >= maxBox*maxBox) return;
    ++index[boite];
    Box *newBox = malloc(sizeof(Box));
    strcpy(newBox -> value, nom);
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

    if (strcmp(nom, "pomme") == 0) {
        compteur_fruits[0]++;
    }
    if (strcmp(nom, "banana") == 0) {
        compteur_fruits[1]++;   
    }
    if (strcmp(nom, "water") == 0) {
        compteur_fruits[2]++;   
    }
    
}
void display_element_of_box (SDL_Renderer *rend, int red, int blue, int green, int opacity, int number, int boite[2]) {
    maxBox = box_height / espace(1);
    //maxBox 
    // printf("%-*d a\n", 20, maxBox);
    // box_height taille carré rouge
    // height_of_element tifezIUGIU
    // espace(var) taille var * (carré noire + gap)
    SDL_Rect element = {(espace(number)%(box_height-height_of_element+gap*3)) + boite[0],espace(espace(number) / (box_height-height_of_element) ) + boite[1],height_of_element,height_of_element};
    //SDL_Rect element = { x, y, height_of_element, height_of_element};
    SDL_SetRenderDrawColor (rend,red,blue,green,opacity);
    SDL_RenderFillRect (rend, &element);
}

void display_text (SDL_Renderer *rend,int x, int y, char write[2048]) {
    //Init TTF
    TTF_Font *font = TTF_OpenFont("arial.ttf", 20);
    SDL_Color textColor = {255, 255, 255, 255};

    SDL_Surface *surface = TTF_RenderText_Solid(font, write, textColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);
        

        int text_width = surface->w;
        int text_height = surface->h;
        SDL_FreeSurface(surface);


        SDL_Rect recttext = { x, y, 0, 0};
        SDL_RenderFillRect(rend, &recttext);
        SDL_Rect text = recttext;
        text.w = text_width;
        text.h = text_height;
        SDL_RenderCopy(rend, texture, NULL, &text);
        SDL_DestroyTexture(texture);
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
            if (strcmp(newElem, "pomme") == 0)
            {
                display_element_of_box (rend,222, 41, 22, 255, nbBox, posBox);
            }
            if (strcmp(newElem, "banana") == 0)
            {
                display_element_of_box (rend, 255, 255, 0, 255, nbBox, posBox);
            }
            if (strcmp(newElem, "water") == 0)
            {
                display_element_of_box (rend,  0,  127,  255, 255, nbBox, posBox);
            }
            
            
            newElem = newElem->next;
            
        }
        Box *newElemB = head[1];
        int posBoxB[2] = {1000, 0};
        for(int nbBox = 0; newElemB != NULL; ++nbBox) {
            display_element_of_box (rend,0,0,0,255,nbBox, posBoxB);
            newElemB = newElemB->next;
        }

        // InfoBar
        SDL_Rect infobar = {900, 0, 300, 600};
        SDL_SetRenderDrawColor(rend, 32, 32, 32 , 255);
        SDL_RenderFillRect (rend, &infobar);


        snprintf(buffer, sizeof(buffer), "Pommes : %d", compteur_fruits[0]);
        display_text (rend,910,10,buffer);

        snprintf(buffer, sizeof(buffer), "Banana : %d", compteur_fruits[1]);
        display_text (rend,910,60,buffer);

        snprintf(buffer, sizeof(buffer), "Water : %d", compteur_fruits[2]);
        display_text (rend,910,110,buffer);

        printf("sorting by %c\n", sorting);





        //création du add button
        SDL_Rect add_button = {0, 540, 200, 75};
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderFillRect (rend, &add_button);
         
         
         
        display_text (rend,50,560,"Add Box");

        
        
        //boutons de tri
        SDL_Rect tri_par_alphabet = {900, 540, 200, 75};
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderFillRect (rend, &tri_par_alphabet);

        display_text (rend,950,560,"A-B-C...Z");


        SDL_Rect tri_par_nombre = {900, 450, 200, 75};
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderFillRect (rend, &tri_par_nombre);

        display_text (rend,920,470,"Ordre decroissant");


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
                            // create_element_of_box(1, "pomme");
                            create_element_of_box(0, "banana");
                            create_element_of_box(0, "water");
                            }
                        }
                        
                        if (isInsideRect(event.button.x, event.button.y, tri_par_alphabet)){
                            sorting = 'A';
                        }
                         if (isInsideRect(event.button.x, event.button.y, tri_par_nombre)){
                            sorting = 'N';
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