#ifndef __H_TETRIS__
#define __H_TETRIS__

#include <stdint.h>
#include <pthread.h>
#include <wchar.h>
#include <time.h>

#define MAP_WIDTH 10
#define MAP_HEIGHT 23

#define BLOCK 'X'
#define EMPTY ' '
#define TICKRATE 50000
#define GAME_SPEED 800000
#define MOVE_PLACE_TIMER 0.25
#define STR_LEN 20

enum {
  NORM_COL,
  RED_COL,
  GREEN_COL,
  YELLOW_COL,
  BLUE_COL,
  MAGENTA_COL,
  CYAN_COL
};

enum {
  SQUARE_PIECE,
  T_PIECE,
  L_L_PIECE,
  L_R_PIECE,
  Z_PIECE,
  S_PIECE,
  LINE_PIECE,
  NOTHING
};

//Starts from 10 as to prevent collision with rot enum
enum {
  RIGHT = 10,
  LEFT,
  DOWN,
  UP
};

enum {
  ROT_ACW,
  ROT_CW,
  HOLD,
  COMMIT
};

enum {
  ROT_90 = 20,
  ROT_180,
  ROT_270,
  ROT_0,
};

//Changing terminal to raw no echo and 
//  resetting to default once done
extern struct termios old_sets, new_sets;

//Strings containing escape codes for each
//  colour
extern char *NORM;
extern char *RED;
extern char *GREEN;
extern char *YELLOW;
extern char *BLUE;
extern char *MAGENTA;
extern char *CYAN;


typedef struct piece {
  //Piece has 4 parts, each occupying some xy tile
  int32_t positions[4][2];
  int32_t last_positions[4][2];
  //Square, line, etc.
  uint8_t type;
  uint8_t next_type;
  //Used to tell whether player is using held piece
  uint8_t hold_flag;
  //Piece player has in hold (not current piece)
  uint8_t hold;
  //cur_dir is for input validity checking
  int8_t cur_dir;
  int orientation;
  int last_orientation;
  int rot_flag;
  int lines_cleared;
  char *colour;
  int lose;
  float difficulty;
  struct timespec timer, comp_timer;
} piece;

//Map tile can either be block or empty
//Colour is string with escape code for colour
typedef struct map_t {
  char map_tile;
  //For non-self colliding rotation checking
  char map_tile_tmp_trns;
  char map_tile_tmp_rot;
  char *colour;
} map_t;

//For passing game data to thread input function as single argument
typedef struct _args {
  map_t **map;
  piece *c_piece;
  pthread_mutex_t *lock;
} _args;


//input.c
  //For asynch input
  void *input(void* t_args);

//logic.c
  //General game logic
  void logic(map_t **game_map, piece *c_piece, pthread_mutex_t *lock);
  //Moves piece
  int transpose(piece *c_piece, map_t **game_map);
  //Commits piece to map and loads a new one
  void place_piece(piece *c_piece, map_t **game_map);
  //Forcing piece down as far as possible
  void quick_drop(map_t **game_map, piece *c_piece);
  //Parses lines and calls clear_line if a line is eligible for clearing
  void check_line_clear(map_t **game_map, piece *c_piece);
  void clear_line(map_t **game_map, int line);
  void new_piece(map_t **game_map, piece *c_piece);

  void hold_piece(map_t **game_map, piece *c_piece);

  //Non-collision for transposing
  void load_map_tmp_trns(map_t **game_map, piece *c_piece);
  void unload_map_tmp_trns(map_t **game_map, piece *c_piece);

  //Non-collision for rotation
  void load_map_tmp_rot(map_t **game_map, piece *c_piece);
  void unload_map_tmp_rot(map_t **game_map, piece *c_piece);

  void update_map(map_t **game_map, piece *c_piece);

  void increase_difficulty(piece *c_piece);

  float time_diff(struct timespec start, struct timespec end);

//rotate.c
  //Logic relating to rotation
  void rotate(map_t **game_map, piece *c_piece);
  void invalid_rotation(map_t **game_map, piece *c_piece);

//shadow.c
  //Logic for shadow position
  //Commit to map
  //Shadow will be drawn before piece, since piece should draw on top
  //  of shadow
  void draw_shadow(map_t **game_map, piece *c_piece);
  void place_shadow(map_t **game_map, piece* c_piece);

//render.c
  //Draws contents of game_t struct
  void render(map_t **game_map, piece c_piece);

//quit_utils.c
  //Handling exits gracefully
  void clean(void);
  void sighandler(int sig);
  void quithandler(int sig);

#endif
