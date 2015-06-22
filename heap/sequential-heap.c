/* A non-thread-safe, pointer-based, binary, max-heap implementation.

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

#include <errno.h>    // ENOMEM
#include <math.h>     // ilogb
#include <stdio.h>    // perror
#include <stdlib.h>   // malloc, free

#include "sequential-heap.h"

// *** private

static void node_free(node_t *node) {
  if (node == NULL) {
    return;
  }

  node_free(node->left_child);
  node_free(node->right_child);

  free(node);
}

static node_t * get_child(node_t *current, size_t indicator) {
  if (indicator > 0) {
    return current->right_child;
  } else {
    return current->left_child;
  }
}

static void set_child(node_t *parent, size_t indicator, node_t *new_child) {
  if (indicator > 0) {
    parent->right_child = new_child;
  } else {
    parent->left_child = new_child;
  }
}

static int merge_on_path(node_t *outsider, node_t *edge,
                              size_t path, size_t mask) {
  while (outsider != NULL) {
    mask >>= 1;
    if ((path & mask) > 0) {
      edge->left_child = outsider->left_child;
      edge = (edge->right_child = outsider);
      outsider = outsider->right_child;
    } else {
      edge->right_child = outsider->right_child;
      edge = (edge->left_child = outsider);
      outsider = outsider->left_child;
    }
  }

  edge->left_child = NULL;
  edge->right_child = NULL;

  return 0;
}

static int insert_on_path(heap_t *heap, node_t *node) {
  size_t path = heap->n_nodes;
  size_t mask = 1 << ilogb(path); // a fast \lfloor \log_2(path) \rfloor

  node_t* parent = NULL;
  node_t* current = heap->root;

  while (current != NULL && heap->less(node->value, current->value)) {
    mask >>= 1;
    parent = current;
    current = get_child(current, path & mask);
  }

  if (parent == NULL) {
    heap->root = node;
  } else {
    set_child(parent, path & mask, node);
  }

  // subtree starting at current is outside the heap, merge it in!

  return merge_on_path(current, node, path, mask);
}

// *** public

int heap_init(heap_t *heap, bool (*less)(T, T)) {
  heap->n_nodes = 0;
  heap->root = NULL;
  heap->less = less;

  return 0;
}

int heap_clear(heap_t *heap) {
  node_free(heap->root);
  heap->root = NULL;
  heap->n_nodes = 0;

  return 0;
}

int heap_insert(heap_t *heap, T value) {
  node_t *node = (node_t *)malloc(sizeof(node_t));
  if (node == NULL) {
    return ENOMEM;
  }

  node->value = value;
  heap->n_nodes += 1;

  return insert_on_path(heap, node);
}

#ifdef UNITTEST_BINARY_HEAP

#include <assert.h>
#include <stdio.h>

bool less(T a, T b) {
  return a < b;
}

size_t cardinality(node_t *node) {
  if (node == NULL) {
    return 0;
  }

  size_t i = cardinality(node->left_child);
  size_t j = cardinality(node->right_child);

  return i + j + 1;
}

bool subheap_is_valid(node_t *node, bool (*less)(T, T)) {
  if (node == NULL) {
    return true;
  }

  node_t *left = node->left_child;
  node_t *right = node->right_child;

  if (left != NULL && less(node->value, left->value)) {
    fprintf(stderr, "Heap-order violation (parent: %d, left: %d)\n",
      node->value, left->value);
    return false;
  } else if (right != NULL && less(node->value, right->value)) {
    fprintf(stderr, "Heap-order violation (parent: %d, right: %d)\n",
      node->value, right->value);
    return false;
  }

  else if (! subheap_is_valid(left, less)) {
    return false;
  }
  else if (! subheap_is_valid(right, less)) {
    return false;
  }
  return true;
}

bool heap_is_valid(heap_t *heap) {
  size_t card = cardinality(heap->root);

  if (card != heap->n_nodes) {
    fprintf(stderr, "Size wrong (count: %ld, n: %ld)\n",
      card, heap->n_nodes);
  }

  if (! subheap_is_valid(heap->root, heap->less)) {
    return false;
  }

  return true;
}

void show(heap_t *heap) {
  printf("n nodes: %ld\n", heap->n_nodes);
  if (heap->n_nodes == 0) {
    return;
  }

  node_t* queue[heap->n_nodes];

  size_t i = 0;
  queue[0] = heap->root;

  size_t j = 0;
  size_t height = 0;
  do {
    node_t* node = queue[i];
    printf("%d ", node->value);
    if (node->left_child != NULL) {
      j += 1;
      queue[j] = node->left_child;
    }
    if (node->right_child != NULL) {
      j += 1;
      queue[j] = node->right_child;
    }
    i += 1;
    if (i == heap->n_nodes || i == (size_t)((1 << (height + 1)) - 1)) {
      printf("\n");
      height += 1;
    }
  } while (i != heap->n_nodes);
}

int main () {
  size_t n;
  T value;

  if (fscanf(stdin, "%zu", &n) != 1)
    return 1;

  heap_t heap;
  heap_init(&heap, less);

  for (size_t i = 0; i != n; ++i) {
    if (fscanf(stdin, "%d", &value) != 1)
      return 1;

    heap_insert(&heap, value);
    show(&heap);
    assert(heap_is_valid(&heap));
  }

  heap_clear(&heap);

  return 0;
}

#endif
