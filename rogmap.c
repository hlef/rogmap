#include "rogmap.h"

#include <stdlib.h>
#include <string.h>

// Available room generators
void (*rooms_generators[]) (map_t*, listing_t*, float) = { generate_rectangular_room, generate_elliptic_room };

int fill_map(map_t* map, float min_filling, float max_room_size) {
    if (min_filling >= 1 ||
            min_filling <= 0 ||
            max_room_size > 1 ||
            (int)(max_room_size * map->height) < 2 ||
            (int)(max_room_size * map->width) < 2) {
        return -1;
    }

    // Allocate memory
    int map_size = map->width * map->height;
    int buffer_size = map_size * max_room_size;

    listing_t* generation_buffer = (listing_t*) malloc(sizeof(listing_t));

    if ( !generation_buffer ) {
        return -1;
    }

    *generation_buffer = (listing_t) { .size = 1, .coordinates = (coordinate*) malloc(buffer_size * sizeof(coordinate)) };

    if ( !generation_buffer->coordinates ) {
        free(generation_buffer);
        return -1;
    }

    // Initialize generation_buffer a coordinate in the map, at random.
    generation_buffer->coordinates[0] = (coordinate) { .x = randrange(map->width - 1, 1), .y = randrange(map->height - 1, 1)};

    // Initialize map
    memset(map->elements, CHAR_EMPTY, map_size * sizeof(char));

    // Generation loop
    int i = 0;
    do {
        get_room_generator()(map, generation_buffer, max_room_size);
        i += insert_room(map, generation_buffer);
    } while (i < map_size * min_filling);

    free(generation_buffer->coordinates);
    free(generation_buffer);
    return 0;
}

int insert_room(map_t* map, listing_t* room) {
    int inserted = 0;
    for ( int i = 0; i < room->size; i++ ) {
        if (ACCESS_XY_IN_ARRAY(map, room->coordinates[i].x, room->coordinates[i].y) != CHAR_ROOM) {
            ACCESS_XY_IN_ARRAY(map, room->coordinates[i].x, room->coordinates[i].y) = CHAR_ROOM;
            inserted++;
        }
    }
    return inserted;
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
void generate_rectangular_room(map_t* map, listing_t* generation_buffer, float max_room_size_factor) {
    coordinate initial_point;

    do {
        initial_point = generation_buffer->coordinates[rand() % generation_buffer->size];
    } while ( !is_suitable_initial_point(map, initial_point) );

    int max_height = MIN(map->height - initial_point.y, initial_point.y);
    max_height = MIN(max_height, map->height * max_room_size_factor * 0.5f);

    int max_width = MIN(map->width - initial_point.x, initial_point.x);
    max_width = MIN(max_width, map->width * max_room_size_factor * 0.5f);

    int height = randrange(max_height, 1);
    int width = randrange(max_width, 1);

    memset(generation_buffer->coordinates, 0, generation_buffer->size * sizeof(coordinate));

    int i = 0;
    for ( int y = -height; y <= height; y++ ) {
        for ( int x = -width; x <= width; x++ ) {
            generation_buffer->coordinates[i] = (coordinate) { .x = initial_point.x + x, .y = initial_point.y + y };
            i++;
        }
    }

    generation_buffer->size = i;
}

/* Generate a elliptic room in passed map with at least one point in passed
   generation_buffer array. Write the coordinates constituing the generated
   room in generation_buffer. */
void generate_elliptic_room(map_t* map, listing_t* generation_buffer, float max_room_size_factor) {
    coordinate initial_point;

    do {
        initial_point = generation_buffer->coordinates[rand() % generation_buffer->size];
    } while ( !is_suitable_initial_point(map, initial_point) );

    int max_height = MIN(map->height - initial_point.y, initial_point.y);
    max_height = MIN(max_height, map->height * max_room_size_factor * 0.5f);

    int max_width = MIN(map->width - initial_point.x, initial_point.x);
    max_width = MIN(max_width, map->width * max_room_size_factor * 0.5f);

    int height = randrange(max_height, 1);
    int width = randrange(max_width, 1);

    int squarew = width*width;
    int squareh = height*height;

    memset(generation_buffer->coordinates, 0, generation_buffer->size * sizeof(coordinate));

    int i = 0;
    for ( int y = -height; y <= height; y++ ) {
        for ( int x = -width; x <= width; x++ ) {
            if ( (x * x * squareh + (y * y) * squarew) <= (squareh * squarew) ) {
                generation_buffer->coordinates[i] = (coordinate) { .x = initial_point.x + x, .y = initial_point.y + y };
                i++;
            }
        }
    }

    generation_buffer->size = i;
}

/* Return a randomly-chosen element from the rooms_generators array */
void ( *get_room_generator() ) (map_t*, listing_t*, float) {
    return rooms_generators[randrange(ARR_SIZE(rooms_generators), 0)];
}
