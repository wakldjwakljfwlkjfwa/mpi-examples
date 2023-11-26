#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct btree {
  struct btree *left;
  struct btree *right;
  int value;
};

struct array {
  int size;
  int *data;
};

struct btree *btree_push(struct btree *head, int new_value);

void btree_print(struct btree *t);

void index_and_edges(int size, struct array *index, struct array *edges);

void array_insert(struct array *arr, struct array *from);

void btree_neighbors(struct btree *t, struct array *index, struct array *edges,
                     struct btree *parent);

void array_print(struct array *arr);

void array_init(struct array *arr);

void array_push(struct array *arr, int value);

int array_last(struct array *arr);

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  srand(time(NULL));

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int *index;
  int *index_size = malloc(sizeof(int));
  int *edges;
  int *edges_size = malloc(sizeof(int));

  if (rank == 0) {
    struct array index_neighbors;
    struct array edges_neighbors;
    array_init(&index_neighbors);
    array_init(&edges_neighbors);
    index_and_edges(size, &index_neighbors, &edges_neighbors);
    *index_size = index_neighbors.size;
    *edges_size = edges_neighbors.size;
    index = index_neighbors.data;
    edges = edges_neighbors.data;
  }

  MPI_Bcast(index_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(edges_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank != 0) {
    index = malloc(sizeof(int) * *index_size);
    edges = malloc(sizeof(int) * *edges_size);
  }

  MPI_Bcast(index, *index_size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(edges, *edges_size, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Comm comm_tree;
  MPI_Graph_create(MPI_COMM_WORLD, size, index, edges, 1, &comm_tree);

  int max_neighbors = 3;
  int neighbors[max_neighbors];
  int num_neighbors;

  MPI_Graph_neighbors(comm_tree, rank, max_neighbors, neighbors);
  MPI_Graph_neighbors_count(comm_tree, rank, &num_neighbors);

  printf("Rank %d has %d neighbors: ", rank, num_neighbors);
  for (int j = 0; j < num_neighbors; ++j) {
    printf("%d ", neighbors[j]);
  }
  printf("\n");

  MPI_Comm_free(&comm_tree);

  MPI_Finalize();
  return 0;
}

void array_print(struct array *arr) {
  printf("printing array: ");
  for (int i = 0; i < arr->size; i++) {
    printf("%d ", arr->data[i]);
  }
  printf("\n");
}

void index_and_edges(int size, struct array *index, struct array *edges) {
  int *btree_push_order = malloc(sizeof(int) * size);
  for (int i =0; i < size; i++) {
    btree_push_order[i] = i;
  }
  for (int i = size - 1; i > 0; i--) {
    int swap_index = rand() % i;
    int tmp = btree_push_order[i];
    btree_push_order[i] =  btree_push_order[swap_index];
    btree_push_order[swap_index] = tmp;
  }

  // if (size % 2 == 0) {
  //   for (int i = 0; i < size / 2; i++) {
  //     btree_push_order[i * 2] = size / 2 - i - 1;
  //     btree_push_order[i * 2 + 1] = size / 2 + i;
  //   }
  // } else {
  //   btree_push_order[0] = size / 2;
  //   for (int i = 0; i < size / 2; i++) {
  //     btree_push_order[i * 2 + 1] = size / 2 - i - 1;
  //     btree_push_order[i * 2 + 2] = size / 2 + i + 1;
  //   }
  // }

  printf("btree push order: ");
  for (int i = 0; i < size; i++) {
    printf("%d ", btree_push_order[i]);
  }
  printf("\n");

  struct btree *head = btree_push(NULL, btree_push_order[0]);
  for (int i = 1; i < size; i++) {
    btree_push(head, btree_push_order[i]);
  }

  btree_neighbors(head, index, edges, NULL);
}

void array_insert(struct array *arr, struct array *from) {
  int *new_data = malloc((arr->size + from->size) * sizeof(int));
  for (int i = 0; i < arr->size; i++) {
    new_data[i] = arr->data[i];
  }
  for (int i = 0; i < from->size; i++) {
    new_data[arr->size + i] = from->data[i];
  }
  free(arr->data);
  arr->data = new_data;
  arr->size = arr->size + from->size;
}

void btree_neighbors(struct btree *t, struct array *index, struct array *edges,
                     struct btree *parent) {
  if (t->left != NULL) {
    btree_neighbors(t->left, index, edges, t);
  }

  int size = 0;
  if (parent != NULL) {
    size++;
  }
  if (t->left != NULL) {
    size++;
  }
  if (t->right != NULL) {
    size++;
  }

  struct array neighbors;
  array_init(&neighbors);
  if (parent != NULL) {
    array_push(&neighbors, parent->value);
  }
  if (t->left != NULL) {
    array_push(&neighbors, t->left->value);
  }
  if (t->right != NULL) {
    array_push(&neighbors, t->right->value);
  }

  int index_last;
  if (index->size == 0) {
    index_last = 0;
  } else {
    index_last = array_last(index);
  }
  array_push(index, neighbors.size + index_last);

  array_insert(edges, &neighbors);

  if (t->right != NULL) {
    btree_neighbors(t->right, index, edges, t);
  }
}

struct btree *btree_push(struct btree *t, int new_value) {
  if (t == NULL) {
    t = (struct btree *)malloc(sizeof(struct btree));
    t->value = new_value;
    t->left = NULL;
    t->right = NULL;
  } else if (t->value > new_value) {
    t->left = btree_push(t->left, new_value);
  } else if (t->value < new_value) {
    t->right = btree_push(t->right, new_value);
  }

  return t;
}

void btree_print(struct btree *t) {
  if (t->left != NULL) {
    btree_print(t->left);
  }
  printf("%d ", t->value);
  if (t->right != NULL) {
    btree_print(t->right);
  }
}

void array_init(struct array *arr) {
  arr->size = 0;
  arr->data = NULL;
}

void array_push(struct array *arr, int value) {
  int *new_data = malloc((arr->size + 1) * sizeof(int));
  for (int i = 0; i < arr->size; i++) {
    new_data[i] = arr->data[i];
  }
  new_data[arr->size] = value;
  arr->size += 1;
  free(arr->data);
  arr->data = new_data;
}

int array_last(struct array *arr) { return arr->data[arr->size - 1]; }
