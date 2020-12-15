#include "fpgaconvnet.hpp"

int fpgaconvnet::sd_init()
{
	FRESULT rc;

	rc = f_mount(&fatfs,"",1);
	if (rc) {
		xil_printf(" ERROR : f_mount returned %d\r\n", rc);
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

FSIZE_t fpgaconvnet::sd_file_size(char *filename)
{
	FIL fil;
	FRESULT rc;
	FSIZE_t length = 0;

	// open file
	rc = f_open(&fil, filename, FA_READ);
	if (rc) {
		xil_printf(" ERROR : %s f_open returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}

	// get length
	length=f_size(&fil);

	// close file
	rc = f_close(&fil);
	if (rc) {
		xil_printf(" ERROR : %s f_close returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}
	// return length
	return length;
}

int fpgaconvnet::sd_read(char *filename, void* dest_addr, u32 byte_len=0)
{
	FIL fil;
	FRESULT rc;
	uint length = 0;
	uint length2 = 0;

	rc = f_open(&fil, filename, FA_READ);
	if (rc) {
		xil_printf(" ERROR : %s f_open returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}
	length=f_size(&fil);

	rc = f_lseek(&fil, 0);
	if (rc) {
		xil_printf(" ERROR : %s f_lseek returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}

	rc = f_read(&fil, dest_addr, length, &length2);
	if (rc) {
		xil_printf(" ERROR : %s f_read returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}

	if((length2 != byte_len)&&(byte_len != 0))
	{
		xil_printf(" ERROR: File: '%s', Expected size: %d; Actual size: %d\r\n", filename, byte_len, length2);
		return XST_FAILURE;
	}

	rc = f_close(&fil);
	if (rc) {
		xil_printf(" ERROR : %s f_close returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}
	if (FPGACONVNET_DEBUG) {
		xil_printf("(DEBUG) SD card: File '%s' read. Length: %d; Addr.: 0x%08x\r\n", filename, length, dest_addr);
	}
	return XST_SUCCESS;
}

int fpgaconvnet::sd_read_config_num_partitions(char *filename)
{
	FIL fil;
	FRESULT rc;
	char buffer[FPGACONVNET_FILE_BUFFER_SIZE];

	rc = f_open(&fil, filename, FA_READ);
	if (rc) {
		xil_printf(" ERROR : %s f_open returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}

	rc = f_lseek(&fil, 0);
	if (rc) {
		xil_printf(" ERROR : %s f_lseek returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}

	int length=f_size(&fil);
	uint length2 = 0;

	rc = f_read(&fil, buffer, length, &length2);
	if (rc) {
		xil_printf(" ERROR : %s f_read returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}

    int num_partitions = 0;
    // iterate over the buffer
    for(int i=0;i<length;i++) {
    	if (buffer[i] == '\n') {
    		num_partitions++;
    	}
    }

	rc = f_close(&fil);
	if (rc) {
		xil_printf(" ERROR : %s f_close returned %d\r\n", filename, rc);
		return XST_FAILURE;
	}
   
    return num_partitions;

}

int fpgaconvnet::sd_read_config(char *filename)
{
    FIL fil;
    FRESULT rc;
    char buffer[FPGACONVNET_FILE_BUFFER_SIZE];

    rc = f_open(&fil, filename, FA_READ);
    if (rc) {
        xil_printf(" ERROR : %s f_open returned %d\r\n", filename, rc);
        return XST_FAILURE;
    }

    rc = f_lseek(&fil, 0);
    if (rc) {
        xil_printf(" ERROR : %s f_lseek returned %d\r\n", filename, rc);
        return XST_FAILURE;
    }

    int length=f_size(&fil);
    uint length2 = 0;

    rc = f_read(&fil, buffer, length, &length2);
    if (rc) {
        xil_printf(" ERROR : %s f_read returned %d\r\n", filename, rc);
        return XST_FAILURE;
    }

    // buffer for parameters
    char * param;

    // iterate over parameters
    for (int i=0; i<n_partitions;i++) {

        partition_info[i].partition_id 	= (i==0) ? atoi(strtok(buffer,",")) : atoi(strtok(NULL,",")); // partition id
        partition_info[i].ports_in 	= atoi(strtok(NULL,",")); // ports in
        partition_info[i].ports_out	= atoi(strtok(NULL,",")); // ports out
        int input_addr 	                = atoi(strtok(NULL,",")); // input address
        int output_addr                 = atoi(strtok(NULL,",")); // output address
        partition_info[i].input_dep	= atoi(strtok(NULL,",")); // input dependence
        partition_info[i].output_dep	= atoi(strtok(NULL,",")); // output dependence
        partition_info[i].wr_factor	= atoi(strtok(NULL,",")); // weights reloading factor
        partition_info[i].batch_size	= atoi(strtok(NULL,",")); // batch size
        partition_info[i].size_in	= atoi(strtok(NULL,",")); // input size
        partition_info[i].size_out	= atoi(strtok(NULL,",")); // output size
        partition_info[i].size_wr	= atoi(strtok(NULL,"\n")); // weights size

    }

    /*
    // split parameters
    char * param = strtok(row,",");

    partition_info[partition_index].batch_size 	= atoi(param);
    param = strtok(NULL,",");

    partition_info[partition_index].rows_in  	= atoi(param);
    param = strtok(NULL,",");
    partition_info[partition_index].cols_in  	= atoi(param);
    param = strtok(NULL,",");
    partition_info[partition_index].channels_in = atoi(param);
    param = strtok(NULL,",");
    partition_info[partition_index].coarse_in 	= atoi(param);
    param = strtok(NULL,",");

    partition_info[partition_index].rows_out  	= atoi(param);
    param = strtok(NULL,",");
    partition_info[partition_index].cols_out  	= atoi(param);
    param = strtok(NULL,",");
    partition_info[partition_index].channels_out= atoi(param);
    param = strtok(NULL,",");
    partition_info[partition_index].coarse_out 	= atoi(param);
    param = strtok(NULL,",");

    strcpy(partition_info[partition_index].partition_filename , param);
    param = strtok(NULL,",");
    strcpy(partition_info[partition_index].input_filename , param);
    param = strtok(NULL,",");
    strcpy(partition_info[partition_index].output_filename, param);
    param = strtok(NULL,",");
    //free(param);
    */

    rc = f_close(&fil);
    if (rc) {
        xil_printf(" ERROR : %s f_close returned %d\r\n", filename, rc);
        return XST_FAILURE;
    }


    return XST_SUCCESS;
}

int fpgaconvnet::sd_write(char *filename, port_t* start_addr, u32 byte_len)
{
    FIL fil;
    FRESULT rc;
    uint length;

    rc = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (rc) {
        xil_printf(" ERROR : %s f_open returned %d\r\n", filename, rc);
        return XST_FAILURE;
    }
    //length=file_size(&fil);

    rc = f_lseek(&fil, 0);
    if (rc) {
        xil_printf(" ERROR : %s f_lseek returned %d\r\n", filename, rc);
        return XST_FAILURE;
    }

    rc = f_write(&fil, start_addr, byte_len, &length);
    if (rc) {
        xil_printf(" ERROR : %s f_write returned %d\r\n", filename, rc);
        return XST_FAILURE;
    }

    if(length != byte_len)
    {
        xil_printf("Expected written bytes: %d. Written bytes: %d\r\n", byte_len, length);
        return XST_FAILURE;
    }

    rc = f_close(&fil);
    if (rc) {
        xil_printf(" ERROR : %s f_close returned %d\r\n", filename, rc);
        return XST_FAILURE;
    }
    if (FPGACONVNET_DEBUG) {
        xil_printf("(DEBUG) SD card: File '%s' written. Length: %d\r\n", filename, length);
    }
    return XST_SUCCESS;
}
