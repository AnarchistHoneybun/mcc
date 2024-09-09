#include "compiler_driver.h"

int main(int argc, char* argv[]) {
    CompilerDriver driver;
    return driver.run(argc, argv);
}