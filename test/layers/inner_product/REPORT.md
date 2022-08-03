
# inner_product report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 14 | False | True | True | False | 


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 14 | 8.0 | 24 | 59 | 


## Resource Results

| TEST | LUT (impl) | FF (impl) | DSP (impl) | BRAM (impl) || LUT (synth) | FF (synth) | DSP (synth) | BRAM (synth) || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:----------:|:---------:|:----------:|:-----------:||:-----------:|:----------:|:-----------:|:------------:||:-----------:|:----------:|:-----------:|:--------------------:|
| 14 | N/A | N/A | N/A | N/A || 884 | 666 | 1 | 2 || 835 | 1103 | 2 | 3 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 14 | N/A | 

