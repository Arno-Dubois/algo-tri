#include "depot.h"

#include <stdlib.h>
#include <string.h>

static void copy_name(char dest[DEPOT_NAME_LEN], const char *src) {
  /* Names longer than DEPOT_NAME_LEN - 1 are truncated */
  size_t len = strlen(src);
  if (len >= DEPOT_NAME_LEN) {
    len = DEPOT_NAME_LEN - 1;
  }
  memcpy(dest, src, len);
  dest[len] = '\0';
}

static void free_items(Item *item) {
  while (item != NULL) {
    Item *next = item->next;
    free(item);
    item = next;
  }
}

void depot_init(Depot *depot, size_t incoming_capacity,
                size_t stored_capacity) {
  memset(depot, 0, sizeof *depot);
  depot->incoming_capacity = incoming_capacity;
  depot->stored_capacity = stored_capacity;
  depot->mode = SORT_ALPHABETICAL;
}

void depot_free(Depot *depot) {
  free_items(depot->incoming_head);
  free_items(depot->stored);

  Stock *stock = depot->inventory;
  while (stock != NULL) {
    Stock *next = stock->next;
    free(stock);
    stock = next;
  }

  SortMode mode = depot->mode;
  depot_init(depot, depot->incoming_capacity, depot->stored_capacity);
  depot->mode = mode;
}

bool depot_receive(Depot *depot, const char *name) {
  if (depot->incoming_count >= depot->incoming_capacity) {
    return false;
  }

  Item *item = malloc(sizeof *item);
  if (item == NULL) {
    return false;
  }
  copy_name(item->name, name);
  item->next = NULL;

  if (depot->incoming_tail == NULL) {
    depot->incoming_head = item;
  } else {
    depot->incoming_tail->next = item;
  }
  depot->incoming_tail = item;
  depot->incoming_count++;
  return true;
}

static bool add_to_inventory(Depot *depot, const char *name) {
  Stock **link = &depot->inventory;
  while (*link != NULL) {
    if (strcmp((*link)->name, name) == 0) {
      (*link)->count++;
      return true;
    }
    link = &(*link)->next;
  }

  Stock *stock = malloc(sizeof *stock);
  if (stock == NULL) {
    return false;
  }
  copy_name(stock->name, name);
  stock->count = 1;
  stock->next = NULL;
  *link = stock;
  depot->inventory_length++;
  return true;
}

bool depot_unload_one(Depot *depot) {
  Item *item = depot->incoming_head;
  if (item == NULL || depot->stored_count >= depot->stored_capacity) {
    return false;
  }
  if (!add_to_inventory(depot, item->name)) {
    return false;
  }

  /* Dequeue from the incoming FIFO */
  depot->incoming_head = item->next;
  if (depot->incoming_head == NULL) {
    depot->incoming_tail = NULL;
  }
  depot->incoming_count--;

  /* Append to the end of the stored list */
  item->next = NULL;
  Item **link = &depot->stored;
  while (*link != NULL) {
    link = &(*link)->next;
  }
  *link = item;
  depot->stored_count++;
  return true;
}

/* True if `a` must come after `b` in the inventory. */
static bool stock_out_of_order(const Stock *a, const Stock *b, SortMode mode) {
  if (mode == SORT_BY_COUNT_DESC && a->count != b->count) {
    return a->count < b->count;
  }
  return strcmp(a->name, b->name) > 0;
}

/* Position of a product in the inventory, used to group stored items. */
static int inventory_rank(const Depot *depot, const char *name) {
  int rank = 0;
  for (const Stock *s = depot->inventory; s != NULL; s = s->next, rank++) {
    if (strcmp(s->name, name) == 0) {
      return rank;
    }
  }
  return rank;
}

/* One bubble-sort swap on the inventory list. */
static bool inventory_sort_step(Depot *depot) {
  for (Stock **link = &depot->inventory; *link != NULL && (*link)->next != NULL;
       link = &(*link)->next) {
    Stock *a = *link;
    Stock *b = a->next;
    if (stock_out_of_order(a, b, depot->mode)) {
      a->next = b->next;
      b->next = a;
      *link = b;
      return true;
    }
  }
  return false;
}

/* One bubble-sort swap on the stored items, ordered by inventory rank. */
static bool stored_sort_step(Depot *depot) {
  for (Item **link = &depot->stored; *link != NULL && (*link)->next != NULL;
       link = &(*link)->next) {
    Item *a = *link;
    Item *b = a->next;
    if (inventory_rank(depot, a->name) > inventory_rank(depot, b->name)) {
      a->next = b->next;
      b->next = a;
      *link = b;
      return true;
    }
  }
  return false;
}

bool depot_sort_step(Depot *depot) {
  return inventory_sort_step(depot) || stored_sort_step(depot);
}

void depot_set_mode(Depot *depot, SortMode mode) { depot->mode = mode; }

int depot_count_of(const Depot *depot, const char *name) {
  for (const Stock *s = depot->inventory; s != NULL; s = s->next) {
    if (strcmp(s->name, name) == 0) {
      return s->count;
    }
  }
  return 0;
}
