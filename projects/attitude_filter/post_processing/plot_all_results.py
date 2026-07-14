import runpy
import glob
import os

# All paths are resolved relative to this script's own folder so the script
# works no matter which directory it is invoked from
script_dir = os.path.dirname(os.path.abspath(__file__))

hdf5_folder = os.path.join(script_dir, "..", "results")
hdf5_files  = sorted(glob.glob(os.path.join(hdf5_folder, "*.hdf5")))
num_mc_runs = len(hdf5_files)

print("Number of runs:", num_mc_runs)


runpy.run_path(os.path.join(script_dir, "plot_true_dynamics.py"))
runpy.run_path(os.path.join(script_dir, "plot_gyro_data.py"))
runpy.run_path(os.path.join(script_dir, "plot_star_tracker_data.py"))
runpy.run_path(os.path.join(script_dir, "plot_attitude_filter.py"))