#!/usr/bin/env python3.5
import argparse
import os
import logging

################################################################################
# LOGGING
################################################################################
logging.basicConfig(
	#filename=str(__file__)+'.log',
	format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
	level=logging.DEBUG
)
################################################################################
# Read similarity file
################################################################################
def load_sim_data(simfile):
	index2object    = []
	object2index    = {}

	sim_value       = {}
	max_value = float("-inf")
	min_value = float("inf")
	with open(simfile) as f:
		for line in f:
			line = line.split()
			o1    = str(line[0])
			o2    = str(line[1])
			value = float(line[2])

			if o1 == o2:
				continue

			#######################################################################
			# Update maps
			#######################################################################
			if o1 not in object2index:
				_id = len(index2object)
				index2object.append(o1)
				object2index[o1] = _id

			if o2 not in object2index:
				_id = len(index2object)
				index2object.append(o2)
				object2index[o2] = _id

			#######################################################################
			# Create lookup key (smallest id first)
			#######################################################################
			if object2index[o1] < object2index[o2]:
				key = o1 + "_" + o2
			else:
				key = o2 + "_" + o1

			#######################################################################
			# Update sim_value map
			#######################################################################
			if key in sim_value:
				if sim_value[key] < value:
					value = sim_value[key]

			if value != float("inf") and value != float("-inf"):
				if value < min_value:
					min_value = value
				if value > max_value:
					max_value = value

			sim_value[key] = value

	return {
		"sim_map":sim_value,
		"index2object":index2object,
		"object2index":object2index,
		"min_value":min_value,
		"max_value":max_value
	}

################################################################################
# Read transclust-formated result file
################################################################################
def load_result_file(filename):
	with open(filename) as f:
		results = []
		for line in f:
			if line.strip().startswith("//"):
				continue

			line = line.split()
			try:
				_cost = float(line[1])
			except:
				_cost = -1

			clusters = line[2].rstrip(";").split(";")

			_clusters = []
			for cluster in clusters:
				_clusters.append(cluster.split(","))

			results.append({
					"threshold": float(line[0]),
					"cost": str(line[1]),
					"clusters": _clusters,
					"_cost":_cost
				})

	return results

################################################################################
# Calculates cost of clustering
################################################################################
def calculate_cost(sim_data,threshold,clustering):
	sim_map = sim_data["sim_map"]
	index2object = sim_data["index2object"]
	object2index = sim_data["object2index"]
	min_value = sim_data["min_value"]
	max_value = sim_data["max_value"]

	#############################################################################
	# Transform clustering to membership vector
	#############################################################################
	membership = [-1]*len(index2object)
	c_id = 0
	for cluster in clustering:
		for obj in cluster:
			membership[int(object2index[obj])] = c_id
		c_id += 1

	#############################################################################
	# Calculate cost
	#############################################################################
	cost = 0
	for i in range(len(membership)):
		for j in range(i+1,len(membership)):
			o1 = index2object[i]
			o2 = index2object[j]
			key = o1 + "_" + o2
			if key in sim_map:
				sim_value = sim_map[key]
			else:
				sim_value = 0

			if sim_value == float("inf"):
				sim_value = max_value

			cost_value = sim_value - threshold

			if membership[i] == membership[j] and cost_value < 0:
				cost = cost + abs(cost_value)
			elif membership[i] != membership[j] and cost_value > 0:
				cost = cost + abs(cost_value)
	return cost

################################################################################
# Print R format
################################################################################
def print_R(data):
	print("file\tthreshold\tcost")
	for filepath, data_arr in data.items():
		filename = os.path.basename(filepath)
		for res in data_arr:
			print("\t".join([filename,str(res['threshold']),str(res['cost'])]))

################################################################################
# Print c3.js JSON format
################################################################################
def print_JSON(data):
	# file data
	fdata = {}
	for filepath, data_arr in data.items():
		filename = os.path.basename(filepath)
		thresholds = []
		costs = []
		for res in data_arr:
			thresholds.append(str(res['threshold']))
			costs.append(str(res['cost']))
		fdata[filename] = {'thresholds':thresholds,'costs':costs}

	xs = []
	for filename in fdata:
		xs.append("\""+"cost_"+filename +"\":\""+"threshold_"+filename+"\"")
	xs = "\"xs\":{"+",".join(xs)+"}"

	columns = []
	for filename, data_arr in fdata.items():
		columns.append('['+','.join(['\"threshold_'+filename+"\""]+data_arr['thresholds'])+']')
		columns.append('['+','.join(['\"cost_'+filename+"\""]+data_arr['costs'])+']')
	columns = "\"columns\":["+",".join(columns)+"]"
	print('{'+xs+','+columns+'}')


################################################################################
# Get variables from res file name
################################################################################
def getVariables(resfile):
	filename = os.path.basename(resfile["filename"])
	filename = filename.replace(".res","")
	file_arr = filename.split("_")
	filename = filename.split("_t_")[0]
	fpt = (True if file_arr[-2] == "wFPT" else False)
	return {
		"name":filename,
		"threshold":file_arr[-3],
		"fpt":fpt,
		"program":file_arr[-1]
	}



################################################################################
################################## START #######################################
################################################################################

################################################################################
# Parse args
################################################################################
parser = argparse.ArgumentParser()
# input path
parser.add_argument(
	"-r",
	help="path/to/res directory - The program will search recursivly from this directory for '*.res' files",
	default="/home/mikkel/Dropbox/Datalogi/Speciale/repo/transclust/data")
parser.add_argument(
	"-s",
	help="path/to/dataset directory - The program will search recursivly from this directory for '*.sim' files",
	default="/home/mikkel/Dropbox/Datalogi/Speciale/repo/transclust/data")
args = parser.parse_args()


################################################################################
# Get all similarity files
################################################################################
logging.info("Collecting similarity files")
SIMILARITY_FILES = []
for root, dirs, files in os.walk(args.s):
	for f in files:
		if f.endswith(".sim"):
			SIMILARITY_FILES.append(os.path.join(root, f))
logging.info("Collecting similarity files ... done")

################################################################################
# Get all result files
################################################################################
logging.info("Collecting result files")
RESULT_FILES = []
for root, dirs, files in os.walk(args.r):
	for f in files:
		if f.endswith(".res"):
			RESULT_FILES.append(os.path.join(root, f))
logging.info("Collecting result files ... done")


################################################################################
# For each sim file
################################################################################
cost_log = os.path.join(args.r,"costs.log")
with open(cost_log, "w") as f:
	f.write("\t".join(["filename","threshold","fpt","program","cost","cpp_cost","\n"]))

for simfile in SIMILARITY_FILES:

	logging.info("Loading similarity file " + str(simfile))
	sim_data = load_sim_data(simfile)


	simfile_name = os.path.basename(simfile).split(".")[0]

	# collect res files and load
	resfiles = []
	logging.info("Loading result files")
	for resfile in RESULT_FILES:
		if simfile_name in resfile:
			resfiles.append({"filename":resfile,"result":load_result_file(resfile)})

	# calculate costs
	for resfile in resfiles:
		for res in resfile["result"]:
			vars = getVariables(resfile)
			threshold = vars["threshold"]
			clusters = res["clusters"]
			num_clusters = len(clusters)
			cost = calculate_cost(sim_data,float(threshold),clusters)
			_cpp_cost = res["_cost"]
			if _cpp_cost != -1:
				if round(_cpp_cost - cost,1) > 0:
					print("ERROR ERROR ERROR")
					print("Cost difference for:")
					print(resfile["filename"])
					print("with threshold: " + str(threshold))
					print("cpp cost:  " +str(_cpp_cost))
					print("calc cost: " +str(cost))
					print("diff:      " + str(abs(_cpp_cost-cost)))


			# write to log
			with open(cost_log, "a") as f:
				f.write("\t".join([
					vars["name"],
					vars["threshold"],
					str(vars["fpt"]),
					vars["program"],
					str(cost),
					str(res["_cost"]),
					"\n"
				]))
