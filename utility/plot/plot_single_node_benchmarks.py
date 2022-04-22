import matplotlib.pyplot as plt
import pandas            as pd

file_prefix = '../../data/outputs/performance/benchmark_single_'
devices     = ['cpp', 'omp', 'tbb', 'cuda']
metrics     = ["minkowski", "schwarzschild", "kerr", "kastor_traschen"]

figure, plots = plt.subplots(1,4)
for device in devices:
    data = pd.read_csv(file_prefix + device + ".csv")
    for index, metric in enumerate(metrics):
        selection = data[data['metric'] == metric].sort_values(by=['width'])
        plots[index].set_title (metric)
        plots[index].set_xlabel("Image Size (Pixels)")
        plots[index].set_ylabel("Time (Milliseconds)")
        plots[index].plot      (selection['width'].apply(str) + 'x' + selection['height'].apply(str), selection['mean'], label=device)
        plots[index].legend    ()
plt.show()