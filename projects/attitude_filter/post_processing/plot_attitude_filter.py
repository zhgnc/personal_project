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


first_run_file = hdf5_files[0]

# Plot true and estimated attitude
fig, axs = plt.subplots(4,1, figsize=(10, 8), sharex=True)
axis_labels = ['q_i', 'q_j', 'q_k', 'q_real']

with h5py.File(first_run_file, "r") as f:
    q_j2000_to_body_est   = f["/attitude_filter/q_j2000_to_body_est"][:]
    q_j2000_to_body_true  = f["/fake_dynamics/quat"][:]
    filter_time_sec       = f["/attitude_filter/current_time_sec"][:]
    sim_time_sec          = f["/sim/current_sim_time_sec"][:]


# True and Estimated Quaternions
axs[0].plot(filter_time_sec[1:], q_j2000_to_body_est[1:, 0], linewidth=3.0, linestyle='-', label="Est")
axs[0].plot(sim_time_sec[1:], q_j2000_to_body_true[1:, 0], linewidth=3.0, linestyle='--', label="True")

axs[1].plot(filter_time_sec[1:], q_j2000_to_body_est[1:, 1], linewidth=3.0, linestyle='-', label="Est")
axs[1].plot(sim_time_sec[1:], q_j2000_to_body_true[1:, 1], linewidth=3.0, linestyle='--', label="True")

axs[2].plot(filter_time_sec[1:], q_j2000_to_body_est[1:, 2], linewidth=3.0, linestyle='-', label="Est")
axs[2].plot(sim_time_sec[1:], q_j2000_to_body_true[1:, 2], linewidth=3.0, linestyle='--', label="True")

axs[3].plot(filter_time_sec[1:], q_j2000_to_body_est[1:, 3], linewidth=3.0, linestyle='-', label="Est")
axs[3].plot(sim_time_sec[1:], q_j2000_to_body_true[1:, 3], linewidth=3.0, linestyle='--', label="True")


for i in range(4):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend(loc="upper right")
    axs[i].grid(True)

axs[3].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Estimated and True Attitude vs Simulation Time", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_est_and_true_attitude")



# Plot true and estimated biases
fig, axs = plt.subplots(3,1, figsize=(10, 8), sharex=True)
axis_labels = ['X Bias [deg]', 'Y Bias [deg]', 'Z Bias [deg]']

with h5py.File(first_run_file, "r") as f:
    est_gyro_biases_deg  = f["/attitude_filter/est_gyro_biases_rad"][:] * rad2deg
    true_gyro_biases_deg = f["/gyro/angle_biases"][:]  * rad2deg
    filter_time_sec       = f["/attitude_filter/current_time_sec"][:]
    sim_time_sec          = f["/sim/current_sim_time_sec"][:]
 
# True and Estimated Biases
axs[0].plot(filter_time_sec[1:], est_gyro_biases_deg[1:, 0], linewidth=3.0, linestyle='-', label="Est")
axs[0].plot(sim_time_sec[1:], true_gyro_biases_deg[1:, 0], linewidth=3.0, linestyle='--', label="True")

axs[1].plot(filter_time_sec[1:], est_gyro_biases_deg[1:, 1], linewidth=3.0, linestyle='-', label="Est")
axs[1].plot(sim_time_sec[1:], true_gyro_biases_deg[1:, 1], linewidth=3.0, linestyle='--', label="True")

axs[2].plot(filter_time_sec[1:], est_gyro_biases_deg[1:, 2], linewidth=3.0, linestyle='-', label="Est")
axs[2].plot(sim_time_sec[1:], true_gyro_biases_deg[1:, 2], linewidth=3.0, linestyle='--', label="True")


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend(loc="upper right")
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Estimated and True Biases vs Simulation Time", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_est_and_true_biases")




# Plot true and estimated misalignments
fig, axs = plt.subplots(3,1, figsize=(10, 8), sharex=True)
axis_labels = ['X Misalign [deg]', 'Y Misalign [deg]', 'Z Misalign [deg]']

with h5py.File(first_run_file, "r") as f:
    est_gyro_misalign_deg  = f["/attitude_filter/est_gyro_to_st_misalignments_rad"][:] * rad2deg
    true_gyro_misalign_deg = f["/gyro/misalignments"][:]  * rad2deg
    filter_time_sec       = f["/attitude_filter/current_time_sec"][:]
    sim_time_sec          = f["/sim/current_sim_time_sec"][:]
 
# True and Estimated Biases
axs[0].plot(filter_time_sec[1:], est_gyro_misalign_deg[1:, 0], linewidth=3.0, linestyle='-', label="Est")
axs[0].plot(sim_time_sec[1:], true_gyro_misalign_deg[1:, 0], linewidth=3.0, linestyle='--', label="True")

axs[1].plot(filter_time_sec[1:], est_gyro_misalign_deg[1:, 1], linewidth=3.0, linestyle='-', label="Est")
axs[1].plot(sim_time_sec[1:], true_gyro_misalign_deg[1:, 1], linewidth=3.0, linestyle='--', label="True")

axs[2].plot(filter_time_sec[1:], est_gyro_misalign_deg[1:, 2], linewidth=3.0, linestyle='-', label="Est")
axs[2].plot(sim_time_sec[1:], true_gyro_misalign_deg[1:, 2], linewidth=3.0, linestyle='--', label="True")


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend(loc="upper right")
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Estimated and True Misalignments vs Simulation Time", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_est_and_true_misalignments")




# Plot true and estimated scale factors
fig, axs = plt.subplots(3,1, figsize=(10, 8), sharex=True)
axis_labels = ['X SF [PPM]', 'Y SF [PPM]', 'Z SF [PPM]']

with h5py.File(first_run_file, "r") as f:
    est_gyro_sf_ppm  = f["/attitude_filter/est_gyro_scale_factors"][:] * 1e6
    true_gyro_sf_ppm = f["/gyro/scale_factors"][:]  * 1e6
    filter_time_sec       = f["/attitude_filter/current_time_sec"][:]
    sim_time_sec          = f["/sim/current_sim_time_sec"][:]
 
# True and Estimated Biases
axs[0].plot(filter_time_sec[1:], est_gyro_sf_ppm[1:, 0], linewidth=3.0, linestyle='-', label="Est")
axs[0].plot(sim_time_sec[1:], true_gyro_sf_ppm[1:, 0], linewidth=3.0, linestyle='--', label="True")

axs[1].plot(filter_time_sec[1:], est_gyro_sf_ppm[1:, 1], linewidth=3.0, linestyle='-', label="Est")
axs[1].plot(sim_time_sec[1:], true_gyro_sf_ppm[1:, 1], linewidth=3.0, linestyle='--', label="True")

axs[2].plot(filter_time_sec[1:], est_gyro_sf_ppm[1:, 2], linewidth=3.0, linestyle='-', label="Est")
axs[2].plot(sim_time_sec[1:], true_gyro_sf_ppm[1:, 2], linewidth=3.0, linestyle='--', label="True")


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend(loc="upper right")
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Estimated and True Scale Factors vs Simulation Time", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_est_and_true_scale_factors")





# Plot rotation attitude error
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [arcsec]', 'Y-Axis [arcsec]', 'Z-Axis [arcsec]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        rot_vec_error   = f["/attitude_filter/rot_vec_error"][:] * rad2deg * 3600.0
        diag_cov        = f["/attitude_filter/diagonal_covariance"][:]
        filter_time_sec = f["/attitude_filter/current_time_sec"][:]
 
    std_3_bounds = 3.0 * np.sqrt(diag_cov[:, 0:3]) * rad2deg * 3600.0

    for i in range(3):
        axs[i].plot(filter_time_sec[1:], rot_vec_error[1:, i], linewidth=0.5, label="Errors" if file_idx == 0 else None)
        
        axs[i].plot(filter_time_sec[1:], std_3_bounds[1:, i], linewidth=3.0, color="red", label="3-Sigma Bounds" if file_idx == 0 else None)
        axs[i].plot(filter_time_sec[1:],-std_3_bounds[1:, i], linewidth=3.0, color="red", label=None)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend(loc="upper right")
    axs[i].grid(True)
    # axs[i].set_ylim(-500, 500)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Rotation Vector Attitude Errors vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_attitude_errors")





# Plot gyro bias error
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis [deg]', 'Y-Axis [deg]', 'Z-Axis [deg]']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        bias_error      = f["/attitude_filter/bias_error"][:] * rad2deg
        diag_cov        = f["/attitude_filter/diagonal_covariance"][:]
        filter_time_sec = f["/attitude_filter/current_time_sec"][:]
 
    std_3_bounds = 3.0 * np.sqrt(diag_cov[:, 3:6]) * rad2deg

    for i in range(3):
        axs[i].plot(filter_time_sec[1:], bias_error[1:, i], linewidth=0.5, label="Errors" if file_idx == 0 else None)
        
        axs[i].plot(filter_time_sec[1:], std_3_bounds[1:, i], linewidth=3.0, color="red", label="3-Sigma Bounds" if file_idx == 0 else None)
        axs[i].plot(filter_time_sec[1:],-std_3_bounds[1:, i], linewidth=3.0, color="red", label=None)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend(loc="upper right")
    axs[i].grid(True)
    # axs[i].set_ylim(-0.01, 0.01)

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
        filter_time_sec    = f["/attitude_filter/current_time_sec"][:]
 
    std_3_bounds = 3.0 * np.sqrt(diag_cov[:, 6:9]) * rad2deg

    for i in range(3):
        axs[i].plot(filter_time_sec[1:], misalignment_error[1:, i], linewidth=0.5, label="Errors" if file_idx == 0 else None)
        
        axs[i].plot(filter_time_sec[1:], std_3_bounds[1:, i], linewidth=3.0, color="red", label="3-Sigma Bounds" if file_idx == 0 else None)
        axs[i].plot(filter_time_sec[1:],-std_3_bounds[1:, i], linewidth=3.0, color="red", label=None)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend(loc="upper right")
    axs[i].grid(True)
    axs[i].set_ylim(-1, 1)


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
        filter_time_sec    = f["/attitude_filter/current_time_sec"][:]
 
    std_3_bounds = 3.0 * np.sqrt(diag_cov[:, 9:12]) * 1e6

    for i in range(3):
        axs[i].plot(filter_time_sec[1:], scale_factor_error[1:, i], linewidth=0.5, label="Errors" if file_idx == 0 else None)
        
        axs[i].plot(filter_time_sec[1:], std_3_bounds[1:, i], linewidth=3.0, color="red", label="3-Sigma Bounds" if file_idx == 0 else None)
        axs[i].plot(filter_time_sec[1:],-std_3_bounds[1:, i], linewidth=3.0, color="red", label=None)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].legend(loc="upper right")
    axs[i].grid(True)
    axs[i].set_ylim(-15000, 15000)

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
        filter_time_sec     = f["/attitude_filter/current_time_sec"][:]
 
    for i in range(3):
        axs[i].plot(filter_time_sec[1:], rot_vec_residual[1:, i], linewidth=0.5)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)
    # axs[i].set_ylim(-1, 1)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Rotation Vector Residual vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_residuals")
