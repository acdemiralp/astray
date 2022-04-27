#!/bin/bash

THIS_SCRIPT=$(readlink -f "$0")
THIS_SCRIPT_PATH=$(dirname "$THIS_SCRIPT")
ROOT_DIR=$THIS_SCRIPT_PATH/../../../

build()
{
  cmake .. -DASTRAY_USE_MPI=ON -DASTRAY_BUILD_APPLICATIONS=ON -DASTRAY_BUILD_TESTS=OFF -DASTRAY_DEVICE_SYSTEM=$1
  cd ..
  ./bootstrap.sh
  cd build
}
run()
{
  mpiexec -n 1  -m 1 -hostfile=$MPIHOSTLIST ./benchmark_cluster
  mpiexec -n 2  -m 1 -hostfile=$MPIHOSTLIST ./benchmark_cluster
  mpiexec -n 4  -m 1 -hostfile=$MPIHOSTLIST ./benchmark_cluster
  mpiexec -n 8  -m 1 -hostfile=$MPIHOSTLIST ./benchmark_cluster
  mpiexec -n 16 -m 1 -hostfile=$MPIHOSTLIST ./benchmark_cluster
}

ssh -t ad784563@login18-g-1 "
  $(typeset -f build);
  $(typeset -f run);

  source ~/.zshrc

  module load gcc/9
  module load cuda/11.5

  export MPIHOSTLIST=$THIS_SCRIPT_PATH/hostlist.txt

  cd $ROOT_DIR
  ./bootstrap.sh
  cd build
  
  build CUDA
  run
  build OMP
  run
  build TBB
  run
  build CPP
  run

  cd $ROOT_DIR/utility/plot/
  python3 plot_cluster_benchmarks.py
"
