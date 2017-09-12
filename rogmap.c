#include "rogmap.h"

#include <stdlib.h>
#include <string.h>

// Available room generators
int (*rooms_generators[]) (map_t*, coordinate*, float) = { generate_rectangular_room, generate_elliptic_room };

int fill_map(map_t* map, float min_filling, float max_room_size) {
    if (min_filling >= 1 ||
            min_filling <= 0 ||
            max_room_size > 1 ||
            (int)(max_room_size * map->height) < 2 ||
            (int)(max_room_size * map->width) < 2) {
        return -1;
    }

    int map_size = map->width * map->height;

    // Initialize map
    memset(map->elements, CHAR_EMPTY, map_size * sizeof(char));

    coordinate* point = malloc(sizeof(coordinate));
    *point = (coordinate) { .x = randrange(map->width, 1), .y = randrange(map->height, 1)};

    // Generation loop
    int i = 0;
    do {
        i += get_room_generator()(map, point, max_room_size);
    } while (i < map_size * min_filling);

    free(point);

    return 0;
}

int add_point(map_t* map, coordinate point) {
    if (ACCESS_XY_IN_ARRAY(map, point.x, point.y) != CHAR_ROOM) {
        ACCESS_XY_IN_ARRAY(map, point.x, point.y) = CHAR_ROOM;
        return 1;
    }
    return 0;
}

int is_suitable_initial_point(map_t* map, coordinate initial_point) {
    if((initial_point.y < 1) || ((map->height - initial_point.y) <= 1) ||
       ((map->width - initial_point.x) <= 1) || (initial_point.x < 1)) {
        return 0;
    }
    return 1;
}

/* Return pseudo random number in range [min, max). If min == max, return max.
   Assumes min <= max <= RAND_MAX */
int randrange(int max, int min) {
    if (max == min) {
        return max;
    }

    max -= (min + 1);

    unsigned int num_bins = (unsigned int) max + 1,
                 num_rand = (unsigned int) RAND_MAX + 1,
                 bin_size = num_rand / num_bins,
                 defect   = num_rand % num_bins, x;

    do {
        x = rand();
    } while ( num_rand - defect <= (unsigned int) x );

    return (x / bin_size) + min;
}

/* Generate a rectangular room in passed map with at least one point in passed
   generation_buffer array. Write the coordinates constituing the generated
   room in generation_buffer. */
int generate_rectangular_room(map_t* map, coordinate* point, float max_room_size_factor) {
    int max_height = MIN(map->height - point->y, point->y);
    max_height = MIN(max_height, map->height * max_room_size_factor * 0.5f);

    int max_width = MIN(map->width - point->x, point->x);
    max_width = MIN(max_width, map->width * max_room_size_factor * 0.5f);

    int height = randrange(max_height, 1);
    int width = randrange(max_width, 1);

    // Update map & select next starting point using reservoir sampling
    coordinate current_replacement;
    int i = 0, len = 1;
    for ( int y = -height; y <= height; y++ ) {
        for ( int x = -width; x <= width; x++ ) {
            coordinate newpnt = (coordinate) { .x = point->x + x, .y = point->y + y };
            i += add_point(map, newpnt);
            if(is_suitable_initial_point(map, newpnt)){
                if((rand() % len) == 0) {
                    current_replacement = newpnt;
                }
                len++;
            }
        }
    }

    *point = current_replacement;

    return i;
}

/* Generate a elliptic room in passed map with at least one point in passed
   generation_buffer array. Write the coordinates constituing the generated
   room in generation_buffer. */
int generate_elliptic_room(map_t* map, coordinate* point, float max_room_size_factor) {
    int max_height = MIN(map->height - point->y, point->y);
    max_height = MIN(max_height, map->height * max_room_size_factor * 0.5f);

    int max_width = MIN(map->width - point->x, point->x);
    max_width = MIN(max_width, map->width * max_room_size_factor * 0.5f);

    int height = randrange(max_height, 1);
    int width = randrange(max_width, 1);

    int squarew = width*width;
    int squareh = height*height;

    // Update map & select next starting point using reservoir sampling
    coordinate current_replacement;
    int i = 0, len = 1;
    for ( int y = -height; y <= height; y++ ) {
        for ( int x = -width; x <= width; x++ ) {
            if ( (x * x * squareh + (y * y) * squarew) <= (squareh * squarew) ) {
                coordinate newpnt = (coordinate) { .x = point->x + x, .y = point->y + y };
                i += add_point(map, newpnt);
                if(is_suitable_initial_point(map, newpnt)){
                    if((rand() % len) == 0) {
                        current_replacement = newpnt;
                    }
                    len++;
                }
            }
        }
    }

    *point = current_replacement;

    return i;
}

/* Return a randomly-chosen element from the rooms_generators array */
int ( *get_room_generator() ) (map_t*, coordinate*, float) {
    return rooms_generators[randrange(ARR_SIZE(rooms_generators), 0)];
}
