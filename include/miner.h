#include "block.h"

#include <vector>
using std::vector;


typedef enum strategy {
    FILL_BLOCK,
    MIN_BASEFEE,
    // MIN_BASEFEE_ONE_DEEP,
    // MIN_BASEFEE_PERIOD,
} strategy_t;


typedef struct miner {
    uint64_t hashrate;
    strategy_t strategy;
    const block_t *head;
} miner_t;

extern miner_t *miners;
extern uint8_t minerCount;
void initMiners(uint8_t count);
block_t *mineBlock(const miner_t *miner, const block_t *parent, uint64_t difficulty);
uint64_t *totalMinerRewards(const block_t *head);

void onBlock(const block_t *block);
void submitTransaction(tx_t tx);
vector<tx_t> *popTransactions(uint64_t gasLimit, uint64_t baseFee);
