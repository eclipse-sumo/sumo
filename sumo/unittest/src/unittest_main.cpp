#include <iostream>
#include <gtest/gtest.h>

int main(int argc, char **argv) {
  std::cout << "Running unit tests\n";

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS(); 
}
