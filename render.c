#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "tetris.h"

void render(map_t **game_map, piece c_piece) {
  system("clear");

  //Spaghetti due to a hack involving segfaults with rotations
  //I displaced the bottom of the map so the piece wouldn't try to write
  //  to protected memory
  for (int i = MAP_HEIGHT-1; i >= 0; --i) {
    if (i > 1)
      printf("%s9 ", NORM);
    for (int j = 0; j < MAP_WIDTH; ++j)
      printf("%s%c ", game_map[i][j].colour,
          game_map[i][j].map_tile);
    if (i == 2) {
      printf("%s9\n", NORM);
      for (int i = 0; i < MAP_WIDTH+1; ++i)
        printf("%s9 ", NORM);
    }
    if (i > 1)
      printf("%s9\n", NORM);
  }
  printf("\nCURRENTLY HOLDING: ");
  switch(c_piece.hold) {
    case SQUARE_PIECE :
      printf("SQUARE\n");
      break;
    case T_PIECE :
      printf("T\n");
      break;
    case L_L_PIECE :
      printf("L_L\n");
      break;
    case L_R_PIECE :
      printf("L_R\n");
      break;
    case Z_PIECE :
      printf("Z\n");
      break;
    case S_PIECE :
      printf("S\n");
      break;
    case LINE_PIECE :
      printf("LINE\n");
      break;
    case NOTHING :
      printf("\n");
      break;
  }
  printf("\nNEXT PIECE: ");
  switch(c_piece.next_type) {
    case SQUARE_PIECE :
      printf("SQUARE\n");
      break;
    case T_PIECE :
      printf("T\n");
      break;
    case L_L_PIECE :
      printf("L_L\n");
      break;
    case L_R_PIECE :
      printf("L_R\n");
      break;
    case Z_PIECE :
      printf("Z\n");
      break;
    case S_PIECE :
      printf("S\n");
      break;
    case LINE_PIECE :
      printf("LINE\n");
      break;
    case NOTHING :
      printf("\n");
      break;
  }
  printf("\nLINES CLEARED: %d\n", c_piece.lines_cleared);
}
