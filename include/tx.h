#include <stdint.h>

typedef struct tx {
    uint64_t maxBribe;
    uint64_t cap;
    uint64_t gas;
} tx_t;


static inline uint64_t effectiveBribe(tx_t *tx, uint64_t baseFee) {
    if (tx->cap > baseFee) {
        return 0;
    }
    uint64_t bribe = tx->cap - baseFee;
    if (tx->maxBribe < bribe) {
        return tx->maxBribe;
    }
    return bribe;
}

static inline uint64_t effectiveGasPrice(tx_t *tx, uint64_t baseFee) {
    return effectiveBribe(tx, baseFee) + baseFee;
}
