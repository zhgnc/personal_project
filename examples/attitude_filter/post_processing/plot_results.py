import h5py
import numpy as np
import matplotlib.pyplot as plt

hdf5_path = "C:/git/personal_project/examples/attitude_filter/results/zach_test_RUN_00001.hdf5"

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

# Get data
with h5py.File(hdf5_path, "r") as f:
    delta_theta_data = f["/gyro/Delta_Angles"][:]
    sim_time_sec     = f["/sim/current_sim_time_sec"][:]


print("delta_theta_data shape:", delta_theta_data.shape)
print("sim_time_sec shape:", sim_time_sec.shape)

print("First 10 sim_time_sec values:", sim_time_sec[:10])
print("Last 10 sim_time_sec values:", sim_time_sec[-10:])

#############
# Plot Data #
#############
plt.style.use('dark_background')

fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

axis_labels = ['X', 'Y', 'Z']
for i in range(3):
    axs[i].plot(sim_time_sec[1:], delta_theta_data[1:, i],
                label=f"Delta Theta {axis_labels[i]}", color=f"C{i}", linewidth=0.25)
    axs[i].set_ylabel("Delta Theta (rad)")
    axs[i].legend()
    axs[i].grid(True)

axs[2].set_xlabel("Simulation Time (sec)")
fig.suptitle("Gyro Delta Thetas Over Simulation Time", fontsize=14)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.show()

