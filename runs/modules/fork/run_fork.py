import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("fork")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['coarse']      = random.randint(1,2)
    runner.parameters['kernel_size'] = random.randint(1,3)
    # run tests
    runner.run()
