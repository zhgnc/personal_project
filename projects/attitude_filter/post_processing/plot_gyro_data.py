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


# Plot measured delta angles
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [deg]', 'Y-Axis [deg]', 'Z-Axis [deg]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        delta_theta_data = f["/gyro/meas_delta_angles"][:]
        sim_time_sec     = f["/sim/current_sim_time_sec"][:]

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], delta_theta_data[1:, i] * rad2deg, linewidth=0.5)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Gyro Measured Delta Angles vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("gyro_meas_delta_angles")


# Plot gyro biases
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Biases [deg/s]', 'Y-Biases [deg/s]', 'Z-Biases [deg/s]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        rate_biases     = f["/gyro/rate_biases"][:]
        sim_time_sec     = f["/sim/current_sim_time_sec"][:]

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], rate_biases[1:, i] * rad2deg, linewidth=0.5)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Gyro Rate Biases vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("gyro_rate_biases")


# Plot gyro misalignments
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [deg]', 'Y-Axis [deg]', 'Z-Axis [deg]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        misalignments    = f["/gyro/misalignments"][:]
        sim_time_sec     = f["/sim/current_sim_time_sec"][:]

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], misalignments[1:, i] * rad2deg, linewidth=0.5)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Gyro Misalignments vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("gyro_misalignments")


# Plot gyro scale factors
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [PPM]', 'Y-Axis [PPM]', 'Z-Axis [PPM]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        scale_factors    = f["/gyro/scale_factors"][:]
        sim_time_sec     = f["/sim/current_sim_time_sec"][:]

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], scale_factors[1:, i] * 1e6, linewidth=0.5)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Gyro Scale Factors vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("gyro_scale_factors")


# Plot total gyro errors
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [deg]', 'Y-Axis [deg]', 'Z-Axis [deg]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        total_error  = f["/gyro/total_delta_angle_error"][:]
        sim_time_sec = f["/sim/current_sim_time_sec"][:]

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], total_error[1:, i] * rad2deg, linewidth=0.5)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Gyro Delta Angle Total Error vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("gyro_total_error")