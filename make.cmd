cd ./strelka
call npx webpack
cd ../
call docker build -t strelka-linux .
docker run -it -v %cd%/vol:/vol strelka-linux