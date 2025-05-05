#include "../src/headers/main.h"
#include "../src/headers/maze.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

void test_countneighbours() {
  maze_height = 10;
  maze_width = 10;
  cell m[maze_height][maze_width];

  initMaze(m);

  m[0][1].visited = 1;
  m[1][0].visited = 1;
  struct node n;
  n.col = 0;
  n.row = 0;
  CU_ASSERT(countNeighbours(m, &n) == 0);

  m[2][3].visited = 1;
  m[1][2].visited = 1;
  n.col = 2;
  n.row = 2;
  CU_ASSERT(countNeighbours(m, &n) == 2);
}

void test_isDone() {
  maze_height = 20;
  maze_width = 30;
  cell m[maze_height][maze_width];

  initMaze(m);

  for (int row = 0; row < maze_height; row++) {
    for (int col = 0; col < maze_width; col++) {
      m[row][col].visited = 1;
    }
  }

  CU_ASSERT(isDone(m) == 1);
  m[9][4].visited = 0;
  CU_ASSERT(isDone(m) == 0);
}

void test_initMaze() {
  maze_height = 20;
  maze_width = 30;
  cell m[maze_height][maze_width];

  initMaze(m);

  for (int row = 0; row < maze_height; row++) {
    for (int col = 0; col < maze_width; col++) {
      CU_ASSERT(m[row][col].visited == false);
      CU_ASSERT(m[row][col].lower == true);
      CU_ASSERT(m[row][col].right == true);
    }
  }
}

int main() {
  CU_initialize_registry();
  CU_pSuite suite = CU_add_suite("maze tests", 0, 0);
  CU_add_test(suite, "Test of init func", test_initMaze);
  CU_add_test(suite, "Test of count neighbour function", test_countneighbours);
  CU_add_test(suite, "Test of isDone function", test_isDone);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return 0;
}
