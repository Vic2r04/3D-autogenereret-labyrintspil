#include "headers/maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int maze_width = 20;
int maze_height = 10;

void initMaze(cell maze[maze_height][maze_width]) {
  for (int row = 0; row < maze_height; row++) {
    for (int col = 0; col < maze_width; col++) {
      cell *current = &maze[row][col];
      current->visited = 0;
      current->lower = 1;
      current->right = 1;
    }
  }
}

bool isDone(cell maze[maze_height][maze_width]) {
  for (int row = 0; row < maze_height; row++) {
    for (int col = 0; col < maze_width; col++) {
      if (!maze[row][col].visited) {
        return false;
      }
    }
  }
  return true;
}

void printMaze(cell maze[maze_height][maze_width]) {
  printf("\n\n\n");
  printf("+");
  for (int col = 0; col < maze_width; col++) {
    printf("--+");
  }
  printf("\n");

  for (int row = 0; row < maze_height; row++) {
    printf("|");
    for (int col = 0; col < maze_width; col++) {
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
    for (int col = 0; col < maze_width; col++) {
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

void randomDir(cell maze[maze_height][maze_width], signed char *dir,
               struct node *current) {
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

      if (current->row + dir[0] < maze_height && current->row + dir[0] >= 0 &&
          current->col + dir[1] < maze_width && current->col + dir[1] >= 0) {
        break;
      }
    }
  }
}

int countNeighbours(cell maze[maze_height][maze_width], struct node *current) {
  int neighbours = 0;

  if (current->row - 1 >= 0) {
    cell neighbour = maze[current->row - 1][current->col];
    if (!neighbour.visited) {
      neighbours++;
    }
  }
  if (current->row + 1 < maze_height) {
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
  if (current->col + 1 < maze_width) {
    cell neighbour = maze[current->row][current->col + 1];
    if (!neighbour.visited) {
      neighbours++;
    }
  }
  return neighbours;
}

struct node *backtrack(cell maze[maze_height][maze_width], struct node *tip) {
  struct node *current = tip;
  while (countNeighbours(maze, current) == 0) {
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

struct node *move(cell maze[maze_height][maze_width], struct node *current,
                  signed char dir[2]) {
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

void genMaze(cell maze[maze_height][maze_width]) {
  srand(time(0));
  signed char dir[2];

  struct node *current = (struct node *)calloc(1, sizeof(struct node));
  maze[current->row][current->col].visited = true;

  while (!isDone(maze)) {
    randomDir(maze, dir, current);

    current = move(maze, current, dir);
    current = backtrack(maze, current);
  }

  freeStack(current);
}
