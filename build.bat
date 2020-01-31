@echo off

SET THIS_SCRIPT_LOCATION=%~dp0
SET HOST_SRC_DIR=%THIS_SCRIPT_LOCATION%src
SET HOST_OUT_DIR=%THIS_SCRIPT_LOCATION%out\docker\windows
SET DOCKERFILE_DIRECTORY=%THIS_SCRIPT_LOCATION%src\build\docker\windows

if not exist %HOST_OUT_DIR% md %HOST_OUT_DIR%

SET RUN_COMMAND="python C:/build/src/build/build.py --out_folder=C:/build/out"

:GETOPTS
 if /I "%1" == "-r" set RUN_COMMAND=%2 & shift
 shift
if not "%1" == "" goto GETOPTS

docker build -t test_build %DOCKERFILE_DIRECTORY% || exit /b %errorlevel%

docker run --rm --name test_run -it^
    --mount type=bind,source=%HOST_SRC_DIR%,target=C:\build\src,readonly^
    --mount type=bind,source=%HOST_OUT_DIR%,target=C:\build\out^
    test_build^
    "%RUN_COMMAND%"
