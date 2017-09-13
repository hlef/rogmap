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

int fill_map(map_t* map_buffer, float min_filling, float max_room_size);

int add_point(map_t* map, coordinate point);

int is_suitable_initial_point(map_t* map, coordinate initial_point);

int randrange(int max, int min);

int (*get_room_generator()) (map_t*, coordinate*, float);

int generate_rectangular_room(map_t* map, coordinate* point, float max_room_size_factor);

int generate_elliptic_room(map_t* map, coordinate* point, float max_room_size_factor);

void compute_room_dimensions(map_t* map, coordinate* point, int *height, int *width, float max_room_size_factor);

#endif
