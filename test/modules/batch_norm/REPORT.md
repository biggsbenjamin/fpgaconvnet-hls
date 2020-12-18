
# batch_norm report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 3 | False | False | False | False | 
| 7 | False | False | False | False | 
| 1 | False | True | False | True | 
| 0 | False | False | False | False | 
| 9 | False | False | False | False | 
| 6 | False | False | False | False | 
| 5 | False | False | False | False | 
| 2 | False | False | False | False | 
| 4 | False | False | False | False | 
| 8 | False | False | False | False | 


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 3 | 12544 | N/A | N/A | 
| 7 | 196 | N/A | N/A | 
| 1 | 50176 | 50182 | N/A | 
| 0 | 392 | N/A | N/A | 
| 9 | 3136 | N/A | N/A | 
| 6 | 12544 | N/A | N/A | 
| 5 | 98 | N/A | N/A | 
| 2 | 100352 | N/A | N/A | 
| 4 | 12544 | N/A | N/A | 
| 8 | 49 | N/A | N/A | 


## Resource Results

| TEST | LUT | FF | DSP | BRAM || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:---:|:--:|:---:|:----:||:---:|:--:|:---:|:----:|
| 3 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 
| 7 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 
| 1 | 111 | 131 | 1 | 0 || 0 | 0 | 0 | 0 | 
| 0 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 
| 9 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 
| 6 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 
| 5 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 
| 2 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 
| 4 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 
| 8 | N/A | N/A | N/A | N/A || 0 | 0 | 0 | 0 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 3 | N/A | 
| 7 | N/A | 
| 1 | 3.902 | 
| 0 | N/A | 
| 9 | N/A | 
| 6 | N/A | 
| 5 | N/A | 
| 2 | N/A | 
| 4 | N/A | 
| 8 | N/A | 

