import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("pool")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['kernel_size'] = random.randint(1,11)
    # run tests
    runner.run()
