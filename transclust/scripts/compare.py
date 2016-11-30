#!/usr/bin/env python3.5
import argparse
import os
import time
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
# Parse args
################################################################################
parser = argparse.ArgumentParser(
	formatter_class=argparse.ArgumentDefaultsHelpFormatter
)

# path to c++ Transclust
parser.add_argument(
	"--cpp",
	help="path/to/c++ transclust",
	default="/home/mikkel/Dropbox/Datalogi/Speciale/repo/transclust/build/release/bin/tclust"
)
parser.add_argument(
	"--cpp_only",
	action="store_true",
	help="Only test on C++ Transclust")

parser.add_argument(
	"--mpi",
	default=1,
	help="Number of mpi processes to spawn")

parser.add_argument(
	"--java_only",
	action="store_true",
	help="Only test on Java Transclust")

parser.add_argument(
	"--force_only",
	action="store_true",
	help="Only test on with FORCE")

# path to java Transclust
parser.add_argument(
	"--java",
	help="path/to/java transclust",
	default="/home/mikkel/Dropbox/Datalogi/Speciale/ref_code/TransClust-2.1.0.23.jar"
)

# output path
parser.add_argument(
	"-o",
	help="path/to/ouput directory - Here a new timestamped directory will be created holding the output",
	default="/home/mikkel/Dropbox/Datalogi/Speciale/repo/transclust/data/results"
)

# input path
parser.add_argument(
	"-i",
	help="path/to/dataset directory - The program will search recursivly from this directory for '*.sim' files",
	default="/home/mikkel/Dropbox/Datalogi/Speciale/repo/transclust/data")

parser.add_argument(
	"-n",
	help="Number of different threshods to try on each similarity file. Uniformly distributed through the similarity range",
	default=10)

parser.add_argument(
	"--memory_limit",
	help="Memory limit, in MB,  for each connected component. 0 is unlimited",
	default=0)

parser.add_argument(
	"--tag",
	help="Will be prepended to the output dir",
	default="test_run")

args = parser.parse_args()

################################################################################
# Setup output directory
################################################################################
oname = time.strftime("%Y-%-m-%d--%H-%M-%S") + "__" + str(args.tag)
odir = os.path.join(args.o,oname)

try:
	os.mkdir(odir)
	logging.info("Created output directory: " + str(odir))
except:
	logging.fatal("Could not create output directory: " + str(odir))

################################################################################
# Get all similarity files
################################################################################
logging.info("Collecting similarity files")
SIMILARITY_FILES = []
for root, dirs, files in os.walk(args.i):
	for f in files:
		if f.endswith(".sim"):
			SIMILARITY_FILES.append(os.path.join(root, f))
logging.info("Collecting similarity files ... done")
################################################################################
# Read similarity files and find min and max values
################################################################################
logging.info("Finding similarity files min/max values")
SIMILARITY_FILE_THRESHOLD = {}
for simfile in SIMILARITY_FILES:
	max_value = float("-inf")
	min_value = float("inf")

	simfile_name = os.path.basename(simfile)
	logging.info("Finding for " + str(simfile_name))
	# for each sim file
	with open(simfile,"r") as f:
		# for each line in simfile
		for line in f:
			line = line.split()
			# make sure that the line is not inf/-inf
			line[2] = line[2].lower()
			if line[2] != "inf" and line[2] != "-inf":
				value = float(line[2])
				if value > max_value:
					max_value = value
				if value < min_value:
					min_value = value

		SIMILARITY_FILE_THRESHOLD[simfile_name] = {"min":min_value,"max":max_value}
	logging.info("Finding for " + str(simfile_name) + " ... done")
logging.info("Collecting similarity files min/max values ... done")

################################################################################
# HELPER FUNCTIONS
################################################################################
def getOutputFileName(simfile_name,threshold,fpt,program):
	simfile_name = simfile_name.split(".")[0]
	if fpt:
		return "_".join([
			simfile_name,
			"t",str(threshold),
			"wFPT",
			program
		])+".res"
	else:
		return "_".join([
			simfile_name,
			"t",str(threshold),
			"nFPT",
			program
		])+".res"

def getCommand(simfile_name,threshold,fpt,program):
	# output file string
	outfile = getOutputFileName(simfile_name,threshold,fpt,program)
	outfile = os.path.join(odir,outfile)

	if program == "cpp":
		command = " ".join([
			args.cpp,
			"-s",simfile,
			"-o",outfile,
			"-t",threshold,
			"--memory_limit",str(args.memory_limit)
		])
		command = "mpirun -n " + str(args.mpi) + " " + command
		if not fpt:
			command = command + " --disable_fpt"

	if program == "java":
		command = " ".join([
			"java -jar",
			args.java,
			"-s",simfile,
			"-o",outfile,
			"-t",threshold,
			"-p","NONE",
			"--defaultmissing","0",
			"--simfileformat","SIMPLE"
		])
		if not fpt:
			command = command + " FPTConfig.MAX_SIZE 0"

	return command


################################################################################
# RUN EACH PROGRAM ON EACH SIMFILE WITH DIFFERENT THRESHOLDS
################################################################################
logging.info("Runnig program")
program_output_log = os.path.join(odir,"run.log")
execution_durations = os.path.join(odir,"execution_duration.log")

if args.force_only:
	algs = [False]
else:
	algs = [True,False]

# write header in output file
with open(execution_durations, "a") as f:
	f.write("\t".join(["program","using_fpt","threshold","similarity_file","time","\n"]))

for simfile in SIMILARITY_FILES:

	# simfile base name
	simfile_name = os.path.basename(simfile)
	logging.info("Running file " + str(simfile_name))

	#############################################################################
	# make threshold range
	#############################################################################
	min_value = SIMILARITY_FILE_THRESHOLD[simfile_name]["min"]
	max_value = SIMILARITY_FILE_THRESHOLD[simfile_name]["max"]

	# Threshold sequence
	t_step = (max_value-min_value)/(int(args.n)+2)

	THRESHOLDS = []
	current_t = round(min_value + t_step,3)

	while(current_t < max_value):
		THRESHOLDS.append(str(current_t))
		current_t = round(current_t + t_step,3)
	THRESHOLDS.pop()
	THRESHOLDS.pop()

	logging.info("Running  with thresholds: [" + ", ".join(THRESHOLDS)+ "]")
	for threshold in THRESHOLDS:
		for fpt in algs:
			for program in ["java","cpp"]:
				if (args.cpp_only and program == "cpp") or (args.java_only and program == "java") or (not args.cpp_only and not args.java_only):
					command = getCommand(simfile_name,threshold,fpt,program)
					logging.info("running: " + command)
					t1 = time.time()
					os.system(command + " >> " + program_output_log)
					t2 = time.time()
					logging.info("running: " + command + " ... done")

					with open(execution_durations, "a") as f:
						f.write("\t".join([program,str(fpt),threshold,simfile_name,str(t2-t1),"\n"]))

	logging.info("Running file " + str(simfile_name) + " ... done")
logging.info("DONE")




#print(odir)
#
#os.system("java -jar " + args.java)
#os.system(args.cpp)


