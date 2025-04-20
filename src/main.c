#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define MAZEWIDTH 50
#define MAZEHEIGHT 20

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

bool is_done() {
  for (int row = 0; row < MAZEHEIGHT; row++) {
    for (int col = 0; col < MAZEWIDTH; col++) {
      if (!maze[row][col].visited) {
        return false;
      }
    }
  }
  return true;
}

void print_maze() {
  printf("\n\n\n");
  printf("+");
  for (int col = 0; col < MAZEWIDTH; col++) {
    printf("--+");
  }
  printf("\n");

  for (int row = 0; row < MAZEHEIGHT; row++) {
    printf("|");
    for (int col = 0; col < MAZEWIDTH; col++) {
      cell *current = &maze[row][col];
      if (current->visited) {
        printf("  ");
      } else {
        printf("##");
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
        printf("--+");
      } else {
        printf("  +");
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

void randomDir(signed char *dir, struct node *current) {
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
}

int countNeighbours(struct node *current) {
  int neighbours = 0;

  if (current->row - 1 >= 0) {
    cell neighbour = maze[current->row - 1][current->col];
    if (!neighbour.visited) {
      neighbours++;
    }
  }
  if (current->row + 1 < MAZEHEIGHT) {
    cell neighbour = maze[current->row + 1][current->col];
    if (!neighbour.visited) {
      neighbours++;
    }
  }
  if (current->col - 1 >= 0) {
    cell neighbour = maze[current->row][current->col - 1];
    if (!neighbour.visited) {
      neighbours++;
    }
  }
  if (current->col + 1 < MAZEWIDTH) {
    cell neighbour = maze[current->row][current->col + 1];
    if (!neighbour.visited) {
      neighbours++;
    }
  }
  return neighbours;
}

struct node *backtrack(struct node *tip) {
  struct node *current = tip;
  while (countNeighbours(current) == 0) {
    if (current->prev == NULL) {
      break;
    }
    struct node *rm = current;
    current = current->prev;
    free(rm);
  }
  return current;
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

  signed char dir[2];

  struct node *current = (struct node *)calloc(1, sizeof(struct node));
  current->row = 0;
  current->col = 0;
  maze[current->row][current->col].visited = true;

  while (!is_done()) {
    print_maze();
    usleep(20000);
    randomDir(dir, current);

    current = move(current, dir);
    current = backtrack(current);
  }
  print_maze();
  freeStack(current);
  return 0;
}
