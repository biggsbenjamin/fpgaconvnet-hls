import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("glue")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['filters']    = random.randint(1,1024)
    runner.parameters['coarse_in']  = random.randint(1,64)
    runner.parameters['coarse_out'] = random.randint(1,64)
    runner.parameters['acc']        = "+".join([ "in[{i}][out_index].read()".format(i=i) for i in range(runner.parameters['coarse_in']) ])
    # run tests
    runner.run()
