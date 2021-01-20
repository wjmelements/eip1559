#include "block.h"

#include <vector>
using std::vector;



typedef enum strategy {
    FILL_BLOCK,
    MIN_BASEFEE,
    // MIN_BASEFEE_ONE_DEEP,
    // MIN_BASEFEE_PERIOD,
    NUM_STRATEGIES,
} strategy_t;

static inline const char *strategyToName(strategy_t strategy) {
    switch (strategy) {
        case FILL_BLOCK:
            return "FILL_BLOCK";
        case MIN_BASEFEE:
            return "MIN_BASEFEE";
        default:
            assert(strategy < NUM_STRATEGIES);
            return "ERROR";
    }
}


typedef struct miner {
    uint64_t hashrate;
    strategy_t strategy;
} miner_t;

extern miner_t *miners;
extern uint16_t minerCount;
void initMiners(uint16_t count);
block_t *mineBlock(const miner_t *miner, uint64_t timestamp, uint64_t difficulty);
const block_t *longestChain();
uint64_t *totalMinerRewards(const block_t *head);

void onBlock(const block_t *block);
void submitTransaction(tx_t tx);
