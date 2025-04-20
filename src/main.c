#include <stdbool.h>
#include <stdio.h>
#define MAZEWIDTH 20
#define MAZEHEIGHT 10

typedef struct node {
  bool visited;
  bool lower;
  bool right;
} node;

node maze[MAZEHEIGHT][MAZEWIDTH];

void print_maze() {

  printf("Current state of the maze:\n");
  printf("|");
  for (int col = 0; col < MAZEWIDTH; col++) {
    if (col != MAZEWIDTH - 1) {
      printf("--");
    } else {
      printf("-");
    }
  }

  printf("|");
  printf("\n");
  for (int row = 0; row < MAZEHEIGHT; row++) {
    printf("|");
    for (int col = 0; col < MAZEWIDTH; col++) {
      node *current = &maze[row][col];
      if (!current->visited) {
        printf(" ");
      } else {
        printf("#");
      }
      current->right = 1;
      if (current->right) {
        printf("|");
      } else {
        printf(" ");
      }
    }

    printf("\n");
    printf("|");
    for (int col = 0; col < MAZEWIDTH; col++) {
      node *current = &maze[row][col];
      current->lower = 1;
      if (current->lower) {
        printf("-");
      } else {
        printf(" ");
      }
      if (col != MAZEWIDTH - 1) {
        printf("-");
      }
    }
    printf("|");
    printf("\n");
  }
}

int main() {
  print_maze();
  return 0;
}
