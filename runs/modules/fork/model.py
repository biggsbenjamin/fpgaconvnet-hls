from modules.module_model import ModuleModel
from fpgaconvnet_optimiser.models.modules import Fork 

MAX_RSC = {
    "LUT"   : 53200,
    "FF"    : 106400,
    "BRAM"  : 280,
    "DSP"   : 220
}

# define resource model
def build_module(parameter):
    return Fork([
            parameter['channels'],
            parameter['rows'],
            parameter['cols']
        ],
        parameter['kernel_size'],
        parameter['coarse']
    )

# load accum model
fork_model = ModuleModel(build_module)
fork_model.load_points("modules/fork/logs")

# filter parameters 
filters = {
    "data_width" : [15,17]
}
fork_model.filter_parameters(filters)

# fit model
fork_model.fit_model()

# plot error
fork_model.plot_error(MAX_RSC)

