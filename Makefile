build:
	gcc -o quadtree quadtree.c compresie.c decompresie.c -g -Wall -lm -std=gnu11
clean:
	rm quadtree
