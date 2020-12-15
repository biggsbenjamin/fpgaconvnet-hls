
# convolution report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 18 | True | False | False | False | 
| 3 | False | True | False | False | 
| 14 | False | True | False | True | 
| 7 | False | True | False | True | 
| 1 | False | True | False | True | 
| 19 | True | False | False | False | 
| 0 | False | False | False | False | 
| 9 | False | True | False | True | 
| 13 | False | True | False | True | 
| 16 | False | True | False | True | 
| 10 | True | True | True | False | 
| 12 | False | True | False | True | 
| 11 | False | True | False | True | 
| 2 | False | True | False | False | 
| 15 | False | True | False | True | 
| 4 | False | True | False | True | 
| 8 | False | False | False | False | 
| 17 | False | True | False | True | 


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 18 | 4816896.0 | N/A | N/A | 
| 3 | 802816.0 | 802838 | N/A | 
| 14 | 14450688.0 | 14450702 | N/A | 
| 7 | 802816.0 | 802840 | N/A | 
| 1 | 400000.0 | 400042 | N/A | 
| 19 | 16000.0 | N/A | N/A | 
| 0 | 2408448.0 | N/A | N/A | 
| 9 | 6422528.0 | 6422549 | N/A | 
| 13 | 9633792.0 | 9633806 | N/A | 
| 16 | 2408448.0 | 2408462 | N/A | 
| 10 | 401408.0 | 401420 | 401902 | 
| 12 | 1038336.0000000001 | 1038364 | N/A | 
| 11 | 519168.00000000006 | 519197 | N/A | 
| 2 | 1605632.0 | 1605654 | N/A | 
| 15 | 9633792.0 | 9633819 | N/A | 
| 4 | 9345024.0 | 9345049 | N/A | 
| 8 | 4917248.0 | N/A | N/A | 
| 17 | 401408.0 | 401459 | N/A | 


## Resource Results

| TEST | LUT | FF | DSP | BRAM || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:---:|:--:|:---:|:----:||:---:|:--:|:---:|:----:|
| 18 | N/A | N/A | N/A | N/A || 0 | 0 | 1 | 4 | 
| 3 | N/A | N/A | N/A | N/A || 0 | 0 | 64 | 136 | 
| 14 | 692 | 765 | 1 | 51 || 0 | 0 | 1 | 10 | 
| 7 | 38397 | 48216 | 64 | 280 || 0 | 0 | 64 | 72 | 
| 1 | 11917 | 14694 | 4 | 12 || 0 | 0 | 4 | 22 | 
| 19 | N/A | N/A | N/A | N/A || 0 | 0 | 100 | 110 | 
| 0 | N/A | N/A | N/A | N/A || 0 | 0 | 1 | 8 | 
| 9 | 5194 | 5819 | 8 | 248 || 0 | 0 | 8 | 33 | 
| 13 | 676 | 757 | 1 | 27 || 0 | 0 | 1 | 7 | 
| 16 | 648 | 777 | 1 | 7 || 0 | 0 | 1 | 3 | 
| 10 | N/A | N/A | N/A | N/A || 0 | 0 | 1 | 2 | 
| 12 | 3327 | 5333 | 9 | 8 || 0 | 0 | 9 | 20 | 
| 11 | 3321 | 5476 | 9 | 6 || 0 | 0 | 9 | 20 | 
| 2 | N/A | N/A | N/A | N/A || 0 | 0 | 32 | 132 | 
| 15 | 3400 | 5475 | 9 | 41 || 0 | 0 | 9 | 67 | 
| 4 | 19999 | 21138 | 8 | 264 || 0 | 0 | 8 | 265 | 
| 8 | N/A | N/A | N/A | N/A || 0 | 0 | 24 | 24 | 
| 17 | 7885 | 12443 | 25 | 6 || 0 | 0 | 25 | 30 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 18 | N/A | 
| 3 | N/A | 
| 14 | 5.207 | 
| 7 | 9.512 | 
| 1 | 7.561 | 
| 19 | N/A | 
| 0 | N/A | 
| 9 | 6.679 | 
| 13 | 5.043 | 
| 16 | 4.756 | 
| 10 | N/A | 
| 12 | 6.181 | 
| 11 | 5.888 | 
| 2 | N/A | 
| 15 | 6.876 | 
| 4 | 7.494 | 
| 8 | N/A | 
| 17 | 7.627 | 

