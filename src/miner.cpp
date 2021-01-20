#include "miner.h"


miner_t *miners;

void initMiners(uint8_t count) {
    if (miners != NULL) {
        free(miners);
    }
    miners = (miner_t *)calloc(count, sizeof(miner_t));
}

block_t *mineBlock(miner_t *miner, block_t *parent) {
    uint32_t txCount;
    /*
    switch (miner->strategy) {
    }
    */
    block_t *block = mallocBlock(txCount);
    block->parent = parent;
    block->miner = miner - miners;
    return block;
}
