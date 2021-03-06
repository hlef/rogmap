#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "rogmap.h"

int main() {
    srand((unsigned) time(NULL));

    int width = 80, height = 40;

    map_t *map = malloc(sizeof(map_t));
    *map = (map_t) { .elements = malloc(height*width*sizeof(char)), .height=height, .width=width };

    if (fill_map(map, 0.4f, 0.05f)) {
        printf("Map generation failed with non zero error code\n");
        return 1;
    }

    // Display map
    for ( int i = 0; i < height*width; i++ ) {
        printf("%c", map->elements[i]);
        if ( (i+1) % width == 0 ) {
            printf("\n");
        }
    }

    free(map->elements);
    free(map);
    return 0;
}
