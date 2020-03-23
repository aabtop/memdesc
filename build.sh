THIS_SCRIPT_LOCATION="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

HOST_SRC_DIR_SHELL=${THIS_SCRIPT_LOCATION}/src
HOST_OUT_DIR_SHELL=${THIS_SCRIPT_LOCATION}/out/docker/linux

DOCKERFILE_DIRECTORY_SHELL=${THIS_SCRIPT_LOCATION}/src/build/docker/linux

if [ -x "$(command -v docker.exe)" ] && [ -x "$(command -v wslpath)" ]; then
    # Presumably running from WSL with Docker Desktop for Windows.
    DOCKER_COMMAND=docker.exe
    HOST_SRC_DIR=$(wslpath -aw $HOST_SRC_DIR_SHELL)
    HOST_OUT_DIR=$(wslpath -aw $HOST_OUT_DIR_SHELL)
    DOCKERFILE_DIRECTORY=$(wslpath -aw $DOCKERFILE_DIRECTORY_SHELL)
elif [ -x "$(command -v docker)" ]; then
    DOCKER_COMMAND=docker
    HOST_SRC_DIR=${HOST_SRC_DIR_SHELL}
    HOST_OUT_DIR=${HOST_OUT_DIR_SHELL}
    DOCKERFILE_DIRECTORY=${DOCKERFILE_DIRECTORY_SHELL}
else
    echo "Cannot find docker in path."
    exit 1
fi

#If we're running on MSYS we need to prepend "winpty" to the docker command.
if [ "$(expr substr $(uname -s) 1 5)" == "MINGW" ]; then
    DOCKER_COMMAND="winpty ${DOCKER_COMMAND}"
fi

# Make sure the host output directory exists.
mkdir -p $HOST_OUT_DIR_SHELL

# The leading double slash is to workaround a path conversion issue when using
# MSYS.
RUN_COMMAND="python3 //build/src/build/build.py --out_folder=//build/out"

# Enable the user to override some default settings via command line parameters.
while getopts r: option
do
case "${option}"
in
# The 'r' flag lets a user specify a custom command to run the container with.
r) RUN_COMMAND=${OPTARG};;
esac
done

# Make sure the Docker container image containing the build environment is
# up to date and then run the actual build command inside the container.
${DOCKER_COMMAND} build -q -t test_build ${DOCKERFILE_DIRECTORY} && \
${DOCKER_COMMAND} run \
    --rm \
    --name test_run \
    -it \
    --mount type=bind,source=${HOST_SRC_DIR},target=/build/src,readonly \
    --mount type=bind,source=${HOST_OUT_DIR},target=/build/out \
    test_build \
    ${RUN_COMMAND}
