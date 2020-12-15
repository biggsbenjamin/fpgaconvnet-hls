
# sliding_window report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 3 | True | True | True | True | 
| 7 | True | True | True | True | 
| 1 | True | True | True | True | 
| 0 | True | True | False | False | 
| 6 | True | True | True | True | 
| 5 | True | True | True | True | 
| 2 | True | True | True | True | 
| 4 | True | True | True | True | 


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 3 | 24200 | 24202 | 24203 | 
| 7 | 6272 | 6500 | 6501 | 
| 1 | 15680 | 15682 | 15683 | 
| 0 | 784 | 786 | N/A | 
| 6 | 200704 | 215298 | 215299 | 
| 5 | 154587 | 154589 | 154590 | 
| 2 | 150528 | 158702 | 158703 | 
| 4 | 1280 | 1282 | 1283 | 


## Resource Results

| TEST | LUT | FF | DSP | BRAM || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:---:|:--:|:---:|:----:||:---:|:--:|:---:|:----:|
| 3 | 877 | 1051 | 0 | 2 || 0 | 0 | 0 | 2 | 
| 7 | 848 | 1047 | 0 | 2 || 0 | 0 | 0 | 2 | 
| 1 | 528 | 632 | 0 | 1 || 0 | 0 | 0 | 1 | 
| 0 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 
| 6 | 1103 | 1450 | 0 | 14 || 0 | 0 | 0 | 14 | 
| 5 | 8427 | 10279 | 0 | 10 || 0 | 0 | 0 | 10 | 
| 2 | 3885 | 4243 | 0 | 6 || 0 | 0 | 0 | 6 | 
| 4 | 503 | 598 | 0 | 1 || 0 | 0 | 0 | 1 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 3 | 6.925 | 
| 7 | 7.229 | 
| 1 | 6.393 | 
| 0 | N/A | 
| 6 | 7.516 | 
| 5 | 9.984 | 
| 2 | 8.424 | 
| 4 | 6.456 | 

