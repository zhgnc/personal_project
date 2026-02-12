import h5py
import math
import matplotlib.pyplot as plt
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


# Plot measured quaternions
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
axis_labels = ['X-Axis', 'Y-Axis', 'Z-Axis']

for file_idx, hdf5_path in enumerate(hdf5_files):
    with h5py.File(hdf5_path, "r") as f:
        q_est        = f["/attitude_filter/q_j2000_to_body_est"][:]
        q_true       = f["/fake_dynamics/quat"][:]
        sim_time_sec = f["/sim/current_sim_time_sec"][:]

    print(q_est)
    
    q_eci_to_est  = R.from_quat(q_est)
    q_eci_to_true = R.from_quat(q_true)
    q_true_to_est = q_eci_to_est * q_eci_to_true.inv()

    # q_error = r_true_to_est.as_quat()
    rot_vec_error = q_true_to_est.as_rotvec()

    print(rot_vec_error)
    print(sim_time_sec)


    for i in range(3):
        axs[i].plot(sim_time_sec[1:], rot_vec_error[1:, i], linewidth=3.0)


for i in range(3):
    axs[i].set_ylabel(axis_labels[i], fontsize=16)
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)", fontsize=16)
fig.suptitle(f"Error in Attitude Estimate vs Simulation Time ({num_mc_runs} MC runs)", fontsize=16)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig("filter_attitude_error")