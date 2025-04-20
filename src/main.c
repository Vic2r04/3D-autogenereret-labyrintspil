#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define MAZEWIDTH 20
#define MAZEHEIGHT 10

typedef struct {
  bool visited;
  bool tip;
  bool lower;
  bool right;
} cell;

cell maze[MAZEHEIGHT][MAZEWIDTH];

void init_maze() {
  for (int row = 0; row < MAZEHEIGHT; row++) {
    for (int col = 0; col < MAZEWIDTH; col++) {
      cell *current = &maze[row][col];
      current->lower = 1;
      current->right = 1;
    }
  }
}

void print_maze() {
  printf("+");
  for (int col = 0; col < MAZEWIDTH; col++) {
    printf("-+");
  }
  printf("\n");

  for (int row = 0; row < MAZEHEIGHT; row++) {
    printf("|");
    for (int col = 0; col < MAZEWIDTH; col++) {
      cell *current = &maze[row][col];
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
      cell *current = &maze[row][col];
      if (current->lower) {
        printf("-+");
      } else {
        printf(" +");
      }
    }
    printf("\n");
  }
}

struct node {
  int col;
  int row;
  struct node *prev;
};

void freeStack(struct node *tip) {
  if (tip->prev == NULL) {
    printf("freeing node at [%d, %d]\n", tip->col, tip->row);
    free(tip);
    return;
  }
  freeStack(tip->prev);
  free(tip);
}

struct node *move(struct node *current, signed char dir[2]) {
  struct node *next = (struct node *)calloc(1, sizeof(struct node));

  next->row = current->row + dir[0];
  next->col = current->col + dir[1];

  next->prev = current;

  maze[current->row][current->col].right = false;
  maze[next->row][next->col].tip = true;
  maze[next->row][next->col].visited = true;

  return next;
}

int main() {
  init_maze();
  int input;
  struct node *current = (struct node *)calloc(1, sizeof(struct node));
  maze[current->row][current->col].tip = true;
  maze[current->row][current->col].visited = true;
  while (1) {
    print_maze();

    scanf("%d", &input);
    if (input == 1) {
      break;
    }
    signed char dir[] = {0, 1};
    current = move(current, dir);
  }
  freeStack(current);
  return 0;
}
