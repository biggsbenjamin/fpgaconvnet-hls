# REPORT

## Baseline Tests

| MODULE | TEST | CSIM | SYNTH | SIM | IMPL | DESCRIPTION |
|:------:|:----:|:----:|:-----:|:---:|:----:|:-----------:|
| relu | 1 | True | True | False | False | relu (coarse) |
| relu | 0 | True | True | False | False | relu (no coarse) |
| concat | 1 | False | False | False | False | concat layer in googlenet (coarse) |
| concat | 0 | True | True | False | False | concat layer in googlenet (no coarse) |
| convolution | 4 | True | True | False | False | mobilenet sep layer |
| convolution | 2 | True | True | False | False | mobilenet depth-wise (no coarse) |
| convolution | 1 | True | True | False | False | googlenet first layer (coarse in and out) |
| convolution | 0 | True | True | False | False | googlenet first layer (no coarse) |
| convolution | 5 | True | True | False | False | lenet second layer (no coarse) |
| convolution | 3 | False | False | False | False | mobilenet depth-wise (with coarse) |



## Latency Results

| MODULE | TEST | MODEL | SYNTH | SIM | DESCRIPTION |
|:------:|:----:|:-----:|:-----:|:---:|:-----------:|
| relu | 1 | 11520 | 11532 | 0 |  relu (coarse) |
| relu | 0 | 3200 | 3203 | 0 |  relu (no coarse) |
| concat | 1 | 50176 | 0 | 0 |  concat layer in googlenet (coarse) |
| concat | 0 | 200704 | 200707 | 0 |  concat layer in googlenet (no coarse) |
| convolution | 4 | 6422528 | 6422539 | 0 |  mobilenet sep layer |
| convolution | 2 | 12845056 | 3612695 | 0 |  mobilenet depth-wise (no coarse) |
| convolution | 1 | 9922500 | 10633361 | 0 |  googlenet first layer (coarse in and out) |
| convolution | 0 | 119070000 | 118014016 | 0 |  googlenet first layer (no coarse) |
| convolution | 5 | 64000 | 64032 | 0 |  lenet second layer (no coarse) |
| convolution | 3 | 6422528 | 0 | 0 |  mobilenet depth-wise (with coarse) |



## Resource Results

| MODULE | TEST | LUT | FF | DSP | BRAM | DESCRIPTION |
|:------:|:----:|:---:|:--:|:---:|:----:|:-----------:|
| relu | 1 | 0 | 0 | 0 | 0 | relu (coarse) | 
| relu | 0 | 0 | 0 | 0 | 0 | relu (no coarse) | 
| concat | 1 | 0 | 0 | 0 | 0 | concat layer in googlenet (coarse) | 
| concat | 0 | 0 | 0 | 0 | 0 | concat layer in googlenet (no coarse) | 
| convolution | 4 | 0 | 0 | 0 | 0 | mobilenet sep layer | 
| convolution | 2 | 0 | 0 | 0 | 0 | mobilenet depth-wise (no coarse) | 
| convolution | 1 | 0 | 0 | 0 | 0 | googlenet first layer (coarse in and out) | 
| convolution | 0 | 0 | 0 | 0 | 0 | googlenet first layer (no coarse) | 
| convolution | 5 | 0 | 0 | 0 | 0 | lenet second layer (no coarse) | 
| convolution | 3 | 0 | 0 | 0 | 0 | mobilenet depth-wise (with coarse) | 


## Timing Results

| MODULE | TEST | CLK | DESCRIPTION |
|:------:|:----:|:---:|:-----------:|
| relu | 1 | 0 | relu (coarse) | 
| relu | 0 | 0 | relu (no coarse) | 
| concat | 1 | 0 | concat layer in googlenet (coarse) | 
| concat | 0 | 0 | concat layer in googlenet (no coarse) | 
| convolution | 4 | 0 | mobilenet sep layer | 
| convolution | 2 | 0 | mobilenet depth-wise (no coarse) | 
| convolution | 1 | 0 | googlenet first layer (coarse in and out) | 
| convolution | 0 | 0 | googlenet first layer (no coarse) | 
| convolution | 5 | 0 | lenet second layer (no coarse) | 
| convolution | 3 | 0 | mobilenet depth-wise (with coarse) | 


## Warnings



