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
        parameter['pad'],
        parameter['data_wordlength']
    )

# load accum model
model = ModuleModel(build_module)
model.load_points("modules/sliding_window/logs")

 #filter parameters 
filters = {
    "data_wordlength" : [0,36]
}
model.filter_parameters(filters)

# fit model
model.fit_model()


# save coefficients
model.save_coefficients("coefficients","sliding_window")

 # plot error
model.plot_error(MAX_RSC)

# print out error
model.print_absolute_error()
