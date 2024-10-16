void ssl_init();
SSL_CTX* create_ssl_ctx();
void ssl_ctx_config(SSL_CTX* ctx, char* ca_cert_path);
SSL* create_ssl(SSL_CTX* ctx, int sock);
int do_ssl_handshake(SSL* ssl);
void *read_sock_t(void *param);
