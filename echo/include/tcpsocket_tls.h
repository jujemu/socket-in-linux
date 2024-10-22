/* 외부에 공개할 헤더파일 */

typedef struct tls_socket {
    SSL* ssl;
    int fd;
} tls_socket;

enum thread_error_code {
    SERVER_CLOSED = -1,
    FAIL_TO_READ = -2
};

/* common ssl */
void ssl_init();

/* client socket */
void connect_with_serv(int client_sock, int port, char *serv_ip_addr);

/* client tls */
SSL_CTX* create_client_ssl_ctx(char* ca_cert_path);
SSL* create_client_ssl(SSL_CTX* ctx, int sock);
int do_ssl_handshake(SSL* ssl);
void *read_sock_t(void *param);

/* server socket */
void bind_serv_sock(int serv_sock, int port);
int accept_and_create_client_sock(int serv_sock);

/* server tls */
SSL_CTX* create_server_ssl_ctx(char* certificate_path, char* key_path);
SSL* create_server_ssl(SSL_CTX* ctx, tls_socket* tls_sockets, int client_sock);
int find_index_sock(tls_socket* tls_sockets, int sock, int top);
