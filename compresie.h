#pragma once
#include <inttypes.h>

typedef struct QuadtreeNode {
    unsigned char blue, red, green;
    uint32_t area;
    int32_t top_left, top_right;
    int32_t bottom_left, bottom_right;
} __attribute__((packed)) QuadtreeNode;

typedef struct pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} __attribute__((packed)) pixel;

//structura de nod ce va fi folosita in arbore
typedef struct node {
    struct node * top_left;
    struct node * top_right;
    struct node * bottom_left;
    struct node * bottom_right;
    pixel data;
} node;

pixel ** get_grid(int x, int y);
void get_info(FILE *fin, int *x, int *y, int *max);
void make_grid(FILE **fin, int x, int y, pixel **grid);
node *get_new_node(pixel pi);
uint64_t mean(pixel **grid, int x, int y, int size);
void get_tree(node **root, pixel **grid, int x, int y, int size, int factor, int32_t *n);
pixel *average(pixel **grid, int x, int y, int size);
//intoarce numarul de frunze
void chainsaw(node **root, QuadtreeNode *v, int depth, int size, int *n, int m, uint32_t *leafs);
void free_grid(pixel ** grid, int x);
unsigned char absc(unsigned char a, unsigned char b);
void decomp(node **root, QuadtreeNode *v, int index);
void decomp_to_grid(node **root, pixel **grid, int x, int y, int size);
void print_grid_to_file(pixel ** grid, int x, int y, FILE **fout);
void h(node **root);
void v(node **root);
void swap(node **n1, node **n2);