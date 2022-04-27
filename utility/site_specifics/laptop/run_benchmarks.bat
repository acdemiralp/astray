set THIS_SCRIPT=%~f0
set THIS_SCRIPT_PATH=%~dp0
set ROOT_DIR=%THIS_SCRIPT_PATH%/../../../

cd %ROOT_DIR%
call bootstrap.bat
cd build

cmake .. -DASTRAY_USE_MPI=OFF -DASTRAY_BUILD_APPLICATIONS=ON -DASTRAY_BUILD_TESTS=OFF -DASTRAY_DEVICE_SYSTEM=CUDA
cd ..
call bootstrap.bat
cd build
call Release\benchmark_single_node.exe

cmake .. -DASTRAY_DEVICE_SYSTEM=OMP
cd ..
call bootstrap.bat
cd build
call Release\benchmark_single_node.exe

cmake .. -DASTRAY_DEVICE_SYSTEM=TBB
cd ..
call bootstrap.bat
cd build
call Release\benchmark_single_node.exe

cmake .. -DASTRAY_DEVICE_SYSTEM=CPP
cd ..
call bootstrap.bat
cd build
call Release\benchmark_single_node.exe

cd %ROOT_DIR%/utility/plot/
py plot_single_node_benchmarks.py