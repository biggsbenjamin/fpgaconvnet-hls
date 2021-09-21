import os
import json
import scipy
import math
import numpy as np
import sklearn.linear_model
from sklearn.neural_network import MLPRegressor
from sklearn.neural_network import MLPClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_absolute_error
from sklearn.metrics import mean_squared_error

import joblib
#import sklearn.metrics.mean_absolute_error
import matplotlib.pyplot as plt

RSC_TYPES=["LUT", "FF", "BRAM", "DSP"]
estimatederror = {
            "LUT"   : 1100000,
            "FF"    : 1100000,
            "DSP"   : 100,
            "BRAM"  : 100
        }
def Get_Average(list):
    sum = 0
    for item in list:
        sum += item
    return sum/len(list)

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
                modelled_rsc = np.dot(model[rsc_type], self.coef[rsc_type])
                # get error
                err[rsc_type]  += abs(modelled_rsc - actual[rsc_type])/len(self.points)
        # return the error
        return err

    def fit_model(self):
        # model and actual resource values
        model  = {
            "LUT"   : [],
            "FF"    : [],
            "DSP"   : [],
            "BRAM"  : []
        }
        actual  = {
            "LUT"   : [],
            "FF"    : [],
            "DSP"   : [],
            "BRAM"  : []
        }            
        # iterate over points
        for point in self.points:
            # get utilisation model
            for (key,value) in model.items():
                value.append(self.module(point["parameters"]).utilisation_model()[key])
            # get actual data
            actual["LUT"].append(point["resources"]["LUT"])
            actual["FF"].append(point["resources"]["FF"])
            actual["DSP"].append(point["resources"]["DSP"])
            actual["BRAM"].append(point["resources"]["BRAM"])
        # get model coefficients
        for rsc_type in RSC_TYPES:
            self.coef[rsc_type] = self.get_nnls_coef(np.array(model[rsc_type]), np.array(actual[rsc_type]))
            
            

        
        
        

    def save_coefficients(self,filepath):
        # LUT
        with open(f"{filepath}_lut.txt", "wb") as f:
            np.savetxt(f,self.coef["LUT"])
        with open(f"{filepath}_lut.npy", "wb") as f:
            np.save(f,self.coef["LUT"])
        # FF 
        with open(f"{filepath}_ff.txt", "wb") as f:
            np.savetxt(f, self.coef["FF"])
        with open(f"{filepath}_ff.npy", "wb") as f:
            np.save(f, self.coef["FF"])
        # DSP 
        with open(f"{filepath}_dsp.txt", "wb") as f:
            np.savetxt(f, self.coef["DSP"])
        with open(f"{filepath}_dsp.npy", "wb") as f:
            np.save(f, self.coef["DSP"])
        # BRAM 
        with open(f"{filepath}_bram.txt", "wb") as f:
            np.savetxt(f, self.coef["BRAM"])
        with open(f"{filepath}_bram.npy", "wb") as f:
            np.save(f, self.coef["BRAM"])



    def print_absolute_error(self):
        # iterate over the different resource types
        rsc_types = ["FF", "LUT", "DSP", "BRAM"]
        for rsc_type in rsc_types:
            # get the difference in resource usage
            actual = np.array([ p["resources"][rsc_type] for p in self.points ])
            predicted = np.array([ self.module(p["parameters"]).rsc(self.coef)[rsc_type] for p in self.points ])
            # get the mean absolute error
            err = np.average(np.absolute(actual - predicted))
            var = math.sqrt(np.var(np.absolute(actual - predicted)))
            print(f"{rsc_type}: error = {err}, var = {var}")

    def plot_error(self, max_rsc):
        
        # create 4 subplots 
        fig, axs = plt.subplots(2,2)

        # LUT
        ## get coordinates
        x = np.array([ self.module(p["parameters"]).rsc(self.coef)["LUT"] for p in self.points ])
        y = np.array([ p["resources"]["LUT"] for p in self.points ])
        ## create scatter plot
        axs[0,0].scatter(x, y, label="LUT", color="r", marker='x')
        axs[0,0].set_title("LUT")

        # FF
        ## get coordinates
        x = np.array([ self.module(p["parameters"]).rsc(self.coef)["FF"] for p in self.points ])                
        y = np.array([ p["resources"]["FF"] for p in self.points ])
        ## create scatter plot
        axs[0,1].scatter(x, y, label="FF", color="g", marker='x')
        axs[0,1].set_title("FF")

        # BRAM 
        ## get coordinates
        x = np.array([ self.module(p["parameters"]).rsc(self.coef)["BRAM"] for p in self.points ])  
        y = np.array([ p["resources"]["BRAM"] for p in self.points ])
        ## create scatter plot
        axs[1,0].scatter(x, y, label="BRAM", color="b", marker='x')
        axs[1,0].set_title("BRAM")

        # DSP 
        ## get coordinates
        x = np.array([ self.module(p["parameters"]).rsc(self.coef)["DSP"] for p in self.points ])  
        y = np.array([ p["resources"]["DSP"] for p in self.points ])
        ## create scatter plot
        axs[1,1].scatter(x, y, label="DSP", color="y", marker='x')
        axs[1,1].set_title("DSP")

        for ax in axs.flatten():
            ax.set_xlim(xmin=0)
            ax.set_ylim(ymin=0)
            ax.grid()
            lims = [
                np.min([ax.get_xlim(), ax.get_ylim()]),  # min of both axes
                np.max([ax.get_xlim(), ax.get_ylim()]),  # max of both axes
            ]
            ax.plot(lims, lims, 'k-', alpha=0.75, zorder=0)

        fig.add_subplot(111, frameon=False)
        plt.tick_params(labelcolor='none', which='both', top=False, bottom=False, left=False, right=False)

        plt.xlabel("Predicted")
        plt.ylabel("Actual")
        
        plt.show()

