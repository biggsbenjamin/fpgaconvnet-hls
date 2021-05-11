from modules.module_model import ModuleModel
from fpgaconvnet_optimiser.models.modules import Accum

MAX_RSC = {
    "LUT"   : 53200,
    "FF"    : 106400,
    "BRAM"  : 280,
    "DSP"   : 220
}

# define resource model
def build_module(parameter):
    return Accum([
        parameter['channels'],
        parameter['rows'],
        parameter['cols']],
        parameter['filters'],
        parameter['groups'],
        parameter['data_width']
    )

# load accum model
accum_model = ModuleModel(build_module)
accum_model.load_points("modules/accum/logs")

# filter parameters 
filters = {
    "data_width" : [15,17]
}
accum_model.filter_parameters(filters)

# fit model
accum_model.fit_model()

# save coefficients
model.save_coefficients("coefficients/accum")

# plot error
accum_model.plot_error(MAX_RSC)

