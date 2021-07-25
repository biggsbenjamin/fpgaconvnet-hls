from modules.module_model import ModuleModel
from fpgaconvnet_optimiser.models.modules import Glue 

MAX_RSC = {
    "LUT"   : 53200,
    "FF"    : 106400,
    "BRAM"  : 280,
    "DSP"   : 220
}

# define resource model
def build_module(parameter):
    return Glue([
            parameter['channels'],
            parameter['rows'],
            parameter['cols']
        ],
        parameter['filters'],
        parameter['coarse_in'],
        parameter['coarse_out']
    )

# load accum model
model = ModuleModel(build_module)
model.load_points("modules/glue/logs")

# filter parameters 
filters = {
    "data_width" : [15.5,16.5]
}
model.filter_parameters(filters)

# fit model
model.fit_model()

# save coefficients
model.save_coefficients("coefficients/glue")

# # plot error
model.plot_error(MAX_RSC)

# print out error
model.print_absolute_error()
