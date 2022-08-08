#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "compresie.h"

unsigned char absc(unsigned char a, unsigned char b)
{
    if(a > b)
        return a-b;
    return b-a;
}

pixel ** get_grid(int x, int y)
{
    pixel ** grid = malloc(sizeof(QuadtreeNode *) * x);
    for(int i = 0; i < x; i++) {
        grid[i] = malloc(sizeof(QuadtreeNode) * y);
    }
    return grid;
}

//prelucrez primele 3 linii
void get_info(FILE *fin, int *x, int *y, int *max)
{
    char buffer[256];
    fgets(buffer, 256, fin);
    fgets(buffer, 256, fin);
    char *p = strtok(buffer, " ");
    *x = atoi(p);
    p = strtok(NULL, " ");
    *y = atoi(p);
    fgets(buffer, 256, fin);
    *max = atoi(buffer);
}

//iau din fin pixelii si ii bag in grid
void make_grid(FILE **fin, int x, int y, pixel **grid)
{
    for(int i = 0; i < x; i++) {
        for(int j = 0; j < y; j++) {
            //citesc 3 bytes deodata
            fread(* (grid + i) + j, 3, 1, *fin);
        }
    }
}

//returneaza un nod nou alocat pentru arbore
node *get_new_node(pixel pi)
{
    node *ptr = malloc(sizeof(node));
    ptr->data = pi;
    ptr->bottom_left = NULL;
    ptr->bottom_right = NULL;
    ptr->top_left = NULL;
    ptr->top_right = NULL;

    return ptr;
}

//face media pe un grid dat, cu formula data in enunt
uint64_t mean(pixel **grid, int x, int y, int size)
{
    uint64_t red = 0, blue = 0, green = 0;
    for(int i = x; i < x + size; i++) {
        for(int j = y; j < y + size; j++) {
            red += (* (* (grid + i) + j)).red;
            blue += (* (* (grid + i) + j)).blue;
            green += (* (* (grid + i) + j)).green;
        }
    }

    red = (red / (size * size));
    blue = (blue / (size * size));
    green = (green / (size * size));

    uint64_t mean = 0;
    for(int i = x; i < x + size; i++) {
        for(int j = y; j < y + size; j++) {
            mean += pow(absc(red, (* (* (grid + i) + j)).red), 2);
            mean += pow(absc(blue, (* (* (grid + i) + j)).blue), 2);
            mean += pow(absc(green, (* (* (grid + i) + j)).green), 2);
        }
    }
    //printf("%ld %ld %ld", red, blue, green);
    mean = mean / (3 * size * size);
    return mean;
}

//returneaza pointer la un pixel cu media pentru grid
pixel *average(pixel **grid, int x, int y, int size)
{
    pixel *pi = malloc(sizeof(pixel));
    pi->blue = 0;
    pi->red = 0;
    pi->green = 0;

    uint64_t r = 0;
    uint64_t g = 0;
    uint64_t b = 0;

    for(int i = x; i < x + size; i++) {
        for(int j = y; j < y + size; j++) {
            r += (* (* (grid + i) + j)).red;
            g += (* (* (grid + i) + j)).blue;
            b += (* (* (grid + i) + j)).green;
        }
    }

    pi->red = r / (size * size);
    pi->blue = g / (size * size);
    pi->green = b / (size * size);

    return pi;
}

//face arbore dintr-un grid dat
//arborele se extinde asemenea definitiilor din enunt
//merg pe un grid, iar daca media este peste factor, continui
//diviziunea
void get_tree(node **root, pixel **grid, int x, int y, int size, int factor, int32_t *n)
{
    if(!size) return;

    //determin daca nodul este o frunza
    uint64_t m = mean(grid, x, y, size);

    //bag informatia in campul de date
    pixel *pi_avg = average(grid, x, y, size);
    (*root) = get_new_node(*pi_avg);
    free(pi_avg);
    *n = *n + 1;
    
    if(m <= factor) {
        return;
    }

    //daca nodul nu este frunza trebuie sa parcurg in continuare grid-ul
    //ma duc recursiv pe cele 4 patrate asemenea cerintei

    get_tree(&((*root)->top_left), grid, x, y, size/2, factor, n);
    get_tree(&((*root)->top_right), grid, x, y + size/2, size/2, factor, n);
    get_tree(&((*root)->bottom_right), grid, x + size/2, y + size/2, size/2, factor, n);
    get_tree(&((*root)->bottom_left), grid, x + size/2, y, size/2, factor, n);
}

//pornesc din varf si ma duc recursiv pe restul, iar apoi dealoc memoria alocata initial
//astfel, construiesc v
//n reprezinta indicele din vector la care s-a ajuns, iar m e cel curent
//trebuie mereu sa stiu ultimul element din vector(indicele lui)

//o abordare asemanatoare se obtine atunci cand construiesc arborele cu copiii
//4i, 4i+1, 4i+2 si 4i+3(e mai usor de urmarit implementarea)

//in fond, este aceeasi idee, arborele se construieste corect
void chainsaw(node **root, QuadtreeNode *v, int depth, int size, int *n, int m, uint32_t *leafs)
{
    v[m].red = (*root)->data.red;
    v[m].green = (*root)->data.green;
    v[m].blue = (*root)->data.blue;
    v[m].area = size / pow(2, 2 * depth);

    //verific daca e frunza
    if((*root)->bottom_left == NULL) {
        v[m].bottom_left = -1;
        v[m].bottom_right = -1;
        v[m].top_left = -1;
        v[m].top_right = -1;
        *leafs = *leafs + 1;
    }
    else {
        v[m].bottom_left = *n + 4;
        v[m].bottom_right = *n + 3;
        v[m].top_left = *n + 1;
        v[m].top_right = *n + 2;
        m = *n;
        *n = *n + 4;
        chainsaw(&((*root)->top_left), v, depth + 1, size, n, m + 1, leafs);
        chainsaw(&((*root)->top_right), v, depth + 1, size, n, m + 2, leafs);
        chainsaw(&((*root)->bottom_right), v, depth + 1, size, n, m + 3, leafs);
        chainsaw(&((*root)->bottom_left), v, depth + 1, size, n, m + 4, leafs);
    }


    free(*root);
}

void free_grid(pixel ** grid, int x)
{
    for(int i = 0; i < x; i++) {
        free(grid[i]);
    }
    free(grid);
}