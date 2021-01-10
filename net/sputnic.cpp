
#include "pbx_net.h"
#include "pbx_whois.h"
#include <pbx.h>

#include <errno.h>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>

//#include <filesystem>
//#include <unistd.h>
//#include <sys/types.h>
//#include <asm/types.h>
//#include <assert.h>

static uint32_t MYIP;
static unsigned char MYMAC[6];

#if 0
  struct sockaddr_in 
  {
    sa_family_t    sin_family; /* address family: AF_INET */
    in_port_t      sin_port;   /* port in network byte order */
    struct in_addr sin_addr;   /* internet address */
  };
  struct in_addr /* Internet address. */
  {
    uint32_t s_addr; /* address in network byte order */
  };
  
  struct hostent
  {
    char *h_name;       // official name of host
    char **h_aliases;   // alias list
    int h_addrtype;     // host address type
    int h_length;       // length of address
    char **h_addr_list; // list of addresses
    char *h_addr;       // = h_addr_list[0] in other words, it is the first host address. 
  }
#endif

//--------------------------------------------------------------------------------------------------
/*
std::string in_addr_to_string(const struct in_addr &sin_addr_)
{
  void *addr = (void*)&sin_addr_;
  char ipstr[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));
  return ipstr;
}
*/
std::string in_addr_to_string(const struct in_addr &sin_addr_)
{
  return inet_ntoa(sin_addr_);
}

std::string in_port_t_to_string(const in_port_t &in_port_t_)
{
  return std::to_string(ntohs(in_port_t_));
}

//--------------------------------------------------------------------------------------------------
int sockaddr_in_to_host_port(std::string &host_, int &port_, struct sockaddr_in *ipv4_)
{
  if (ipv4_ != (struct sockaddr_in *)(0))
  {
    if (ipv4_->sin_addr.s_addr != INADDR_ANY)
    {
      struct in_addr *paddr = &(ipv4_->sin_addr);
      struct hostent *ph = gethostbyaddr((char *)paddr, sizeof(struct sockaddr_in), AF_INET);
      if (ph != (struct hostent *)(0))
        host_ = ph->h_name;
      else
        host_ = inet_ntoa(ipv4_->sin_addr);

      port_ = ntohs(ipv4_->sin_port);

      return 0;
    }
    return -2;
  }
  return -1;
}
int host_port_to_sockaddr_in(struct sockaddr_in &ipv4_, const char *url_, int port_)
{
  ipv4_.sin_port = htons(port_);
  ipv4_.sin_family = AF_INET;
  ipv4_.sin_addr.s_addr = INADDR_ANY;

  if (url_ != nullptr)
  {
    struct hostent *ph = gethostbyname(url_); // gethostbyname vs getaddrinfo
    if (ph == (struct hostent *)(0))
    {
      std::cerr << "gethostbyname() FAILED for: [" << url_ << "]" << std::endl;
      return -1;
    }
    else if (ph->h_addrtype != AF_INET)
    {
      std::cerr << "gethostbyname() FAILED for: [" << url_ << "] h_addrtype=" << ph->h_addrtype << std::endl;
      return -2;
    }
    else if (ph->h_length != sizeof(ipv4_.sin_addr.s_addr))
    {
      std::cerr << "gethostbyname() FAILED for: [" << url_ << "] h_length=" << ph->h_length
                << "!=" << ipv4_.sin_addr.s_addr << std::endl;
      return -3;
    }
    else
    {
      ipv4_.sin_addr.s_addr = *((unsigned long *)ph->h_addr);
    }
  }
  return 0;
}

//--------------------------------------------------------------------------------------------------
int host_port_to_sockaddr_in_2(struct sockaddr_in *ipv4_, const char *url_, int port_)
{
  struct addrinfo hints;
  ::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;     // AF_INET, AF_INET6
  hints.ai_socktype = SOCK_STREAM; // SOCK_STREAM, SOCK_DGRAM
  hints.ai_protocol = 0;           // any protocol
  std::string port = std::to_string(port_);
  struct addrinfo *result = nullptr;
  int rtn = 0;
  if (strcmp(url_, "ANY") == 0 || strcmp(url_, "0.0.0.0") == 0)
  {
    hints.ai_flags |= AI_PASSIVE;
    rtn = ::getaddrinfo(nullptr, port.data(), &hints, &result);
  }
  else
  {
    rtn = ::getaddrinfo(url_, port.data(), &hints, &result);
  }
  if (rtn == 0)
  {
    for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next)
    {
      if (rp->ai_family != AF_INET || rp->ai_addrlen != sizeof(struct sockaddr_in))
      {
        *ipv4_ = *(struct sockaddr_in *)(rp->ai_addr);
        break;
      }
      else // ipv6
      {
        /*
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr;
        void *addr = &(ipv6->sin6_addr);
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(rp->ai_family, addr, ipstr, sizeof ipstr);
        std::cout << " address= " << ipstr << std::endl;
        */
      }
    }
  }
  if (result != NULL)
    ::freeaddrinfo(result);
  return rtn;
}

int host_to_sockaddr_in_2(struct sockaddr_in *ipv4_, const char *url_)
{
  struct addrinfo *result = nullptr;
  int rtn = 0;
  if (strcmp(url_, "ANY") == 0 || strcmp(url_, "0.0.0.0") == 0)
  {
    return -1;
  }
  else
  {
    rtn = ::getaddrinfo(url_, NULL, NULL, &result);
  }
  if (rtn == 0)
  {
    if (result->ai_family != PF_INET )
      return -2;
    *ipv4_ = *(struct sockaddr_in *)(result->ai_addr);
  }
  if (result != NULL)
    ::freeaddrinfo(result);
  return rtn;
}

//--------------------------------------------------------------------------------------------------
int set_non_block( int fd_ )
{
  int rc = 0;
  int flag = fcntl( fd_, F_GETFL);
  if( flag < 0)
    rc = -errno;
  flag |= O_NONBLOCK;
  rc = fcntl( fd_, F_SETFL, flag);
  if( rc < 0)
    rc = -errno;
  return rc;
}

//--------------------------------------------------------------------------------------------------
int tcp_client_init( struct sockaddr_in *ipv4_, int port_ = -1 )
{
  int rc = 0;
  int fd = socket( PF_INET, SOCK_STREAM, IPPROTO_IP);
  //int flag = 1;
  if( !fd)
  {
    rc = -1;
    goto err;
  }
  
  if( port_ != -1 )
    ipv4_->sin_port = htons( port_ );

  // (1) TCP_NODELAY is used for disabling Nagle's algorithm.
  // Nagle's algorithm is for reducing more number of small network packets in wire.
  // The algorithm is: if data is smaller than a limit (usually MSS), 
  // wait until receiving ACK for previously sent packets and in the mean time accumulate data from user. 
  // Then send the accumulated data.
  // This will help in applications like telnet.
  // (2) TCP_CORK aggressively accumulates data.
  // http://baus.net/on-tcp_cork/
  // http://ccr.sigcomm.org/archive/2001/jan01/ccr-200101-mogul.pdf
/*
  rc = setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
  if( rc < 0)
  {
    rc = -2;
    goto err;
  }
*/

  rc = connect( fd, (const sockaddr *)ipv4_, sizeof(*ipv4_));
  if( rc < 0)
  {
    rc = -3;
    goto err;
  }

  rc = set_non_block( fd);

err:
  return rc == 0 ? fd : rc;
}

//--------------------------------------------------------------------------------------------------
int tcp_server_init( int fd_ )
{
  int rc = 0;
  struct sockaddr in_addr;
  socklen_t in_len = sizeof(in_addr);
  //int flag = 1;

  int fd = accept( fd_, &in_addr, &in_len);
  if( fd < 0)
  {
    rc = -1;
    goto err;
  }
/*
  rc = setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
  if( rc < 0)
  {
    rc = -2;
    goto err;
  }
*/  
  rc = set_non_block( fd);

err:
  return rc;
}

//--------------------------------------------------------------------------------------------------
int tcp_create_and_bind( const char* ip_, uint16_t port_ )
{
  int rc = 0;
  struct sockaddr_in addr;
  int flag;
  int fd = socket( PF_INET, SOCK_STREAM, IPPROTO_IP);
  if( !fd )
  {
    rc = -1;
    goto err;
  }
  memset( &addr, 0, sizeof(addr));
  ::inet_pton( AF_INET, ip_, (void*)&addr.sin_addr);
  addr.sin_family = PF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port_);

  flag = 1;
  rc = setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int));
  if( rc < 0)
  {
    rc = -2;
    goto err;
  }

  rc = bind( fd, (struct sockaddr*)&addr, sizeof(addr));
  if( rc < 0)
  {
    rc = -3;
    goto err;
  }
err:
  return (rc==0 ? fd : rc);
}

//--------------------------------------------------------------------------------------------------
int tcp_read( int fd_, uint8_t* buf_, int count_)
{
  int nb = 0;
  do
  {
    int n = read( fd_, buf_, count_);
    if( n == 0)
      return nb;
    else if( n < 0)
      return nb;
    count_ -= n;
    buf_ += n;
    nb += n;
  }while( count_ > 0);
  return nb;
}


//--------------------------------------------------------------------------------------------------
// first way to retrieve the IP (using gethostbyname)
//--------------------------------------------------------------------------------------------------
// stackoverflow.com/questions/39566240/how-to-get-the-external-ip-address-in-c
std::string get_Website(const char *url_, int port_ = 80)
{
  std::string website_HTML;

  struct sockaddr_in ipv4; // created from url_/port_
  if (host_port_to_sockaddr_in(ipv4, url_, port_) < 0)
    return "";

  int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // IPPROTO_IP
  if (connect(socket_fd, (sockaddr *)(&ipv4), sizeof(ipv4)) != 0)
  {
    std::cerr << "connect() FAILED" << std::endl;
    return website_HTML;
  }

  std::string get_http = "GET / HTTP/1.1\r\nHost: " + std::string(url_) + "\r\nConnection: close\r\n\r\n";
  send(socket_fd, get_http.c_str(), strlen(get_http.c_str()), 0);

  int nDataLength = {};
  char buffer[10000];
  while ((nDataLength = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0)
  {
    int i = 0;
    while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r')
    {
      website_HTML += buffer[i];
      i += 1;
    }
  }
  // std::cout << "\n-----------------------------------------------" << std::endl;
  // std::cout << website_HTML << std::endl;
  // std::cout << "-----------------------------------------------" << std::endl;

  close(socket_fd);
  return website_HTML;
}

std::string get_public_IP()
{
  std::string rtn;
  std::string webip = get_Website("api.ipify.org", 80); // https://www.ipify.org/
  std::istringstream ss(webip);
  std::string line;
  while (getline(ss, line, '\n'))
  {
    std::string regstr2("(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    auto rtn1 = pbx::regex_search(line, regstr2);
    if (rtn1.size() == 1)
    {
      rtn = line;
    }
  }
  return rtn;
}

//--------------------------------------------------------------------------------------------------
// second way to retrieve the IP (using getaddrinfo)
//--------------------------------------------------------------------------------------------------
// https://github.com/troglobit/lipify
#define IPIFY_HOST "api.ipify.org"
#define PACKAGE_NAME "toot"
#define PACKAGE_VERSION "1.0"
#define AGENT_NAME PACKAGE_NAME "/" PACKAGE_VERSION
#define HTTP_REQUEST                                                                                                   \
  "GET / HTTP/1.0\r\n"                                                                                                 \
  "Host: " IPIFY_HOST "\r\n"                                                                                           \
  "User-Agent: " AGENT_NAME "\r\n\r\n";

/*
 * Connect to api.ipify.org using either address protocol supported.  We
 * want to connect using TCP, so ask getaddrinfo() for a TCP connection
 * over either IPv4 or IPv6, then use the first successful connection.
 */
int ipify_connect(const char *host_, const char *port_)
{
  int sd;

#if 0
  struct addrinfo
  {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    socklen_t ai_addrlen;
    struct sockaddr *ai_addr; // used to connect
    char *ai_canonname;
    struct addrinfo *ai_next;
  };
#endif

  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;     // AF_INET, AF_INET6
  hints.ai_socktype = SOCK_STREAM; // SOCK_STREAM, SOCK_DGRAM
  hints.ai_protocol = 0;

#if 0
  The getaddrinfo() function allocates and initializes a linked list of
  addrinfo structures, one for each network address that matches node
  and service, subject to any restrictions imposed by hints, and
  returns a pointer to the start of the list in res.  The items in the
  linked list are linked by the ai_next field.
#endif
  struct addrinfo *result;
  // int rc = getaddrinfo( IPIFY_HOST, "80", &hints, &result );
  int rc = getaddrinfo(host_, port_, &hints, &result);
  if (rc || !result)
    return -1;

  for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next)
  {
    if (rp->ai_family == AF_INET) // IPv4
    {
      {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
        void *addr = &(ipv4->sin_addr);
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(rp->ai_family, addr, ipstr, sizeof(ipstr));
        std::cout << " IP address of host(" << host_ << ")= " << ipstr << std::endl;
      }

      sd = socket(rp->ai_family, rp->ai_socktype, 0);
      if (sd < 0)
        continue;
      rc = connect(sd, result->ai_addr, result->ai_addrlen);
      if (rc)
      {
        close(sd);
        sd = -1;
        continue;
      }
      break;
    }
    else // IPv6
    {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr;
      void *addr = &(ipv6->sin6_addr);
      char ipstr[INET6_ADDRSTRLEN];
      inet_ntop(rp->ai_family, addr, ipstr, sizeof ipstr);
      std::cout << " address= " << ipstr << std::endl;
    }
  }
  freeaddrinfo(result);
  return sd;
}

int ipify_query(int sd, char *addr, size_t len)
{
  const char *req = HTTP_REQUEST;
  ssize_t rc;
  char tmp[sizeof(struct in6_addr)];
  char buf[512], *ptr;
  int domain;

  rc = send(sd, req, strlen(req), 0);
  if (rc < 0)
    return -1;

  rc = recv(sd, buf, sizeof(buf), 0);
  if (rc < 0)
    return -1;
  buf[rc] = 0;

  ptr = strstr(buf, "200 OK");
  if (!ptr)
    return 1;

  ptr = strstr(ptr, "\r\n\r\n");
  if (!ptr)
    return 1;
  ptr += 4;

  domain = AF_INET;
  if (!inet_pton(domain, ptr, tmp))
  {
    domain = AF_INET6;
    if (!inet_pton(domain, ptr, tmp))
      return 1;
  }

  if (!inet_ntop(domain, tmp, addr, len))
    return 1;

  return 0;
}

int ipify_disconnect(int sd)
{
  shutdown(sd, SHUT_RDWR);
  return close(sd);
}

int ipify(char *addr, size_t len)
{
  int sd = ipify_connect(IPIFY_HOST, "80");
  if (sd < 0)
    return 1;

  int ret = ipify_query(sd, addr, len);
  ret |= ipify_disconnect(sd);

  return ret;
}

void test_ipify()
{
  char addr[256];
  if (ipify(addr, sizeof(addr)) <= 0)
  {
    std::cout << "test_ipify " << addr << std::endl;
  }
}

//--------------------------------------------------------------------------------------------------
int main()
{
  /*
  int uid = geteuid();
  if (uid != 0)
  {
    std::cerr << "You're not root ! " << uid << std::endl;
    exit(1);
  }
  */

  std::cout << "-----------------------------------------------" << std::endl;
  auto interfaces = pbx::get_all_nic();
  for (auto &it : interfaces)
    std::cout << "interface[" << it.first << "]=" << it.second
              << " IP=" << pbx::get_nic_ip(MYIP, (const char *)it.second.data()) << " / " << pbx::get_nic_ip(it.second)
              << " MAC=" << pbx::get_nic_mac((unsigned char *)MYMAC, (const unsigned char *)it.second.data())
              << std::endl;

  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << "IP: " << pbx::get_nic_ip(MYIP, (const char *)"enp2s0") << std::endl;
  std::cout << "MAC: " << pbx::get_nic_mac((unsigned char *)MYMAC, (const unsigned char *)"enp2s0") << std::endl;
  std::cout << "if_nametoindex = " << if_nametoindex("enp2s0") << std::endl;

  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << "public IP: " << get_public_IP() << std::endl;
  test_ipify();

  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << get_Website("www.zerohedge.com", 80) << std::endl;

  std::cout << "-----------------------------------------------" << std::endl;
  {
    struct sockaddr_in ipv4;
    const char *host = "api.ipify.org";

    if (host_port_to_sockaddr_in(ipv4, host, 80) == 0)
    {
      std::cout << "IP address of host(" << host << ")= " << in_addr_to_string(ipv4.sin_addr) << std::endl;
    }

    if (host_port_to_sockaddr_in_2(&ipv4, host, 80) == 0)
    {
      std::cout << "IP address of host 2(" << host << ")= " << in_addr_to_string(ipv4.sin_addr) << std::endl;
    }

    std::string host2;
    int port;
    if (sockaddr_in_to_host_port(host2, port, &ipv4) >= 0)
      std::cout << host2 << " " << port << std::endl;

    if( host_to_sockaddr_in_2(&ipv4, host) == 0)
      std::cout << "IP address of host 2(" << host << ")= " << in_addr_to_string(ipv4.sin_addr) << std::endl;
  }
  std::cout << "-----------------------------------------------" << std::endl;
}
// clang-format off
// clang-format-9 -i sputnic.cpp
// find . -regex '.*\.\(cpp\|hpp\|cu\|c\|h\)' -exec clang-format -style=file -i {} \;
// git clone https://github.com/poudlar/sniffer
// git commit -m "first commit"
// git remote add origin https://github.com/poudlar/sniffer.git
// git push -u origin master
//
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++17 -I.. sputnic.cpp 
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a -I.. sputnic.cpp
// sudo ./a.out

