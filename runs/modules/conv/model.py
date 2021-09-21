from modules.module_model_neuralnetwork import ModuleModel
#from modules.module_model_randomforest import ModuleModel
#from modules.module_model_linearmodel import ModuleModel
from fpgaconvnet_optimiser.models.modules import Conv 

MAX_RSC = {
    "LUT"   : 53200,
    "FF"    : 106400,
    "BRAM"  : 280,
    "DSP"   : 220
}

# define resource model
def build_module(parameter):
    return Conv([
            parameter['channels'],
            parameter['rows'],
            parameter['cols']
        ],
        parameter['filters'],
        parameter['fine'],
        parameter['kernel_size'],
        1,
        parameter['data_wordlength'],
        parameter['acc_wordlength'],
        parameter['weight_wordlength']     
    )

# load accum model
model = ModuleModel(build_module)
model.load_points("modules/conv/logs")

# filter parameters 
#filters = {
#   "fine" : [1,100],
#    "weight_wordlength" : [0,16]
#}
#model.filter_parameters(filters)

# fit model
model.fit_model()

# save coefficients
model.save_coefficients("coefficients","conv")
#model.save_coefficients("coefficients/conv")

 # plot error
model.plot_error(MAX_RSC)

# print out error
model.print_absolute_error()
