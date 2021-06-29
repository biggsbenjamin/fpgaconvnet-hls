import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("glue")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['filters']    = random.randint(1,64)
    runner.parameters['coarse_in']  = random.randint(1,64)
    runner.parameters['coarse_out'] = random.randint(1,64)
    runner.parameters['acc']        = "+".join([ "in[{i}][out_index].read()".format(i=i) for i in range(runner.parameters['coarse_in']) ])
    runner.parameters['acc_wordlength_integer']=random.randint(1,16)
    runner.parameters['acc_wordlength']=2*runner.parameters['acc_wordlength_integer']
    runner.parameters['data_wordlength_integer']=random.randint(1,16)
    runner.parameters['data_wordlength']=2*runner.parameters['data_wordlength_integer']
    # run tests
    runner.run()
