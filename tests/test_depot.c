#include "depot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #cond); \
      failures++;                                                              \
    }                                                                          \
  } while (0)

static void sort_fully(Depot *depot) {
  int guard = 10000;
  while (depot_sort_step(depot) && --guard > 0) {
  }
  CHECK(guard > 0);
}

static void unload_all(Depot *depot) {
  while (depot_unload_one(depot)) {
  }
}

static void receive_all(Depot *depot, const char *names[], size_t count) {
  for (size_t i = 0; i < count; i++) {
    CHECK(depot_receive(depot, names[i]));
  }
}

static void test_fifo_order(void) {
  Depot depot;
  depot_init(&depot, 10, 10);
  const char *names[] = {"c", "a", "b"};
  receive_all(&depot, names, 3);

  CHECK(depot.incoming_count == 3);
  CHECK(strcmp(depot.incoming_head->name, "c") == 0);

  CHECK(depot_unload_one(&depot));
  CHECK(depot.incoming_count == 2);
  CHECK(depot.stored_count == 1);
  CHECK(strcmp(depot.stored->name, "c") == 0);
  CHECK(strcmp(depot.incoming_head->name, "a") == 0);

  unload_all(&depot);
  CHECK(depot.incoming_head == NULL);
  CHECK(depot.incoming_tail == NULL);
  CHECK(strcmp(depot.stored->next->next->name, "b") == 0);
  depot_free(&depot);
}

static void test_counts_each_item_once(void) {
  Depot depot;
  depot_init(&depot, 10, 10);
  const char *names[] = {"apple", "banana", "apple"};
  receive_all(&depot, names, 3);

  /* Items on the incoming dock are not in stock yet */
  CHECK(depot_count_of(&depot, "apple") == 0);

  unload_all(&depot);
  CHECK(depot_count_of(&depot, "apple") == 2);
  CHECK(depot_count_of(&depot, "banana") == 1);
  CHECK(depot.inventory_length == 2);
  depot_free(&depot);
}

static void test_capacity_limits_do_not_lose_items(void) {
  Depot depot;
  depot_init(&depot, 3, 2);
  const char *names[] = {"a", "b", "c"};
  receive_all(&depot, names, 3);
  CHECK(!depot_receive(&depot, "d")); /* dock full */

  unload_all(&depot);
  CHECK(depot.stored_count == 2);
  CHECK(depot.incoming_count == 1); /* warehouse full: item stays on the dock */
  CHECK(depot_count_of(&depot, "c") == 0);
  depot_free(&depot);
}

static void test_sort_alphabetical(void) {
  Depot depot;
  depot_init(&depot, 10, 10);
  const char *names[] = {"water", "apple", "orange", "apple", "banana"};
  receive_all(&depot, names, 5);
  unload_all(&depot);

  sort_fully(&depot);
  const char *expected[] = {"apple", "banana", "orange", "water"};
  const Stock *s = depot.inventory;
  for (size_t i = 0; i < 4; i++, s = s->next) {
    CHECK(s != NULL && strcmp(s->name, expected[i]) == 0);
  }

  const char *expected_items[] = {"apple", "apple", "banana", "orange",
                                  "water"};
  const Item *item = depot.stored;
  for (size_t i = 0; i < 5; i++, item = item->next) {
    CHECK(item != NULL && strcmp(item->name, expected_items[i]) == 0);
  }
  depot_free(&depot);
}

static void test_sort_by_count_desc_with_alphabetical_ties(void) {
  Depot depot;
  depot_init(&depot, 10, 10);
  const char *names[] = {"b", "c", "c", "a", "c", "b", "d"};
  receive_all(&depot, names, 7);
  unload_all(&depot);

  depot_set_mode(&depot, SORT_BY_COUNT_DESC);
  sort_fully(&depot);
  const char *expected[] = {"c", "b", "a", "d"};
  const Stock *s = depot.inventory;
  for (size_t i = 0; i < 4; i++, s = s->next) {
    CHECK(s != NULL && strcmp(s->name, expected[i]) == 0);
  }

  const char *expected_items[] = {"c", "c", "c", "b", "b", "a", "d"};
  const Item *item = depot.stored;
  for (size_t i = 0; i < 7; i++, item = item->next) {
    CHECK(item != NULL && strcmp(item->name, expected_items[i]) == 0);
  }
  depot_free(&depot);
}

static void test_sort_step_reports_done(void) {
  Depot depot;
  depot_init(&depot, 10, 10);
  CHECK(!depot_sort_step(&depot)); /* empty */

  const char *names[] = {"b", "a"};
  receive_all(&depot, names, 2);
  unload_all(&depot);
  CHECK(depot_sort_step(&depot));
  sort_fully(&depot);
  CHECK(!depot_sort_step(&depot));
  depot_free(&depot);
}

static void test_free_resets_depot(void) {
  Depot depot;
  depot_init(&depot, 4, 4);
  depot_set_mode(&depot, SORT_BY_COUNT_DESC);
  const char *names[] = {"a", "b"};
  receive_all(&depot, names, 2);
  depot_unload_one(&depot);

  depot_free(&depot);
  CHECK(depot.incoming_head == NULL && depot.stored == NULL &&
        depot.inventory == NULL);
  CHECK(depot.incoming_count == 0 && depot.stored_count == 0);
  CHECK(depot.stored_capacity == 4);
  CHECK(depot.mode == SORT_BY_COUNT_DESC);
  CHECK(depot_receive(&depot, "c")); /* still usable */
  depot_free(&depot);
}

int main(void) {
  test_fifo_order();
  test_counts_each_item_once();
  test_capacity_limits_do_not_lose_items();
  test_sort_alphabetical();
  test_sort_by_count_desc_with_alphabetical_ties();
  test_sort_step_reports_done();
  test_free_resets_depot();

  if (failures > 0) {
    fprintf(stderr, "%d check(s) failed\n", failures);
    return EXIT_FAILURE;
  }
  printf("All tests passed\n");
  return EXIT_SUCCESS;
}
