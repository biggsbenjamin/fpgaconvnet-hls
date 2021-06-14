import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("fork")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['coarse']      = random.randint(1,2)
    runner.parameters['kernel_size'] = random.randint(1,3)
    runner.parameters['data_wordlength_integer']=random.randint(1,16)
    runner.parameters['data_wordlength']=2*runner.parameters['data_wordlength_integer']
    # run tests
    runner.run()
