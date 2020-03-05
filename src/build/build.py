import argparse
import os
import subprocess

parser = argparse.ArgumentParser(description='Run a build.')
parser.add_argument('-o', '--out_folder', required=True,
                    help='Location of build output folder.')
args = parser.parse_args()

src_folder = os.path.join(os.path.dirname(os.path.realpath(__file__)), os.pardir)

os.chdir(args.out_folder)
subprocess.run(['cmake', src_folder]).check_returncode()
subprocess.run(['cmake', '--build', '.']).check_returncode()

# And finally run the tests before we call it a day.
subprocess.run(['ctest']).check_returncode()
