#from modules.module_model_neuralnetwork import ModuleModel
from modules.module_model_randomforest import ModuleModel
from fpgaconvnet_optimiser.models.modules import Pool 

MAX_RSC = {
    "LUT"   : 53200,
    "FF"    : 106400,
    "BRAM"  : 280,
    "DSP"   : 220
}

# define resource model
def build_module(parameter):
    return Pool([
            parameter['channels'],
            parameter['rows'],
            parameter['cols']
        ],
        parameter['kernel_size'],
        parameter['batch_size'],
        parameter['data_wordlength']
    )

# load accum model
model = ModuleModel(build_module)
model.load_points("modules/pool/logs")

# filter parameters 
filters = {
    "data_wordlength" : [0,36]
}
model.filter_parameters(filters)

# fit model
model.fit_model()

# save coefficients
model.save_coefficients("coefficients","pool")

# # plot error
model.plot_error(MAX_RSC)

# print out error
model.print_absolute_error()
