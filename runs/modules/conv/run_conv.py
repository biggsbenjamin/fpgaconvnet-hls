import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("conv")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['filters']     = random.randint(1,512)
    runner.parameters['kernel_size'] = random.randint(1,11)
    runner.parameters['fine']        = random.choice(
        [1,runner.parameters['kernel_size'],runner.parameters['kernel_size']**2])
    # create random weights
    max_value = 2**((runner.parameters['data_width']/2)-1)
    runner.gen_data(
        [
            runner.parameters['channels'],
            runner.parameters['filters'],
            runner.parameters['kernel_size'],
            runner.parameters['kernel_size']
            
        ],
        "modules/conv/data/weights.csv",
        data_range=[-max_value,max_value])
    # run tests
    runner.run()
