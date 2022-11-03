#include <stdlib.h>
#include <time.h>
#include "tetris.h"

void logic(map_t **game_map, piece *c_piece, pthread_mutex_t *lock) {
  pthread_mutex_lock(lock);

  //Without this, shadow overwrites piece positions during quick drop
  int shadow_flag = 0;
  switch (c_piece->cur_dir) {
    case LEFT :
    case RIGHT :
    case DOWN :
      transpose(c_piece, game_map);
      break;
    case ROT_ACW :
    case ROT_CW :
      rotate(game_map, c_piece);
      break;
    case HOLD :
      hold_piece(game_map, c_piece);
      break;
    case COMMIT :
      quick_drop(game_map, c_piece);
      shadow_flag = 1;
      break;
    default :
      break;
  }
  if (!shadow_flag)
  draw_shadow(game_map, c_piece);

  pthread_mutex_unlock(lock);
}

//Returns 1 if transpose was possible, else returns 0
int transpose(piece *c_piece, map_t **game_map) {
  //For ignoring self collision during transpose validity check
  load_map_tmp_trns(game_map, c_piece);

  switch (c_piece->cur_dir) {
    //Three things can happen: piece moves down one tile, piece placed as it collided
    //  with other piece, and piece placed as it collided with floor
    case DOWN :
      //Iterates over all 4 segment positions of piece. If any intersect with bottom,
      //  place piece
      for (int i = 0; i < 4; ++i)
        if (c_piece->positions[i][0]-1 <= 1) {
          if (time_diff(c_piece->comp_timer, c_piece->timer) > MOVE_PLACE_TIMER)
            place_piece(c_piece, game_map);
          else
            goto invalid;
        }

      //Iterates over all 4 segment positions of piece. If any intersect with an 'X',
      //  place piece
      for (int i = 0; i < 4; ++i)
        if (game_map[c_piece->positions[i][0]-1][c_piece->positions[i][1]].map_tile == 'X')
          place_piece(c_piece, game_map);

      for (int i = 0; i < 4; ++i)
        c_piece->last_positions[i][0] = c_piece->positions[i][0];
      for (int i = 0; i < 4; ++i)
        c_piece->last_positions[i][1] = c_piece->positions[i][1];
      //Update positions
      for (int i = 0; i < 4; ++i)
        c_piece->positions[i][0] -= 1;
      //Update map to reflect changes
      update_map(game_map, c_piece);
      break;
  //Do not update if any segment intersects with left wall or other piece
  case LEFT :
      for (int i = 0; i < 4; ++i)
        if ((c_piece->positions[i][1]-1 < 0) ||
            (c_piece->positions[i][1] >= MAP_WIDTH))
          goto invalid;

      for (int i = 0; i < 4; ++i)
        if (game_map[c_piece->positions[i][0]][c_piece->positions[i][1]-1].map_tile == 'X')
          goto invalid;

      for (int i = 0; i < 4; ++i)
        c_piece->last_positions[i][0] = c_piece->positions[i][0];
      for (int i = 0; i < 4; ++i)
        c_piece->last_positions[i][1] = c_piece->positions[i][1];

      for (int i = 0; i < 4; ++i)
        c_piece->positions[i][1] -= 1;

      clock_gettime(CLOCK_MONOTONIC, &(c_piece->comp_timer));
      update_map(game_map, c_piece);
      break;
  case RIGHT :
      for (int i = 0; i < 4; ++i)
        if (c_piece->positions[i][1]+1 >= MAP_WIDTH)
          goto invalid;

      for (int i = 0; i < 4; ++i)
        if (game_map[c_piece->positions[i][0]][c_piece->positions[i][1]+1].map_tile == 'X')
          goto invalid;

      //for (int i = 0; i < 4; ++i)
        //game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = 'X';

      for (int i = 0; i < 4; ++i)
        c_piece->last_positions[i][0] = c_piece->positions[i][0];
      for (int i = 0; i < 4; ++i)
        c_piece->last_positions[i][1] = c_piece->positions[i][1];
      for (int i = 0; i < 4; ++i)
        c_piece->positions[i][1] += 1;

      clock_gettime(CLOCK_MONOTONIC, &(c_piece->comp_timer));
      update_map(game_map, c_piece);
      break;
  }
  if (c_piece->rot_flag)
    unload_map_tmp_trns(game_map, c_piece);
  return 1;
invalid:
  //Revert changes for ignoring self-collision
  if (!(c_piece->rot_flag))
    for (int i = 0; i < 4; ++i)
      game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = 'X';
  else
    unload_map_tmp_trns(game_map, c_piece);
  return 0;
}

void place_piece(piece *c_piece, map_t **game_map) {
    //Revert contents of piece that was temporarily changed during self-collision ignore
    for (int i = 0; i < 4; ++i)
      game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = 'X';

    c_piece->hold_flag = 0;
    //Line clears can only occur the moment a piece has been placed
    new_piece(game_map, c_piece); 
    check_line_clear(game_map, c_piece);
}

//Reuses code from move down transpose
//Repeats downward transpose until collision with either floor or piece
void quick_drop(map_t **game_map, piece *c_piece) {
  while (1) {
    //For ignoring self collision during transpose validity check
    for (int i = 0; i < 4; ++i)
      game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = 'O';
    //If collision with bottom, place piece
    for (int i = 0; i < 4; ++i)
      if (c_piece->positions[i][0]-1 <= 1) {
        place_piece(c_piece, game_map);
        goto placed;
      }

    //If collision with block, place piece
    for (int i = 0; i < 4; ++i)
      if (game_map[c_piece->positions[i][0]-1][c_piece->positions[i][1]].map_tile == 'X') {
        place_piece(c_piece, game_map);
        goto placed;
      }
    //Replace temp characters for non-self collision with original characters
    for (int i = 0; i < 4; ++i)
      game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = 'X';
    //Save previous positions for being overwritten on map later
    for (int i = 0; i < 4; ++i)
      c_piece->last_positions[i][0] = c_piece->positions[i][0];
    for (int i = 0; i < 4; ++i)
      c_piece->last_positions[i][1] = c_piece->positions[i][1];
    //Update positions
    for (int i = 0; i < 4; ++i)
      c_piece->positions[i][0] -= 1;
    //Update map to reflect changes
    update_map(game_map, c_piece);
  }
placed:
}

void check_line_clear(map_t **game_map, piece *c_piece) {
  //j declared outside for as its value will be needed later
  int j;
  for (int i = 0; i < MAP_HEIGHT; ++i) {
    for (j = 0; j < MAP_WIDTH; ++j) {
      if (game_map[i][j].map_tile == 'X')
        continue;
      else
        break;
    }
    //j would only equal this if the entire row was filled with blocks
    if (j == MAP_WIDTH) {
      c_piece->lines_cleared++;
      if (c_piece->lines_cleared%10 == 0 && c_piece->lines_cleared <= 130)
        increase_difficulty(c_piece);
      clear_line(game_map, i);
      //i decremented as rows above move down to take place of previously read line
      --i;
    } 
  }
}

void clear_line(map_t **game_map, int line) {
  for (int i = line; i < MAP_HEIGHT-1; ++i) {
    for (int j = 0; j < MAP_WIDTH; ++j) {
      if (i == MAP_HEIGHT-2)
        game_map[i][j].map_tile = ' ';
      game_map[i][j].map_tile = game_map[i+1][j].map_tile;
    }
  }
}

void new_piece(map_t **game_map, piece *c_piece) {
  static int first_pass = 1;
  //Hold flag is used to control behaviour of new piece to give us the type of the
  //  piece we're holding
  if (c_piece->hold_flag == 1) {
    c_piece->type = c_piece->hold;
  } else {
    if (first_pass) {
      //Initializes next piece for first pass
      //This path is never resolved after this
      first_pass = 0;
      c_piece->next_type = rand()%7;
      c_piece->type = c_piece->next_type;
      c_piece->next_type = rand()%7;
    } else {
      //Normal new piece either after first pass or non-hold call
      c_piece->type = c_piece->next_type;
      c_piece->next_type = rand()%7;
    }
  }
  //Initialize with standard orientation. I think some Tetris games use the
  //  orientation of the placed piece as the orientation of the new piece, but
  //  that is both unfun, and would require more code that wouldn't benefit the
  //  user
  c_piece->orientation = ROT_0;

  //Piece positions are defined relative to origin of piece
  c_piece->positions[0][0] = MAP_HEIGHT-2;
  c_piece->positions[0][1] = MAP_WIDTH/2;
  switch (c_piece->type) {
    case SQUARE_PIECE :
      c_piece->positions[1][0] = c_piece->positions[0][0]-1;
      c_piece->positions[1][1] = c_piece->positions[0][1];

      c_piece->positions[2][0] = c_piece->positions[0][0];
      c_piece->positions[2][1] = c_piece->positions[0][1]+1;

      c_piece->positions[3][0] = c_piece->positions[0][0]-1;
      c_piece->positions[3][1] = c_piece->positions[0][1]+1;
      c_piece->colour = YELLOW;
      break;
    case T_PIECE :
      c_piece->positions[1][0] = c_piece->positions[0][0];
      c_piece->positions[1][1] = c_piece->positions[0][1]+1;

      c_piece->positions[2][0] = c_piece->positions[0][0];
      c_piece->positions[2][1] = c_piece->positions[0][1]+2;

      c_piece->positions[3][0] = c_piece->positions[0][0]-1;
      c_piece->positions[3][1] = c_piece->positions[0][1]+1;
      c_piece->colour = BLUE;
      break;
    case L_R_PIECE :
      c_piece->positions[1][0] = c_piece->positions[0][0]-1;
      c_piece->positions[1][1] = c_piece->positions[0][1];

      c_piece->positions[2][0] = c_piece->positions[0][0]-2;
      c_piece->positions[2][1] = c_piece->positions[0][1];

      c_piece->positions[3][0] = c_piece->positions[0][0]-2;
      c_piece->positions[3][1] = c_piece->positions[0][1]+1;
      c_piece->colour = GREEN;
      break;
    case L_L_PIECE :
      c_piece->positions[1][0] = c_piece->positions[0][0]-1;
      c_piece->positions[1][1] = c_piece->positions[0][1];

      c_piece->positions[2][0] = c_piece->positions[0][0]-2;
      c_piece->positions[2][1] = c_piece->positions[0][1];

      c_piece->positions[3][0] = c_piece->positions[0][0]-2;
      c_piece->positions[3][1] = c_piece->positions[0][1]-1;
      c_piece->colour = CYAN;
      break;
    case Z_PIECE :
      c_piece->positions[1][0] = c_piece->positions[0][0];
      c_piece->positions[1][1] = c_piece->positions[0][1]+1;

      c_piece->positions[2][0] = c_piece->positions[0][0]-1;
      c_piece->positions[2][1] = c_piece->positions[0][1]+1;

      c_piece->positions[3][0] = c_piece->positions[0][0]-1;
      c_piece->positions[3][1] = c_piece->positions[0][1]+2;
      c_piece->colour = RED;
      break;
    case S_PIECE :
      c_piece->positions[1][0] = c_piece->positions[0][0];
      c_piece->positions[1][1] = c_piece->positions[0][1]-1;

      c_piece->positions[2][0] = c_piece->positions[0][0]-1;
      c_piece->positions[2][1] = c_piece->positions[0][1]-1;

      c_piece->positions[3][0] = c_piece->positions[0][0]-1;
      c_piece->positions[3][1] = c_piece->positions[0][1]-2;
      c_piece->colour = MAGENTA;
      break;
    case LINE_PIECE :
      c_piece->positions[1][0] = c_piece->positions[0][0]-1;
      c_piece->positions[1][1] = c_piece->positions[0][1];

      c_piece->positions[2][0] = c_piece->positions[0][0]-2;
      c_piece->positions[2][1] = c_piece->positions[0][1];

      c_piece->positions[3][0] = c_piece->positions[0][0]-3;
      c_piece->positions[3][1] = c_piece->positions[0][1];
      //Not enough colours for each piece to have a unique colour
      c_piece->colour = NORM;
      break;
  }
  //Check if new piece intersects any pre-existing blocks
  //If so, lose
  for (int i = 0; i < 4; ++i) {
    if (game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile == 'X')
      // TODO : Update this to have a lose / retry screen, instead of immediately quitting
      //  out of the program
      c_piece->lose = 1;
  }
}

void hold_piece(map_t **game_map, piece *c_piece) {
  static int is_first = 1;
  if (c_piece->hold_flag == 0) {
    for (int i = 0; i < 4; ++i)
      game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = ' ';
    c_piece->hold_flag = 1;
    if (is_first != 1) {
      int type_tmp = c_piece->type;
      new_piece(game_map, c_piece);
      c_piece->type = c_piece->hold;
      c_piece->hold = type_tmp;
    }
    else {
      is_first = 0;
      c_piece->hold = c_piece->type;
      new_piece(game_map, c_piece);
    }
    update_map(game_map, c_piece);
  }
}

//Update tmp blocks of map with piece positions. Used for non-destructive
//  validity checking
void load_map_tmp_trns(map_t **game_map, piece *c_piece) {
  for (int i = 0; i < 4; ++i) {
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = ' ';
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile_tmp_trns = 'O';
  }
}

//Revert changes made in load_map_tmp function once validity has been determined
void unload_map_tmp_trns(map_t **game_map, piece *c_piece) {
  for (int i = 0; i < 4; ++i)
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile_tmp_rot = ' ';
}

void load_map_tmp_rot(map_t **game_map, piece *c_piece) {
  for (int i = 0; i < 4; ++i) {
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = ' ';
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile_tmp_rot = 'O';
  }
}

//Revert changes made in load_map_tmp function once validity has been determined
void unload_map_tmp_rot(map_t **game_map, piece *c_piece) {
  for (int i = 0; i < 4; ++i)
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile_tmp_rot = ' ';
}

//Commit piece positions to map
void update_map(map_t **game_map, piece *c_piece) {
  //Clear last positions
  for (int i = 0; i < 4; ++i)
    game_map[c_piece->last_positions[i][0]][c_piece->last_positions[i][1]].map_tile = ' ';
  //Place new positions and give appropriate colour to tiles
  for (int i = 0; i < 4; ++i)  {
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = 'X';
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].colour = c_piece->colour;
  }
}

void increase_difficulty(piece *c_piece) {
  static int difficulty = 0;
  float difficulties[] = {0.8, 0.72, 0.63, 0.55, 0.47, 0.38, 0.22, 0.13, 0.1, 0.07,
      0.05, 0.03, 0.02};
  c_piece->difficulty = difficulties[difficulty++];
}

float time_diff(struct timespec start, struct timespec end) {
  return ((double)end.tv_sec + 1.0e-9*end.tv_nsec) -
    ((double)start.tv_sec + 1.0e-9*start.tv_nsec);
}
