import os
import matplotlib.pyplot as plt

def read_dat_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        # Assuming the data is in two columns separated by a space
        x_values = [float(line.split()[0]) for line in lines]
        y_values = [float(line.split()[1]) for line in lines]
    return x_values, y_values

def plot_graph(x_values, y_values):
    plt.plot(x_values, y_values, label='Data')
    plt.title('Execution Time and Number of Threads Used (5M iterations)')
    plt.xlabel('Number of Threads')
    plt.ylabel('Execution Time (Milliseconds)')
    plt.legend()
    plt.show()
    plt.savefig('output_plot.png')

if __name__ == "__main__":
    # Replace 'your_file.dat' with the actual path to your .dat file
    script_directory = os.path.dirname(os.path.abspath(__file__))
    data_folder = os.path.join(script_directory, '/', 'obj32')
    file_path = os.path.join(script_directory+data_folder, 'execution_times.dat')

    x_values, y_values = read_dat_file(file_path)
    plot_graph(x_values, y_values)