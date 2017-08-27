#ifndef GENCONSTANTS
// We assume that MAX_ROOM_SIZE_FACTOR <= 1
#define MAX_ROOM_SIZE_FACTOR 1/2
#endif

#ifndef MAPCHARACTERS
#define CHAR_ROOM '#'
#define CHAR_EMPTY '.'
#endif

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

typedef enum {RANDOM, SMALLROOMS, BIGROOMS, BOSS} map_type_t;

int fill_map(map_t* map_buffer, map_type_t map_type);

void insert_room(map_t* map, listing_t* room);

int is_suitable_initial_point(map_t* map, coordinate initial_point, int dir_right, int dir_up);

int randrange(int max, int min);

void (*get_room_generator()) (map_t*, listing_t*, listing_t*);

void generate_rectangular_room(map_t* map, listing_t* available_space, listing_t* room_buffer);

void generate_elliptic_room(map_t* map, listing_t* available_space, listing_t* room_buffer);

// Available room generators
void (*rooms_generators[]) (map_t*, listing_t*, listing_t*) = { generate_rectangular_room, generate_elliptic_room};
