import sys
sys.path.append("..")

from modules.module_model import ModuleModel
from models.modules.Accum import Accum

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
def resource_model(parameter):
    return Accum([
        parameter['channels'],
        parameter['rows'],
        parameter['cols']],
        parameter['filters'],
        parameter['groups'],
        parameter['data_width']
    ).utilisation_model()

accum_model.fit_model_resources(resource_model)
