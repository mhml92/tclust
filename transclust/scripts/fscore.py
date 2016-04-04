#!/usr/bin/env python3
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("f",help="Input file")
#parser.add_argument("gs", help="Golden standard file to compare against")
args = parser.parse_args()


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
                clusters.append(objects.split(","))
            data.append({
                "threshold":threshold,
                "cost":cost,
                "clusters":clusters}
            )
    return data

def parse_two_col_gs(file_name):
    data = {}
    with open(file_name,"r") as f:
        for line in f:
            line = line.split()
            data[str(line[0])] = str(line[1])

    return data

print(parse_transclust_output(args.f))

