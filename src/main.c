#include <SDL.h>
#include <SDL_ttf.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "depot.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

#define CELL_SIZE 50
#define CELL_GAP 5
#define GRID_COLUMNS 6
#define BOX_SIZE (GRID_COLUMNS * (CELL_SIZE + CELL_GAP) + CELL_GAP)
#define BOX_CAPACITY (GRID_COLUMNS * GRID_COLUMNS)

#define FRAME_TIME_MS (1000 / 60)
#define DELIVERY_SIZE 5
#define UNLOAD_INTERVAL_MS 150
#define SORT_INTERVAL_MS 80

#define FONT_PATH "assets/fonts/DejaVuSans.ttf"
#define FONT_SIZE 20

typedef struct {
  const char *name;
  SDL_Color color;
} Product;

static const Product CATALOG[] = {
    {"apple", {222, 41, 22, 255}},      {"banana", {240, 200, 0, 255}},
    {"blueberry", {110, 80, 200, 255}}, {"orange", {245, 130, 20, 255}},
    {"water", {0, 127, 255, 255}},
};
#define CATALOG_SIZE (sizeof CATALOG / sizeof CATALOG[0])

static const SDL_Color COLOR_BACKGROUND = {203, 198, 198, 255};
static const SDL_Color COLOR_BOX = {90, 94, 107, 255};
static const SDL_Color COLOR_SIDEBAR = {32, 32, 32, 255};
static const SDL_Color COLOR_BUTTON = {0, 0, 0, 255};
static const SDL_Color COLOR_BUTTON_ACTIVE = {40, 110, 200, 255};
static const SDL_Color COLOR_TEXT_LIGHT = {255, 255, 255, 255};
static const SDL_Color COLOR_TEXT_DARK = {30, 30, 30, 255};

static const SDL_Rect INCOMING_BOX = {40, 80, BOX_SIZE, BOX_SIZE};
static const SDL_Rect STORED_BOX = {415, 80, BOX_SIZE, BOX_SIZE};
static const SDL_Rect SIDEBAR = {790, 0, WINDOW_WIDTH - 790, WINDOW_HEIGHT};
static const SDL_Rect BUTTON_DELIVERY = {40, 490, 200, 56};
static const SDL_Rect BUTTON_CLEAR = {260, 490, 150, 56};
static const SDL_Rect BUTTON_SORT_ALPHA = {815, 520, 170, 56};
static const SDL_Rect BUTTON_SORT_COUNT = {1005, 520, 170, 56};

static SDL_Color product_color(const char *name) {
  for (size_t i = 0; i < CATALOG_SIZE; i++) {
    if (strcmp(CATALOG[i].name, name) == 0) {
      return CATALOG[i].color;
    }
  }
  /* Unknown product: derive a stable color from its name */
  unsigned hash = 5381;
  for (const char *c = name; *c != '\0'; c++) {
    hash = hash * 33u + (unsigned char)*c;
  }
  SDL_Color color = {(Uint8)hash, (Uint8)(hash >> 8), (Uint8)(hash >> 16), 255};
  return color;
}

static bool point_in_rect(int x, int y, const SDL_Rect *rect) {
  SDL_Point point = {x, y};
  return SDL_PointInRect(&point, rect);
}

static void fill_rect(SDL_Renderer *renderer, const SDL_Rect *rect,
                      SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, rect);
}

/* Draws text with its top-left corner at (x, y). If `center_in` is not NULL,
 * the text is centered inside that rectangle instead. */
static void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                      int x, int y, SDL_Color color,
                      const SDL_Rect *center_in) {
  SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
  if (surface == NULL) {
    return;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_Rect dest = {x, y, surface->w, surface->h};
  if (center_in != NULL) {
    dest.x = center_in->x + (center_in->w - surface->w) / 2;
    dest.y = center_in->y + (center_in->h - surface->h) / 2;
  }
  SDL_FreeSurface(surface);
  if (texture != NULL) {
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
  }
}

static void draw_button(SDL_Renderer *renderer, TTF_Font *font,
                        const SDL_Rect *rect, const char *label, bool active) {
  fill_rect(renderer, rect, active ? COLOR_BUTTON_ACTIVE : COLOR_BUTTON);
  draw_text(renderer, font, label, 0, 0, COLOR_TEXT_LIGHT, rect);
}

/* Draws a linked list of items as a grid of colored cells inside `box`. */
static void draw_items(SDL_Renderer *renderer, const SDL_Rect *box,
                       const Item *items) {
  int index = 0;
  for (const Item *item = items; item != NULL; item = item->next, index++) {
    SDL_Rect cell = {
        box->x + CELL_GAP + (index % GRID_COLUMNS) * (CELL_SIZE + CELL_GAP),
        box->y + CELL_GAP + (index / GRID_COLUMNS) * (CELL_SIZE + CELL_GAP),
        CELL_SIZE,
        CELL_SIZE,
    };
    fill_rect(renderer, &cell, product_color(item->name));
  }
}

static void draw_box(SDL_Renderer *renderer, TTF_Font *font,
                     const SDL_Rect *box, const char *title, const Item *items,
                     size_t count, size_t capacity) {
  char label[64];
  draw_text(renderer, font, title, box->x, box->y - 32, COLOR_TEXT_DARK, NULL);
  fill_rect(renderer, box, COLOR_BOX);
  draw_items(renderer, box, items);
  snprintf(label, sizeof label, "%zu / %zu", count, capacity);
  draw_text(renderer, font, label, box->x, box->y + box->h + 8, COLOR_TEXT_DARK,
            NULL);
}

static void draw_sidebar(SDL_Renderer *renderer, TTF_Font *font,
                         const Depot *depot, bool sorted) {
  fill_rect(renderer, &SIDEBAR, COLOR_SIDEBAR);
  draw_text(renderer, font, "Inventory", SIDEBAR.x + 25, 30, COLOR_TEXT_LIGHT,
            NULL);

  int y = 80;
  for (const Stock *stock = depot->inventory; stock != NULL;
       stock = stock->next) {
    char line[64];
    SDL_Rect swatch = {SIDEBAR.x + 25, y + 3, 18, 18};
    fill_rect(renderer, &swatch, product_color(stock->name));
    snprintf(line, sizeof line, "%s : %d", stock->name, stock->count);
    draw_text(renderer, font, line, SIDEBAR.x + 55, y, COLOR_TEXT_LIGHT, NULL);
    y += 36;
  }

  const char *status = depot->inventory == NULL ? "Empty"
                       : sorted                 ? "Sorted"
                                                : "Sorting...";
  draw_text(renderer, font, status, SIDEBAR.x + 25, 480, COLOR_TEXT_LIGHT,
            NULL);

  draw_button(renderer, font, &BUTTON_SORT_ALPHA, "A → Z",
              depot->mode == SORT_ALPHABETICAL);
  draw_button(renderer, font, &BUTTON_SORT_COUNT, "Count ↓",
              depot->mode == SORT_BY_COUNT_DESC);
}

static void receive_delivery(Depot *depot) {
  for (int i = 0; i < DELIVERY_SIZE; i++) {
    if (!depot_receive(depot, CATALOG[rand() % CATALOG_SIZE].name)) {
      break;
    }
  }
}

static TTF_Font *load_font(void) {
  /* Look next to the executable first, then in the working directory. */
  char *base = SDL_GetBasePath();
  if (base != NULL) {
    char path[1024];
    snprintf(path, sizeof path, "%s%s", base, FONT_PATH);
    SDL_free(base);
    TTF_Font *font = TTF_OpenFont(path, FONT_SIZE);
    if (font != NULL) {
      return font;
    }
  }
  return TTF_OpenFont(FONT_PATH, FONT_SIZE);
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }
  if (TTF_Init() != 0) {
    fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  int exit_code = EXIT_FAILURE;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  TTF_Font *font = NULL;

  window =
      SDL_CreateWindow("algo-tri - warehouse sorting", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (window == NULL) {
    fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
    goto cleanup;
  }
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
    goto cleanup;
  }
  font = load_font();
  if (font == NULL) {
    fprintf(stderr, "Error loading font '%s': %s\n", FONT_PATH, TTF_GetError());
    goto cleanup;
  }

  srand((unsigned)time(NULL));

  Depot depot;
  depot_init(&depot, BOX_CAPACITY, BOX_CAPACITY);

  Uint32 last_unload = SDL_GetTicks();
  Uint32 last_sort = SDL_GetTicks();
  bool sorted = true;
  bool running = true;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_MOUSEBUTTONDOWN &&
                 event.button.button == SDL_BUTTON_LEFT) {
        int x = event.button.x;
        int y = event.button.y;
        if (point_in_rect(x, y, &BUTTON_DELIVERY)) {
          receive_delivery(&depot);
        } else if (point_in_rect(x, y, &BUTTON_CLEAR)) {
          depot_free(&depot);
        } else if (point_in_rect(x, y, &BUTTON_SORT_ALPHA)) {
          depot_set_mode(&depot, SORT_ALPHABETICAL);
        } else if (point_in_rect(x, y, &BUTTON_SORT_COUNT)) {
          depot_set_mode(&depot, SORT_BY_COUNT_DESC);
        }
      }
    }

    /* Update the simulation at a fixed pace so the sorting is visible */
    Uint32 now = SDL_GetTicks();
    if (now - last_unload >= UNLOAD_INTERVAL_MS) {
      depot_unload_one(&depot);
      last_unload = now;
    }
    if (now - last_sort >= SORT_INTERVAL_MS) {
      sorted = !depot_sort_step(&depot);
      last_sort = now;
    }

    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g,
                           COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_RenderClear(renderer);

    draw_box(renderer, font, &INCOMING_BOX, "Incoming (FIFO queue)",
             depot.incoming_head, depot.incoming_count,
             depot.incoming_capacity);
    draw_box(renderer, font, &STORED_BOX, "Warehouse", depot.stored,
             depot.stored_count, depot.stored_capacity);
    draw_button(renderer, font, &BUTTON_DELIVERY, "Add delivery", false);
    draw_button(renderer, font, &BUTTON_CLEAR, "Clear", false);
    draw_sidebar(renderer, font, &depot, sorted);

    SDL_RenderPresent(renderer);

    /* Cap the frame rate in case VSync is unavailable */
    Uint32 frame_time = SDL_GetTicks() - now;
    if (frame_time < FRAME_TIME_MS) {
      SDL_Delay(FRAME_TIME_MS - frame_time);
    }
  }

  depot_free(&depot);
  exit_code = EXIT_SUCCESS;

cleanup:
  if (font != NULL) {
    TTF_CloseFont(font);
  }
  if (renderer != NULL) {
    SDL_DestroyRenderer(renderer);
  }
  if (window != NULL) {
    SDL_DestroyWindow(window);
  }
  TTF_Quit();
  SDL_Quit();
  return exit_code;
}
