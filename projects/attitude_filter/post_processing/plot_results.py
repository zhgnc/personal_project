import h5py
import numpy as np
import math
import matplotlib.pyplot as plt

import glob
import os

rad2deg = 180.0 / math.pi

# Path to folder containing HDF5 files
hdf5_folder = "C:/git/personal_project/projects/attitude_filter/results/"
hdf5_files  = sorted(glob.glob(os.path.join(hdf5_folder, "*.hdf5")))

num_mc_runs = len(hdf5_files)
print("Number of runs:", num_mc_runs)

if not hdf5_files:
    raise FileNotFoundError(f"No HDF5 files found in folder: {hdf5_folder}")

plt.style.use('dark_background')

##############################
# Plot Measured Delta Angles #
##############################
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X', 'Y', 'Z']

# Loop over each HDF5 file
for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        delta_theta_data = f["/gyro/meas_delta_angles"][:]
        sim_time_sec     = f["/gyro/angle_biases"][:]
        sim_time_sec     = f["/gyro/misalignments"][:]
        sim_time_sec     = f["/gyro/scale_factors"][:]
        sim_time_sec     = f["/sim/current_sim_time_sec"][:]

    # Plot each dataset with a different color and label
    for i in range(3):
        axs[i].plot(sim_time_sec[1:], delta_theta_data[1:, i], linewidth=0.5)

# Set labels, legends, and grid
for i in range(3):
    axs[i].set_ylabel("Delta Theta (rad)", fontsize=16)
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Gyro Delta Thetas Over Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("gyro_data")

######################
# Plot Fake Dynamics #
######################
first_hdf5_file = hdf5_files[0]

with h5py.File(first_hdf5_file, "r") as f:
    body_rates       = f["/fake_dynamics/body_rates"][:]
    q_true           = f["/fake_dynamics/quat"][:]
    rot_vec_attitude = f["/fake_dynamics/rot_vec_attitude"][:]

fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

axs[0].plot(sim_time_sec[1:], body_rates[1:, 0] * rad2deg, linewidth=3.0, color="blue")
axs[0].set_ylabel("X-Axis [deg/s]", fontsize=16)
axs[0].grid(True)

axs[1].plot(sim_time_sec[1:], body_rates[1:, 1] * rad2deg, linewidth=3.0, color="green")
axs[1].set_ylabel("Y-Axis [deg/s]", fontsize=16)
axs[1].grid(True)

axs[2].plot(sim_time_sec[1:], body_rates[1:, 2] * rad2deg, linewidth=3.0, color="red")
axs[2].set_ylabel("Z-Axis [deg/s]", fontsize=16)
axs[2].grid(True)

axs[2].set_xlabel("Sim Time (sec)", fontsize=16)
fig.suptitle(f"True Body Rates vs Simulation Time", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("True Body Rates")




fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

axs[0].plot(sim_time_sec[1:], rot_vec_attitude[1:, 0] * rad2deg, linewidth=3.0, color="blue")
axs[0].set_ylabel("X-Axis [deg]", fontsize=16)
axs[0].grid(True)

axs[1].plot(sim_time_sec[1:], rot_vec_attitude[1:, 1] * rad2deg, linewidth=3.0, color="green")
axs[1].set_ylabel("Y-Axis [deg]", fontsize=16)
axs[1].grid(True)

axs[2].plot(sim_time_sec[1:], rot_vec_attitude[1:, 2] * rad2deg, linewidth=3.0, color="red")
axs[2].set_ylabel("Z-Axis [deg]", fontsize=16)
axs[2].grid(True)

axs[2].set_xlabel("Sim Time (sec)", fontsize=16)
fig.suptitle(f"Rotation Vector Attitude vs Simulation Time", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("True Rotation Vector Attitude")




fig, axs = plt.subplots(4, 1, figsize=(10, 8), sharex=True)

axs[0].plot(sim_time_sec[1:], q_true[1:, 0], linewidth=3.0, color="green")
axs[0].set_ylabel("i", fontsize=16)
axs[0].grid(True)

axs[1].plot(sim_time_sec[1:], q_true[1:, 1], linewidth=3.0, color="red")
axs[1].set_ylabel("j", fontsize=16)
axs[1].grid(True)

axs[2].plot(sim_time_sec[1:], q_true[1:, 2], linewidth=3.0, color="purple")
axs[2].set_ylabel("k", fontsize=16)
axs[2].grid(True)

axs[3].plot(sim_time_sec[1:], q_true[1:, 3], linewidth=3.0, color="blue")
axs[3].set_ylabel("Real", fontsize=16)
axs[3].grid(True)

axs[3].set_xlabel("Sim Time (sec)", fontsize=16)
fig.suptitle(f"True Quaternion vs Simulation Time", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("True Quatenion Attitude")

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