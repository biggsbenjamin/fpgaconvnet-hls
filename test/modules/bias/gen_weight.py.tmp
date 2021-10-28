import numpy as np
import random
import csv
CONV_CHANNELS=64
CONV_FILTERS=32
CONV_KERNEL_SIZE=3
if CONV_KERNEL_SIZE == 1:
    dim=[
    CONV_CHANNELS*CONV_FILTERS
    ]
else:
    dim=[
    CONV_CHANNELS*CONV_FILTERS,
    CONV_KERNEL_SIZE,
    CONV_KERNEL_SIZE
    ]
output_path='/home/wz2320/fpgaconvnet-hls/test/modules/conv/src/weights.csv'
data = np.ndarray(dim,dtype=float)
# assign values
for index,_ in np.ndenumerate(data):
    data[index] = random.uniform(0,1)
# save to csv
with open(output_path,"w") as f:
    writer = csv.writer(f)
    writer.writerows([data.reshape(-1).tolist()])
 
