import json
import os
import sys

class report:

    def __init__(self, report_name):
        
        # name of report
        self.report_name = report_name

        # initialise tables
        self.table_pass = """
## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
"""       
        self.table_latency = """
## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
"""
        self.table_resources = """
## Resource Results

| TEST | LUT | FF | DSP | BRAM || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:---:|:--:|:---:|:----:||:---:|:--:|:---:|:----:|
"""

        self.table_clk_period = """
## Timing Results

| TEST | CLK |
|:----:|:---:|
"""

    def append_table_pass(self, test_num, result_path):
        # get result
        with open(result_path,'r') as f:
            result = json.load(f)
        # append to table_pass
        self.table_pass += "| {test_num} | {csim} | {synth} | {sim} | {impl} | \n".format(
            test_num=test_num,
            csim =result['pass']['csim'],
            synth=result['pass']['synth'],
            sim  =result['pass']['sim'],
            impl =result['pass']['impl']
        )

    def append_table_latency(self, test_num, model_path, result_path):
        # get result
        with open(result_path,'r') as f:
            result = json.load(f)
        # get model 
        with open(model_path,'r') as f:
            model = json.load(f)
        # append to table_latency
        self.table_latency += "| {test_num} | {model} | {synth} | {sim} | \n".format(
            test_num=test_num,
            model=model['latency'],
            synth=result['latency']['synth'],
            sim  =result['latency']['sim']
        )

    def append_table_resources(self, test_num, model_path, result_path):
        # get result
        with open(result_path,'r') as f:
            result = json.load(f)
        # get model 
        with open(model_path,'r') as f:
            model = json.load(f)
        # append to table_latency
        self.table_resources += "| {test_num} | {LUT} | {FF} | {DSP} | {BRAM} || {LUT_model} | {FF_model} | {DSP_model} | {BRAM_model} | \n".format(
            test_num=test_num,
            LUT =result['resources']['LUT'] ,
            FF  =result['resources']['FF']  ,
            DSP =result['resources']['DSP'] ,
            BRAM=result['resources']['BRAM'],
            LUT_model =model['resources']['LUT'] ,
            FF_model  =model['resources']['FF']  ,
            DSP_model =model['resources']['DSP'] ,
            BRAM_model=model['resources']['BRAM'],
        )

    def append_table_clk_period(self, test_num, result_path):
        # get result
        with open(result_path,'r') as f:
            result = json.load(f)
        # append to table_clk_period
        self.table_clk_period += "| {test_num} | {clk} | \n".format(
            test_num=test_num,
            clk=result['clk_period']
        )

    def print_report(self):
        return f"""
# {self.report_name}
{self.table_pass}
{self.table_latency}
{self.table_resources}
{self.table_clk_period}
"""

if __name__ == "__main__":
    tmp = report("test")
    print( tmp.print_report() )
