import sys
sys.path.append('../../..')
sys.path.append('..')

import re
import os
import json
import argparse
from tools.hls_logger import hls_log
from tools.reporter import report

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Module Report Generator")
    parser.add_argument('-m','--module', required=True, help='Name of module')
    args = parser.parse_args()

    # create a report instance
    module_report = report(f"{args.module} report")

    # iterate over tests
    for config_file in os.listdir("config"):
        # get test number
        test_num = int(re.search(r"\d+",config_file).group(0))
        # get hls logs
        module_test_log = hls_log(f"{args.module}_top", f"{args.module}_hls_prj/solution{test_num}")
        # generate results of test
        results = {
            "pass" : module_test_log.check_pass(),
            "latency" : {
                "synth" : "N/A", #module_test_log.get_synth_latency(),
                "sim"   : "N/A", #module_test_log.get_sim_latency(),
            },
            "resources"  : {
                "BRAM"  : "N/A",
                "LUT"   : "N/A",
                "FF"    : "N/A",
                "DSP"   : "N/A",
            },
            "clk_period" : "N/A", #module_test_log.get_clk_period()
        }
        # update latency
        try:
            results["latency"]["synth"] = module_test_log.get_synth_latency()
        except AssertionError:
            pass
        try:
            results["latency"]["sim"]   = module_test_log.get_sim_latency()
        except AssertionError:
            pass
        try:
            results["resources"]  = module_test_log.get_impl_resources()
            results["clk_period"] = module_test_log.get_clk_period()
        except AssertionError:
            pass
        # save results
        with open(f"rpt/result_{test_num}.json","w") as f:
            json.dump(results,f)
        # append to module report
        result_path = f"rpt/result_{test_num}.json"
        model_path  = f"data/test_{test_num}/model.json"
        if os.path.exists(result_path) and os.path.exists(model_path):
            module_report.append_table_pass(test_num, result_path)
            module_report.append_table_latency(test_num, model_path, result_path)
            module_report.append_table_resources(test_num, model_path, result_path)
            module_report.append_table_clk_period(test_num, result_path)
    
    # save report
    with open("REPORT.md","w") as f:
        f.write(module_report.print_report())
