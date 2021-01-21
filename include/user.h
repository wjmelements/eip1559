#include "tx.h"

typedef enum userPattern {
    PATIENT_TRANSFER,
    PATIENT_ERC20,
    URGENT_SWAP,
    AGGREGATOR_SWAPS,
    GASTOKEN_MINT,
    CONTRACT_DEPLOYMENT,
    NUM_PATTERNS,
} userPattern_t;


tx_t nextTx();
void setUserPatternDistribution(uint64_t transactorDistribution[NUM_PATTERNS]);
