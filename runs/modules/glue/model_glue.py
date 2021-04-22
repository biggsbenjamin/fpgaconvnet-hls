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
glue_model = ModuleModel(build_module)
glue_model.load_points("modules/glue/logs")

# filter parameters 
filters = {
    "data_width" : [15,17]
}
glue_model.filter_parameters(filters)

# fit model
glue_model.fit_model()

# plot error
glue_model.plot_error(MAX_RSC)

