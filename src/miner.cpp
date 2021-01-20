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

vector<tx_t> sorted_txs[NUM_STRATEGIES];
const block_t *head[NUM_STRATEGIES];


void mergeSortTxs(vector<tx_t> &txs, uint64_t baseFee, uint64_t beginInclusive, uint64_t endExclusive) {
    if (endExclusive - beginInclusive < 2) {
        return;
    }
    uint64_t split = (beginInclusive + endExclusive) / 2;
    mergeSortTxs(txs, baseFee, beginInclusive, split);
    mergeSortTxs(txs, baseFee, split, endExclusive);
    // inline merge
    for (uint64_t i = beginInclusive; i < endExclusive && split < endExclusive; i++) {
        uint64_t bribe0 = effectiveBribe(&txs[i], baseFee);
        uint64_t bribe1 = effectiveBribe(&txs[split], baseFee);
        if (bribe0 >= bribe1) {
            continue;
        }
        // shift
        tx_t least = txs[split];
        for (uint64_t j = split + 1; --j > i;) {
            txs[j] = txs[j - 1];
        }
        
        txs[i] = least;
        split++;
    }
}

void insertTx(vector<tx_t> &txs, const block_t *parent, tx_t tx) {
    uint64_t baseFee = nextBaseFee(parent);
    uint64_t index = txs.size();
    txs.push_back(tx);
    mergeSortTxs(txs, baseFee, 0, txs.size());
}

void submitTransaction(tx_t tx) {
    for (uint8_t strategy = 0; strategy < NUM_STRATEGIES; strategy++) {
        insertTx(sorted_txs[strategy], head[strategy], tx);
    }
}
vector<tx_t> *popTransactions(uint64_t gasLimit, uint64_t baseFee) {
    return new vector<tx_t>; // TODO
}
void onBlock(const block_t *block) {

}

block_t *mineBlock(const miner_t *miner, const block_t *parent, uint64_t difficulty) {
    vector<tx_t> *txs;

    uint64_t baseFee;
    uint64_t parentGasTarget;
    if (parent == NULL) {
        parentGasTarget = 10000000;
        baseFee = 1000000000;
    } else {
        parentGasTarget = parent->gasTarget;
        baseFee = nextBaseFee(parent);
    }
    switch (miner->strategy) {
        case FILL_BLOCK:
            txs = popTransactions(parentGasTarget * ELASTICITY_MULTIPLIER, baseFee);
        break;
        case MIN_BASEFEE:
            txs = popTransactions(parentGasTarget, baseFee);
        break;
        default:
            assert(miner->strategy < NUM_STRATEGIES);
    }
    block_t *block = mallocBlock(txs->size());
    block->parent = parent;
    if (parent == NULL) {
        block->td = difficulty;
    } else {
        block->td = parent->td + difficulty;
    }
    block->difficulty = difficulty;
    block->gasTarget = parentGasTarget;  // TODO test gas target changing strategies
    block->baseFee = baseFee;
    block->miner = miner - miners;

    block->gasUsed = 0;
    block->totalFees = 0;
    for (uint64_t i = 0; i < block->txCount; i++) {
        tx_t tx = (*txs)[i];
        block->txs[i] = tx;
        block->gasUsed += tx.gas;
        block->totalFees += tx.gas * effectiveBribe(&tx, baseFee);
    }
    return block;
}

uint64_t *totalMinerRewards(const block_t *head) {
    uint64_t *rewards = (uint64_t *)calloc(minerCount, sizeof(uint64_t));
    while (head) {
        rewards[head->miner] += head->totalFees;
        head = head->parent;
    }
    return rewards;
}
