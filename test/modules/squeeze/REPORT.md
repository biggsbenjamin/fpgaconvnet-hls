
# squeeze report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 1 | True | True | True | True | 
| 0 | True | False | False | False | 


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 1 | 2880 | 289 | 291 | 
| 0 | 2880 | N/A | N/A | 


## Resource Results

| TEST | LUT | FF | DSP | BRAM || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:---:|:--:|:---:|:----:||:---:|:--:|:---:|:----:|
| 1 | 485 | 931 | 0 | 0 || 0 | 0 | 0 | 0 | 
| 0 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 1 | 6.079 | 
| 0 | N/A | 

