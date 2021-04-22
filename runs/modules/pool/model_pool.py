from modules.module_model import ModuleModel
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
        parameter['kernel_size']
    )

# load accum model
pool_model = ModuleModel(build_module)
pool_model.load_points("modules/pool/logs")

# filter parameters 
filters = {
    "data_width" : [15,17]
}
pool_model.filter_parameters(filters)

# fit model
pool_model.fit_model()

# plot error
pool_model.plot_error(MAX_RSC)

