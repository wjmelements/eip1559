#include "miner.h"
#include "user.h"

#include <assert.h>
#include <stdlib.h>



static uint64_t transactorRate[NUM_PATTERNS];
static uint64_t transactorRateTotal;

void setUserPatternDistribution(uint64_t transactorDistribution[NUM_PATTERNS]) {
    transactorRateTotal = 0;
    for (uint8_t i = 0; i < NUM_PATTERNS; i++) {
        transactorRate[i] = transactorDistribution[i];
        transactorRateTotal += transactorRate[i];
    }
}


tx_t nextTx() {
    uint64_t chance = arc4random() % transactorRateTotal;
    uint8_t pattern;
    for (pattern = 0; pattern < NUM_PATTERNS; pattern++) {
        if (transactorRate[pattern] > chance) {
            break;
        }
        chance -= transactorRate[pattern];
    }
    assert(pattern < NUM_PATTERNS);
    const block_t *head = longestChain();
    uint64_t lastGasPrice;
    if (head == NULL) {
        lastGasPrice = 1000;
    } else {
        lastGasPrice = effectiveGasPrice(&head->txs[head->txCount - 1], head->baseFee);
    }
    uint64_t cap = 500000;
    if (lastGasPrice > cap) {
        cap = lastGasPrice;
    } else {
        cap -= arc4random() % cap;
    }
    cap += (arc4random() % cap) / 2;
    uint64_t bribe = cap;
    uint64_t gas = 21000;

    switch (pattern) {
        case PATIENT_TRANSFER:
            bribe /= 5;
            break;
        case PATIENT_ERC20:
            bribe /= 4;
            gas = 53000;
            break;
        case URGENT_SWAP:
            cap = cap * 11 / 10 + 1;
            gas = 153000;
            break;
        case AGGREGATOR_SWAPS:
            cap = cap * 10 / 9 + 1;
            gas = 590000;
            break;
        case GASTOKEN_MINT:
            bribe /= 10;
            cap /= 3;
            gas = 5700000;
            break;
        case CONTRACT_DEPLOYMENT:
            bribe /= 2;
            cap = cap * 5 / 4;
            gas = 9000000;
            break;
        default:
            assert(pattern && false);
            break;
    }
    return { bribe, cap, gas };
}
