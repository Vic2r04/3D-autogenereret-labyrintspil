#include <stdio.h>
#define MAZEWIDTH 6
#define MAZEHEIGHT 6

typedef struct node {
  char visited;
  struct node *children[4];
} node;

node maze[MAZEHEIGHT][MAZEWIDTH];

void print_maze() {
  printf("Current state of the maze:\n");
  for (int row = 0; row < MAZEHEIGHT; row++) {
    printf("{ ");
    for (int col = 0; col < MAZEWIDTH; col++) {
      char *format = "%d, ";
      if (col == MAZEWIDTH - 1) {
        format = "%d ";
      }
      printf(format, maze[row][col].visited);
    }
    printf("}\n");
  }
}

int main() {
  print_maze();
  return 0;
}
