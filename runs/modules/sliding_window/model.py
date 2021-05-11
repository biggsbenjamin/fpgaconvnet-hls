from modules.module_model import ModuleModel
from fpgaconvnet_optimiser.models.modules import SlidingWindow 

MAX_RSC = {
    "LUT"   : 53200,
    "FF"    : 106400,
    "BRAM"  : 280,
    "DSP"   : 220
}

# define resource model
def build_module(parameter):
    return SlidingWindow([
            parameter['channels'],
            parameter['rows'],
            parameter['cols']
        ],
        parameter['kernel_size'],
        parameter['stride'],
        parameter['pad'],
        parameter['pad'],
        parameter['pad'],
        parameter['pad']
    )

# load accum model
sliding_window_model = ModuleModel(build_module)
sliding_window_model.load_points("modules/sliding_window/logs")

# filter parameters 
filters = {
    "data_width" : [15,17]
}
sliding_window_model.filter_parameters(filters)

# fit model
sliding_window_model.fit_model()

print(sliding_window_model.coef)

# plot error
sliding_window_model.plot_error(MAX_RSC)

