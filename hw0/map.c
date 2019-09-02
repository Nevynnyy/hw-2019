#include <stdio.h>
#include <stdlib.h>

int foo;

int recur(int i) {
    int j = i;

    printf("recur call %d: stack@ %lx\n", i, &j);
    if (i > 0) {
        return recur(i-1);
    }
    return 0;
}

int stuff = 7;

int main (int argc, char *argv[]) {
    int i;
    char *buf1 = malloc(100);
    char *buf2 = malloc(100);

    printf("_main  @ %lx\n", &main);
    printf("recur @ %lx\n", &recur);
    printf("_main stack: %lx\n", &i);
    printf("static data: %lx\n", &stuff);
    printf("Heap: malloc 1: %lx\n", (long unsigned int) buf1);
    printf("Heap: malloc 2: %lx\n", (long unsigned int) buf2);
    recur(3);
    return 0;
}
