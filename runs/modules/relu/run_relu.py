import random
from modules.module_runner import ModuleRunner

# create runner
runner = ModuleRunner("relu")

while 1:
    # generate parameters
    runner.gen_parameters()
    # run tests
    runner.run()
