#!/usr/bin/python

import sys
import random


num_objects = int(sys.argv[1])
num_clusters = int(sys.argv[2])
output_dir = sys.argv[3]

h_sim = 100
l_sim = 10
jitter = 10

obj = []
obj.extend(xrange(1,num_objects+1))

clt = []
for i in xrange(0,num_objects):
    clt.append(random.randint(1,num_clusters))

f = open(output_dir+"/data.txt","w+")
for i in xrange(0,num_objects):
    for j in xrange(i+1,num_objects):
        pair = str(obj[i]) + "\t" + str(obj[j]) + "\t"
        if clt[i] == clt[j]:
            pair = pair + str(h_sim + (random.uniform(-1,1)*jitter))
        else:
            pair = pair + str(l_sim + (random.uniform(-1,1)*jitter))
        f.write(pair+"\n")
f.close()


f = open(output_dir+"/data_gs.txt","w+")
for i in xrange(0,num_objects):
    f.write(str(i+1)+"\t"+str(clt[i])+"\n")
f.close()
