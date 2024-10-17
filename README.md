# Socket programming in linux
Echo server and client with TLS.

## Prerequisite
```
apt update && apt install libssl-dev build-essential
```

## How to run

```sh
source ./build.sh

# server
./build/server_app [PORT] # Ex> ./build/server_app 443

# client
./build/client_app [IP address] [PORT] # Ex> ./build/client_app 127.0.0.1 443
```
