/*
 * Empty C++ Application
 */

#include "fpgaconvnet.hpp"
#include "xil_printf.h"

int main()
{

    printf("starting ... \n\r");
    
    // network class
    fpgaconvnet net;
	net.init("conf.csv"); // TODO: create partitions in this function n_partitions);

    // print the network config
	//net.config_print();

    // run the network
    net.run_network();

	// print profiling information
    net.profiling_print();

	// free up all memory
	net.unassign_address_space();

	printf("done! \n\r");

	return 0;
}
