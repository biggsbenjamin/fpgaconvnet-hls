
# pooling report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 13 | False | True | True | False | 


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 13 | 1024.0 | 3076 | 3082 | 


## Resource Results

| TEST | LUT (impl) | FF (impl) | DSP (impl) | BRAM (impl) || LUT (synth) | FF (synth) | DSP (synth) | BRAM (synth) || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:----------:|:---------:|:----------:|:-----------:||:-----------:|:----------:|:-----------:|:------------:||:-----------:|:----------:|:-----------:|:--------------------:|
| 13 | N/A | N/A | N/A | N/A || 7084 | 4010 | 0 | 5 || 4515 | 6535 | 0 | 15 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 13 | N/A | 

