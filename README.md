# Socket programming in linux
Echo server and client with TLS.

## Prerequisite
```
apt update && apt install libssl-dev build-essential cmake
```

## How to run

```sh
cmake -H. -Bbuild -G "Unix Makefiles"
cmake --build build

# server
cd build
./server_app

# client
cd build
./client_app
```
