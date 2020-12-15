# REPORT

## Baseline Tests

| MODULE | TEST | CSIM | SYNTH | SIM | IMPL | DESCRIPTION |
|:------:|:----:|:----:|:-----:|:---:|:----:|:-----------:|
| pool | 0 | True | True | True | False | max pooling |
| relu | 0 | True | True | True | False | normal relu |
| conv | 2 | False | False | False | False | depthwise |
| conv | 1 | True | True | True | True | regular conv (fine=9,k=3) |
| conv | 0 | True | True | True | False | regular conv (fine=5,k=5) |
| conv | 3 | True | True | True | True | point-wise conv |
| sliding_window | 2 | True | True | True | False | googlenet first layer |
| sliding_window | 1 | True | True | True | False | lenet pool1 layer |
| sliding_window | 0 | True | True | True | False | lenet conv1 layer |
| sliding_window | 3 | True | True | True | False | first pool layer googlenet |
| fork | 1 | True | True | True | False |  |
| fork | 0 | True | True | True | False |  |
| glue | 2 | True | True | True | False | only coarse in (2) |
| glue | 1 | True | True | True | False | only coarse out (2) |
| glue | 0 | True | True | True | False | no coarse |
| glue | 3 | True | True | True | False | coarse in and out (2 for both) |
| accum | 2 | True | True | True | False | accum with group=2 |
| accum | 1 | True | True | True | False | only 1 channel |
| accum | 0 | True | True | True | False | regular accum (group=1) |
| accum | 3 | True | True | True | False | depthwise accum |
| split | 2 | True | True | True | False | two inputs, 2 streams  |
| split | 1 | True | True | True | False | two inputs |
| split | 0 | True | True | True | False | 1 input |



## Latency Results

| MODULE | TEST | MODEL | SYNTH | SIM | DESCRIPTION |
|:------:|:----:|:-----:|:-----:|:---:|:-----------:|
| pool | 0 | 1000 | 1013 | 1013 |  max pooling |
| relu | 0 | 64 | 67 | 67 |  normal relu |
| conv | 2 | 12 | 0 | 0 |  depthwise |
| conv | 1 | 24 | 37 | 37 |  regular conv (fine=9,k=3) |
| conv | 0 | 25000 | 25018 | 25014 |  regular conv (fine=5,k=5) |
| conv | 3 | 24 | 30 | 30 |  point-wise conv |
| sliding_window | 2 | 150528 | 158704 | 158704 |  googlenet first layer |
| sliding_window | 1 | 15680 | 15684 | 15684 |  lenet pool1 layer |
| sliding_window | 0 | 15680 | 15684 | 15684 |  lenet conv1 layer |
| sliding_window | 3 | 602112 | 602117 | 602117 |  first pool layer googlenet |
| fork | 1 | 12 | 15 | 15 |   |
| fork | 0 | 4 | 7 | 7 |   |
| glue | 2 | 11520.0 | 11523 | 11523 |  only coarse in (2) |
| glue | 1 | 5760.0 | 5763 | 5763 |  only coarse out (2) |
| glue | 0 | 11520.0 | 11523 | 11523 |  no coarse |
| glue | 3 | 5760.0 | 5763 | 5763 |  coarse in and out (2 for both) |
| accum | 2 | 10000 | 10005 | 10005 |  accum with group=2 |
| accum | 1 | 1000 | 1003 | 1003 |  only 1 channel |
| accum | 0 | 5000 | 5005 | 5005 |  regular accum (group=1) |
| accum | 3 | 10000 | 10005 | 10005 |  depthwise accum |
| split | 2 | 1000 | 503 | 503 |  two inputs, 2 streams  |
| split | 1 | 1000 | 1003 | 1003 |  two inputs |
| split | 0 | 1000 | 1003 | 1003 |  1 input |



## Resource Results

| MODULE | TEST | LUT | FF | DSP | BRAM | DESCRIPTION |
|:------:|:----:|:---:|:--:|:---:|:----:|:-----------:|
| pool | 0 | 0 | 0 | 0 | 0 | max pooling | 
| relu | 0 | 0 | 0 | 0 | 0 | normal relu | 
| conv | 2 | 0 | 0 | 0 | 0 | depthwise | 
| conv | 1 | 1323 | 2088 | 9 | 0 | regular conv (fine=9,k=3) | 
| conv | 0 | 0 | 0 | 0 | 0 | regular conv (fine=5,k=5) | 
| conv | 3 | 93 | 119 | 1 | 0 | point-wise conv | 
| sliding_window | 2 | 0 | 0 | 0 | 0 | googlenet first layer | 
| sliding_window | 1 | 0 | 0 | 0 | 0 | lenet pool1 layer | 
| sliding_window | 0 | 0 | 0 | 0 | 0 | lenet conv1 layer | 
| sliding_window | 3 | 0 | 0 | 0 | 0 | first pool layer googlenet | 
| fork | 1 | 0 | 0 | 0 | 0 |  | 
| fork | 0 | 0 | 0 | 0 | 0 |  | 
| glue | 2 | 0 | 0 | 0 | 0 | only coarse in (2) | 
| glue | 1 | 0 | 0 | 0 | 0 | only coarse out (2) | 
| glue | 0 | 0 | 0 | 0 | 0 | no coarse | 
| glue | 3 | 0 | 0 | 0 | 0 | coarse in and out (2 for both) | 
| accum | 2 | 0 | 0 | 0 | 0 | accum with group=2 | 
| accum | 1 | 0 | 0 | 0 | 0 | only 1 channel | 
| accum | 0 | 0 | 0 | 0 | 0 | regular accum (group=1) | 
| accum | 3 | 0 | 0 | 0 | 0 | depthwise accum | 
| split | 2 | 0 | 0 | 0 | 0 | two inputs, 2 streams  | 
| split | 1 | 0 | 0 | 0 | 0 | two inputs | 
| split | 0 | 0 | 0 | 0 | 0 | 1 input | 


## Timing Results

| MODULE | TEST | CLK | DESCRIPTION |
|:------:|:----:|:---:|:-----------:|
| pool | 0 | 0 | max pooling | 
| relu | 0 | 0 | normal relu | 
| conv | 2 | 0 | depthwise | 
| conv | 1 | 5.517 | regular conv (fine=9,k=3) | 
| conv | 0 | 0 | regular conv (fine=5,k=5) | 
| conv | 3 | 4.32 | point-wise conv | 
| sliding_window | 2 | 0 | googlenet first layer | 
| sliding_window | 1 | 0 | lenet pool1 layer | 
| sliding_window | 0 | 0 | lenet conv1 layer | 
| sliding_window | 3 | 0 | first pool layer googlenet | 
| fork | 1 | 0 |  | 
| fork | 0 | 0 |  | 
| glue | 2 | 0 | only coarse in (2) | 
| glue | 1 | 0 | only coarse out (2) | 
| glue | 0 | 0 | no coarse | 
| glue | 3 | 0 | coarse in and out (2 for both) | 
| accum | 2 | 0 | accum with group=2 | 
| accum | 1 | 0 | only 1 channel | 
| accum | 0 | 0 | regular accum (group=1) | 
| accum | 3 | 0 | depthwise accum | 
| split | 2 | 0 | two inputs, 2 streams  | 
| split | 1 | 0 | two inputs | 
| split | 0 | 0 | 1 input | 


## Warnings



