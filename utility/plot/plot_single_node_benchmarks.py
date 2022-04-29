import matplotlib.pyplot as plt
import pandas            as pd

file_prefix = '../../data/outputs/performance/benchmark_single_'
devices     = ['cpp', 'cuda', 'omp', 'tbb']
metrics     = ["minkowski", "schwarzschild", "kerr", "kastor_traschen"]
colors      = [(0.4, 0.4, 0.4), (0.45, 0.75, 0), (0.75, 0.225, 0.225), (0, 0.45, 0.75)]

figure, plots = plt.subplots(1,4)
for device_index, device in enumerate(devices):
    data = pd.read_csv(file_prefix + device + ".csv")
    for index, metric in enumerate(metrics):
        selection = data[data['metric'] == metric].sort_values(by=['width'])
        plots[index].set_title (metric)
        plots[index].set_xlabel("Image Size (Pixels)")
        plots[index].set_ylabel("Time (Milliseconds)")
        plots[index].plot      (selection['width'].apply(str) + 'x' + selection['height'].apply(str), selection['mean'], label=device, color=colors[device_index])
        plots[index].legend    ()
plt.show()