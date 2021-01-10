
#include "pbx_net.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <netdb.h> // gethostbyname

// https://stackoverflow.com/questions/41229601/openssl-in-c-socket-connection-https-client

using namespace std;

SSL *ssl;

int RecvPacket()
{
  int len = 100;
  char buf[1000000];
  do
  {
    len = SSL_read(ssl, buf, 100);
    buf[len] = 0;
    printf(buf);
  } while (len > 0);
  if (len < 0)
  {
    int err = SSL_get_error(ssl, len);
    if (err == SSL_ERROR_WANT_READ)
      return 0;
    if (err == SSL_ERROR_WANT_WRITE)
      return 0;
    if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL || err == SSL_ERROR_SSL)
      return -1;
  }
}

int SendPacket(const char *buf)
{
  int len = SSL_write(ssl, buf, strlen(buf));
  if (len < 0)
  {
    int err = SSL_get_error(ssl, len);
    switch (err)
    {
    case SSL_ERROR_WANT_WRITE:
      return 0;
    case SSL_ERROR_WANT_READ:
      return 0;
    case SSL_ERROR_ZERO_RETURN:
    case SSL_ERROR_SYSCALL:
    case SSL_ERROR_SSL:
    default:
      return -1;
    }
  }
}

void log_ssl()
{
  int err;
  while (err = ERR_get_error())
  {
    char *str = ERR_error_string(err, 0);
    if (!str)
      return;
    printf(str);
    printf("\n");
    fflush(stdout);
  }
}

//--------------------------------------------------------------------------------------------------
std::string view_gethostbyname( const char* url_ ) // www.google.com
{
  hostent* ph = gethostbyname( url_ ); // 172.217.13.99, 173.194.222.139
  if( ph == NULL )
    return "ERROR\n";

  std::stringstream oss;
  oss << "struct hostent.h_name         = " << ph->h_name << "\n";
  oss << "struct hostent.h_addrtype     = " << ph->h_addrtype<< "\n";
  oss << "struct hostent.h_length       = " << ph->h_length << "\n";
  for( int i(0); ph->h_aliases[i] != NULL; ++i)
  {
    oss << "struct hostent.h_aliases[" << i << "]  = " << ph->h_aliases[i] << "\n"; 
  }
  for( int i(0); ph->h_addr_list[i] != NULL; ++i)
  {
    struct in_addr *p = (struct in_addr *)ph->h_addr_list[i];
    char* IPbuffer = inet_ntoa(*p); 
    oss << "struct hostent.h_addr_list[" << i << "] = " << IPbuffer << "\n"; // Network byte order (big-endian)
  }
  return oss.str();
}

//--------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  
  std::cout << pbx::get_nic_ip( "enp2s0" ) << std::endl;
  std::cout << pbx::getInterfaceFromAddress( "enp2s0" ) << std::endl;
  std::cout << pbx::getInterfaceFromAddress( "eth0" ) << std::endl;
  std::cout << pbx::getInterfaceFromAddress( "www.google.ru" ) << "      (173.194.222.139)" << std::endl;
  std::cout << view_gethostbyname( "www.google.ru" );

  return 0;
  


  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    printf("Error creating socket.\n");
    return -1;
  }
  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = inet_addr("173.194.222.139"); // address of google.ru 
  sa.sin_port = htons(443);
  socklen_t socklen = sizeof(sa);
  if (connect(fd, (struct sockaddr *)&sa, socklen))
  {
    printf("Error connecting to server.\n");
    return -1;
  }

  SSL_library_init();
  SSLeay_add_ssl_algorithms();
  SSL_load_error_strings();
  
  const SSL_METHOD *meth = TLSv1_2_client_method();
  SSL_CTX *ctx = SSL_CTX_new(meth);
  ssl = SSL_new(ctx);
  if (!ssl)
  {
    printf("Error creating SSL.\n");
    log_ssl();
    return -1;
  }
  
  int sock = SSL_get_fd(ssl);
  SSL_set_fd(ssl, fd);
  int err = SSL_connect(ssl);
  if (err <= 0)
  {
    printf("Error creating SSL connection.err = % x\n ", err);
    log_ssl();
    fflush(stdout);
    return -1;
  }
  printf("SSL connection using %s\n ", SSL_get_cipher(ssl));

  char *request = "GET https://www.google.ru/intl/en/about/company/facts/HTTP/1.1\r\n\r\n ";
  SendPacket(request);
  RecvPacket();
  return 0;
}

// yum install openssl
// yum install openssl-devel
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a -I.. wget_ssl.cpp -lcrypto -lssl


