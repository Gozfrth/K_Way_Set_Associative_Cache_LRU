import matplotlib.pyplot as plt
import pandas as pd

# Read the CSV file
data = pd.read_csv("cache_simulation.csv")

# Plot the data
plt.figure(figsize=(10, 6))
plt.plot(data['Step'], data['HitCount'], label='Hit Count')
plt.plot(data['Step'], data['MissCount'], label='Miss Count')
plt.xlabel('Step')
plt.ylabel('Count')
plt.title('Cache Hit and Miss Counts Over Time')
plt.legend()
plt.grid(True)

# Save the plot as an image file
plt.savefig("cache_simulation_plot.png")

# Optionally, you can print a message to confirm the plot was saved
print("Plot saved as cache_simulation_plot.png")

