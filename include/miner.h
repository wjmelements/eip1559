#include "block.h"


typedef enum strategy {
    FILL_BLOCK,
    MIN_BASEFEE,
    // MIN_BASEFEE_ONE_DEEP,
    // MIN_BASEFEE_PERIOD,
} strategy_t;


typedef struct miner {
    uint64_t hashrate;
    strategy_t strategy;
} miner_t;

extern miner_t *miners;
extern uint8_t minerCount;
void initMiners(uint8_t count);
block_t *mineBlock(miner_t *miner, block_t *parent);
uint64_t *totalMinerRewards(block_t *head);
