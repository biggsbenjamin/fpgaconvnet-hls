#include "fpgaconvnet.hpp"

int fpgaconvnet::timer_init()
{
	int Status;
	//	Timer Init
	timer_config = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
	Status = XScuTimer_CfgInitialize(&timer, timer_config,
			timer_config->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XScuTimer_LoadTimer(&timer, TIMER_LOAD_VALUE);
	XScuTimer_Start(&timer);

	return XST_SUCCESS;
}

void fpgaconvnet::timer_start()
{
	XScuTimer_Start(&timer);
	XScuTimer_RestartTimer(&timer);
}

float fpgaconvnet::timer_stop()
{
	long TmrCntValue;
	long clk_cyles;
	float t;	// Time elapsed

	TmrCntValue = XScuTimer_GetCounterValue(&timer);
	clk_cyles = TIMER_LOAD_VALUE-TmrCntValue;
	t = (clk_cyles*1000.0)/CLOCK_FREQ; // time in milliseconds

	return t;
}
