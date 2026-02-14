import h5py
import math
import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial.transform import Rotation as R

import glob
import os

rad2deg = 180.0 / math.pi

hdf5_folder = "C:/git/personal_project/projects/attitude_filter/results/"
hdf5_files  = sorted(glob.glob(os.path.join(hdf5_folder, "*.hdf5")))

num_mc_runs = len(hdf5_files)

if not hdf5_files:
    raise FileNotFoundError(f"No HDF5 files found in folder: {hdf5_folder}")

plt.style.use('dark_background')


# Plot rotation attitude error
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [deg]', 'Y-Axis [deg]', 'Z-Axis [deg]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        rot_vec_error = f["/attitude_filter/rot_vec_error"][:] * rad2deg
        diag_cov      = f["/attitude_filter/diagonal_covariance"][:]
        sim_time_sec  = f["/sim/current_sim_time_sec"][:]
 
    std_3_bounds = 3.0 * np.sqrt(diag_cov[:, 0:3]) * rad2deg

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], rot_vec_error[1:, i], linewidth=0.5, label="Errors" if file_idx == 0 else None)
        
        axs[i].plot(sim_time_sec[1:], std_3_bounds[1:, i], linewidth=3.0, color="red", label="3-Sigma Bounds" if file_idx == 0 else None)
        axs[i].plot(sim_time_sec[1:],-std_3_bounds[1:, i], linewidth=3.0, color="red", label=None)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend()
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Rotation Vector Attitude Errors vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_attitude_errors")





# Plot gyro bias error
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [deg]', 'Y-Axis [deg]', 'Z-Axis [deg]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        bias_error = f["/attitude_filter/bias_error"][:] * rad2deg
        diag_cov      = f["/attitude_filter/diagonal_covariance"][:]
        sim_time_sec  = f["/sim/current_sim_time_sec"][:]
 
    std_3_bounds = 3.0 * np.sqrt(diag_cov[:, 3:6]) * rad2deg

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], bias_error[1:, i], linewidth=0.5, label="Errors" if file_idx == 0 else None)
        
        axs[i].plot(sim_time_sec[1:], std_3_bounds[1:, i], linewidth=3.0, color="red", label="3-Sigma Bounds" if file_idx == 0 else None)
        axs[i].plot(sim_time_sec[1:],-std_3_bounds[1:, i], linewidth=3.0, color="red", label=None)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend()
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Gyro Bias Estimate Errors vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_bias_errors")



# Plot gyro misalignment error
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [deg]', 'Y-Axis [deg]', 'Z-Axis [deg]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        misalignment_error = f["/attitude_filter/misalignment_error"][:] * rad2deg
        diag_cov           = f["/attitude_filter/diagonal_covariance"][:]
        sim_time_sec       = f["/sim/current_sim_time_sec"][:]
 
    std_3_bounds = 3.0 * np.sqrt(diag_cov[:, 6:9]) * rad2deg

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], misalignment_error[1:, i], linewidth=0.5, label="Errors" if file_idx == 0 else None)
        
        axs[i].plot(sim_time_sec[1:], std_3_bounds[1:, i], linewidth=3.0, color="red", label="3-Sigma Bounds" if file_idx == 0 else None)
        axs[i].plot(sim_time_sec[1:],-std_3_bounds[1:, i], linewidth=3.0, color="red", label=None)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend()
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Gyro to Star Tracker Misalignment Estimate Errors vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_misalignment_errors")



# Plot gyro scale factor error
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [PPM]', 'Y-Axis [PPM]', 'Z-Axis [PPM]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        scale_factor_error = f["/attitude_filter/scale_factor_error"][:] * 1e6
        diag_cov           = f["/attitude_filter/diagonal_covariance"][:]
        sim_time_sec       = f["/sim/current_sim_time_sec"][:]
 
    std_3_bounds = 3.0 * np.sqrt(diag_cov[:, 9:12]) * 1e6

    for i in range(3):
        axs[i].plot(sim_time_sec[1:], scale_factor_error[1:, i], linewidth=0.5, label="Errors" if file_idx == 0 else None)
        
        axs[i].plot(sim_time_sec[1:], std_3_bounds[1:, i], linewidth=3.0, color="red", label="3-Sigma Bounds" if file_idx == 0 else None)
        axs[i].plot(sim_time_sec[1:],-std_3_bounds[1:, i], linewidth=3.0, color="red", label=None)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend()
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Gyro Scale Factor Estimate Errors vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_scale_factor_errors")





# Plot residual
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [deg]', 'Y-Axis [deg]', 'Z-Axis [deg]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        rot_vec_residual = f["/attitude_filter/rot_vec_residual"][:] * rad2deg
        sim_time_sec     = f["/sim/current_sim_time_sec"][:]
 
    for i in range(3):
        axs[i].plot(sim_time_sec[1:], rot_vec_residual[1:, i], linewidth=0.5)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)
    # axs[i].set_ylim(-1, 1)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Rotation Vector Residual vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_residuals")
