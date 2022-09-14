#ifndef EXIT_MERGE_TB_HPP_
#define EXIT_MERGE_TB_HPP_

#include "common.hpp"

#include "exit_merge_param.hpp"

typedef b_data_t em_t;

/////////////////////////////////

void exit_merge_top(
    stream_t(em_t) in[EXIT_MERGE_EXITS],
    stream_t(em_t) &out
);

/////////////////////////////////

#endif
