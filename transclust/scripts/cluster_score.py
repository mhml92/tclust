#!/usr/bin/env python3.5
import sys
import argparse
import ntpath
import math
import operator as op
from functools import reduce


parser = argparse.ArgumentParser()
parser.add_argument("f",help="Input file")
parser.add_argument("--trans_gs", help="Golden standard file in the format of transclust output")
parser.add_argument("--col_gs", help="Golden standard file in the two column format")
parser.add_argument("--fscore",help="Compare using f-score (default)",action="store_true")
parser.add_argument("--rand_index",help="Compare using Rand Index",action="store_true")
parser.add_argument("-H",help="Prepend header to output",action="store_true")
args = parser.parse_args()

if(args.trans_gs == None and args.col_gs == None):
	sys.exit("No golden standard file specified")

if(args.trans_gs != None and args.col_gs != None):
	sys.exit("There can be only one golden standard file")

if(args.fscore and args.rand_index):
	sys.exit("Only one analysis at a time")

# two column golden standard file format parser template
def parse_two_col_gs(file_name):
	data = {}
	cluster_map = {}
	with open(file_name,"r") as f:
		for line in f:
			line = line.split()
			obj = str(line[0])
			c_id = str(line[1])
			if c_id not in cluster_map:
				cluster_map[c_id] = []
				cluster_map[c_id].append(obj)

	data['clusters'] = []
	for key in cluster_map:
		data['clusters'].append(cluster_map[key])

	return data

# Get clusters and cost for each threshold
def parse_transclust_output(file_name):
	data = []
	with open(file_name,"r") as f:
		for line in f:
			line = line.split()
			threshold = line.pop(0)
			cost = line.pop(0)
			cluster_id = -1
			clusters = []
			line = line[0].split(";")
			for objects in line:
				cluster_id = cluster_id +1
				objects = objects.split(",")
				if '' == objects[-1]:
					objects.pop()
				clusters.append(objects)

			data.append({
				"threshold":threshold,
				"cost":cost,
				"clusters":clusters})
	return data

def fscore(clusters, ref_clusters):
	fm = 0.0
	for ref_cluster in ref_clusters:
		max_score = 0.0
		for cluster in clusters:
			common_elements = len(set(cluster).intersection(ref_cluster))
			if common_elements > 0:

					tp = common_elements
					fp = len(cluster) - common_elements
					fn = len(ref_cluster) - common_elements

					precision = tp/(tp+fp)
					recall = tp/(tp+fn)

					score = (2*precision*recall)/(precision+recall)

					if score > max_score:
						max_score = score
						if max_score == 1:
							break
		fm += max_score

	return fm / len(ref_clusters)

def randIndex(clusters, ref_clusters):
	# get all objects in one array
	ref_objects = []
	ref_membership = []
	cluster_num = 0
	for ref in ref_clusters:
		ref_objects = ref_objects + ref
		ref_membership.extend([cluster_num]*len(ref))
		cluster_num = cluster_num+1

	ref_objects, ref_membership = (list(t) for t in zip(*sorted(zip(ref_objects,ref_membership))))

	objects = []
	membership = []
	cluster_num = 0
	for cls in clusters:
		objects = objects + cls
		membership.extend([cluster_num]*len(cls))
		cluster_num = cluster_num+1

	objects, membership = (list(t) for t in zip(*sorted(zip(objects,membership))))

	a = 0
	b = 0
	for i in range(0,len(ref_objects)):
		for j in range(i+1,len(ref_objects)):
			if ref_membership[i] == ref_membership[j]:
				if membership[i] == membership[j]:
					a = a+1
			else:
				if membership[i] != membership[j]:
					b = b+1

	return (a+b)/ncr(len(ref_objects),2)

# n choose r
def ncr(n, r):
	r = min(r, n-r)
	if r == 0: return 1
	numer = reduce(op.mul, range(n, n-r, -1))
	denom = reduce(op.mul, range(1, r+1))
	return numer//denom

# get filename from a path
def path_leaf(path):
	head, tail = ntpath.split(path)
	return tail or ntpath.basename(head)

def printR(args,result,reference,multiline,func):
	if args.H:
		print("threshold\tfile\tgs_file\tmeasure")

	for i in range(0,len(result)):
		#sys.stderr.write("calculating " + str(i) + " of " + str(len(result)) + "\n")
		if multiline:
			# find threshold in reference
			found = False
			ref_i = 0
			while(not found):
				res_t = float(result[i]["threshold"])
				ref_t = float(reference[ref_i]['threshold'])
				print(res_t,ref_t)
				if math.isclose(res_t,ref_t,abs_tol=0.01):
					print("true")
					found = True
				else:
					ref_i += 1
			if not found:
				sys.exit("No matching threshold found")

			fm = func(result[i]["clusters"],reference[ref_i]["clusters"])
			gsfile = path_leaf(args.trans_gs)
		else:
			fm = func(result[i]["clusters"],reference["clusters"])
			gsfile = path_leaf(args.col_gs)

	th = result[i]["threshold"]
	f_1 = path_leaf(args.f)
	print(str(th)+"\t"+f_1+"\t"+gsfile+"\t"+str(fm))

################################################################################
# Start
################################################################################

# read input file
result = parse_transclust_output(args.f)
# read reference file
if args.col_gs != None:
	reference = parse_two_col_gs(args.col_gs)
else:
	reference = parse_transclust_output(args.trans_gs)

# choose analysis
if args.rand_index:
	func = randIndex
else:
	func = fscore

# choose comparison method
if args.trans_gs != None:
	multiline = True
else:
	multiline = False

printR(args,result,reference,multiline,func)
