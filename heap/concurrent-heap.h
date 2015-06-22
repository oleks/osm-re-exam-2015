/* TODO: A thread-safe, pointer-based, binary, max-heap implementation.

Copyright (c) OSM 2015 Course Team

Licensed under cc by-sa 3.0 with attribution required.

See also: https://creativecommons.org/licenses/by-sa/3.0/

This C version derived with changes from a C++ version found here:

  Nejla (http://codereview.stackexchange.com/users/31327/nejla).  Heap
  implementation using pointer. Stack Exchange. 2013-10-28. Accessed:
  2015-06-17.

  URL: http://codereview.stackexchange.com/questions/33365/heap-implementation-using-pointer

  (Archived by WebCite® at http://www.webcitation.org/6ZM1nBgH5)

*/

#ifndef OSM2015_CONCURRENT_HEAP_H
#define OSM2015_CONCURRENT_HEAP_H

#include <stdbool.h>  // bool
#include <stddef.h>   // size_t

#define T int         // the type of elements stored in the heap

typedef struct node {
  struct node* left_child;
  struct node* right_child;
  T value;
} node_t;

typedef struct heap {
  size_t n_nodes;
  node_t *root;
  bool (*less)(T, T); // the lesser elements go further down in the heap
} heap_t;

// Initialize the heap.
//
// Should be called exactly once, before all operations on the heap by the
// process.
//
// Return: 0 on success, non-zero on failure.
int heap_init(heap_t *heap, bool (*less)(T,T));

// Clear and free the heap.
//
// Should be called exactly once, after all operations on the heap are done.
//
// Return: 0 on success, non-zero on failure.
int heap_clear(heap_t *heap);

// Insert value into the heap.
//
// Returns: 0 on success, nonzero on error.
int heap_insert(heap_t *heap, T value);

#endif // OSM2015_CONCURRENT_HEAP_H
