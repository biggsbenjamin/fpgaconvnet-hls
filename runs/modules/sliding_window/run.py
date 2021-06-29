import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("sliding_window")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['rows']        = runner.parameters['cols'] # fix the rows and cols the same
    runner.parameters['kernel_size'] = random.randint(1,11)
    runner.parameters['pad']         = random.randint(0,10)
    runner.parameters['stride'] = random.choice(
        runner.get_factors( int(runner.parameters['cols'] - runner.parameters['kernel_size'] +2*runner.parameters['pad']) ))
    runner.parameters['data_wordlength_integer']=random.randint(1,16)
    runner.parameters['data_wordlength']=2*runner.parameters['data_wordlength_integer']
    # run tests
    runner.run()
