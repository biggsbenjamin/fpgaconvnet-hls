from modules.module_model import ModuleModel
from fpgaconvnet_optimiser.models.modules.Accum import Accum

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

accum_model = ModuleModel(build_module)

accum_model.load_points("modules/accum/logs")
print(accum_model.points[0])

accum_model.fit_model()

err = accum_model.get_absolute_error()
print(err)


"""
# create module model
accum_model = ModuleModel("accum")

# filter parameters 
filters = {
    "data_width" : [15,17]
}
accum_model.filter_parameters(filters)

# create models
## dynamic power model TODO
## static power model TODO
## resource model
accum_model.fit_model_resources(resource_model)
"""
