#include "common_tb.hpp"
#include "delay.hpp"

int main()
{

	hls::stream<data_t> in;
	hls::stream<data_t> out;

	for(int i=0;i<DELAY_TEST_SIZE;i++)
    in.write(0);

	delay_top(in,out,0);

	for(int i=0;i<DELAY_TEST_SIZE;i++)
		out.read();

  return 0;

}
