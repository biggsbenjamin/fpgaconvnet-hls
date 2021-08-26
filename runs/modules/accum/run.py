import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("accum")

for _ in range(runner.max_runners):
    # generate parameters
    #runner.gen_parameters()
    runner.parameters['freq'] = 200 

    runner.parameters['groups'] = random.randint(0,1)
    if runner.parameters['groups'] == 0:
        runner.parameters['groups'] = int(2**random.randint(1,8))
    
    runner.parameters['channels'] = int(random.randint(1,int(256/runner.parameters['groups'])) * runner.parameters['groups'])
    runner.parameters['filters'] = int(random.randint(1,int(256/runner.parameters['groups'])) * runner.parameters['groups'])

    runner.parameters['batch_size'] = 256
    runner.parameters['rows']       = random.randint(1,min(224,int(224*256/max(runner.parameters['channels'],runner.parameters['filters']))))
    runner.parameters['cols']       = runner.parameters['rows']

    runner.parameters['data_width'] = 30
    # run tests
    runner.run()
