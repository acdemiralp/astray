import matplotlib.pyplot as plt
import pandas            as pd

# If image size is an array of numbers, creates strong scaling plots.
# If image size is an array of arrays , creates weak   scaling plots.
def create_plots(file_prefix, devices, process_counts, metrics, image_size, colors, plots):
    filtered = []
    for device in devices:
        for index, process_count in enumerate(process_counts):
            data = pd.read_csv(file_prefix + device + "_" + str(process_count) + ".csv")
            for metric in metrics:
                selection = data[
                    (data['metric'] == metric) &
                    (data['width' ] == (image_size[index][0] if isinstance(image_size[0], list) else image_size[0])) &
                    (data['height'] == (image_size[index][1] if isinstance(image_size[0], list) else image_size[1])) ]
                filtered.append({
                    'device'       : device            , 
                    'process_count': str(process_count), 
                    'metric'       : metric            , 
                    'mean'         : selection['mean'].max()})
    filtered = pd.DataFrame(filtered)

    for device_index, device in enumerate(devices):
        for index, metric in enumerate(metrics):
            selection = filtered[
                (filtered['device'] == device) &
                (filtered['metric'] == metric) ]
            plots[index].set_title (metric)
            plots[index].set_xlabel("Process Count")
            plots[index].set_ylabel("Time (Milliseconds)")
            plots[index].plot      (selection['process_count'], selection['mean'], label=device, color=colors[device_index])
            plots[index].legend    ()

file_prefix               = '../../data/outputs/performance/benchmark_cluster_'
devices                   = ['cpp', 'cuda', 'omp', 'tbb']
metrics                   = ['minkowski', 'schwarzschild', 'kerr', 'kastor_traschen']
process_counts            = [1, 2, 4, 8, 16] # , 32, 64]
strong_scaling_image_size = [1024, 1024]
weak_scaling_image_sizes  = [[512, 512], [724, 724] ,[1024, 1024], [1448, 1448], [2048, 2048]] # , [2896, 2896], [4096, 4096]]
colors                    = [(0.4, 0.4, 0.4), (0.45, 0.75, 0), (0.75, 0.225, 0.225), (0, 0.45, 0.75)]

figure, plots  = plt.subplots(2,4)
create_plots(file_prefix, devices, process_counts, metrics, strong_scaling_image_size, colors, plots[0])
create_plots(file_prefix, devices, process_counts, metrics, weak_scaling_image_sizes , colors, plots[1])
plt.show()
