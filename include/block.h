#include "tx.h"
#include <stdlib.h>

typedef struct block {
    struct block *parent;
    uint64_t difficulty;
    uint64_t td;
    uint64_t baseFee;
    uint64_t totalFees;
    uint8_t miner;
    uint8_t transactions;
    tx_t txs[0];
} block_t;


static inline block_t *mallocBlock(uint32_t txCount) {
    return (block_t *)malloc(sizeof(block_t) + sizeof(tx_t) * txCount);
}
