from modules.module_model_linearmodel import ModuleModel
from fpgaconvnet_optimiser.models.modules import ReLU 

MAX_RSC = {
    "LUT"   : 53200,
    "FF"    : 106400,
    "BRAM"  : 280,
    "DSP"   : 220
}

# define resource model
def build_module(parameter):
    return ReLU([
            parameter['channels'],
            parameter['rows'],
            parameter['cols']
        ],
        parameter['batch_size'],
        parameter['data_wordlength']
    )

# load relu model
model = ModuleModel(build_module)
model.load_points("modules/relu/logs")

# filter parameters 
filters = {
    "data_wordlength" : [0,32]
}
model.filter_parameters(filters)

# fit model
model.fit_model()

# save coefficients
model.save_coefficients("coefficients/relu")

# # plot error
model.plot_error(MAX_RSC)

# print out error
model.print_absolute_error()
