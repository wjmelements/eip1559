#include "miner.h"
#include "user.h"

void reconfig(uint64_t startTime, uint16_t numMiners, uint64_t relativeHashrates[NUM_STRATEGIES], uint64_t transactorDistribution[NUM_PATTERNS]);
void run(uint64_t until);
void printSummary();
