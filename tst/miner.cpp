#include <assert.h>

#include "miner.h"


void test_miner_index() {
    initMiners(3);
    block_t *block = mineBlock(&miners[1], NULL);
    assert(block->miner == 1);
}

int main() {
    test_miner_index();
    return 0;
}
