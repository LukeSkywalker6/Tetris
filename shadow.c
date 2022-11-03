#include <stdint.h>
#include "tetris.h"

void draw_shadow(map_t **game_map, piece *c_piece) {
  int shadow_valid = 0;

  //Since shadow will be derived from piece position, we'll be manipulating it directly
  int piece_original_position[4][2];
  for (int i = 0; i < 4; ++i) {
    piece_original_position[i][0] = c_piece->positions[i][0];
    piece_original_position[i][1] = c_piece->positions[i][1];
  }

  //For ignoring self collision during shadow transpose validity check
  for (int i = 0; i < 4; ++i)
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = '-';

  int drop_distance = 0;
  while (!shadow_valid) {
    //Place shadow if shadow intersects floor
    for (int i = 0; i < 4; ++i)
      if (c_piece->positions[i][0]-1 <= 1) {
        if (drop_distance != 0)
          place_shadow(game_map, c_piece);
        shadow_valid = 1;
      }

    //Place shadow if shadow intersects piece segment
    for (int i = 0; i < 4; ++i) 
      if (game_map[c_piece->positions[i][0]-1][c_piece->positions[i][1]].map_tile == 'X') {
        if (drop_distance != 0)
          place_shadow(game_map, c_piece);
        shadow_valid = 1;
      }

    //If current position doesn't satisfy shadow conditions, move down
    if (!shadow_valid)
      for (int i = 0; i < 4; ++i)
        c_piece->positions[i][0] -= 1;
    drop_distance++;
  }

  //Revert changes to piece position
  for (int i = 0; i < 4; ++i) {
    c_piece->positions[i][0] = piece_original_position[i][0];
    c_piece->positions[i][1] = piece_original_position[i][1];
  }
  update_map(game_map, c_piece);
}

//Commit shadow to map
void place_shadow(map_t **game_map, piece* c_piece) {
  //Used for removing previous shadow
  static int32_t shadow_last_position[4][2];

  for (int i = 0; i < 4; ++i)
    if (game_map[shadow_last_position[i][0]][shadow_last_position[i][1]].map_tile != 'X')
      game_map[shadow_last_position[i][0]][shadow_last_position[i][1]].map_tile = ' ';

  //Remember position of shadow for removing later
  for (int i = 0; i < 4; ++i) {
    shadow_last_position[i][0] = c_piece->positions[i][0];
    shadow_last_position[i][1] = c_piece->positions[i][1];
  }

  for (int i = 0; i < 4; ++i) {
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile = '-';
    game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].colour = NORM;
  }
}
