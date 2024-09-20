:: build electron app
call docker build -t strelka-build -f ./Dockerfile-electron .
call docker run -it -v %cd%/strelka-build:/strelka-build strelka-build

:: make xorg
call docker build -t strelka-xorg -f ./Dockerfile/xorg .
docker run -it -v %cd%/xorg:/xorg strelka-xorg

:: make iso
call docker build -t strelka-linux .
docker run -it -v %cd%/vol:/vol strelka-linux