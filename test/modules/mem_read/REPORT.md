
# MEM_READ Report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 2 | True | True | True | True | 
| 0 | True | False | False | False | 
| 1 | True | False | False | False | 
| 3 | True | True | True | True | 



## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 2 | 16 | 19 | 19 | 
| 0 | 125 | 0 | 0 | 
| 1 | 16000 | 0 | 0 | 
| 3 | 16 | 19 | 19 | 



## Resource Results

| TEST | LUT | FF | DSP | BRAM |
|:----:|:---:|:--:|:---:|:----:|
| 2 | 93 | 267 | 0 | 0 | 
| 0 | 0 | 0 | 0 | 0 | 
| 1 | 0 | 0 | 0 | 0 | 
| 3 | 114 | 351 | 0 | 0 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 2 | 4.798 | 
| 0 | 0 | 
| 1 | 0 | 
| 3 | 4.857 | 


## Warnings



