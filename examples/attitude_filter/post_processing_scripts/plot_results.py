import h5py
import numpy as np
import os

hdf5_path = "C:/git/personal_project/examples/attitude_filter/results/zach_test_RUN_00000.hdf5"

# Open the HDF5 file in read-only mode
with h5py.File(hdf5_path, "r") as f:
    # Print the file structure (optional, for debugging)
    print("File structure:")
    f.visititems(lambda name, obj: print(f"{name} -> {type(obj)}"))

    # Navigate to the gyro group and read the delta_thetas dataset
    delta_thetas = f["/gyro/delta_thetas"][:]

# Convert to NumPy array (already NumPy type, but ensures proper type)
delta_thetas = np.array(delta_thetas)

print("\nDelta Thetas shape:", delta_thetas.shape)
print("First few rows:\n", delta_thetas[:5])
