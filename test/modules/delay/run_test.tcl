open_project -reset delay_test_prj
set_top delay_top

add_files ../../src/delay.cpp -cflags "-I../../include"
add_files -tb delay_tb.cpp -cflags "-I../../include -I../../src"

open_solution -reset "solution1"
set_part {xc7z020clg484-1} -tool vivado

csim_design -compiler gcc
csynth_design
cosim_design
#export_design -flow syn -rtl verilog -format ip_catalog

exit
