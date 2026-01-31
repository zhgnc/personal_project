import h5py
import numpy as np
import matplotlib.pyplot as plt

import h5py
import numpy as np
import matplotlib.pyplot as plt
import glob
import os

# Path to folder containing HDF5 files
hdf5_folder = "C:/git/personal_project/projects/attitude_filter/results/"
hdf5_files  = sorted(glob.glob(os.path.join(hdf5_folder, "*.hdf5")))
num_mc_runs = len(hdf5_files)
print(num_mc_runs)

if not hdf5_files:
    raise FileNotFoundError(f"No HDF5 files found in folder: {hdf5_folder}")

#############
# Plot Data #
#############
plt.style.use('dark_background')
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

axis_labels = ['X', 'Y', 'Z']

# Loop over each HDF5 file
for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        delta_theta_data = f["/gyro/Delta_Angles"][:]
        sim_time_sec     = f["/sim/current_sim_time_sec"][:]

    # Plot each dataset with a different color and label
    for i in range(3):
        axs[i].plot(sim_time_sec[1:], delta_theta_data[1:, i], linewidth=0.5)

# Set labels, legends, and grid
for i in range(3):
    axs[i].set_ylabel("Delta Theta (rad)")
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)")
fig.suptitle(f"Gyro Delta Thetas Over Simulation Time ({num_mc_runs} MC runs)", fontsize=14)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("gyro_data")

#############
# Read Data #
#############
# # Print all attributes and values 
# def print_attributes(name, obj):
#     """Callback to print all attributes of a group or dataset"""
#     if obj.attrs:
#         print(f"\nAttributes for {name}:")
#         for key, val in obj.attrs.items():
#             print(f"  {key}: {val}")

# with h5py.File(hdf5_path, "r") as f:
#     print("=== All HDF5 Attributes ===")
#     f.visititems(print_attributes)


# print("delta_theta_data shape:", delta_theta_data.shape)
# print("sim_time_sec shape:", sim_time_sec.shape)

# print("First 10 sim_time_sec values:", sim_time_sec[:10])
# print("Last 10 sim_time_sec values:", sim_time_sec[-10:])