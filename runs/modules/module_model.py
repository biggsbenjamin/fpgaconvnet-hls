import os
import json
import scipy
import numpy as np
import sklearn.linear_model
import matplotlib.pyplot as plt

RSC_TYPES=["LUT", "FF", "BRAM", "DSP"]

class ModuleModel:

    def __init__(self, build_module):
        
        # save module
        self.module = build_module

        # coeffcients for the model
        self.coef = {
            "LUT"   : np.array([]),
            "FF"    : np.array([]),
            "DSP"   : np.array([]),
            "BRAM"  : np.array([])
        }

        """
        # reset utilisation coefficients to 1
        for rsc_type in RSC_TYPES:
            for i in range(len(self.module.rsc_coef)):
                self.module.rsc_coef[rsc_type][i] = 1.0
        """
        # points for model fitting
        self.points = []
    
    def load_points(self, filepath):
        result_files = os.listdir(filepath)
        for result_file in result_files:
            if result_file.endswith(".json"):
                try:
                    with open(f"{filepath}/{result_file}","r") as f:
                        tmp = json.load(f)
                        self.points.append(tmp)
                except:
                    print(f"Cannot open {result_file}") 

    def filter_parameters(self, filters):
        res = []
        for point in self.points:
            valid = True
            for filter in filters:
                if (point['parameters'][filter] < filters[filter][0]) or (point['parameters'][filter] > filters[filter][1]):
                    valid = False
            if valid == True:
                res.append(point)
        self.points = res

    def get_nnls_coef(self, model, rsc):
        nnls = sklearn.linear_model.LinearRegression(positive=True, fit_intercept=False)
        return nnls.fit(model,rsc).coef_

    def get_absolute_error(self):
        # save error
        err = {
            "LUT"  : 0.0,
            "FF"   : 0.0,
            "DSP"  : 0.0,
            "BRAM" : 0.0
        }
        # iterate over points
        for point in self.points:
            # get the resource model results
            model  = self.module(point["parameters"]).utilisation_model()
            actual = point["resources"] 
            for rsc_type in RSC_TYPES:
                # get modelled resources
                modelled_rsc = np.dot(model, self.coef[rsc_type])
                # get error
                err[rsc_type]  += abs(modelled_rsc - actual[rsc_type])/len(self.points)
        # return the error
        return err

    def fit_model(self):
        # model and actual resource values
        model   = []
        actual  = {
            "LUT"   : [],
            "FF"    : [],
            "DSP"   : [],
            "BRAM"  : []
        }
        # iterate over points
        for point in self.points:
            # get utilisation model
            model.append( self.module(point["parameters"]).utilisation_model() )
            # get actual data
            actual["LUT"].append(point["resources"]["LUT"])
            actual["FF"].append(point["resources"]["FF"])
            actual["DSP"].append(point["resources"]["DSP"])
            actual["BRAM"].append(point["resources"]["BRAM"])
        # get model coefficients
        for rsc_type in RSC_TYPES:
            self.coef[rsc_type] = self.get_nnls_coef(np.array(model), np.array(actual[rsc_type]))

    def plot_error(self, max_rsc):
        # iterate over resource types
        for rsc_type in RSC_TYPES:
            # get coordinates
            x = np.array([ np.dot(self.module(p["parameters"]).utilisation_model(), self.coef[rsc_type])
                    for p in self.points ])
            y = np.array([ p["resources"][rsc_type] for p in self.points ])
            # create scatter plot
            plt.scatter(x/max_rsc[rsc_type], y/max_rsc[rsc_type], label=rsc_type, marker='x')

        plt.xlabel("Predicted")
        plt.ylabel("Actual")
        
        plt.legend()
        plt.xlim(xmin=0)
        plt.ylim(ymin=0)
        plt.grid()
        plt.show()

