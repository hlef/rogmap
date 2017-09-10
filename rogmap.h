#ifndef ROGMAP_H

#define ROGMAP_H
#define CHAR_ROOM '#'
#define CHAR_EMPTY '.'

#define ARR_SIZE(arr) (sizeof((arr)) / sizeof((arr[0])) )

// Simulate map[x][y] for a 1-dimensional array
#define ACCESS_XY_IN_ARRAY(map, x, y) map->elements[y*map->width + x]

#define MIN(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

typedef struct {
  int x, y;
} coordinate;

typedef struct {
  int width, height;
  char* elements;
} map_t;

typedef struct {
  int size;
  coordinate* coordinates;
} listing_t;

int fill_map(map_t* map_buffer, float min_filling, float max_room_size);

int insert_room(map_t* map, listing_t* room);

int is_suitable_initial_point(map_t* map, coordinate initial_point, int dir_right, int dir_up);

int randrange(int max, int min);

void (*get_room_generator()) (map_t*, listing_t*, listing_t*, float);

void generate_rectangular_room(map_t* map, listing_t* available_space, listing_t* room_buffer, float max_room_size_factor);

void generate_elliptic_room(map_t* map, listing_t* available_space, listing_t* room_buffer, float max_room_size_factor);

#endif
