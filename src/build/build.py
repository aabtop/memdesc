import argparse
import json
import os
import shutil
import subprocess

parser = argparse.ArgumentParser(description="Run a build.")
parser.add_argument(
    "-o", "--out_folder", required=True, help="Location of build output folder."
)
args = parser.parse_args()

src_folder = os.path.join(
    os.path.dirname(os.path.realpath(__file__)), os.pardir
)

os.chdir(args.out_folder)

# Build the main executable.
subprocess.run(
    ["cmake", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON", src_folder]
).check_returncode()
subprocess.run(
    ["cmake", "--build", ".", "--target", "memdesc"]
).check_returncode()

# Build and run the tests.
subprocess.run(
    ["cmake", "--build", ".", "--target", "memdesc_tests"]
).check_returncode()
subprocess.run(["ctest", "--output-on-failure"]).check_returncode()

# Build and run the end-to-end tests.
subprocess.run(
    ["cmake", "--build", ".", "--target", "end_to_end_c_tests"]
).check_returncode()

# Run clang-tidy, if it's available.
if shutil.which("run-clang-tidy"):
    files_in_build = []
    with open("compile_commands.json") as compile_commands_file:
        files_in_build = [x["file"] for x in json.load(compile_commands_file)]

    files_to_scan = [
        x
        for x in files_in_build
        if not (
            ("parser_wrapper" in x)
            or ("lexer_wrapper" in x)
            or ("third_party" in x)
        )
    ]

    # Run clang-tidy.
    subprocess.run(
        ["run-clang-tidy", "-quiet", "-header-filter=src/", "-p=./"]
        + files_to_scan
    ).check_returncode()
