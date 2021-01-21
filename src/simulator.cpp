#include "simulator.h"

static uint64_t totalHashrate;
static uint64_t now;


void reconfig(uint64_t startTime, uint16_t numMiners, uint64_t relativeHashrates[NUM_STRATEGIES], uint64_t transactorDistribution[NUM_PATTERNS]) {
    now = startTime;
    totalHashrate = 0;
    setUserPatternDistribution(transactorDistribution);
    initMiners(numMiners);

    assert(numMiners >= NUM_STRATEGIES);
    assert(numMiners < 1 << 12);
    uint64_t totalRelativeHashrate = 0;
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        totalRelativeHashrate += relativeHashrates[i];
    }
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        relativeHashrates[i] *= 1 << 24;
        relativeHashrates[i] /= totalRelativeHashrate;
    }
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        totalHashrate += relativeHashrates[i];
    }
    assert(totalHashrate <= 1 << 24);
    assert(totalHashrate > 1 << 23);

    for (uint16_t i = NUM_STRATEGIES; i < minerCount; i++) {
        miners[i].strategy = (strategy_t) (arc4random() % NUM_STRATEGIES);
        uint32_t maxHashrate = (1 << 24) / numMiners;
        if (maxHashrate > relativeHashrates[miners[i].strategy] / (numMiners - i)) {
            maxHashrate = relativeHashrates[miners[i].strategy] / (numMiners - i) + 1;
        }
        if (maxHashrate > (1 + relativeHashrates[miners[i].strategy] / (1 << 5))) {
            maxHashrate = (1 + relativeHashrates[miners[i].strategy] / (1 << 5));
        }
        miners[i].hashrate = arc4random() % maxHashrate;
        miners[i].hashrate += arc4random() % (1 << 5);
        miners[i].hashrate += 1;
        assert(miners[i].hashrate < relativeHashrates[miners[i].strategy]);
        printf("%3u %s %llu\n", i, strategyToName((strategy)miners[i].strategy), miners[i].hashrate);
        relativeHashrates[miners[i].strategy] -= miners[i].hashrate;
    }
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        miners[i].strategy = (strategy_t)i;
        miners[i].hashrate = relativeHashrates[miners[i].strategy];
    }
}

static void tick() {
    now++;
    while (arc4random() & 3) {
        submitTransaction(nextTx());
    }
    uint32_t chance = arc4random() % (1 << 28);
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
    for (uint16_t i = 0; i < minerCount; i++) {
        totalByStrategy[miners[i].strategy] += minerRewards[i];
    }
    for (uint8_t i = 0 ; i < NUM_STRATEGIES; i++) {
        printf("%s\t:%20llu\n", strategyToName((strategy_t)i), totalByStrategy[i]);
    }
}
