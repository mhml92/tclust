#! /bin/bash
#
#SBATCH --account sdusscsa_slim   # account
#SBATCH --nodes 16                 # number of nodes
#SBATCH -c 24
#SBATCH --ntasks-per-node 1       # number of MPI tasks per node
#SBATCH --time 0:10:00            # max time (HH:MM:SS)

echo Running on "$(hostname)"
echo Available nodes: "$SLURM_NODELIST"
echo Slurm_submit_dir: "$SLURM_SUBMIT_DIR"

# Load the modules previously used when compiling the application
module purge
module add gcc/5.4.0 openmpi/1.10.4

if [ -n "$SLURM_CPUS_PER_TASK" ]; then
	omp_threads=$SLURM_CPUS_PER_TASK
else
	omp_threads=1
fi
export OMP_NUM_THREADS=$omp_threads
echo Start time: "$(date)"
srun ../../build/release/bin/tclust -s ../../data/actino/actino.sim -t 42 --external_sorting_ram 0 --tmp_dir /home/milev10/tmp/ > actino_n_16_c_24.txt
echo End time: "$(date)"

echo Done.
