#include "randstate.h"

gmp_randstate_t state;

//初始化gmp的随机数生成器
void randstate_init(uint64_t seed) {
  gmp_randinit_mt(state);
  gmp_randseed_ui(state, seed);
}

//清除随机数生成器state
void randstate_clear() { gmp_randclear(state); }
