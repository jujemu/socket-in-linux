# Socket programming in linux
Echo server and client with TLS.

## Prerequisite
```
apt update && apt install libssl-dev build-essential cmake
```

## How to run

```sh
cmake -H. -Becho/build -G "Unix Makefiles"
cmake --build echo/build

# server
cd echo/build
./server_app

# client
cd echo/build
./client_app
```
