#ifndef ID_PIPELINE_HPP_
#define ID_PIPELINE_HPP_

#include "common.hpp"

/**
 * ID PIPELINE
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FIRST_STAGE_CYCLES,
    unsigned int EARLY_STAGE_CYCLES,
    unsigned int LATE_STAGE_CYCLES
> //this will be rows/cols/channels of the input

void id_pipeline(
    stream_t(data_t) &in,
    stream_t(data_t) &ctrl,
    stream_t(uint16_t) &out_early,
    stream_t(uint16_t) &out_late
)
{

#pragma HLS INLINE OFF //prevents functions being inlined
    
    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int sample_size  = ROWS*COLS*CHANNELS;
    const unsigned int first_stage  = FIRST_STAGE_CYCLES;
    const unsigned int first_stage_m1  = FIRST_STAGE_CYCLES-1;
    const unsigned int early_stage  = EARLY_STAGE_CYCLES;
    const unsigned int early_stage_m1  = EARLY_STAGE_CYCLES-1;
    const unsigned int late_stage   = LATE_STAGE_CYCLES;
    const unsigned int late_stage_m1   = LATE_STAGE_CYCLES-1;
    const unsigned int max_val = 0xFFFF;
 
#pragma HLS STREAM variable=in 
#pragma HLS STREAM variable=ctrl 
//DO_PRAGMA(HLS STREAM variable=ctrl depth=batch_size)
#pragma HLS STREAM variable=out_early 
#pragma HLS iSTREAM variable=out_late

    uint16_t b_index=0;
    uint16_t batch_id_in, fs_tmp, es_tmp, ls_tmp;
    data_t ctrl_tmp;

    uint16_t mem_first_stage[first_stage] = {max_val};
#pragma HLS RESOURCE variable=mem_first_stage core=RAM_T2P_BRAM latency=1
    uint16_t mem_early_stage[early_stage] = {max_val};
#pragma HLS RESOURCE variable=mem_early_stage core=RAM_T2P_BRAM latency=1
    uint16_t mem_late_stage[late_stage] = {max_val};
#pragma HLS RESOURCE variable=mem_late_stage core=RAM_T2P_BRAM latency=1

    unsigned int fs_wr=0; //TODO pick better counter sizes
    unsigned int fs_rd=1;
    unsigned int es_wr=0;
    unsigned int es_rd=1;
    unsigned int ls_wr=0;
    unsigned int ls_rd=1;

    //batch_loop: for(uint32_t b_index=0;b_index<batch_size;b_index++) 
    while (b_index<batch_size)
    {
        //insert into first stage
        if (in.read_nb(batch_id)) {
            mem_first_stage[fs_rd] = batch_id;
        } else {
            mem_first_stage[fs_rd] = max_val;
        }

        //read values from id piplines
        fs_tmp = mem_first_stage[fs_rd];
        es_tmp = mem_early_stage[es_rd];
        ls_tmp = mem_early_stage[ls_rd];
        
        //write from first stage to early or late, depending on control
        if (ctrl.read_nb(ctrl_tmp)) { //if the timing is wrong, everything breaks
            if (ctrl_tmp == 0.0) { //insert batch id into short path
                mem_early_stage[es_wr] = max_val;
                mem_late_stage[ls_wr] = fs_tmp;
            } else { //insert batch id into short path
                mem_early_stage[es_wr] = fs_tmp;
                mem_late_stage[ls_wr] = max_val;
            }
        } else {
            mem_early_stage[es_wr] = max_val;
            mem_late_stage[ls_wr] = max_val;
        }

        //write out batch id from early/late pipelines when they arrive
        if (es_tmp != max_val) {
            out_early.write_nb(es_tmp); 
            b_index++;
        } 
        if (ls_tmp != max_val) {
            out_late.write_nb(ls_tmp); 
            b_index++;
        } 

        //increment read and write pointers for id pipelines
        fs_wr++;
        fs_rd++;
        es_wr++;
        es_rd++;
        ls_wr++;
        ls_rd++;

        //wrap values around to zero when max latency size reached
        if (fs_wr >= first_stage_m1) {
            fs_wr = 0;
        }
        if (fs_rd >= first_stage_m1) {
            fs_rd = 0;
        }
        if (es_wr >= early_stage_m1) {
            es_wr = 0;
        }
        if (es_rd >= early_stage_m1) {
            es_rd = 0;
        }
        if (ls_wr >= late_stage_m1) {
            ls_wr = 0;
        }
        if (ls_rd >= late_stage_m1) {
            ls_rd = 0;
        }
    }
}

#endif


