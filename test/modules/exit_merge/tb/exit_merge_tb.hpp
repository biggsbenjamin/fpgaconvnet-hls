#ifndef EXIT_MERGE_TB_HPP_
#define EXIT_MERGE_TB_HPP_

#include "common.hpp"

#include "exit_merge_param.hpp"

/////////////////////////////////

void exit_merge_top(
    stream_t(data_t) in[EXIT_MERGE_EXITS],
    stream_t(data_t) &out
);

/////////////////////////////////

#endif
