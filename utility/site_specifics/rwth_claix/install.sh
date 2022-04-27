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

  zsh -l
"
