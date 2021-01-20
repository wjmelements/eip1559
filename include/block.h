#include "tx.h"

#include <assert.h>
#include <stdlib.h>

typedef struct block {
    struct block *parent;
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
    return (block_t *)malloc(sizeof(block_t) + sizeof(tx_t) * txCount);
}

static const uint64_t ELASTICITY_MULTIPLIER = 2;
static const uint64_t BASE_FEE_MAX_CHANGE_DENOMINATOR = 8;

// https://github.com/ethereum/EIPs/blob/master/EIPS/eip-1559.md
static inline void validateBlock(block_t *block) {
    block_t *parent = block->parent;
    assert(block->gasUsed <= block->gasTarget * ELASTICITY_MULTIPLIER);
    if (parent == NULL) {
        return;
    }
    assert(block->gasTarget <= parent->gasTarget * 1025 / 1024);
    assert(block->gasTarget >= parent->gasTarget * 1023 / 1024);
    if (parent->gasUsed == parent->gasTarget) {
        assert(block->gasTarget == parent->gasTarget);
    } else if (parent->gasUsed > parent->gasTarget) {
        uint64_t feeDelta = parent->baseFee * (parent->gasUsed - parent->gasTarget) / parent->gasTarget / BASE_FEE_MAX_CHANGE_DENOMINATOR;
        if (feeDelta < 1) {
            feeDelta = 1;
        }
        assert(block->baseFee == parent->baseFee + feeDelta);
    } else {
        uint64_t feeDelta = parent->baseFee * (parent->gasTarget - parent->gasUsed) /parent->gasTarget / BASE_FEE_MAX_CHANGE_DENOMINATOR;
        assert(block->baseFee == parent->baseFee - feeDelta);
    }
    // TODO validate transactions
}
