#ifndef DEPOT_H
#define DEPOT_H

#include <stdbool.h>
#include <stddef.h>

/*
 * A small warehouse ("depot") simulation built on singly linked lists:
 *
 *   incoming  : FIFO queue of items waiting to be unloaded
 *   stored    : items currently in the warehouse
 *   inventory : one entry per product name with its stock count
 *
 * Sorting is done incrementally (one bubble-sort swap per call) so the UI
 * can animate it. The inventory is sorted first, then the stored items are
 * regrouped to follow the inventory order.
 */

#define DEPOT_NAME_LEN 32

typedef struct Item {
  char name[DEPOT_NAME_LEN];
  struct Item *next;
} Item;

typedef struct Stock {
  char name[DEPOT_NAME_LEN];
  int count;
  struct Stock *next;
} Stock;

typedef enum { SORT_ALPHABETICAL, SORT_BY_COUNT_DESC } SortMode;

typedef struct {
  Item *incoming_head;
  Item *incoming_tail;
  size_t incoming_count;
  size_t incoming_capacity;

  Item *stored;
  size_t stored_count;
  size_t stored_capacity;

  Stock *inventory;
  size_t inventory_length;

  SortMode mode;
} Depot;

void depot_init(Depot *depot, size_t incoming_capacity, size_t stored_capacity);
void depot_free(Depot *depot);

/* Enqueue an item on the incoming dock. Returns false if the dock is full. */
bool depot_receive(Depot *depot, const char *name);

/* Move the oldest incoming item into the warehouse and update the inventory.
 * Returns false if there is nothing to unload or the warehouse is full. */
bool depot_unload_one(Depot *depot);

/* Perform one sorting swap. Returns true if something moved,
 * false once both the inventory and the stored items are in order. */
bool depot_sort_step(Depot *depot);

void depot_set_mode(Depot *depot, SortMode mode);

/* Stock count for a product name (0 if unknown). */
int depot_count_of(const Depot *depot, const char *name);

#endif /* DEPOT_H */
