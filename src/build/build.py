import subprocess
import os

os.chdir('/build/out')
subprocess.run(['cmake', '/build/src']).check_returncode()
subprocess.run(['cmake', '--build', '.', '--', '-j', str(os.cpu_count())]).check_returncode()

