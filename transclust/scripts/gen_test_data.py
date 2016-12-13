#!/usr/bin/env python3.5
import argparse
import multiprocessing
import time
import random
import math
import os

################################################################################
# Parse args
################################################################################
parser = argparse.ArgumentParser(
	formatter_class=argparse.ArgumentDefaultsHelpFormatter
)

parser.add_argument(
	"--cluster_size",
	help = "Number of elements in each cluster",
	default=100
	#requires = True
)

parser.add_argument(
	"--num_clusters_pr_cc",
	help = "Number of clusters pr Connected Component",
	default=3
	#requires = True
)

parser.add_argument(
	"--num_cc",
	help = "Number of Connected Components",
	default = 2
	#requires = True
)


parser.add_argument(
	"--layout_dim",
	help = "Dimensions for which to layout the elements, used for calculating "
		"similarity",
	default=3
)

parser.add_argument(
	"--num_processes",
	help = "Size of process pool",
	default=4
)

parser.add_argument(
	"--prop_missing",
	help = "The properbility that an edge is missing in a cluster",
	default=0.1
)

parser.add_argument(
	"--cluster_radius",
	help = "radius of each cluster (element are uniformly random distributed "
		"in the dimension between -cluster_radius/2:cluster_radius/2)",
	default=10
)

parser.add_argument(
	"--opt_threshold",
	help = "Optimal threshold to use, this is the threshold for which the COST fit",
	default=50
)

args = parser.parse_args()


def mp_worker(data):
	id = str(data["id"])
	num_clusters = data["num_clusters"]
	cluster_size = data["cluster_size"]
	clusters = []
	# generate a list of clusters for this connected component
	for cluster in range(num_clusters):
		pos = []
		for element in range(cluster_size):
			_pos = []
			for d in range(args.layout_dim):
				_pos.append(random.uniform(-args.cluster_radius/2,args.cluster_radius/2))
			pos.append(_pos)
		clusters.append(pos)
	# - write out the clusters in the simple 3-columns format
	# - chain together each cluster by one edge to create connected component
	sim_value_offset = args.opt_threshold + (3*args.cluster_radius)
	with open("cc_id_" + str(id)+".cc", "wt") as out_file:
		for c in range(len(clusters)):
			pos = clusters[c]
			# chain together connected components
			if(c != 0):
				out_file.write(
					str(id)+"_"+str(c-1)+"_"+str(0) + "\t" +
					str(id)+"_"+str(c)+"_"+str(0) + "\t" +
					str(sim_value_offset) + "\n"
				)

			i = 0
			while(i < len(pos)):
				j = i + 1
				while(j < len(pos)):
					p1 = pos[i]
					p2 = pos[j]
					dist = 0
					for d in range(len(p1)):
						dist = dist + math.pow(p1[d]-p2[d],2)
					dist = math.sqrt(dist)

					sim_value = sim_value_offset + (args.cluster_radius-dist)

					if(args.prop_missing < random.uniform(0,1)):
						out_file.write(
							str(id)+"_"+str(c)+"_"+str(i) + "\t" +
							str(id)+"_"+str(c)+"_"+str(j) + "\t" +
							str(sim_value) + "\n"
						)

					j = j+1
				i = i + 1
def mp_handler(data):
	p = multiprocessing.Pool()
	p.map(mp_worker, data)

data = []
for i in range(int(args.num_cc)):
	data.append(
		{
			"id":int(i),
			"num_clusters":int(args.num_clusters_pr_cc),
			"cluster_size":int(args.cluster_size)
		})
# gen connected components
mp_handler(data)

os.system("cat *.cc >> num_cluster_"+str(int(args.num_cc)*int(args.num_clusters_pr_cc))+"_t_50.sim && rm *.cc")


