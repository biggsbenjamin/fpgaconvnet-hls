from modules.module_model import ModuleModel
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
        1
    )

# load accum model
conv_model = ModuleModel(build_module)
conv_model.load_points("modules/conv/logs")

# filter parameters 
filters = {
    "data_width" : [15,17]
}
conv_model.filter_parameters(filters)

# fit model
conv_model.fit_model()

# plot error
conv_model.plot_error(MAX_RSC)

