import matplotlib.pyplot as plt
import pandas            as pd

def create_plot(benchmark, device, metric, plot):
    data         = benchmark[benchmark['metric'] == metric]
    data         = data.sort_values(by=['width'])
    plot.set_title (metric)
    plot.plot      (data['width'].apply(str) + 'x' + data['height'].apply(str), data['mean'], label=device)
    plot.set_xlabel("Image Size (Pixels)")
    plot.set_ylabel("Time (Milliseconds)")
    plot.legend    ()

benchmarks = {
    'cpp' : '../../data/outputs/performance/benchmark_single_cpp.csv' , 
    'omp' : '../../data/outputs/performance/benchmark_single_omp.csv' , 
    'tbb' : '../../data/outputs/performance/benchmark_single_tbb.csv' ,
    'cuda': '../../data/outputs/performance/benchmark_single_cuda.csv'}
figure, plots = plt.subplots(1,4)
for key in benchmarks:
    benchmark = pd.read_csv(benchmarks[key])
    create_plot(benchmark, key, 'minkowski'      , plots[0]) 
    create_plot(benchmark, key, 'schwarzschild'  , plots[1])
    create_plot(benchmark, key, 'kerr'           , plots[2])
    create_plot(benchmark, key, 'kastor_traschen', plots[3])
plt.show()