#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "rogmap.h"

#define ARR_SIZE(arr) (sizeof((arr)) / sizeof((arr[0])) )

// Simulate map[x][y] for a 1-dimensional array
#define ACCESS_XY_IN_ARRAY(map, x, y) map->elements[y*map->width + x]

#define MIN(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

int main() {
    srand((unsigned) time(NULL));

    map_t *map = malloc(sizeof(map_t));
    *map = (map_t) { .elements = malloc(50*100*sizeof(char)), .height=50, .width=100 };

    fill_map(map, 10);

    // Display map
    for ( int i = 0; i < 100*50; i++ ) {
        printf("%c", map->elements[i]);
        if ( (i+1) % 100 == 0 ) {
            printf("\n");
        }
    }

    free(map->elements);
    free(map);
    return 1;
}

int fill_map(map_t* map, int density) {
    // Allocate memory
    int map_size = map->width * map->height;

    listing_t* selectable_space = malloc(sizeof(listing_t));

    if ( !selectable_space ) {
        return -1;
    }

    *selectable_space = (listing_t) { .coordinates = malloc(map_size * sizeof(coordinate)),
                                      .size = map_size };

    if ( !selectable_space->coordinates ) {
        free(selectable_space);
        return -1;
    }

    listing_t* room_buffer = malloc(sizeof(listing_t));

    if ( !room_buffer ) {
        free(selectable_space->coordinates);
        free(selectable_space);
        return -1;
    }

    *room_buffer = (listing_t) { .coordinates = calloc(map_size, sizeof(coordinate)),
                                 .size = map_size };

    if ( !room_buffer->coordinates ) {
        free(selectable_space->coordinates);
        free(selectable_space);
        free(room_buffer);
        return -1;
    }

    // Initialize selectable_space
    for ( int i = 0, y = 0; y < map->height; y++ ) {
        for ( int x = 0; x < map->width; x++ ) {
            if( x < map->width && y < map->height ) {
                selectable_space->coordinates[i] = (coordinate) { .x = x, .y = y };
                i++;
            }
        }
    }

    // Initialize map
    memset(map->elements, CHAR_EMPTY, map_size * sizeof(char));

    // Generation loop
    int i = 0;
    do {
        void (*room_generator)(map_t*, listing_t*, listing_t*) = get_room_generator();
        room_generator(map, selectable_space, room_buffer);
        insert_room(map, room_buffer);
        memcpy(selectable_space->coordinates, room_buffer->coordinates, room_buffer->size * sizeof(coordinate));
        selectable_space->size = room_buffer->size;
        i++;
    } while (i < density);

    free(selectable_space->coordinates);
    free(selectable_space);
    free(room_buffer->coordinates);
    free(room_buffer);
    return 0;
}

void insert_room(map_t* map, listing_t* room) {
    for ( int i = 0; i < room->size; i++ ) {
        ACCESS_XY_IN_ARRAY(map, room->coordinates[i].x, room->coordinates[i].y) = CHAR_ROOM;
    }
}

int is_suitable_initial_point(map_t* map, coordinate initial_point, int dir_right, int dir_up) {
    int valid_height;

    if (dir_up) {
        valid_height = initial_point.y > 2;
    } else {
        valid_height = map->height - initial_point.y > 2;
    }

    if (dir_right) {
        return map->width - initial_point.x > 2 && valid_height;
    } else {
        return initial_point.x > 2 && valid_height;
    }
}

/* Return pseudo random number in range [min, max).
   Assumes min < max <= RAND_MAX */
int randrange(int max, int min) {
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

/* Generate a rectangular room in passed map, with at least one point in passed
   selectable_space array. Write the coordinates of the generated room's points
   to passed room_buffer */
void generate_rectangular_room(map_t* map, listing_t* selectable_space, listing_t* room_buffer) {
    coordinate initial_point;

    // 1 for right, 0 for left
    int dir_right = randrange(2, 0);

    // 1 for up, 0 for down
    int dir_up = randrange(2, 0);

    do {
        initial_point = selectable_space->coordinates[rand() % selectable_space->size];
    } while ( !is_suitable_initial_point(map, initial_point, dir_right, dir_up) );

    int width, height;

    if(dir_up) {
        height = randrange(MIN(map->height * MAX_ROOM_SIZE_FACTOR, initial_point.y), 2);
    } else {
        height = randrange(MIN(map->height * MAX_ROOM_SIZE_FACTOR, map->height - initial_point.y), 2);
    }

    if(dir_right) {
        width = randrange(MIN(map->width * MAX_ROOM_SIZE_FACTOR, map->width - initial_point.x), 2);
    } else {
        width = randrange(MIN(map->width * MAX_ROOM_SIZE_FACTOR, initial_point.x), 2);
    }

    int x, y = initial_point.y, i = 0;
    for ( int h = 0; h < height; h++ ) {
        x = initial_point.x;

        for ( int w = 0; w < width; w++ ) {
            room_buffer->coordinates[i] = (coordinate) { .x = x, .y = y };
            dir_right ? x++ : x--;
            i++;
        }

        dir_up ? y-- : y++;
    }

    room_buffer->size = i;
}

void generate_elliptic_room(map_t* map, listing_t* selectable_space, listing_t* room_buffer){
    generate_rectangular_room(map, selectable_space, room_buffer);
    // TODO
}

/* Return a randomly-chosen element from the rooms_generators array */
void ( *get_room_generator() ) (map_t*, listing_t*, listing_t*) {
    return rooms_generators[randrange(ARR_SIZE(rooms_generators), 0)];
}
