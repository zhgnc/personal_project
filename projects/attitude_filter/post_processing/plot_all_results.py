import runpy
import glob
import os

hdf5_folder = "C:/git/personal_project/projects/attitude_filter/results/"
hdf5_files  = sorted(glob.glob(os.path.join(hdf5_folder, "*.hdf5")))
num_mc_runs = len(hdf5_files)

print("Number of runs:", num_mc_runs)


runpy.run_path("plot_true_dynamics.py")
runpy.run_path("plot_gyro_data.py")
runpy.run_path("plot_star_tracker_data.py")
runpy.run_path("plot_attitude_filter.py")