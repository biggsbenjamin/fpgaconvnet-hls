
# MEM_WRITE Report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 2 | True | True | True | True | 
| 1 | True | True | True | True | 
| 0 | True | False | False | False | 
| 3 | True | True | True | True | 



## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 2 | 4 | 7 | 7 | 
| 1 | 16 | 19 | 19 | 
| 0 | 10000 | 0 | 0 | 
| 3 | 4 | 7 | 7 | 



## Resource Results

| TEST | LUT | FF | DSP | BRAM |
|:----:|:---:|:--:|:---:|:----:|
| 2 | 117 | 275 | 0 | 0 | 
| 1 | 84 | 195 | 0 | 0 | 
| 0 | 0 | 0 | 0 | 0 | 
| 3 | 142 | 357 | 0 | 0 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 2 | 4.156 | 
| 1 | 4.435 | 
| 0 | 0 | 
| 3 | 4.161 | 


## Warnings



