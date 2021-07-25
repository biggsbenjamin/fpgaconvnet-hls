import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("conv")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['filters']     = random.randint(1,96)
    runner.parameters['kernel_size'] = random.randint(1,11)
    runner.parameters['fine']        = random.choice(
        [1,runner.parameters['kernel_size'],runner.parameters['kernel_size']**2])
    # create random weights
    runner.parameters['data_wordlength_integer']=random.randint(1,16)
    runner.parameters['data_wordlength']=2*runner.parameters['data_wordlength_integer']
    runner.parameters['weight_wordlength_integer']=random.randint(1,16)
    runner.parameters['weight_wordlength']=2*runner.parameters['weight_wordlength_integer']
    runner.parameters['acc_wordlength_integer']=random.randint(1,16)
    runner.parameters['acc_wordlength']=2*runner.parameters['acc_wordlength_integer'] 
    max_value = 2**((runner.parameters['acc_wordlength']/2)-1)
    runner.gen_data(
        [
            runner.parameters['channels']*runner.parameters['filters'],
            runner.parameters['kernel_size'],
            runner.parameters['kernel_size']
            
        ],
        "modules/conv/data/weights.csv",
        data_range=[-max_value,max_value])
    # run tests
    runner.run()
