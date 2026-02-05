import h5py
import math
import matplotlib.pyplot as plt

import glob
import os

rad2deg = 180.0 / math.pi

hdf5_folder = "C:/git/personal_project/projects/attitude_filter/results/"
hdf5_files  = sorted(glob.glob(os.path.join(hdf5_folder, "*.hdf5")))

num_mc_runs = len(hdf5_files)

if not hdf5_files:
    raise FileNotFoundError(f"No HDF5 files found in folder: {hdf5_folder}")

plt.style.use('dark_background')


# Plot measured quaternions
fig, axs = plt.subplots(4, 1, figsize=(10, 8), sharex=True)
axis_labels = ['i', 'j', 'k', 'real']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        q_meas       = f["/star_tracker/q_j2000_to_star_tracker_meas"][:]
        sim_time_sec = f["/sim/current_sim_time_sec"][:]

    for i in range(4):
        axs[i].plot(sim_time_sec[1:], q_meas[1:, i], linewidth=0.5)


for i in range(4):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Star Tracker Measured Quaternion vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("star_tracker_meas_quats")


# Plot measurement error
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [Deg]', 'Y-Axis [Deg]', 'Z-Axis [Deg]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        error        = f["/star_tracker/measurement_error_rad"][:]
        sim_time_sec = f["/sim/current_sim_time_sec"][:]

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], error[1:, i] * rad2deg, linewidth=0.5)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Star Tracker Measurement Error Per Axis vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("star_tracker_meas_error")


# Plot star tracker valid flags
fig, axs = plt.subplots(1, 1, figsize=(10, 8), sharex=True)
axis_labels = ['Tracker Valid Flag [1 == Valid]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        valid_flag   = f["/star_tracker/measurement_valid"][:]
        sim_time_sec = f["/sim/current_sim_time_sec"][:]

        axs.plot(sim_time_sec, valid_flag, linewidth=3.0, color="red")


axs.set_ylabel(axis_labels, fontsize=16)
axs.grid(True)

axs.set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Star Tracker Measurement Valid Flag vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("star_tracker_meas_flag")