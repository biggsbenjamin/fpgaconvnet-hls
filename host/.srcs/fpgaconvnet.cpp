#include "fpgaconvnet.hpp"

int fpgaconvnet::config_init(char * filename, bool preload_weights = false, bool preload_partitions = false) {

    // update the number of partitions
    n_partitions = sd_read_config_num_partitions(filename);
    partition_info = new partition_info_t[n_partitions];

    // load config from csv
    sd_read_config(filename);

    // iterate over the partitions
    for (int i=0; i<n_partitions; i++) {

    	// assign filenames
    	sprintf(partition_info[i].partition_filename, "p%d.bin", i);
    	sprintf(partition_info[i].input_filename	, "i%d.bin", i);
    	sprintf(partition_info[i].output_filename	, "o%d.bin", i);
	// iterate over weights reloading factor
    	for (int j=0; j<partition_info[i].wr_factor; j++) {
            sprintf(partition_info[i].weights_filename[j], "w%d_%d.bin", i, j);
    	}

    	// set flags
    	partition_info[i].INPUT_ADDR_ASSIGNED	    = false;
    	partition_info[i].OUTPUT_ADDR_ASSIGNED      = false;
    	partition_info[i].WEIGHTS_ADDR_ASSIGNED	    = false;
    	partition_info[i].PARTITION_ADDR_ASSIGNED   = false;
    	partition_info[i].PARTITION_DONE 	    = false;
    	partition_info[i].LOAD_PARTITION 	    = true;
    	partition_info[i].GEN_INPUT		    = false;
    	partition_info[i].LOAD_INPUT		    = (i==0) ? true : false;
    	partition_info[i].LOAD_WEIGHTS		    = true;
    	partition_info[i].LOAD_FIRST_WEIGHTS	    = true;
    	partition_info[i].UNLOAD_WEIGHTS	    = true;
    	partition_info[i].SAVE_OUTPUT		    = (i==n_partitions-1) ? true : false;

    	// reset timing of each partition
    	partition_info[i].t_load_input 	    = 0.0;
    	partition_info[i].t_load_partition  = 0.0;
    	partition_info[i].t_read_weights_sd = 0.0;
    	partition_info[i].t_load_weights_pl = 0.0;
    	partition_info[i].t_run_partition   = 0.0;
    	partition_info[i].t_save_output     = 0.0;

    	// get size of bitstream
    	partition_info[i].size_partition = sd_file_size(partition_info[i].partition_filename);

    	// preload weights
    	if ( preload_weights ) {
            // clear load weights flag
            partition_info[i].LOAD_WEIGHTS 	= false;
            partition_info[i].UNLOAD_WEIGHTS    = false;
            // set weights address assigned flag
            partition_info[i].WEIGHTS_ADDR_ASSIGNED = true;
            // iterate over weights reloading factor
            for (int j=0; j<partition_info[i].wr_factor; j++) {
                // assign memory for weights
                partition_info[i].data_wr_ptr[j] = (port_t *) malloc( partition_info[i].size_wr * sizeof(port_t) );
                //read weights to DDR
                sd_read(
                    partition_info[i].weights_filename[j],
                    partition_info[i].data_wr_ptr[j],
                    partition_info[i].size_wr * sizeof(port_t)
                );
            }
    	}

    	// preload partitions
    	if ( preload_partitions ) {
            // assign partition address space
            partition_info[i].partition_ptr = (u32 *) malloc( partition_info[i].size_partition );
            partition_info[i].PARTITION_ADDR_ASSIGNED = true;
            // load from file
            sd_read(
                partition_info[i].partition_filename,
                partition_info[i].partition_ptr,
                partition_info[i].size_partition
            );
    	}

    }

    return XST_SUCCESS;
}

int fpgaconvnet::init(char * filename) {

    // general setup
    Xil_DCacheFlush();
    Xil_DCacheDisable();

    // initialise libraries
    int reconf_success 	= reconf_init();
    int timer_success  	= timer_init();
    int sd_success		= sd_init();
    int hw_success		= hw_init();
    int config_success	= config_init(filename, true, true);

    printf("\n\r------\t Initialisation \t\t------\n\r");
    printf("------ hardware \t: %s \t\t------\n\r",(hw_success		==XST_SUCCESS) ? "DONE" : "FAILED");
    printf("------ timer    \t: %s \t\t------\n\r",(timer_success	==XST_SUCCESS) ? "DONE" : "FAILED");
    printf("------ sd card  \t: %s \t\t------\n\r",(sd_success		==XST_SUCCESS) ? "DONE" : "FAILED");
    printf("------ reconfig \t: %s \t\t------\n\r",(reconf_success	==XST_SUCCESS) ? "DONE" : "FAILED");
    printf("------ config   \t: %s \t\t------\n\r",(config_success	==XST_SUCCESS) ? "DONE" : "FAILED");
    return 0;
}

int fpgaconvnet::config_print(void) {

    printf("------------------------------------------------------------------------------\n\r");
    printf("------\t\t Printing Configuration Information \t\t\t------\n\r");
    printf("------------------------------------------------------------------------------\n\r");
    for( int i=0; i<n_partitions; i++ ) {
        printf("------\t\t Partition ID : ( %d ) \t\t\t\t\t------\n\r", partition_info[i].partition_id);
        printf("------\t \t | (in) \t| (out) \t| (wr) \t\t| \t------\n\r");
        printf("------\t ports : | %d \t\t| %d \t\t| %d \t\t| \t------\n\r"    , partition_info[i].ports_in      , partition_info[i].ports_out   , partition_info[i].ports_wr );
        printf("------\t size  : | %010d \t| %010d \t| %010d \t| \t------\n\r" , partition_info[i].size_in       , partition_info[i].size_out    , partition_info[i].size_wr  );
        printf("------\t addr  : | 0x%08X \t| 0x%08X \t| \t - \t| \t------\n\r", partition_info[i].data_in_ptr   , partition_info[i].data_out_ptr );
        printf("------\t name  : | %s \t| %s \t| \t - \t| \t------\n\r"        , partition_info[i].input_filename, partition_info[i].output_filename );
        printf("------\t (parameters) \t\t\t\t\t\t\t------\n\r");
        printf("------\t batch size  \t\t\t\t: %d \t\t\t------\n\r"           , partition_info[i].batch_size);
        printf("------\t weights reloading factor  \t\t: %d \t\t\t------\n\r" , partition_info[i].wr_factor );
        printf("------\t (flags) \t\t\t\t\t\t\t------\n\r");
        printf("------\t LOAD PARTITION \t: %s  \t\t\t\t------\n\r"     , (partition_info[i].LOAD_PARTITION)     ? "TRUE" : "FALSE");
        printf("------\t GEN INPUT \t\t: %s  \t\t\t\t------\n\r"        , (partition_info[i].GEN_INPUT)          ? "TRUE" : "FALSE");
        printf("------\t LOAD INPUT \t\t: %s  \t\t\t\t------\n\r"       , (partition_info[i].LOAD_INPUT)         ? "TRUE" : "FALSE");
        printf("------\t LOAD WEIGHTS \t\t: %s  \t\t\t\t------\n\r"     , (partition_info[i].LOAD_WEIGHTS)       ? "TRUE" : "FALSE");
        printf("------\t LOAD FIRST WEIGHTS \t: %s  \t\t\t\t------\n\r" , (partition_info[i].LOAD_FIRST_WEIGHTS) ? "TRUE" : "FALSE");
        printf("------\t UNLOAD WEIGHTS \t: %s  \t\t\t\t------\n\r"     , (partition_info[i].UNLOAD_WEIGHTS)     ? "TRUE" : "FALSE");
        printf("------\t SAVE OUTPUT \t\t: %s  \t\t\t\t------\n\r"      , (partition_info[i].SAVE_OUTPUT)        ? "TRUE" : "FALSE");
        printf("------------------------------------------------------------------------------\n\r");
    }

    return 0;
}

int fpgaconvnet::profiling_print(void) {

    printf("--------------------------------------------------------------\n\r");
    printf("------\t\t Printing Profiling Information \t------\n\r");
    printf("--------------------------------------------------------------\n\r");

    for( int i=0; i<n_partitions; i++ ) {

        float total_time =
            partition_info[i].t_load_input +
            partition_info[i].t_load_partition +
            partition_info[i].t_read_weights_sd * partition_info[i].wr_factor +
            partition_info[i].t_load_weights_pl * partition_info[i].wr_factor +
            partition_info[i].t_run_partition   * partition_info[i].wr_factor +
            partition_info[i].t_save_output;

        printf("------\t\t Partition ID : ( %d ) \t\t\t------\n\r"			, partition_info[i].partition_id     );
        printf("------\t load input time \t\t: %0.3f (ms) \t------\n\r"			, partition_info[i].t_load_input     );
        printf("------\t load partition time \t\t: %0.3f (ms) \t------\n\r"		, partition_info[i].t_load_partition );
        printf("------\t read weights from SD card (avg): %0.3f (ms) \t------\n\r"	, partition_info[i].t_read_weights_sd);
        printf("------\t write weights to PL (avg) \t: %0.3f (ms) \t------\n\r"		, partition_info[i].t_load_weights_pl);
        printf("------\t running partition (avg) \t: %0.3f (ms) \t------\n\r"		, partition_info[i].t_run_partition  );
        printf("------\t saving output \t\t\t: %0.3f (ms) \t------\n\r"			, partition_info[i].t_save_output    );
        printf("--------------------------------------------------------------\n\r");
        printf("------\t TOTAL TIME \t\t\t: %0.3f (ms) \t------\n\r", total_time);
        printf("--------------------------------------------------------------\n\r");

    }

}

int fpgaconvnet::unassign_address_space(void) {

    // iterate over the partitions
    for (int i=0; i<n_partitions; i++) {

    	// unassign input
    	if ( partition_info[i].INPUT_ADDR_ASSIGNED ) {
	    free ( partition_info[i].data_in_ptr );
	    partition_info[i].INPUT_ADDR_ASSIGNED = false;
    	}

    	// unassign output
    	if ( partition_info[i].OUTPUT_ADDR_ASSIGNED ) {
	    free ( partition_info[i].data_out_ptr );
	    partition_info[i].OUTPUT_ADDR_ASSIGNED = false;
    	}

    	// unassign weights
    	if ( partition_info[i].WEIGHTS_ADDR_ASSIGNED ) {
    	    for (int j=0; j<partition_info[i].wr_factor;j++) {
    	        free ( partition_info[i].data_wr_ptr[j] );
    	    }
	    partition_info[i].WEIGHTS_ADDR_ASSIGNED = false;
    	}

    	// unassign partition
    	if ( partition_info[i].PARTITION_ADDR_ASSIGNED ) {
	    free ( partition_info[i].partition_ptr );
	    partition_info[i].PARTITION_ADDR_ASSIGNED = false;
    	}

    }

    return XST_SUCCESS;
}

int fpgaconvnet::free_featuremap_space(void) {

    // iterate over the partitions
    for (int i=0; i<n_partitions; i++) {
    	if ( partition_info[i].INPUT_ADDR_ASSIGNED ) {
            // find input partition
            int input_partition  = partition_info[i].input_dep;
            // free up input space
            if ( partition_info[i].PARTITION_DONE && partition_info[input_partition].PARTITION_DONE ) {
                free ( partition_info[i].data_in_ptr );
                partition_info[i].INPUT_ADDR_ASSIGNED = false;
            }
    	}
    	if ( partition_info[i].OUTPUT_ADDR_ASSIGNED ) {
            // find output partition
            int output_partition = partition_info[i].output_dep;
            // free up output space
            if ( partition_info[i].PARTITION_DONE && partition_info[output_partition].PARTITION_DONE ) {
                free ( partition_info[i].data_out_ptr );
                partition_info[i].OUTPUT_ADDR_ASSIGNED = false;
            }
    	}
    }

    return XST_SUCCESS;
}

int fpgaconvnet::find_partition_index(int partition_id) {

    // iterate over partition indices
    for ( int partition_index=0; partition_index<n_partitions; partition_index++ ) {
        // return
        if ( partition_info[partition_index].partition_id == partition_id ) {
            return partition_index;
        }
    }
}

int fpgaconvnet::run_partition(int partition_index) {

    // check if ready
    int status = hw_check_ready() & hw_check_idle();
    printf("\n\r------\t Hardware Status : %s \t------\n\r",(status) ? "READY" : "NOT READY");
    if (!status) {
        xil_printf(" ERROR : hardware not ready\r\n");
        return XST_FAILURE;
    }

    // load partition
    if ( partition_info[partition_index].LOAD_PARTITION ) {
            // TODO
    }

    // reconfiguring partition
    printf("Reconfiguring partition Partition (partition index : %d)\n\r", partition_index);
    timer_start();
    reconf_partition(partition_index);
    partition_info[partition_index].t_load_partition = timer_stop();

    // load input
    if ( partition_info[partition_index].LOAD_INPUT ) {
        printf("Loading Input (partition index : %d)\n\r", partition_index);
        timer_start();
        sd_read(
            partition_info[partition_index].input_filename,
            partition_info[partition_index].data_in_ptr,
            partition_info[partition_index].size_in * sizeof(port_t)
        );
        partition_info[partition_index].t_load_input = timer_stop();
    }

    // Set port offset
    XFpgaconvnet_ip_Set_fpgaconvnet_in_0( &hw, (u32) partition_info[partition_index].data_in_ptr  );
    XFpgaconvnet_ip_Set_fpgaconvnet_out_0(&hw, (u32) partition_info[partition_index].data_out_ptr );

    // iterate over weights reloading indexes
    for ( int wr_index=0; wr_index<partition_info[partition_index].wr_factor; wr_index++ ) {

    	// set weights reloading index
    	XFpgaconvnet_ip_Set_weights_reloading_index(&hw, wr_index);
        // Set port offset
    	XFpgaconvnet_ip_Set_fpgaconvnet_wr_0( &hw, (u32) partition_info[partition_index].data_wr_ptr[wr_index]  );

        // load weights from sd
        if ( (wr_index != 0 && !partition_info[partition_index].LOAD_FIRST_WEIGHTS ) && partition_info[partition_index].LOAD_WEIGHTS ) {
            printf("Loading Weights (partition index : %d, weights reloading index : %d)\n\r", partition_index, wr_index);
            timer_start();
            sd_read(
                partition_info[partition_index].weights_filename[wr_index],
                partition_info[partition_index].data_wr_ptr[wr_index],
                partition_info[partition_index].size_wr * sizeof(port_t)
            );
            partition_info[partition_index].t_read_weights_sd += ( timer_stop() / partition_info[partition_index].wr_factor );
        }

    	// reload weights
	if ( wr_index != 0 && !partition_info[partition_index].LOAD_FIRST_WEIGHTS ) {
	    printf("Reload Weights (partition index : %d, weights reloading index : %d)\n\r", partition_index, wr_index);
            timer_start();
            XFpgaconvnet_ip_Set_mode(&hw,FPGACONVNET_PL_LOAD_WEIGHTS);
            hw_start();
            while(!hw_check_done());
            partition_info[partition_index].t_load_weights_pl += ( timer_stop() / partition_info[partition_index].wr_factor );
        }

        // run hardware
	printf("Running Partition (partition index : %d, weights reloading index : %d)\n\r", partition_index, wr_index);
    	timer_start();
    	XFpgaconvnet_ip_Set_mode(&hw,FPGACONVNET_PL_RUN_PARTITIONS);
    	hw_start();
        while(!hw_check_done());
        partition_info[partition_index].t_run_partition += ( timer_stop() / partition_info[partition_index].wr_factor );

        // unload weights
        if ( partition_info[partition_index].UNLOAD_WEIGHTS  ) {
            free ( partition_info[partition_index].data_wr_ptr[wr_index] );
        }

    }

	// write output
    if ( partition_info[partition_index].SAVE_OUTPUT  ) {
	printf("Saving Output (partition index : %d)\n\r", partition_index);
    	timer_start();
        sd_write(
            partition_info[partition_index].output_filename,
            partition_info[partition_index].data_out_ptr ,
            partition_info[partition_index].size_out * sizeof(port_t)
        );
        partition_info[partition_index].t_save_output = timer_stop();
    }

    // reset hardware
    hw_init();

    // set done flag
    partition_info[partition_index].PARTITION_DONE	= true;

    return XST_SUCCESS;
}

int fpgaconvnet::run_network(void) {

    // setup address space of first partition
    partition_info[0].data_in_ptr = (port_t *) malloc( partition_info[0].batch_size * partition_info[0].size_in  * sizeof(port_t) );

    // iterate over the partitions in order
    for (int i=0;i<n_partitions;i++) {

        // free up space of done partitions
        free_featuremap_space();

        // set input address space to that of previous partition
        if (i !=0 ) {
            partition_info[i].data_in_ptr = partition_info[find_partition_index(partition_info[i].input_dep)].data_out_ptr;
        }

        // assign space for output of partition
        partition_info[i].data_out_ptr = (port_t *) malloc( partition_info[i].batch_size * partition_info[i].size_out  * sizeof(port_t) );

        // adjust flags

        // run network
        run_partition(i);

    }
}
