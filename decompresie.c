#include <stdio.h>
#include <stdlib.h>
#include "compresie.h"

void swap(node **n1, node **n2)
{
    node *aux = *n1;
    *n1 = *n2;
    *n2 = aux;
}

//functie recursiva care primeste vector de quadtreenode si face
//arborele
void decomp(node **root, QuadtreeNode *v, int index)
{  
    pixel pi;
    pi.red = v[index].red;
    pi.blue = v[index].blue;
    pi.green = v[index].green;
    *root = get_new_node(pi);
    //acum merg recursiv pe ramuri
    if(v[index].bottom_left == -1) return;

    decomp(&((*root)->top_left), v, v[index].top_left);
    decomp(&((*root)->top_right), v, v[index].top_right);
    decomp(&((*root)->bottom_right), v, v[index].bottom_right);
    decomp(&((*root)->bottom_left), v, v[index].bottom_left);
}

void decomp_to_grid(node **root, pixel **grid, int x, int y, int size)
{
    if(*root == NULL) return;
    //verific daca nodul este frunza
    if((*root)->bottom_left == NULL) {
        for(int i = x; i < x + size; i++)
            for(int j = y; j < y + size; j++) {
                grid[i][j].red = (*root)->data.red; 
                grid[i][j].green = (*root)->data.green; 
                grid[i][j].blue = (*root)->data.blue; 
            }
        free(*root);
        return;
    }

    //in cazul in care nodul curent nu este frunza, nu pot completa nimic in grid
    //deci merg recursiv pana la frunze

    decomp_to_grid(&((*root)->top_left), grid, x, y, size/2);
    decomp_to_grid(&((*root)->top_right), grid, x, y + size/2, size/2);
    decomp_to_grid(&((*root)->bottom_right), grid, x + size/2, y + size/2, size/2);
    decomp_to_grid(&((*root)->bottom_left), grid, x + size/2, y, size/2);

    free(*root);

}

void print_grid_to_file(pixel ** grid, int x, int y, FILE **fout)
{
    fprintf(*fout, "P6\n%d %d\n255\n", x, y);
    for(int i = 0; i < x; i++)
        for(int j = 0; j < y; j++)
            fwrite(&(grid[i][j]), 3, 1, *fout);
}

//o parcurgere normala ca pana acum, dar dupa ce merg pe ramuri fac swapuri
void h(node **root)
{
    //nu continui daca e null
    if((*root)->bottom_left == NULL) {
        return;
    }

    h(&(*root)->top_left);
    h(&(*root)->top_right);
    h(&(*root)->bottom_right);
    h(&(*root)->bottom_left);
    //incep swapurile abia dupa parcurgere
    swap(&((*root)->top_left), &((*root)->top_right));
    swap(&((*root)->bottom_left), &((*root)->bottom_right));
}

void v(node **root)
{
    if((*root)->bottom_left == NULL) {
        return;
    }

    v(&(*root)->top_left);
    v(&(*root)->top_right);
    v(&(*root)->bottom_right);
    v(&(*root)->bottom_left);

    swap(&((*root)->top_left), &((*root)->bottom_left));
    swap(&((*root)->top_right), &((*root)->bottom_right));
    //swap(&((*root)->top_left), &((*root)->bottom_left));
}