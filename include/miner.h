#include "block.h"


typedef enum strategy {
    FILL_BLOCK,
    HOLD_BASEFEE,
    // HOLD_BASEFEE_ONE_DEEP,
    // HOLD_BASEFEE_PERIOD,
} strategy_t;


typedef struct miner {
    strategy_t strategy;
} miner_t;

extern miner_t *miners;
void initMiners(uint8_t count);
block_t *mineBlock(miner_t *miner, block_t *parent);
