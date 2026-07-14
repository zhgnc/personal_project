import h5py
import math
import matplotlib.pyplot as plt

import glob
import os

rad2deg = 180.0 / math.pi

hdf5_folder = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "results")
figures_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "figures")
os.makedirs(figures_dir, exist_ok=True)
hdf5_files  = sorted(glob.glob(os.path.join(hdf5_folder, "*.hdf5")))

num_mc_runs = len(hdf5_files)

if not hdf5_files:
    raise FileNotFoundError(f"No HDF5 files found in folder: {hdf5_folder}")

plt.style.use('dark_background')

# Plot true body rates
first_hdf5_file = hdf5_files[0]

with h5py.File(first_hdf5_file, "r") as f:
    body_rates       = f["/fake_dynamics/body_rates"][:]
    q_true           = f["/fake_dynamics/quat"][:]
    rot_vec_attitude = f["/fake_dynamics/rot_vec_attitude"][:]
    sim_time_sec = f["/sim/current_sim_time_sec"][:]

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
plt.savefig(figures_dir + "/True Body Rates")



# Plot true rot vec attitude
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
plt.savefig(figures_dir + "/True Rotation Vector Attitude")



# Plot true quaternion
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
plt.savefig(figures_dir + "/True Quatenion Attitude")