#!/usr/bin/env python3.5
import argparse
import os

################################################################################
# Parse args
################################################################################
parser = argparse.ArgumentParser()
parser.add_argument("simfile",help="Three column similarity file (for non \
		symetrical objects are smallest similarity used)")
parser.add_argument("-R",action="store_true",help="Prints the results as an R table with headers (default)")
parser.add_argument("-J",action="store_true",help="Prints the results as c3.js JSON")
parser.add_argument("resfiles",nargs='+',help="TransClust result file(s)")
args = parser.parse_args()

if not args.R and not args.J:
	args.R = True

################################################################################
# Read similarity file
################################################################################
def load_sim_data(simfile):
	index2object    = []
	#index2object_id = []
	object2index    = {}

	sim_value       = {}
	hasPartner      = {}
	with open(simfile) as f:
		for line in f:
			line = line.split()
			o1    = str(line[0])
			o2    = str(line[1])
			value = float(line[2])

			#######################################################################
			# Update maps
			#######################################################################
			if o1 not in object2index:
				_id = len(index2object)
				index2object.append(o1)
				object2index[o1] = _id
				#index2object_id.append(_id)

			if o2 not in object2index:
				_id = len(index2object)
				index2object.append(o2)
				object2index[o2] = _id
				#index2object_id.append(_id)

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
				hasPartner[key] = True
				if sim_value[key] < value:
					value = sim_value[key]
			else:
				hasPartner[key] = False

			sim_value[key] = value
	num_o = len(index2object)
	matrix = [[-1 for i in range(num_o)] for j in range(num_o)]

	for i in range(num_o):
		for j in range(i+1,num_o):
			key = str(index2object[i]+"_"+index2object[j])
			if key not in sim_value:
				val = 0
			else:
				if hasPartner[key]:
					val = sim_value[key]
				else:
					val = 0
			matrix[i][j] = val
			matrix[j][i] = val

	return {"matrix":matrix,"object2index":object2index}

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
			clusters = line[2].rstrip(";").split(";")

			_clusters = []
			for cluster in clusters:
				_clusters.append(cluster.split(","))
			results.append(
				{
					"threshold": float(line[0]),
					"cost": str(line[1]),
					"clusters": _clusters
				}
			)

	return results

################################################################################
# Calculates cost of clustering
################################################################################
def calculate_cost(sim_data,threshold,clustering):
	sim_matrix = sim_data["matrix"]
	object2index = sim_data["object2index"]
	#############################################################################
	# Transform clustering to membership vector
	#############################################################################
	membership = [-1]*len(sim_matrix)
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
			sim_value = sim_matrix[i][j] - threshold
			if membership[i] == membership[j] and sim_value < 0:
				cost -= sim_value
			elif membership[i] != membership[j] and sim_value > 0:
				cost += sim_value
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
################################################################################

# load similarity file
sim_data = load_sim_data(args.simfile)

# load result files
resfiles = []
for filename in args.resfiles:
	if os.path.isdir(filename):
		for f in os.listdir(filename):
			f = os.path.join(filename,f)
			if os.path.isfile(f):
				resfiles.append({"filename":f,"result":load_result_file(f)})

	else:
		resfiles.append({"filename":filename,"result":load_result_file(filename)})

# calculate cost for each result
data = {}
for resfile in resfiles:
	data[resfile["filename"]] = []
	filename = os.path.basename(resfile["filename"])
	for res in resfile["result"]:
		threshold = res["threshold"]
		clusters = res["clusters"]
		cost = calculate_cost(sim_data,threshold,clusters)
		data[resfile["filename"]].append({"threshold":threshold,"cost":cost})

# print data
if args.R:
	print_R(data)
elif args.J:
	print_JSON(data)


