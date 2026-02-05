# osdev

Сначала необходимо собрать nodejs отдельным образом docker
Затем собрать iso образ системы

## Сборка nodejs

```
docker build -f ./Dockerfile-nodejs -t node-img .
docker run -it -v $pwd/node:/node node-img
```

## Сборка образа docker

```
docker build -t strelka-linux .
```

## Сборка iso

```
docker run -it -v $pwd/vol:/vol strelka-linux
```

## WAT to WASM

```
npm i -g wabt
wat2wasm <filename>
```
