#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAZEWIDTH 5
#define MAZEHEIGHT 3

typedef struct {
  bool visited;
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
        printf(" ");
      } else {
        printf("#");
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

signed char *randomDir(struct node *current) {
  signed char *dir = (signed char *)malloc(sizeof(signed char) * 2);

  char rn;
  while (true) {
    rn = rand() % 4;
    switch (rn) {
    case 0:
      dir[0] = 1;
      dir[1] = 0;
      break;
    case 1:
      dir[0] = 0;
      dir[1] = 1;
      break;
    case 2:
      dir[0] = -1;
      dir[1] = 0;
      break;
    case 3:
      dir[0] = 0;
      dir[1] = -1;
      break;
    }
    struct node *prev = current->prev;
    if (prev == NULL ||
        !maze[current->row + dir[0]][current->col + dir[1]].visited) {

      if (current->row + dir[0] < MAZEHEIGHT && current->row + dir[0] >= 0 &&
          current->col + dir[1] < MAZEWIDTH && current->col + dir[1] >= 0) {
        break;
      }
    }
  }

  return dir;
}

void freeStack(struct node *tip) {
  if (tip->prev == NULL) {
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

  if (dir[0] == 1) {
    maze[current->row][current->col].lower = false;
  } else if (dir[0] == -1) {
    maze[current->row - 1][current->col].lower = false;
  } else if (dir[1] == 1) {
    maze[current->row][current->col].right = false;
  } else if (dir[1] == -1) {
    maze[current->row][current->col - 1].right = false;
  }

  maze[next->row][next->col].visited = true;

  return next;
}

int main() {
  srand(time(0));
  init_maze();
  int input;
  signed char *dir = NULL;
  struct node *current = (struct node *)calloc(1, sizeof(struct node));
  current->row = 0;
  current->col = 2;
  maze[current->row][current->col].visited = true;
  while (1) {
    print_maze();
    dir = randomDir(current);

    scanf("%d", &input);
    if (input == 1) {
      break;
    }

    current = move(current, dir);
  }
  freeStack(current);
  free(dir);
  return 0;
}
