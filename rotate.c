#include "tetris.h"

//Defining rotation was so sprawly that I had to make it into its own source file for
//  convenience sake. Makes this much easier to navigate
void rotate(map_t **game_map, piece *c_piece) {
  //For updating map
  for (int i = 0; i < 4; ++i) {
    c_piece->last_positions[i][0] = c_piece->positions[i][0];
    c_piece->last_positions[i][1] = c_piece->positions[i][1];
  }
  //Non-destructive map update for validity checking
  load_map_tmp_rot(game_map, c_piece);

  //For reverting orientation if desired rotation is invalid
  c_piece->last_orientation = c_piece->orientation;

  switch (c_piece->cur_dir) {
    case ROT_ACW :
      //Rotation is type and orientation dependent because I'm silly
      switch (c_piece->type) {
        //Square piece doesn't do anything when rotating
        case SQUARE_PIECE :
          break;
        //Revolve around center point ([1][0/1])
        case T_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
              //Move left part to bottom
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] += 1;

              //Move right piece to top
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] -= 1;

              //Move bottom piece to right
              c_piece->positions[3][0] += 1;
              c_piece->positions[3][1] += 1;

              c_piece->orientation = ROT_270;
              goto nofall;
            case ROT_270:
              //Move bottom piece to right
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] += 1;

              //Move top piece to left
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] -= 1;

              //Move right piece to top
              c_piece->positions[3][0] += 1;
              c_piece->positions[3][1] -= 1;

              c_piece->orientation = ROT_180;
              goto nofall;
            case ROT_180:
              //Move right piece to top
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] -= 1;

              //Move left piece to bottom
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] += 1;

              //Move top piece to left
              c_piece->positions[3][0] -= 1;
              c_piece->positions[3][1] -= 1;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
              //Move top piece to left
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] -= 1;

              //Move bottom piece to right
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] += 1;

              //Move left piece to bottom
              c_piece->positions[3][0] -= 1;
              c_piece->positions[3][1] += 1;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        //Revolve around block around above elbow ([1][0/1])
        case L_L_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
              //Move top piece part to left
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] -= 1;

              //Move bottom piece to right
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] += 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][1] += 2;


              c_piece->orientation = ROT_270;
              goto nofall;
            case ROT_270:
              //Move left piece to bottom
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] += 1;

              //Move right piece to top
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] -= 1;

              //Move bottom right piece to top right
              c_piece->positions[3][0] += 2;

              c_piece->orientation = ROT_180;
              goto nofall;
            case ROT_180:
              //Move bottom piece to right
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] += 1;

              //Move top piece to left
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] -= 1;

              //Move top right piece to top left
              c_piece->positions[3][1] -= 2;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
              //Move right piece to top
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] -= 1;

              //Move left piece to bottom
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] += 1;

              //Move top left piece to bottom left
              c_piece->positions[3][0] -= 2;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        //Revolve around block around above elbow
        case L_R_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
              //Move top piece part to left
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] -= 1;

              //Move bottom piece to right
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] += 1;

              //Move bottom right piece to top right
              c_piece->positions[3][0] += 2;


              c_piece->orientation = ROT_270;
              goto nofall;
            case ROT_270:
              //Move left piece to bottom
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] += 1;

              //Move right piece to top
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] -= 1;

              //Move top right piece to top left
              c_piece->positions[3][1] -= 2;

              c_piece->orientation = ROT_180;
              goto nofall;
            case ROT_180:
              //Move bottom piece to right
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] += 1;

              //Move top piece to left
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] -= 1;

              //Move top left piece to bottom left
              c_piece->positions[3][0] -= 2;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
              //Move right piece to top
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] -= 1;

              //Move left piece to bottom
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] += 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][1] += 2;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        //Revolve around bottom center
        case Z_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
            case ROT_180:
              //Move right piece to top
              c_piece->positions[0][0] -= 2;

              //Move left piece to bottom
              c_piece->positions[1][0] -= 1;
              c_piece->positions[1][1] -= 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] += 1;
              c_piece->positions[3][1] -= 1;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
            case ROT_270:
              //Move right piece to top
              c_piece->positions[0][0] += 2;

              //Move left piece to bottom
              c_piece->positions[1][0] += 1;
              c_piece->positions[1][1] += 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] -= 1;
              c_piece->positions[3][1] += 1;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        //Revolve around bottom center
        case S_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
            case ROT_180:
              //Move right piece to top
              c_piece->positions[0][1] -= 2;

              //Move left piece to bottom
              c_piece->positions[1][0] -= 1;
              c_piece->positions[1][1] -= 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] -= 1;
              c_piece->positions[3][1] += 1;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
            case ROT_270:
              //Move right piece to top
              c_piece->positions[0][1] += 2;

              //Move left piece to bottom
              c_piece->positions[1][0] += 1;
              c_piece->positions[1][1] += 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] += 1;
              c_piece->positions[3][1] -= 1;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        //Revolve around second top
        case LINE_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
            case ROT_180:
              //Move right piece to top
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] -= 1;

              //Move left piece to bottom
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] += 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] += 2;
              c_piece->positions[3][1] += 2;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
            case ROT_270:
              //Move right piece to top
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] += 1;

              //Move left piece to bottom
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] -= 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] -= 2;
              c_piece->positions[3][1] -= 2;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
      }
    case ROT_CW :
      switch (c_piece->type) {
        case SQUARE_PIECE :
          break;
        case T_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
              //Move left part to top
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] += 1;

              //Move right piece to bottom
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] -= 1;

              //Move bottom piece to left
              c_piece->positions[3][0] += 1;
              c_piece->positions[3][1] -=1;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
              //Move top piece to right
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] += 1;

              //Move bottom piece to left
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] -= 1;

              //Move left piece to top
              c_piece->positions[3][0] += 1;
              c_piece->positions[3][1] += 1;

              c_piece->orientation = ROT_180;
              goto nofall;
            case ROT_180:
              //Move right piece to bottom
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] -= 1;

              //Move left piece to top
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] += 1;

              //Move top piece to right
              c_piece->positions[3][0] -= 1;
              c_piece->positions[3][1] += 1;

              c_piece->orientation = ROT_270;
              goto nofall;
            case ROT_270:
              //Move bottom piece to left
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] -= 1;

              //Move top piece to right
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] += 1;

              //Move right piece to bottom
              c_piece->positions[3][0] -= 1;
              c_piece->positions[3][1] -= 1;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        case L_L_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
              //Move top piece to right
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] += 1;

              //Move bottom piece to left
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] -= 1;

              //Move bottom left piece to top left
              c_piece->positions[3][0] += 2;


              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
              //Move right piece to bottom
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] -= 1;

              //Move left piece to top
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] += 1;

              //Move top left piece to top right
              c_piece->positions[3][1] += 2;

              c_piece->orientation = ROT_180;
              goto nofall;
            case ROT_180:
              //Move bottom piece to left
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] -= 1;

              //Move top piece to right
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] += 1;

              //Move top right piece to bottom right
              c_piece->positions[3][0] -= 2;

              c_piece->orientation = ROT_270;
              goto nofall;
            case ROT_270:
              //Move left piece to top
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] += 1;

              //Move right piece to bottom
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] -= 1;

              //Move bottom right piece to bottom left
              c_piece->positions[3][1] -= 2;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        case L_R_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
              //Move top piece to right
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] += 1;

              //Move bottom piece to left
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] -= 1;

              //Move bottom right piece to bottom left
              c_piece->positions[3][1] -= 2;


              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
              //Move right piece to bottom
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] -= 1;

              //Move left piece to top
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] += 1;

              //Move bottom left piece to top left
              c_piece->positions[3][0] += 2;

              c_piece->orientation = ROT_180;
              goto nofall;
            case ROT_180:
              //Move bottom piece to left
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] -= 1;

              //Move top piece to right
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] += 1;

              //Move top left piece to top right
              c_piece->positions[3][1] += 2;

              c_piece->orientation = ROT_270;
              goto nofall;
            case ROT_270:
              //Move left piece to top
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] += 1;

              //Move right piece to bottom
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] -= 1;

              //Move top right piece to bottom right
              c_piece->positions[3][0] -= 2;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        case Z_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
            case ROT_180:
              //Move right piece to top
              c_piece->positions[0][0] -= 2;

              //Move left piece to bottom
              c_piece->positions[1][0] -= 1;
              c_piece->positions[1][1] -= 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] += 1;
              c_piece->positions[3][1] -= 1;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
            case ROT_270:
              //Move right piece to top
              c_piece->positions[0][0] += 2;

              //Move left piece to bottom
              c_piece->positions[1][0] += 1;
              c_piece->positions[1][1] += 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] -= 1;
              c_piece->positions[3][1] += 1;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        case S_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
            case ROT_180:
              //Move right piece to top
              c_piece->positions[0][1] -= 2;

              //Move left piece to bottom
              c_piece->positions[1][0] -= 1;
              c_piece->positions[1][1] -= 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] -= 1;
              c_piece->positions[3][1] += 1;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
            case ROT_270:
              //Move right piece to top
              c_piece->positions[0][1] += 2;

              //Move left piece to bottom
              c_piece->positions[1][0] += 1;
              c_piece->positions[1][1] += 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] += 1;
              c_piece->positions[3][1] -= 1;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
        case LINE_PIECE :
          switch (c_piece->orientation) {
            case ROT_0:
            case ROT_180:
              //Move right piece to top
              c_piece->positions[0][0] -= 1;
              c_piece->positions[0][1] -= 1;

              //Move left piece to bottom
              c_piece->positions[2][0] += 1;
              c_piece->positions[2][1] += 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] += 2;
              c_piece->positions[3][1] += 2;

              c_piece->orientation = ROT_90;
              goto nofall;
            case ROT_90:
            case ROT_270:
              //Move right piece to top
              c_piece->positions[0][0] += 1;
              c_piece->positions[0][1] += 1;

              //Move left piece to bottom
              c_piece->positions[2][0] -= 1;
              c_piece->positions[2][1] -= 1;

              //Move bottom left piece to bottom right
              c_piece->positions[3][0] -= 2;
              c_piece->positions[3][1] -= 2;

              c_piece->orientation = ROT_0;
              goto nofall;
          }
    }
  }
nofall:
  //Check collision
  for (int i = 0; i < 4; ++i)
    if (((game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile == 'X') &&
        (game_map[c_piece->positions[i][0]][c_piece->positions[i][1]].map_tile_tmp_rot != 'O')) ||
        (c_piece->positions[i][0] <= 1) ||
        (c_piece->positions[i][1] >= MAP_WIDTH) ||
        (c_piece->positions[i][1] < 0) ||
        (c_piece->positions[i][0] > MAP_HEIGHT))
      invalid_rotation(game_map, c_piece);
  //Revert temp map changes when checking validity and update map
  unload_map_tmp_rot(game_map, c_piece);
  update_map(game_map, c_piece);
}

void invalid_rotation(map_t **game_map, piece *c_piece) {
  c_piece->orientation = c_piece->last_orientation;
  unload_map_tmp_rot(game_map, c_piece);
  for (int i = 0; i < 4; ++i) {
    c_piece->positions[i][0] = c_piece->last_positions[i][0];
    c_piece->positions[i][1] = c_piece->last_positions[i][1];
  }
  update_map(game_map, c_piece);
}
