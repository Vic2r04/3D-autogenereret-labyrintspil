#include "../src/headers/maze.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

void test_initMaze() {
  maze_height = 20;
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
  CU_basic_run_tests();
  CU_cleanup_registry();
  return 0;
}
