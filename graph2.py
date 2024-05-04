import matplotlib.pyplot as plt
import numpy as np

# Cache sizes (in log scale)
cache_sizes = [14, 15, 16, 17, 18]  # Example cache sizes (adjust as needed)
# Bus write traffic for direct-mapped cache
direct_mapped_traffic = [100, 200, 300, 400, 500]  # Example traffic data (adjust as needed)
# Bus write traffic for 2-way set associative cache
two_way_traffic = [120, 220, 320, 420, 520]  # Example traffic data (adjust as needed)
# Bus write traffic for 4-way set associative cache
four_way_traffic = [140, 240, 340, 440, 540]  # Example traffic data (adjust as needed)

# Plot the graph
plt.figure(figsize=(10, 6))
plt.plot(cache_sizes, direct_mapped_traffic, label='Direct-Mapped Cache', marker='o')
plt.plot(cache_sizes, two_way_traffic, label='2-Way Set Associative Cache', marker='s')
plt.plot(cache_sizes, four_way_traffic, label='4-Way Set Associative Cache', marker='^')
plt.xlabel('Cache Size (log scale)')
plt.ylabel('Bus Write Traffic (log scale)')
plt.title('Bus Write Traffic vs Cache Size for Write-Back Caches')
plt.legend()
plt.grid(True)
plt.yscale('log')
plt.xscale('log')

# Save the plot as a PNG file
plt.savefig('graph2.png')
