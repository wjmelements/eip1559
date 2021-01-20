#include <assert.h>

#include "miner.h"


void test_miner_index() {
    initMiners(3);
    block_t *block0 = mineBlock(&miners[1], NULL, 1);
    assert(block0->miner == 1);
    assert(block0->parent == NULL);
    assert(block0->difficulty == 1);
    block_t *block1 = mineBlock(&miners[2], block0, 2);
    assert(block1->parent == block0);
    assert(block1->miner == 2);
    assert(block1->difficulty == 2);
}


extern void mergeSortTxs(vector<tx_t> &txs, uint64_t baseFee, uint64_t beginInclusive, uint64_t endExclusive);

#define assertSorted(txs, baseFee) \
    for (uint64_t i = 1; i < txs.size(); i++) { \
        false && printf("[%llu] = %llu, [%llu] = %llu\n", i - 1, effectiveBribe(&txs[i - 1], baseFee), i, effectiveBribe(&txs[i], baseFee)); \
        assert(effectiveBribe(&txs[i - 1], baseFee) <= effectiveBribe(&txs[i], baseFee));\
    }

void test_mergesort() {
    vector<tx_t> txs;
    mergeSortTxs(txs, 100, 0, 0);
    assertSorted(txs, 100);
    tx_t tx0 = { 100, 200, 1 };
    txs.push_back(tx0);
    mergeSortTxs(txs, 100, 0, 1);
    assertSorted(txs, 100);
    tx_t tx1 = { 101, 200, 2 };
    txs.push_back(tx1);
    mergeSortTxs(txs, 100, 0, 2);
    assertSorted(txs, 100);
    tx_t tx2 = { 200, 200, 3 };
    txs.push_back(tx2);
    mergeSortTxs(txs, 100, 0, 3);
    assertSorted(txs, 100);
    tx_t tx3 = { 201, 201, 4 };
    txs.push_back(tx3);
    mergeSortTxs(txs, 100, 0, 4);
    assertSorted(txs, 100);

    assert(txs[3] == tx3);

    txs.push_back({ 300, 400, 5 });
    txs.push_back({ 301, 401, 5 });
    txs.push_back({ 303, 403, 5 });
    txs.push_back({ 302, 402, 5 });
    mergeSortTxs(txs, 100, 0, 8);
    assertSorted(txs, 100);
}

void test_mineBlock() {
    initMiners(3);
    for (uint64_t i = 0; i < 10; i++) {
        submitTransaction({ 100 + i, 105 - i, 6000000 + i});
    }
    miners[0].strategy = FILL_BLOCK;
    miners[1].strategy = MIN_BASEFEE;
    block_t *genesis = mineBlock(&miners[0], NULL, 1000);
    assert(genesis != NULL);
    assert(genesis->txCount > 1);
    validateBlock(genesis);
    block_t *child = mineBlock(&miners[1], genesis, 1000);
    assert(child != NULL);
    assert(child->txCount < 2);
    validateBlock(child);
    return;
}


int main() {
    test_miner_index();
    test_mergesort();
    test_mineBlock();
    return 0;
}
