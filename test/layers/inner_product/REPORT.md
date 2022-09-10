
# inner_product report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
| 15 | False | True | True | False | 


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
| 15 | 7200.0 | 7210 | 7234 | 


## Resource Results

| TEST | LUT (impl) | FF (impl) | DSP (impl) | BRAM (impl) || LUT (synth) | FF (synth) | DSP (synth) | BRAM (synth) || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:----------:|:---------:|:----------:|:-----------:||:-----------:|:----------:|:-----------:|:------------:||:-----------:|:----------:|:-----------:|:--------------------:|
| 15 | N/A | N/A | N/A | N/A || 2932 | 2218 | 1 | 35 || 1951 | 2689 | 2 | 10 | 


## Timing Results

| TEST | CLK |
|:----:|:---:|
| 15 | N/A | 

