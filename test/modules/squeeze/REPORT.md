
# squeeze report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 1 | True | True | False | True | 
| 0 | True | True | False | True | 


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 1 | 2880 | 289 | N/A | 
| 0 | 2880 | 1442 | N/A | 


## Resource Results

| TEST | LUT | FF | DSP | BRAM || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:---:|:--:|:---:|:----:||:---:|:--:|:---:|:----:|
| 1 | 485 | 931 | 0 | 0 || 0 | 0 | 0 | 0 | 
| 0 | 687 | 934 | 0 | 0 || 0 | 0 | 0 | 0 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 1 | 6.079 | 
| 0 | 6.288 | 

