#include "simulator.h"

static uint64_t totalHashrate;
static uint64_t now;


void reconfig(uint64_t startTime, uint16_t numMiners, uint64_t relativeHashrates[NUM_STRATEGIES]) {
    now = startTime;
    totalHashrate = 0;
    initMiners(numMiners);

    assert(numMiners >= NUM_STRATEGIES);
    assert(numMiners < 1 << 10);
    uint64_t totalRelativeHashrate = 0;
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        totalRelativeHashrate += relativeHashrates[i];
    }
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        relativeHashrates[i] *= 1 << 16;
        relativeHashrates[i] /= totalRelativeHashrate;
    }
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        totalHashrate += relativeHashrates[i];
    }
    assert(totalHashrate < 1 << 16);
    assert(totalHashrate > 1 << 15);

    for (uint16_t i = NUM_STRATEGIES; i < minerCount; i++) {
        miners[i].strategy = (strategy_t) (arc4random() % NUM_STRATEGIES);
        miners[i].hashrate = arc4random() % (relativeHashrates[miners[i].strategy] / 7);
        miners[i].hashrate += 1 << 2;
        miners[i].hashrate += arc4random() % 1 << 6;
        assert(miners[i].hashrate < relativeHashrates[miners[i].strategy]);
        relativeHashrates[miners[i].strategy] -= miners[i].hashrate;
    }
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        miners[i].strategy = (strategy_t)i;
        miners[i].hashrate = relativeHashrates[miners[i].strategy];
    }
}

static void tick() {
    now++;
    uint32_t chance = arc4random() % 1 << 20;
    if (chance > totalHashrate) {
        return;
    }
    uint16_t miner = 0;
    for (; miner < minerCount; miner++) {
        if (miners[miner].hashrate > chance) {
            break;
        }
        chance -= miners[miner].hashrate;
    }
    const block_t *block = mineBlock(miners + miner, now, 1 << 20);
    onBlock(block);
}

void run(uint64_t until) {
    while (now < until) tick();
}
void printSummary() {
    const block_t *head = longestChain();
    uint64_t *minerRewards = totalMinerRewards(head);
    uint64_t totalByStrategy[NUM_STRATEGIES];
    for (uint8_t i = 0 ; i < NUM_STRATEGIES; i++) {
        totalByStrategy[i] = 0;
    }
    uint64_t totalMinerRewards;
    for (uint8_t i = 0; i < minerCount; i++) {
        totalByStrategy[miners[i].strategy] += minerRewards[i];
    }
    for (uint8_t i = 0 ; i < NUM_STRATEGIES; i++) {
        printf("%s:%llu\n", strategyToName((strategy_t)i), totalByStrategy[i]);
    }
}
