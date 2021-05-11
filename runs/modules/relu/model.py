from modules.module_model import ModuleModel
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
        ]
    )

# load relu model
relu_model = ModuleModel(build_module)
relu_model.load_points("modules/relu/logs")

# filter parameters 
filters = {
    "data_width" : [15,17]
}
relu_model.filter_parameters(filters)

# fit model
relu_model.fit_model()

# plot error
relu_model.plot_error(MAX_RSC)

