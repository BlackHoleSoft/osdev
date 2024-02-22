# osdev


## Build

Build docker image:
 - `docker build buildenv -t sys-c`

Enter build environment:
 - Linux: `docker run --rm -it -v "$(pwd)":/root/env sys-c`
 - Windows: `docker run --rm -it -v "%cd%":/root/env sys-c`
 - Windows (PowerShell): `docker run --rm -it -v "${pwd}:/root/env" sys-c`

Build for x86:
 - `make build`

To leave the build environment, enter `exit`.