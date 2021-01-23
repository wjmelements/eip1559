#include "simulator.h"


int main() {
    uint64_t minerStrategies[NUM_STRATEGIES] = {
        50, 50
    };
    uint64_t transactorModels[NUM_PATTERNS] = {
        100, 40, 10, 3, 2, 1,
    };
    reconfig(500, 500, minerStrategies, transactorModels);
    run(500 + 1000000);
    printSummary();
    return 0;
}
