#include "tx.h"

#include <assert.h>
#include <stdlib.h>

typedef struct block {
    const struct block *parent;
    uint64_t timestamp;
    uint64_t txCount;
    uint64_t td;
    uint64_t difficulty;
    uint64_t gasTarget;
    uint64_t gasUsed;
    uint64_t baseFee;
    uint64_t totalFees;
    uint8_t miner;
    tx_t txs[0];
} block_t;


static inline block_t *mallocBlock(uint32_t txCount) {
    block_t *block = (block_t *)malloc(sizeof(block_t) + sizeof(tx_t) * txCount);
    block->txCount = txCount;
    return block;
}

static const uint64_t ELASTICITY_MULTIPLIER = 2;
static const uint64_t BASE_FEE_MAX_CHANGE_DENOMINATOR = 8;


static uint64_t nextBaseFee(const block_t *parent) {
    if (parent->gasUsed == parent->gasTarget) {
        return parent->gasTarget;
    } else if (parent->gasUsed > parent->gasTarget) {
        uint64_t feeDelta = parent->baseFee * (parent->gasUsed - parent->gasTarget) / parent->gasTarget / BASE_FEE_MAX_CHANGE_DENOMINATOR;
        if (feeDelta < 1) {
            feeDelta = 1;
        }
        return parent->baseFee + feeDelta;
    } else {
        uint64_t feeDelta = parent->baseFee * (parent->gasTarget - parent->gasUsed) /parent->gasTarget / BASE_FEE_MAX_CHANGE_DENOMINATOR;
        return parent->baseFee - feeDelta;
    }
}

// https://github.com/ethereum/EIPs/blob/master/EIPS/eip-1559.md
static inline void validateBlock(const block_t *block) {
    const block_t *parent = block->parent;
    assert(block->gasUsed <= block->gasTarget * ELASTICITY_MULTIPLIER);
    if (parent == NULL) {
        return;
    }
    assert(block->gasTarget <= parent->gasTarget * 1025 / 1024);
    assert(block->gasTarget >= parent->gasTarget * 1023 / 1024);
    assert(block->baseFee == nextBaseFee(parent));
    assert(block->timestamp > parent->timestamp);
    // TODO validate transactions
}
