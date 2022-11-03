#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include "tetris.h"

//Clear screen, revert terminal config, revert colour
void clean(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &old_sets);
  system("clear");
  write(STDOUT_FILENO, NORM, 6);
  exit(EXIT_SUCCESS);
}

//Graceful ctrl + c SIGTERM
void sighandler(int sig) {
  tcsetattr(STDIN_FILENO, TCSANOW, &old_sets);
  system("clear");
  write(STDOUT_FILENO, NORM, 6);
  exit(EXIT_SUCCESS);
}

//I use ctrl + \ as push to talk; this is to prevent quitting when doing that, since
//  ctrl + \ is coincidentally the SIGQUIT signal
void quithandler(int sig) {
  ;
}
