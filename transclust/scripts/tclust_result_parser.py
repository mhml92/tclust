#!/usr/bin/env python3.5
import argparse
import os
from datetime import datetime

def getTimeFromLine(line):
	time = line.split(" ")
	time = time[0] + " " + time[1]
	time = datetime.strptime(time,'%Y-%m-%d %H:%M:%S.%f')
	return time


parser = argparse.ArgumentParser()
# input path
parser.add_argument(
	"-i",
	help="path/to/res directory - Recursivly look for *.tcr (TransClust Result) files",
	required = True
	)
args = parser.parse_args()

TCR = []
for root, dirs, files in os.walk(args.i):
	for f in files:
		if f.endswith(".tcr"):
			TCR.append(os.path.join(root, f))

# print header
print("input,file,nproc,ncpu,phase,seconds")
for file in TCR:
	abs_path = os.path.abspath(file)
	dir_name = os.path.dirname(abs_path).split("/")[-1]

	filename = file.split("/")[-1].split(".")[0]
	nproc = int(filename.split("_")[-3])
	ncpu = int(filename.split("_")[-1])

	read_file_time = 0
	sort_file_time = 0
	init_cc_time = 0
	clustering_time = 0
	with open(abs_path) as f:
		read_start = 0
		sort_start = 0
		init_start = 0
		cluster_start = 0
		for line in f:
			if "[InputParser::getConnectedComponents@71] Reading input file" in line:
				read_start = getTimeFromLine(line)

			if "[InputParser::getConnectedComponents@129] Reading input file...done" in line:
				read_end = getTimeFromLine(line)
				read_file_time = (read_end - read_start).total_seconds()

			if "[InputParser::getConnectedComponents@135] Sorting input" in line:
				sort_start = getTimeFromLine(line)

			if "[InputParser::getConnectedComponents@139] Sorting input...done" in line:
				sort_end = getTimeFromLine(line)
				sort_file_time = (sort_end - sort_start).total_seconds()

			if "[InputParser::getConnectedComponents@145] Building DUF" in line:
				init_start = getTimeFromLine(line)

			if "[main@500] Clustering " in line:
				init_end = getTimeFromLine(line)
				init_cc_time = (init_end - init_start).total_seconds()
				cluster_start = init_end

			if "[finalize_master@345] Threshold:" in line:
				cluster_end = getTimeFromLine(line)
				clustering_time = (cluster_end - cluster_start).total_seconds()

	input = filename.split("_")[0]
	print(",".join((input,filename,str(nproc),str(ncpu),"reading file",str(read_file_time))))
	print(",".join((input,filename,str(nproc),str(ncpu),"sorting file",str(sort_file_time))))
	print(",".join((input,filename,str(nproc),str(ncpu),"Init CC",str(init_cc_time))))
	print(",".join((input,filename,str(nproc),str(ncpu),"Clustering",str(clustering_time))))

