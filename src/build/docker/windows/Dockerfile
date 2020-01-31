FROM mcr.microsoft.com/windows/servercore:1903

LABEL description="Environment for building the project in Windows."

RUN powershell -Command \
    iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1')); \
    choco feature disable --name showDownloadProgress

# This is a big one, be careful when it gets rebuilt, it can take 15 minutes.
RUN choco install -y visualstudio2019-workload-vctools

RUN choco install -y python3 winflexbison3
RUN choco install -y cmake --installargs 'ADD_CMAKE_TO_PATH=System'

# The following two commands make it so that the results of running
# vcvars64.bat are baked into the image.
COPY msvc_env_setup_scripts C:/msvc_env_setup_scripts
RUN powershell C:/msvc_env_setup_scripts/setvcbuildtoolsenv.ps1
