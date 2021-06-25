import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("glue")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['coarse_in']  = random.randint(1,16)
    runner.parameters['coarse_out'] = random.randint(1,8)
    runner.parameters['filters']    = random.randint(1,8)*runner.parameters['coarse_out']
    # run tests
    runner.run()
