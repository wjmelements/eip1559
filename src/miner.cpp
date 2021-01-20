#include "miner.h"


miner_t *miners;
uint8_t minerCount;

void initMiners(uint8_t count) {
    if (miners != NULL) {
        free(miners);
    }
    miners = (miner_t *)calloc(count, sizeof(miner_t));
    minerCount = count;
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

uint64_t *totalMinerRewards(block_t *head) {
    uint64_t *rewards = (uint64_t *)calloc(minerCount, sizeof(uint64_t));
    while (head) {
        rewards[head->miner] += head->totalFees;
        head = head->parent;
    }
    return rewards;
}
