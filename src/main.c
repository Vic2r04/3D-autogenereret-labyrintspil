#include <stdbool.h>
#include <stdio.h>
#define MAZEWIDTH 20
#define MAZEHEIGHT 10

typedef struct node {
  bool visited;
  bool tip;
  bool lower;
  bool right;
} node;

node maze[MAZEHEIGHT][MAZEWIDTH];

void init_maze() {
  for (int row = 0; row < MAZEHEIGHT; row++) {
    for (int col = 0; col < MAZEWIDTH; col++) {
      node *current = &maze[row][col];
      current->lower = 1;
      current->right = 1;
    }
  }
}

void print_maze() {

  printf("Current state of the maze:\n");

  printf("+");
  for (int col = 0; col < MAZEWIDTH; col++) {
    printf("-+");
  }
  printf("\n");

  for (int row = 0; row < MAZEHEIGHT; row++) {
    printf("|");
    for (int col = 0; col < MAZEWIDTH; col++) {
      node *current = &maze[row][col];
      if (current->visited) {
        printf("#");
      } else if (current->tip) {
        printf("@");
      } else {
        printf(" ");
      }
      if (current->right) {
        printf("|");
      } else {
        printf(" ");
      }
    }

    printf("\n");
    printf("+");
    for (int col = 0; col < MAZEWIDTH; col++) {
      node *current = &maze[row][col];
      if (current->lower) {
        printf("-+");
      } else {
        printf(" +");
      }
    }
    printf("\n");
  }
}

int main() {
  init_maze();
  print_maze();
  return 0;
}
