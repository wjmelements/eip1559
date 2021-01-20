#include "simulator.h"


int main() {
    uint64_t minerStrategies[NUM_STRATEGIES] = {
        60, 40
    };
    uint64_t transactorModels[NUM_PATTERNS] = {
        100,
    };
    reconfig(500, 500, minerStrategies, transactorModels);
    run(500 + 10000000);
    printSummary();
    return 0;
}
