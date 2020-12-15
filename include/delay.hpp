#ifndef DELAY_HPP_
#define DELAY_HPP_

#include "common.hpp"
#include "ap_utils.h"

template<typename T, int DELAY>
void delay(
  stream_t(T) &in,
  stream_t(T) &out
)
{
	DO_PRAGMA(HLS PIPELINE II=DELAY)
	#pragma HLS unroll region

	out.write(in.read());

	delay_loop: for(int j=0;j<DELAY-1;j++)
	{
	  ap_wait();
	}
}

template<typename T, int DELAY,int KERNEL_SIZE>
void delay3D(
  stream_t(T) in[KERNEL_SIZE][KERNEL_SIZE],
  stream_t(T) out[KERNEL_SIZE][KERNEL_SIZE]
)
{
    #pragma inline off
    DO_PRAGMA(HLS PIPELINE II=DELAY)

    for(int k1=0;k1<KERNEL_SIZE;k1++)
    {
      for(int k2=0;k2<KERNEL_SIZE;k2++)
      {
        out[k1][k2].write(in[k1][k2].read());
      }
    }

    delay_loop: for(int j=0;j<DELAY-1;j++)
    {
      ap_wait();
    }
}

template<typename T, int DELAY,int COARSE_IN, int COARSE_OUT>
void delay3D(
  stream_t(T) in[COARSE_IN][COARSE_OUT],
  stream_t(T) out[COARSE_IN][COARSE_OUT]
)
{
    #pragma inline off
    DO_PRAGMA(HLS PIPELINE II=DELAY)

    for(int k1=0;k1<COARSE_IN;k1++)
    {
      for(int k2=0;k2<COARSE_OUT;k2++)
      {
        out[k1][k2].write(in[k1][k2].read());
      }
    }

    delay_loop: for(int j=0;j<DELAY-1;j++)
    {
      ap_wait();
    }
}
#endif
