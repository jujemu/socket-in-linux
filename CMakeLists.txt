cmake_minimum_required(VERSION 3.10)
project(echo)

include_directories(echo/include tcpsocket_tls)
add_compile_options(-Wall -Wextra -Wunused-variable -Wshadow -Wuninitialized)

# Create the shared library for tcpsocket_tls
add_library(tcp_socket_tls SHARED
    tcpsocket_tls/client_socket.c
    tcpsocket_tls/client_tls.c
    tcpsocket_tls/common_tls.c
    tcpsocket_tls/error_handler.c
    tcpsocket_tls/server_socket.c
    tcpsocket_tls/server_tls.c
)

install(TARGETS tcp_socket_tls LIBRARY DESTINATION ${CMAKE_BINARY_DIR})

# Client executable
add_executable(client_app echo/client/client.c)
target_link_libraries(client_app tcp_socket_tls ssl crypto)

# Server executable
add_executable(server_app echo/server/server.c)
target_link_libraries(server_app tcp_socket_tls ssl crypto)

install(TARGETS client_app server_app RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
