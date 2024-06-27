import matplotlib.pyplot as plt
import pandas as pd
import sys

def plot_cache_data(csv_file, output_png):
    data = pd.read_csv(csv_file)

    plt.figure(figsize=(15, 9))
    plt.plot(data['Step'], data['HitCount'], label='Hit Count')
    plt.plot(data['Step'], data['MissCount'], label='Miss Count')
    plt.xlabel('Step')
    plt.ylabel('Count')
    plt.title('Cache Hit and Miss Counts Over Time')
    plt.legend()
    plt.grid(True)

    plt.savefig(output_png)

    print(f"Plot saved as {output_png}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 plotter.py <input_csv_file> <output_png_file>")
        sys.exit(1)

    input_csv = sys.argv[1]
    output_png = sys.argv[2]
    plot_cache_data(input_csv, output_png)
