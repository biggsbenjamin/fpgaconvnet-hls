#include "fpgaconvnet.hpp"

int fpgaconvnet::hw_init()
{
	int Status;

	hw_config->DeviceId = HW_ID;
	hw_config->Ctrl_BaseAddress = HW_BASE_ADDR;

	Status = XFpgaconvnet_ip_CfgInitialize(&hw, hw_config);
	if (Status != XST_SUCCESS) {
		xil_printf("CNN core CFG initialisation failed (Status = %d)\r\n", Status);
		return XST_FAILURE;
	}
	XFpgaconvnet_ip_InterruptGlobalEnable(&hw);

	return XST_SUCCESS;
}

int 	fpgaconvnet::hw_check_idle()  { return XFpgaconvnet_ip_IsIdle(&hw); }
int 	fpgaconvnet::hw_check_done()  { return XFpgaconvnet_ip_IsDone(&hw); }
int 	fpgaconvnet::hw_check_ready() { return XFpgaconvnet_ip_IsReady(&hw); }
void 	fpgaconvnet::hw_start() { XFpgaconvnet_ip_Start(&hw); }


void fpgaconvnet::hw_make_file_name(char* dest, char* str, u8 i)
{
	char aux[20];
	char number[20];

	strcpy(aux,str);
	itoa(i,number,10);
	strcat(aux,number);
	strcat(aux,".bin");
	strcpy(dest,aux);
}

u32 fpgaconvnet::hw_array_max(u32* array, int length)
{
	u32 max_value = 0;
	int i;

	for(i=0;i<length;i++)
	{
		if(array[i] > max_value)
			max_value = array[i];
	}

	return max_value;
}
