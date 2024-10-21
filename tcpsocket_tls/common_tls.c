#include <openssl/err.h>
#include <openssl/ssl.h>

void ssl_init() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_crypto_strings();
}
