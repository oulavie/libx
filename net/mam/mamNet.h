
#pragma once
#ifndef mam_net_h
#define mam_net_h

#include <sys/inotify.h>
#include <sys/signal.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <errno.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace mam
{

//--------------------------------------------------------------------------------------------------
int create_timer_fd(int millisec_interval_)
{
  int fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
  struct itimerspec timespec;
  bzero(&timespec, sizeof(timespec));
  timespec.it_interval.tv_sec = millisec_interval_ / 1000;
  timespec.it_interval.tv_nsec = (millisec_interval_ % 1000) * 1000000;
  timespec.it_value.tv_sec = 0;
  timespec.it_value.tv_nsec = 1;
  timerfd_settime(fd, 0, &timespec, 0);
  return fd;
}

template <typename T> void read_timer_fd(int fd_, T &&t_)
{
  int timerElapsed{0};
  ssize_t bytes = read(fd_, (void*)&timerElapsed, 8);
  if (bytes == 8)
    t_(timerElapsed);
}

//--------------------------------------------------------------------------------------------------
int create_stdin_fd()
{
  return 0;
}

template <typename T> void read_stdin_fd(int fd_, T &&t_)
{
  std::vector<char> v;
  const size_t STDIN_MAX_READ_ALLOWED = 5;
  static char buffer[STDIN_MAX_READ_ALLOWED];
  ssize_t bytes;
  do
  {
    bytes = read(fd_, buffer, STDIN_MAX_READ_ALLOWED);
    mam::append(v, buffer, buffer + bytes);
  } while (bytes == STDIN_MAX_READ_ALLOWED);
  t_(v);
}

//--------------------------------------------------------------------------------------------------
int create_signal_fd(int signum_)
{
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, signum_);
  sigprocmask(SIG_BLOCK, &mask, NULL);
  return signalfd(-1, &mask, 0);
}
template <typename... args> int create_signal_fd(int signum_, args... args_)
{
  create_signal_fd(signum_);
  return create_signal_fd(args_...);
}
int create_signal_fd(const std::initializer_list<int> &signums_)
{
  sigset_t mask;
  sigemptyset(&mask);
  for (auto sig : signums_)
    sigaddset(&mask, sig);
  sigprocmask(SIG_BLOCK, &mask, NULL);
  return signalfd(-1, &mask, 0);
}

template <typename T> void read_signal_fd(int fd_, T &&t_)
{
  struct signalfd_siginfo fdsi;
  ssize_t s = read(fd_, (void *)&fdsi, sizeof(struct signalfd_siginfo));
  if (s == sizeof(struct signalfd_siginfo))
    t_(fdsi.ssi_signo); // uint32_t ssi_signo; Signal number
}

//--------------------------------------------------------------------------------------------------
int create_inotify_fd()
{
  int fd = inotify_init();
  return fd;
}
int add_inotify_fd(int fd_, const char *pathname_, uint32_t mask_ = IN_MODIFY)
{
  int wd = inotify_add_watch(fd_, pathname_, mask_);
  return wd;
}

struct inotify_event_header
{
  int wd;          // Watch descriptor
  uint32_t mask;   // Mask of events
  uint32_t cookie; // Unique cookie associating related events (for rename)
  uint32_t len;    // Size of name field
  // char     name[];   // Optional null-terminated name
};

template <typename T> void read_inotify_fd(int fd_, T &&t_)
{
  const size_t INOTIFY_EVENT_HEADER_SIZE{sizeof(struct inotify_event_header)};
  const size_t READ_BUFFER_SIZE{1024 * 1024};
  static char buffer[READ_BUFFER_SIZE];

  ssize_t bytes = ::read(fd_, buffer, INOTIFY_EVENT_HEADER_SIZE);

  if (unlikely(bytes == 0))
  {
    return;
  }
  if (unlikely(bytes < 0))
  {
    return;
  }
  std::vector<char> v;
  struct inotify_event_header *pieh = (struct inotify_event_header *)(&buffer);
  if (pieh->len > 0)
  {
    do
    {
      bytes = ::read(fd_, buffer, READ_BUFFER_SIZE);
      mam::append(v, buffer, buffer + bytes);
    } while (bytes == READ_BUFFER_SIZE);
    t_(pieh->wd, v);
  }
}

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
      std::cerr << "gethostbyname() FAILED for: [" << url_ << "]" << AT << std::endl;
      return -1;
    }
    else if (ph->h_addrtype != AF_INET)
    {
      std::cerr << "gethostbyname() FAILED for: [" << url_ << "] h_addrtype=" << ph->h_addrtype << AT << std::endl;
      return -2;
    }
    else if (ph->h_length != sizeof(ipv4_.sin_addr.s_addr))
    {
      std::cerr << "gethostbyname() FAILED for: [" << url_ << "] h_length=" << ph->h_length
                << "!=" << ipv4_.sin_addr.s_addr << AT << std::endl;
      return -3;
    }
    else
    {
      ipv4_.sin_addr.s_addr = *((unsigned long *)ph->h_addr);
    }
  }
  return 0;
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
    if (result->ai_family != PF_INET)
      return -2;
    *ipv4_ = *(struct sockaddr_in *)(result->ai_addr);
  }
  if (result != NULL)
    ::freeaddrinfo(result);
  return rtn;
}

//--------------------------------------------------------------------------------------------------
int set_non_block(int fd_)
{
  int rc = 0;
  int flag = fcntl(fd_, F_GETFL);
  if (flag < 0)
    rc = -errno;
  flag |= O_NONBLOCK;
  rc = fcntl(fd_, F_SETFL, flag);
  if (rc < 0)
    rc = -errno;
  return rc;
}

//--------------------------------------------------------------------------------------------------
int tcp_client_init(struct sockaddr_in *ipv4_, int protocol_ = IPPROTO_IP, int port_ = -1)
{
  int rc = 0;
  int fd = socket(PF_INET, SOCK_STREAM, protocol_);
  // int flag = 1;
  if (!fd)
  {
    rc = -1;
    goto err;
  }

  if (port_ != -1)
    ipv4_->sin_port = htons(port_);

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

  rc = connect(fd, (const sockaddr *)ipv4_, sizeof(*ipv4_));
  if (rc < 0)
  {
    rc = -3;
    goto err;
  }

  rc = set_non_block(fd);

err:
  return rc == 0 ? fd : rc;
}

//--------------------------------------------------------------------------------------------------
int host_port_to_sockaddr_in_2(struct sockaddr_in *ipv4_, const char *url_, const char *port_)
{
  std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
  struct addrinfo hints;
  ::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // AF_INET, AF_INET6, AF_UNSPEC
  hints.ai_socktype = SOCK_STREAM; // SOCK_STREAM, SOCK_DGRAM
  hints.ai_protocol = 0;           // any protocol
  struct addrinfo *result = nullptr;
  int rtn = 0;
  if (url_ == NULL || strcmp(url_, "ANY") == 0 || strcmp(url_, "0.0.0.0") == 0)
  {
    std::cout << __PRETTY_FUNCTION__ << " ai_family=" << hints.ai_family << " ai_socktype=" << hints.ai_socktype
              << " ai_protocol=" << hints.ai_protocol << " port=" << port_ << AT << std::endl;
    hints.ai_flags |= AI_PASSIVE;
    rtn = ::getaddrinfo(NULL, port_, &hints, &result);
  }
  else
  {
    std::cout << __PRETTY_FUNCTION__ << " ai_family=" << hints.ai_family << " ai_socktype=" << hints.ai_socktype
              << " ai_protocol=" << hints.ai_protocol << " url=" << url_ << " port=" << port_ << AT << std::endl;
    rtn = ::getaddrinfo(url_, port_, &hints, &result);
  }
  // getaddrinfo() returns 0 if it succeeds, or one of the following nonzero error codes
  if (rtn == 0)
  {
    for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next)
    {
      if (rp->ai_family != AF_INET || rp->ai_addrlen != sizeof(struct sockaddr_in))
        continue;

      {
        *ipv4_ = *(struct sockaddr_in *)(rp->ai_addr);
        break;
      }
      // else // ipv6
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
  else
  {
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, errno=" << ::strerror(errno) << AT << std::endl;
    switch (rtn)
    {
    case EAI_ADDRFAMILY:
    case EAI_AGAIN:
    case EAI_BADFLAGS:
    case EAI_FAIL:
    case EAI_FAMILY:
    case EAI_MEMORY:
    case EAI_NODATA:
    case EAI_NONAME:
    case EAI_SERVICE:
    case EAI_SOCKTYPE:
    case EAI_SYSTEM:
      break;
    default:
      break;
    }
  }
  if (result != NULL)
    ::freeaddrinfo(result);
  return rtn;
}

//--------------------------------------------------------------------------------------------------
bool set_nonblocking_fd(int fd_)
{
  std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
  int flag{1};
  if (::ioctl(fd_, FIONBIO, &flag) < 0)
  {
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, errno=" << ::strerror(errno) << AT << std::endl;
    ::close(fd_);
    return false;
  }
  return true;
}
bool set_reuseaddr_fd(int fd_)
{
  std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
  int flag{1};
  if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0)
  {
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, errno=" << ::strerror(errno) << AT << std::endl;
    ::close(fd_);
    return false;
  }
  return true;
}
bool set_tcpnodelay_fd(int fd_)
{
  std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
  // TCP_NODELAY is used for disabling Nagle's algorithm.
  int flag{1};
  if (::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int)) < 0)
  {
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, errno=" << ::strerror(errno) << AT << std::endl;
    return false;
  }
  return true;
}
int server_tcp_fd(const char *port_)
{
  std::cout << __PRETTY_FUNCTION__ << " port=" << port_ << AT << std::endl;
  int fd{::socket(PF_INET, SOCK_STREAM, AF_UNSPEC)};
  if (fd < 0)
  {
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, errno=" << ::strerror(errno) << AT << std::endl;
    ::close(fd);
    return -1;
  }
  if (!set_nonblocking_fd(fd))
  {
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, errno=" << ::strerror(errno) << AT << std::endl;
    ::close(fd);
    return -1;
  }
  if (!set_reuseaddr_fd(fd))
  {
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, errno=" << ::strerror(errno) << AT << std::endl;
    ::close(fd);
    return -1;
  }
  struct sockaddr_in ipv4;
  if (host_port_to_sockaddr_in_2(&ipv4, NULL, port_) != 0)
  {
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, errno=" << ::strerror(errno) << AT << std::endl;
    ::close(fd);
    return -1;
  }
  struct sockaddr *p = reinterpret_cast<struct sockaddr *>(&ipv4);
  if (::bind(fd, p, sizeof(*p)) != 0)
  {
    // On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, bind(), errno=" << ::strerror(errno) << AT << std::endl;
    ::close(fd);
    return -1;
  }
  std::cout << __PRETTY_FUNCTION__ << " fd=" << fd << AT << std::endl;
  return fd;
}

bool server_listen(int fd_)
{
  std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << AT << std::endl;
  if (::listen(fd_, SOMAXCONN) != 0)
  // On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
  {
    std::cerr << __PRETTY_FUNCTION__ << "FAILED, listen(), errno=" << ::strerror(errno) << AT << std::endl;
    ::close(fd_);
    return false;
  }
  std::cout << __PRETTY_FUNCTION__ << " ok!" << AT << std::endl;
  return true;
}

int server_accept_fd(int fd_)
{
  std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
  int fd = ::accept(fd_, 0, 0);
  if (fd < 0)
  {
    if (errno == EMFILE) // running out of file descriptors
    {
    }
    std::cerr << "FAILED, errno=" << ::strerror(errno) << AT << std::endl;
    return -1;
  }
  if (!set_nonblocking_fd(fd))
  {
    return -1;
  }
  if (!set_tcpnodelay_fd(fd))
  {
    return -1;
  }
  int size{0};
  if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(int)))
  {
  }
  if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int)))
  {
  }
  // SO_RCVTIMEO  SO_SNDTIMEO
  // SO_RCVLOWAT  SO_SNDLOWAT
  // SO_DONTROUTE
  // SO_OOBINLINE
  // SO_LINGER
  std::cout << __PRETTY_FUNCTION__ << " fd=" << fd << AT << std::endl;
  return fd;
}

//--------------------------------------------------------------------------------------------------
int tcp_server_init(int fd_)
{
  std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << AT << std::endl;
  int rc = 0;
  struct sockaddr in_addr;
  socklen_t in_len = sizeof(in_addr);
  // int flag = 1;

  int fd = accept(fd_, &in_addr, &in_len);
  if (fd < 0)
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
  rc = set_non_block(fd);

err:
  return rc;
}

//--------------------------------------------------------------------------------------------------
int tcp_create_and_bind(const char *ip_, uint16_t port_)
{
  int flag = 1;
  int rc = 0;
  struct sockaddr_in addr;
  int fd = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
  if (!fd)
  {
    rc = -1;
    goto err;
  }
  memset(&addr, 0, sizeof(addr));
  ::inet_pton(AF_INET, ip_, (void *)&addr.sin_addr);
  addr.sin_family = PF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port_);

  rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(int));
  if (rc < 0)
  {
    rc = -2;
    goto err;
  }

  rc = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (rc < 0)
  {
    rc = -3;
    goto err;
  }
err:
  return (rc == 0 ? fd : rc);
}

//--------------------------------------------------------------------------------------------------
size_t tcp_read(int fd_, uint8_t *buf_, size_t count_, ssize_t& error_)
{
  size_t nb = 0;
  do
  {
    ssize_t n = read(fd_, buf_, count_);
    if (n == 0)
    {
      return nb;
    }
    else if (n < 0)
    {
      error_ = n;
      return nb;
    }
    count_ -= n;
    buf_ += n;
    nb += n;
  } while (count_ > 0);
  return nb;
}

//--------------------------------------------------------------------------------------------------
int tcp_write(int fd_, uint8_t *buf_, int count_)
{
  int nb = 0;
  do
  {
    int n = write(fd_, buf_, count_);
    if (n <= 0)
    {
      if (errno == EAGAIN)
        continue;
      else
        return nb;
    }
    count_ -= n;
    buf_ += n;
    nb += n;
  } while (count_ > 0);
  return nb;
}

} // namespace mam

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
