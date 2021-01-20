#include <assert.h>

#include "miner.h"


void test_miner_index() {
    initMiners(3);
    block_t *block0 = mineBlock(&miners[1], NULL);
    assert(block0->miner == 1);
    assert(block0->parent == NULL);
    block_t *block1 = mineBlock(&miners[2], block0);
    assert(block1->parent == block0);
    assert(block1->miner == 2);
}

int main() {
    test_miner_index();
    return 0;
}
