#ifndef TX_H
#define TX_H

#include <stdint.h>

typedef struct tx {
    uint64_t maxBribe;
    uint64_t cap;
    uint64_t gas;
} tx_t;

static inline bool operator==(const tx& lhs, const tx& rhs) {
    return lhs.maxBribe == rhs.maxBribe 
        && lhs.cap == rhs.cap
        && lhs.gas == rhs.gas;
}



static inline uint64_t effectiveBribe(const tx_t *tx, uint64_t baseFee) {
    if (tx->cap <= baseFee) {
        return 0;
    }
    uint64_t bribe = tx->cap - baseFee;
    if (tx->maxBribe < bribe) {
        return tx->maxBribe;
    }
    return bribe;
}

static inline uint64_t effectiveGasPrice(const tx_t *tx, uint64_t baseFee) {
    return effectiveBribe(tx, baseFee) + baseFee;
}


#endif
