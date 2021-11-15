#include <iostream>
#include "puzzle.h"

int main() {
    solver sl;
    sl.start.read();
    sl.iterate();
    return 0;
}
