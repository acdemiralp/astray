#!/bin/bash

THIS_SCRIPT=$(readlink -f "$0")
THIS_SCRIPT_PATH=$(dirname "$THIS_SCRIPT")
ROOT_DIR=$THIS_SCRIPT_PATH/../../../

ssh -t ad784563@login18-g-1 "
  source ~/.zshrc

  module load gcc/9
  module load cuda/11.5

  export MPIHOSTLIST=$THIS_SCRIPT_PATH/hostlist.txt

  cd $ROOT_DIR
  ./bootstrap.sh
  
  cd build
  cmake .. -DASTRAY_USE_MPI=ON -DASTRAY_BUILD_APPLICATIONS=ON -DASTRAY_DEVICE_SYSTEM=CUDA
  cd ..
  ./bootstrap.sh
  cd build

  mpiexec -n 1 -m 1 ./benchmark_cluster
  mpiexec -n 2 -m 2 ./benchmark_cluster
  mpiexec -n 4 -m 4 ./benchmark_cluster
  mpiexec -n 8 -m 8 ./benchmark_cluster
  mpiexec -n 16 -m 16 ./benchmark_cluster
  
  cmake .. -DASTRAY_DEVICE_SYSTEM=OMP
  cd ..
  ./bootstrap.sh  
  cd build

  mpiexec -n 1 -m 1 ./benchmark_cluster
  mpiexec -n 2 -m 2 ./benchmark_cluster
  mpiexec -n 4 -m 4 ./benchmark_cluster
  mpiexec -n 8 -m 8 ./benchmark_cluster
  mpiexec -n 16 -m 16 ./benchmark_cluster
  
  cmake .. -DASTRAY_DEVICE_SYSTEM=TBB
  cd ..
  ./bootstrap.sh  
  cd build

  mpiexec -n 1 -m 1 ./benchmark_cluster
  mpiexec -n 2 -m 2 ./benchmark_cluster
  mpiexec -n 4 -m 4 ./benchmark_cluster
  mpiexec -n 8 -m 8 ./benchmark_cluster
  mpiexec -n 16 -m 16 ./benchmark_cluster
  
  cmake .. -DASTRAY_DEVICE_SYSTEM=CPP
  cd ..
  ./bootstrap.sh  
  cd build

  mpiexec -n 1 -m 1 ./benchmark_cluster
  mpiexec -n 2 -m 2 ./benchmark_cluster
  mpiexec -n 4 -m 4 ./benchmark_cluster
  mpiexec -n 8 -m 8 ./benchmark_cluster
  mpiexec -n 16 -m 16 ./benchmark_cluster
"
