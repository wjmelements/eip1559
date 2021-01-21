#include "simulator.h"


int main() {
    uint64_t minerStrategies[NUM_STRATEGIES] = {
        60, 40
    };
    uint64_t transactorModels[NUM_PATTERNS] = {
        100, 40, 10, 3, 2, 1,
    };
    reconfig(500, 500, minerStrategies, transactorModels);
    run(500 + 10000);
    printSummary();
    return 0;
}
