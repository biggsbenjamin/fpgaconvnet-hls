
# convolution report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 0 | True | True | True | True | 
| 1 | True | True | True | False | 
| 10 | False | False | False | False | 
| 2 | True | True | True | True | 
| 3 | True | True | True | True | 
| 4 | True | True | False | False | 
| 20 | False | False | False | False | 


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 0 | 2408448.0 | 2408461 | 2411447 | 
| 1 | 400000.0 | 400040 | 406845 | 
| 10 | 401408.0 | N/A | N/A | 
| 2 | 1605632.0 | 1605657 | 1613604 | 
| 3 | 802816.0 | 802841 | 806820 | 
| 4 | 9345024.0 | 9345048 | N/A | 
| 20 | 16000.0 | N/A | N/A | 


## Resource Results

| TEST | LUT | FF | DSP | BRAM || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:---:|:--:|:---:|:----:||:---:|:--:|:---:|:----:|
| 0 | 507 | 659 | 1 | 2 || 0 | 0 | 1 | 8 | 
| 1 | N/A | N/A | N/A | N/A || 0 | 0 | 4 | 22 | 
| 10 | N/A | N/A | N/A | N/A || 0 | 0 | 1 | 2 | 
| 2 | 16660 | 22495 | 32 | 64 || 0 | 0 | 32 | 188 | 
| 3 | 31277 | 41700 | 64 | 128 || 0 | 0 | 64 | 192 | 
| 4 | N/A | N/A | N/A | N/A || 0 | 0 | 8 | 270 | 
| 20 | N/A | N/A | N/A | N/A || 0 | 0 | 100 | 117 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 0 | 4.703 | 
| 1 | N/A | 
| 10 | N/A | 
| 2 | 6.924 | 
| 3 | 8.458 | 
| 4 | N/A | 
| 20 | N/A | 

