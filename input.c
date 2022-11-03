#include <stdio.h>
#include "tetris.h"

void *input(void* t_args) {
  //Unpacking generic thread argument into seperate structs
  _args *thread_args = (_args*)t_args;
  piece *c_piece = thread_args->c_piece;
  map_t **game_map = thread_args->map;
  pthread_mutex_t *lock = thread_args->lock;

  while (1) {
    c_piece->cur_dir = getchar();
    switch (c_piece->cur_dir) {
      case 'a' :
        c_piece->cur_dir = LEFT;
        break;
      case 'd' :
        c_piece->cur_dir = RIGHT;
        break;
      case 's' :
        c_piece->cur_dir = DOWN;
        break;
      case 'q' :
        c_piece->cur_dir = ROT_ACW;
        break;
      case 'w' :
        c_piece->cur_dir = ROT_CW;
        break;
      case 'e' :
        c_piece->cur_dir = HOLD;
        break;
      case ' ' :
        c_piece->cur_dir = COMMIT;
        break;
      default :
        continue;
    }
    logic(game_map, c_piece, lock);
    render(game_map, *c_piece);
  }
}
