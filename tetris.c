#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>
#include <locale.h>
#include <wchar.h>
#include "tetris.h"

struct termios old_sets, new_sets;

char *NORM = "\033[1;0m";
char *RED = "\033[1;31m";
char *GREEN = "\033[1;32m";
char *YELLOW = "\033[1;33m";
char *BLUE = "\033[1;34m";
char *MAGENTA = "\033[1;35m";
char *CYAN = "\033[1;36m";

int main(void) {
  //Used for printing block characters
  // TODO : Actually use block characters
  setlocale(LC_ALL, "");

  //Sets terminal to raw no echo for responsive,
  //  non-buffered input
  tcgetattr(STDIN_FILENO, &old_sets);
  new_sets = old_sets;

  new_sets.c_lflag &= (~ICANON & ~ECHO);
  cfsetospeed(&new_sets, B9600);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_sets);

  srand(time(NULL));

  //Double pointer as game map is 2D array of tiles,
  //  each with associated colour data
  map_t **game_map;

  //Allocates memory for map struct
  game_map = (map_t**)malloc(sizeof(map_t*)*MAP_HEIGHT);
  for (int32_t i = 0; i < MAP_HEIGHT; ++i)
    game_map[i] = (map_t*)malloc(sizeof(map_t)*MAP_WIDTH);

  //Initializes array as spaces of no colour
  for (int32_t i = 0; i < MAP_HEIGHT; ++i)
    for (int32_t j = 0; j < MAP_WIDTH; ++j) {
      game_map[i][j].map_tile = ' ';
      game_map[i][j].map_tile_tmp_trns = ' ';
      game_map[i][j].map_tile_tmp_rot = ' ';
      game_map[i][j].colour = NORM;
      }
  printf("%s\n", NORM);

  //Makes sure terminal settings are reverted to
  //  default upon ctrl + 'c' or normal termination
  signal(SIGINT, sighandler);
  atexit(clean);

  //Don't terminate upon ctrl + '\'
  signal(SIGQUIT, quithandler);

  //Initialize piece position with random piece type
  piece c_piece;
  new_piece(game_map, &c_piece);
  c_piece.hold_flag = 0;
  c_piece.lines_cleared = 0;
  c_piece.hold = NOTHING;
  c_piece.next_type = rand()%7;
  c_piece.lose = 0;
  c_piece.difficulty = 0.8;

  //Setting up input thread and mutex
  pthread_mutex_t lock;
  pthread_mutex_init(&lock, NULL);
  pthread_t input_thread;

  _args t_args = {game_map, &c_piece, &lock};

  pthread_create(&input_thread, NULL, input, &t_args);

  clock_gettime(CLOCK_MONOTONIC, &(c_piece.comp_timer));
  while (!(c_piece.lose)) {
    // TODO : Change to account for movements of player
    usleep(c_piece.difficulty*10e5);
    clock_gettime(CLOCK_MONOTONIC, &(c_piece.timer));
    c_piece.cur_dir = DOWN;
    logic(game_map, &c_piece, &lock);
    render(game_map, c_piece);
  }
  pthread_cancel(input_thread);

  int c;
  printf("YOU LOST! PRESS 'X' TO QUIT.\n");
  while ((c = getchar()) != 'x')
    ;

  free(game_map);

  return 0;
}
