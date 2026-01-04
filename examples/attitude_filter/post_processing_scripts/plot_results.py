import h5py
import numpy as np
import matplotlib.pyplot as plt

hdf5_path = "C:/git/personal_project/examples/attitude_filter/results/zach_test_RUN_00000.hdf5"

#############
# Read Data #
#############

with h5py.File(hdf5_path, "r") as f:
    delta_theta_data = f["/gyro/Delta_Angles"][:]
    test_data        = f["/gyro/Test_Increment"][:]

#############
# Plot Data #
#############

fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

axis_labels = ['X', 'Y', 'Z']
for i in range(3):
    axs[i].plot(delta_theta_data[1:, i], label=f"Delta Theta {axis_labels[i]}", color=f"C{i}")
    axs[i].set_ylabel("Delta Theta")
    axs[i].legend()
    axs[i].grid(True)

axs[2].set_xlabel("Sample Index")
fig.suptitle("Gyro Delta Thetas Over Samples", fontsize=14)
plt.tight_layout(rect=[0, 0, 1, 0.96])  # Leave space for suptitle
plt.show()
