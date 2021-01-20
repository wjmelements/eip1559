#include "simulator.h"


int main() {
    uint64_t allocations[NUM_STRATEGIES] = {
        60, 40
    };
    reconfig(500, 1000, allocations);
    run(500 + 6000000);
    printSummary();
    return 0;
}
