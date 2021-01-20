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
        assert(effectiveBribe(&txs[i - 1], baseFee) >= effectiveBribe(&txs[i], baseFee));\
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

    assert(txs[0] == tx3);

    txs.push_back({ 300, 400, 5 });
    txs.push_back({ 301, 401, 5 });
    txs.push_back({ 303, 403, 5 });
    txs.push_back({ 302, 402, 5 });
    mergeSortTxs(txs, 100, 0, 8);
    assertSorted(txs, 100);
}


int main() {
    test_miner_index();
    test_mergesort();
    return 0;
}
