#include "miner.h"

#include <string>
using std::to_string;


miner_t *miners;
uint16_t minerCount;
// Because block producer strategies may disregard certain blocks
// different block producer strategies have different pending transaction pools and heads
vector<tx_t> sortedTxs[NUM_STRATEGIES];  // sorted least to greatest, on-demand
const block_t *heads[NUM_STRATEGIES];
static const uint64_t TXPOOL_SLOTS = 6000;

void initMiners(uint16_t count) {
    if (miners != NULL) {
        free(miners);
    }
    miners = (miner_t *)calloc(count, sizeof(miner_t));
    minerCount = count;
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        heads[i] = NULL;
        sortedTxs[i].clear();
    }
}


const block *longestChain() {
    uint64_t bestTd = 0;
    const block *bestBlock = NULL;
    for (uint8_t i = 0; i < NUM_STRATEGIES; i++) {
        if (heads[i] == NULL) {
            continue;
        }
        if (heads[i]->td <= bestTd) {
            continue;
        }
        bestBlock = heads[i];
        bestTd = bestBlock->td;
    }
    return bestBlock;
}


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
        if (bribe0 < bribe1) {
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

static void insertTx(vector<tx_t> &txs, const block_t *parent, tx_t tx) {
    uint64_t index = txs.size();
    txs.push_back(tx);
}

void submitTransaction(tx_t tx) {
    for (uint8_t strategy = 0; strategy < NUM_STRATEGIES; strategy++) {
        insertTx(sortedTxs[strategy], heads[strategy], tx);
    }
}

static vector<tx_t> *popTransactions(strategy_t strategy, uint64_t gasLimit, uint64_t baseFee) {
    mergeSortTxs(sortedTxs[strategy], baseFee, 0, sortedTxs[strategy].size());
    if (sortedTxs[strategy].size() > TXPOOL_SLOTS) {
        sortedTxs[strategy].resize(TXPOOL_SLOTS);
    }
    uint64_t gasUsed = 0;
    vector<tx_t> *txs = new vector<tx_t>;
    do {
        auto tx = sortedTxs[strategy].crbegin();
        if (tx == sortedTxs[strategy].crend()) {
            return txs;
        }
        uint64_t gas = tx->gas;
        if (gas + gasUsed > gasLimit) {
            return txs;
        }
        gasUsed += gas;
        txs->push_back(*tx);
        sortedTxs[strategy].pop_back();
    } while(true);
}

void onBlock(const block_t *block) {
    miner_t *producer = miners + block->miner;
    printf("%s\t%3u\t%6llu\t@\t%6llu\t%8llu\t%8llu\t%9llu\n", strategyToName(producer->strategy), block->miner, block->height, block->timestamp, block->gasUsed, block->baseFee, block->totalFees);
    strategy_t producerStrategy = producer->strategy;
    //printf("[%p %p] %p\n", heads[0], heads[1], block);
    assert(heads[producerStrategy] == block);
    for (uint8_t strategy = 0; strategy < NUM_STRATEGIES; strategy++) {
        if (strategy == producerStrategy) {
            continue;
        }
        switch (strategy) {
            case MIN_BASEFEE:
                if (block->gasUsed > block->gasTarget) {
                    printf("Ignoring because gasUsed is too high\n");
                    continue;
                }
                // fallthrough
            case FILL_BLOCK:
                if (heads[strategy] != NULL && block->td <= heads[strategy]->td) {
                    continue;
                }
            break;
        }
        // rewind to new head
        heads[strategy] = block;
        // copy tx pool
        sortedTxs[strategy] = sortedTxs[producerStrategy];
    }
}

block_t *mineBlock(const miner_t *miner, uint64_t timestamp, uint64_t difficulty) {
    vector<tx_t> *txs;
    const block_t *parent = heads[miner->strategy];

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
            txs = popTransactions(miner->strategy, parentGasTarget * ELASTICITY_MULTIPLIER, baseFee);
        break;
        case MIN_BASEFEE:
            txs = popTransactions(miner->strategy, parentGasTarget, baseFee);
        break;
        default:
            assert(miner->strategy < NUM_STRATEGIES);
    }
    block_t *block = mallocBlock(txs->size());
    block->parent = parent;
    if (parent == NULL) {
        block->td = difficulty;
        block->height = 0;
    } else {
        block->td = parent->td + difficulty;
        assert(block->td > parent->td);
        block->height = parent->height + 1;
    }
    block->timestamp = timestamp;
    block->difficulty = difficulty;
    block->gasTarget = parentGasTarget;  // TODO test gas target changing strategies
    block->baseFee = baseFee;
    block->miner = miner - miners;

    block->gasUsed = 0;
    block->totalFees = BLOCK_REWARD; 
    for (uint64_t i = 0; i < block->txCount; i++) {
        tx_t tx = (*txs)[i];
        block->txs[i] = tx;
        block->gasUsed += tx.gas;
        block->totalFees += tx.gas * effectiveBribe(&tx, baseFee);
    }
    heads[miner->strategy] = block;
    return block;
}

mpz_class *totalMinerRewards(const block_t *head) {
    mpz_class *rewards = new mpz_class[minerCount];
    while (head) {
        assert(head->totalFees);
        mpz_class part(to_string(head->totalFees).c_str());
        rewards[head->miner] += part;
        head = head->parent;
    }
    return rewards;
}
