#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "compresie.h"

//dau free dupa fiecare strdup
//prima data iau primul argument, si fac cazurile
int main(int argc, char *argv[]) 
{
    char *type = strdup(argv[1]);

    //cazul de compresie
    if(!strcmp(type, "-c")) {
        int factor = atoi(argv[2]);
        char *input = strdup(argv[3]);
        char *output = strdup(argv[4]);

        //extrag din binar datele necesare
        FILE  *fin = fopen(input, "rb");
        int width, height;
        int max;
        get_info(fin, &width, &height, &max);
        
        //grid-ul de pixeli cu care voi lucra
        //il construiesc conform functiei make_grid
        pixel ** grid = get_grid(width, height);
        make_grid(&fin, width, height, grid);
        fclose(fin);

        node *quadtree;
        int32_t noduri = 0;
        
        //construiesc arborele pe baza factorului si al gridului dat
        //formulele se gasesc in cerinta
        get_tree(&quadtree, grid, 0, 0, width, factor, &noduri);
        
        //dupa ce am construit arborele, il voi baga intr-un vector asemenea cerintei
        //nu stiu numarul de frunze, dar voi afla numarul lor cand voi parcurge arborele

        //alocarea pentru vectorul de quadtreenode
        QuadtreeNode *v = calloc(noduri, sizeof(QuadtreeNode));
        uint32_t leafs = 0;
        int elemente_vector = 0;
        chainsaw(&quadtree, v, 0, width * height, &elemente_vector, 0, &leafs);

        //scriu datele in fisierul de output(pastrez ordinea data in enunt)
        FILE *fout = fopen(output, "wb");
        fwrite(&leafs, 4, 1, fout);
        fwrite(&noduri, 4, 1, fout);
        for(int i = 0; i < noduri; i++) {
            fwrite(&(v[i].blue), 1, 1, fout);
            fwrite(&(v[i].green), 1, 1, fout);
            fwrite(&(v[i].red), 1, 1, fout);
            fwrite(&(v[i].area), sizeof(uint32_t), 1, fout);
            fwrite(&(v[i].top_left), sizeof(int32_t), 1, fout);
            fwrite(&(v[i].top_right), sizeof(int32_t), 1, fout);
            fwrite(&(v[i].bottom_left), sizeof(int32_t), 1, fout);
            fwrite(&(v[i].bottom_right), sizeof(int32_t), 1, fout);
        }

        fclose(fout);
        free(v);
        free(input);
        free(output);
        free_grid(grid, width);
    }


    //cazul de decompresie
    if(!strcmp(type, "-d")) {
        char *input = strdup(argv[2]);
        char *output = strdup(argv[3]);

        FILE *fin = fopen(input, "rb");
        FILE *fout = fopen(output, "wb");
        int noduri;
        int leafs;
        fread(&leafs, 4, 1, fin);
        fread(&noduri, 4, 1, fin);

        //asemenea compresiei, citesc datele in ordinea data(acolo le scriu)
        QuadtreeNode *v = malloc(sizeof(QuadtreeNode) * noduri);
        for(int i = 0; i < noduri; i++) {
            fread(&(v[i].blue), 1, 1, fin);
            fread(&(v[i].green), 1, 1, fin);
            fread(&(v[i].red), 1, 1, fin);
            fread(&(v[i].area), sizeof(uint32_t), 1, fin);
            fread(&(v[i].top_left), sizeof(int32_t), 1, fin);
            fread(&(v[i].top_right), sizeof(int32_t), 1, fin);
            fread(&(v[i].bottom_left), sizeof(int32_t), 1, fin);
            fread(&(v[i].bottom_right), sizeof(int32_t), 1, fin);
        }

        //functia de decomp ce face arbore din vector
        node *quadtree;
        decomp(&quadtree, v, 0);

        //ma folosesc de faptul ca size = width = height
        //astfel, transform arborele in grid cu funtia decomp_to_grid
        //printez in fisier grid-ul obtinut
        //dealoc memoria la final, ca la primul subpunct
        pixel **grid;
        int size = sqrt(v[0].area);
        grid = get_grid(size, size);
        decomp_to_grid(&quadtree, grid, 0, 0, size);
        print_grid_to_file(grid, size, size, &fout);

        free(input);
        free(output);
        free_grid(grid, size);
        free(v);
        fclose(fin);
        fclose(fout);
    }
    
    //cazul de mirror
    
    //cel mai usor de explicitat caz
    //iau imaginea, o fac arbore, apoi cu functiile recursive
    //h si v inversez nodurile corespunzator(swapurile sunt in fisierul decompresie.c)
    if(!strcmp(type, "-m")) {
        char *mirror = strdup(argv[2]);
        int factor = atoi(argv[3]);
        char *input = strdup(argv[4]);
        char *output = strdup(argv[5]);

        FILE *fin = fopen(input, "rb");
        FILE *fout = fopen(output, "wb");

        int width, height;
        int max;
        get_info(fin, &width, &height, &max);

        pixel ** grid = get_grid(width, height);
        make_grid(&fin, width, height, grid);
        fclose(fin);

        node *quadtree;
        int32_t noduri = 0;
        get_tree(&quadtree, grid, 0, 0, width, factor, &noduri);
        if(!strcmp(mirror, "h")) {
            h(&quadtree);
        }
        else v(&quadtree);

        decomp_to_grid(&quadtree, grid, 0, 0, width);
        //inainte de chainsaw am nevoie de o prelucrare a arborelui

        print_grid_to_file(grid, width, height, &fout);


        fclose(fout);
        free(mirror);
        free(input);
        free(output);
        free_grid(grid, width);
        //free(v);
    }
    free(type);
}