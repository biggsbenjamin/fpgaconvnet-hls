import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("accum")

while 1:
    # generate parameters
    runner.gen_parameters()
    runner.parameters['filters'] = random.randint(1,1024)
    channels_group_factor = runner.get_factors(runner.parameters['channels']) 
    filters_group_factor  = runner.get_factors(runner.parameters['filters'])
    runner.parameters['groups'] = random.choice(list(set(channels_group_factor)&set(filters_group_factor)))
    # run tests
    runner.run()
