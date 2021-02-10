
// sudo apt-get update
// sudo apt-get install libssl1.0.0 libssl-dev

#include <openssl/conf.h>
#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <thread>
#include <type_traits>
#include <vector>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>


//------------------------------------------------------------------------------
#if 0
namespace {

int BIO_net_should_retry( int e)
{
  std::cout << __func__ << " e=" << e << " errno=" << strerror(errno) << " (" << errno << ")";
  if( e == 0 && errno == 0)
  {
    std::cout << "BIO will retry\n";
    return 1;
  }
  if( e == 0 || e==-1)
  {
    switch( errno)
    {
    case ENOTCONN:
    case EINTR:
    case EAGAIN:
    case EPROTO:
    case EINPROGRESS:
    case EALREADY:
      std::cout << "BIO will retry\n";
      return 1;
    default:
      break;
    }
  }
  std::cout << "BIO wont retry\n";
  return 0;
}

extern "C" int net_write( BIO* self, const char* buf, int sz)
{
  std::cout << "BIO net_write " << sz << "\n";
  assert( self);
  if( !buf)
    return 0;
  if( !self->init)
    return 0;
  assert( self->num);
  errno = 0;
  int w = write( self->num, buf, sz);
  BIO_clear_retry_flags( self);
  if( w < 0)
  {
    if( BIO_net_should_retry(w))
    {
      BIO_set_retry_write( self);
    }
  }
  return w;
}

extern "C" int net_read( BIO* self, char* buf, int sz)
{
  std::cout << "BIO net_read " << sz << "\n";
  assert( self);
  if( !buf)
    return 0;
  if( !self->init)
    return 0;
  assert( self->num);
  errno = 0;
  int r = read( self->num, buf, sz);
  BIO_clear_retry_flags(self);
  if( r <= 0)
  {
    if( BIO_net_should_retry(r))
    {
      BIO_set_retry_read( self);
    }
  }
  return r;
}

extern "C" int net_puts( BIO* self, const char* buf)
{
  std::cout << "BIO net_puts " << "\n";
  return net_write( self, buf, strlen(buf));
}

extern "C" int net_gets( BIO* self, char* buf, int sz)
{
  assert( self);
  if( sz == 0)
    return 0;
  assert( buf);
  auto s = buf;
  auto const e = buf + sz - 1;
  while( e!=s && net_read( self, s, 1)==1 && *s++!='\n')
  {}
  *s = '\0';
  return int(s-buf);
}

extern "C" int net_create( BIO* self)
{
  std::cout << "BIO net_create\n" << std::flush;
  assert( self);
  self->init = 0;
  self->shutdown = 0;
  self->flags = 0;
  self->num = 0;
  self->ptr = nullptr;
  return 1;
}

extern "C" int net_destroy( BIO* self)
{
  std::cout << "BIO net_destroy\n" << std::flush;
  if( !self)
    return 0;
  if( self->shutdown)
  {
    if( self->init)
    {
      assert( self->num);
      close( self->num);
      self->num = 0;
    }
    self->init = 0;
    self->flags = 0;
  }
  return 1;
}

extern "C" long net_ctrl( BIO* self, int cmd, int long num, void* ptr)
{
  assert( self);
  switch( cmd)
  {
  case BIO_C_SET_FD:
    std::cout << "BIO net_ctrl set fd=" << num << "\n";
    net_destroy( self);
    assert( ptr);
    self->num = *(int*)ptr;
    self->shutdown = int(num);
    self->init = 1;
    return 1;
  case BIO_C_GET_FD:
    std::cout << "BIO net_ctrl get fd=" << num << "\n";
    if( self->init)
    {
      if( ptr)
      {
        return *(int*)ptr = self->num;
      }
    }
    return -1;
  case BIO_CTRL_FLUSH:
  case BIO_CTRL_DUP:
    std::cout << "BIO net_ctrl flush/dup";
    return 1;
  case BIO_CTRL_SET_CLOSE:
    std::cout << "BIO net_ctrl set close=" << num << "\n";
    self->shutdown = int(num);
    return 1;
  case BIO_CTRL_GET_CLOSE:
    std::cout << "BIO net_ctrl get close=" << num << "\n";
    return self->shutdown;
  default: 
    std::cout << "BIO net_ctrl default:" << cmd << "\n";
    return 0;
  }
  return 0;
}

BIO_METHOD net_bio_vtable = {
  BIO_TYPE_SOCKET,
  "netBIO",
  net_write,
  net_read,
  net_puts,
  net_gets,
  net_ctrl,
  net_create,
  net_destroy,
  nullptr,
};

BIO_METHOD* BIO_s_net()
{
  return &net_bio_vtable;
}

BIO* BIO_new_msnet( int net, int close_flag)
{
  BIO* r = BIO_new( BIO_s_net());
  if( r)
    BIO_set_fd( r, net, close_flag);
  return r;
}

}
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class E>
constexpr typename std::underlying_type<E>::type to_int(E e) noexcept
{
  return static_cast< typename std::underlying_type<E>::type>(e);
}

//------------------------------------------------------------------------------
template< class E>
constexpr E to_enum( typename std::underlying_type<E>::type e) noexcept
{
  return static_cast<E>(e);
}

//------------------------------------------------------------------------------
std::string dirname( const std::string& path)
{
  size_t const dirpos = path.find_last_of("/");
  if( dirpos == std::string::npos)
  {
    return ".";
  }
  else
  {
    return path.substr( 0, dirpos);
  }
}

//------------------------------------------------------------------------------
class OpenSSLCategoryImpl : public std::error_category
{
public:
  const char* name() const noexcept override { return "OpenSSL"; }
  std::string message( int ev) const override
  {
    char buf[256];
    ::ERR_error_string_n( ev, buf, sizeof(buf));
    return buf;
  }
};

std::error_category const& openssl_category()
{
  static OpenSSLCategoryImpl const openssl_category_instance;
  return openssl_category_instance;
}

enum OpenSSLErrorValue : unsigned long {};

std::error_code make_error_code( OpenSSLErrorValue ev)
{
  return std::error_code( to_int(ev), openssl_category());
}
std::error_code make_openssl_error_code()
{
  char const* file;
  int line;
  char const* data;
  int flags;
  auto const ev = ::ERR_get_error_line_data( &file, &line, &data, &flags);
  auto const ec = make_error_code( to_enum<OpenSSLErrorValue>(ev));
  assert( uintmax_t(ec.value()) == uintmax_t(ev));
  if( ec)
  {
    char const *ds = "";
    if( data && (ERR_TXT_STRING & flags))
      ds = data;
    printf("%s:%d %s(%d) %s(%p/%X)\n", file, line, ec.message().c_str(), ec.value(), ds, data, flags);
  }
  return ec;
}

//------------------------------------------------------------------------------
int verify_certificate(int preverified, X509_STORE_CTX* ctx)
{
  X509* cert = X509_STORE_CTX_get_current_cert( ctx);
  char subject_name[256];
  X509_NAME_oneline( X509_get_subject_name(cert), subject_name, 256);
  char issuer_name[256];
  X509_NAME_oneline( X509_get_issuer_name(cert), issuer_name, 256);
  int const depth = X509_STORE_CTX_get_error_depth( ctx);
  int const err = X509_STORE_CTX_get_error( ctx);
  std::cout << "err " << err << ": " << X509_verify_cert_error_string(err) << std::endl;
  if( depth > 3)
  {
    X509_STORE_CTX_set_error(ctx, X509_V_ERR_CERT_CHAIN_TOO_LONG);
  }
  auto const verified = preverified; // = rfc2818_verification( preverified, ctx, server_host); todo at depth = 0 
  return verified;
}

//------------------------------------------------------------------------------
int tcp_connect( char const* host, int port, char const* source)
{
  struct hostent *h = gethostbyname(host);
  if( !h)
    throw std::system_error( h_errno, std::system_category());
  struct in_addr iaddr;
  iaddr = *(struct in_addr*)(h->h_addr_list[0]);
  int sock = socket( AF_INET, SOCK_STREAM, 0);
  if( source)
  {
    struct hostent* b = gethostbyname( source);
    if( !b)
      throw std::system_error( h_errno, std::system_category());
    struct in_addr iaddr; // ??????????
    iaddr = *(struct in_addr*)(b->h_addr_list[0]);
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = 0;
    saddr.sin_addr = iaddr;
    if( bind( sock, (struct sockaddr*)(&saddr), sizeof(saddr)) == -1)
      throw std::system_error( h_errno, std::system_category());
  }
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr = iaddr;
  if( connect( sock, (struct sockaddr*)(&saddr), sizeof(saddr)) == -1)
    throw std::system_error( h_errno, std::system_category());

  long arg = fcntl( sock, F_GETFL, nullptr);
  if( arg == -1)
    throw std::system_error( h_errno, std::system_category());

  arg &= ~O_NONBLOCK;

  if( fcntl( sock, F_SETFL, arg) == -1)
    throw std::system_error( h_errno, std::system_category());

  return sock;
}

//------------------------------------------------------------------------------
void info_callback( const SSL* ssl, int where, int ret)
{
  std::cout << "info_callback";
  if( ret == 0)
  {
    std::cout << ": eroor \n" << std::flush;
    return;
  }

  char const* ws = "unknown";
  if( where & SSL_CB_LOOP)
    ws = "LOOP";
  else if( where & SSL_CB_HANDSHAKE_START)
    ws = "HANDSHAKE_START";
  else if( where & SSL_CB_HANDSHAKE_DONE)
    ws = "HANDSHAKE_SONE";

  std::cout << "" << ws << ": " << SSL_state_string_long(ssl)
                        << " [" << SSL_state_string(ssl)
                        << "]\n" << std::flush;
}

//------------------------------------------------------------------------------
std::vector< std::unique_ptr< std::mutex >> static_locks;
std::once_flag init_static_locks_flag;
void init_static_locks()
{
  //static_locks.resize( ::CRYPTO_num_locks() );
  for( auto& mp : static_locks )
  {
    mp.reset( new std::mutex );
  }
}

extern"C" unsigned long id_fn()
{
  static thread_local auto const tid = ::syscall( SYS_gettid );
  return (unsigned long)tid;
}
extern"C" unsigned long locker_fn( int mode, int n, const char*, int )
{
  if( CRYPTO_LOCK & mode )
  {
    static_locks[n]->lock();
  }
  else
  {
    static_locks[n]->unlock();
  }
}

// https://stackoverflow.com/questions/29845527/how-to-properly-uninitialize-openssl
//
// Startup
//   SSL_library_init(); 1
//   SSL_load_error_strings(); 2
//   FIPS_mode_set(1); 3
//   CRYPTO_set_id_callback(<fn>); 4
//   CRYPTO_set_locking_callback(<fn>); 5
//
// Shutdown
//   FIPS_mode_set(0); 1
//   CRYPTO_set_locking_callback(NULL); 2
//   CRYPTO_set_id_callback(NULL); 3
//   ENGINE_cleanup(); 4
//   CONF_modules_unload(); 5
//   ERR_free_strings(); 6
//   EVP_cleanup(); 7
//   CRYPTO_cleanup_all_ex_data(); 8
//
// And, for each thread:
//   ERR_remove_state();

class SSLThread
{
  static std::mutex _mutex;
  static size_t _nthreads;
  thread_local static size_t _ninstances;
public:
  SSLThread()
  {
    if( _ninstances++ == 0 )
    {
      std::lock_guard< std::mutex > lock( _mutex );
      if( _nthreads++ == 0 )
      {
        //CRYPTO_malloc_debug_init();
        //::CRYPTO_dbg_set_options( V_CRYPTO_MDEBUG_ALL );
        ::CRYPTO_mem_ctrl( CRYPTO_MEM_CHECK_ON );

        ::SSL_library_init(); // 1
        ::SSL_load_error_strings(); // 2
        ::OpenSSL_add_all_algorithms();
        std::call_once( init_static_locks_flag, init_static_locks );
        //::CRYPTO_set_id_callback( &id_fn ); // 4
        //::CRYPTO_set_locking_callback( &locker_fn ); // 5
      }
    }
  }
  ~SSLThread()
  {
    while( make_openssl_error_code() )
    {
    }

    if( --_ninstances == 0 )
    {
      ERR_remove_state( id_fn() );

      std::lock_guard< std::mutex > lock( _mutex );
      if( --_nthreads == 0 )
      {
        //FIPS_mode_set(0); 1
        //::CRYPTO_set_locking_callback( nullptr ); // 2
        //::CRYPTO_set_id_callback( nullptr ); // 3
        ENGINE_cleanup(); // 4
        CONF_modules_unload( 1 ); // 5
        ERR_free_strings(); // 6
        EVP_cleanup(); // 7
        CRYPTO_cleanup_all_ex_data(); // 8
        ERR_remove_state( 0 );
        //CRYPTO_mem_leaks_fp( stdout );
      }
    }
  }
  SSLThread( const SSLThread& ) = delete;
  const SSLThread& operator=( const SSLThread& ) = delete;
  SSLThread( SSLThread&& ) = delete;
  const SSLThread& operator=( SSLThread&& ) = delete;
};

std::mutex SSLThread::_mutex;
size_t SSLThread::_nthreads = 0;
thread_local size_t SSLThread::_ninstances = 0;

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  if( argc < 3)
    std::cerr << "usage: <host> <port> [<cert dir>] [bind_addr]\n";
  std::string const cadir = argc > 3 ? argv[3] : dirname(argv[0]);
  std::cout << cadir << " cadir " << "\n" << std::flush;
 
  SSLThread thread;
  try
  {
    {
      std::cout << "Own IPs\n";
      struct ifaddrs* ifaddr;
      if( getifaddrs( &ifaddr) == -1)
        throw std::system_error( errno, std::system_category());
      for( auto ifa = ifaddr; ifa; ifa= ifa->ifa_next)
      {
        auto const family = ifa->ifa_addr->sa_family;
        if( family == AF_INET)
        {
          std::cout << ifa->ifa_name << " F " << family;
          char host[256];
          if( int const s = getnameinfo( ifa->ifa_addr, sizeof( struct sockaddr_in), host, 256, nullptr, 0, NI_NUMERICHOST))
          {
            std::cout << " = " << gai_strerror(s);
          }
          std::cout << " = " << host << "\n";
        }
      }
      freeifaddrs( ifaddr);
    }
    {
      std::cout << "Target IPs\n";
      struct hostent* h = gethostbyname(argv[1]);
      if( !h)
        throw std::system_error( h_errno, std::system_category());
      for( auto p = h->h_addr_list; *p; ++p)
      {
        struct in_addr iaddr;
        iaddr = *(struct in_addr*)(*p);
        char s[256];
        inet_ntop( AF_INET, &iaddr, s, 256);
        std::cout << argv[1] << " = " << s << "\n" << std::flush;
      }
    }
    if(argc > 4)
    {
      std::cout << "Bind IPs\n";
      struct hostent *h = gethostbyname(argv[4]);
      if( !h)
        throw std::system_error( h_errno, std::system_category());
      for( auto p = h->h_addr_list; *p; ++p)
      {
        struct in_addr iaddr;
        iaddr = *(struct in_addr*)(*p);
        char s[256];
        inet_ntop( AF_INET, &iaddr, s, 256);
        std::cout << argv[1] << " = " << s << "\n" << std::flush;
      }
    }

    SSL_METHOD const *meth = SSLv23_method();
    if( !meth)
      throw std::system_error( make_openssl_error_code());

    SSL_CTX* ctx = SSL_CTX_new( meth);
    if( !ctx)
      throw std::system_error( make_openssl_error_code());
  
    if(!SSL_CTX_use_certificate_chain_file( ctx, (cadir+"cert.pem").c_str()))
      throw std::system_error( make_openssl_error_code());

    if(!SSL_CTX_use_PrivateKey_file( ctx, (cadir+"key.pem").c_str(), SSL_FILETYPE_PEM))
      throw std::system_error( make_openssl_error_code());

    if( SSL_CTX_check_private_key(ctx))
      throw std::system_error( make_openssl_error_code());

    SSL_CTX_set_verify( ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, &verify_certificate);
    if( !SSL_CTX_load_verify_locations( ctx, (cadir+"/cachain.crt.pem").c_str(), nullptr))
      throw std::system_error( make_openssl_error_code());

    std::cout << "tcp_connect:\n" << std::flush;
    int sock = tcp_connect( argv[1], atoi(argv[2]), argc > 4 ? argv[4] : nullptr);
    std::cout << "sock " << sock << "\n" << std::flush;

    SSL* ssl = SSL_new(ctx);

    if( !ssl)
      throw std::system_error( make_openssl_error_code());

    SSL_set_info_callback( ssl, &info_callback);
#if 1 
    SSL_set_fd( ssl, sock);
#else
    BIO* bio = BIO_new_msnet( sock, 1);
    SSL_set_bio( ssl, bio, bio);
#endif
    
    std::cout << "SSL_set_connect_state:\n" << std::flush;
    SSL_set_connect_state( ssl);

    std::cout << "SSL_do_handshake:\n" << std::flush;
    int const handshake_r = SSL_do_handshake( ssl);
    std::cout << "handshake_r = " << handshake_r << "\n" << std::flush;
    if( !handshake_r)
      throw std::system_error( make_openssl_error_code());

    //--------------------------------------------------------------------------
    enum { max_length = 1024 };
    char request[ max_length];
    std::cin.getline( request, max_length);
    size_t request_len = strlen( request);
    std::cout << "sending: [" << request << "]\n" << std::flush;
    int const w = SSL_write( ssl, request, request_len);
    switch( auto const se = SSL_get_error( ssl, w))
    {
    case SSL_ERROR_NONE:
      if( request_len != size_t(w))
      {
        std::cout << "sent " << w << "\n";
        throw std::system_error( std::make_error_code( std::errc::io_error), "Incomplete write");
      }
      break;
    case SSL_ERROR_WANT_READ:
      std::cout << "SSL_ERROR_READ";
      throw std::system_error( make_openssl_error_code());
      break;
    case SSL_ERROR_WANT_WRITE:
      std::cout << "SSL_ERROR_WRITE";
      throw std::system_error( make_openssl_error_code());
      break;
    case SSL_ERROR_ZERO_RETURN:
      std::cout << "SSL_ERROR_ZERO_RETURN";
      throw std::system_error( make_openssl_error_code());
      break;
    case SSL_ERROR_SYSCALL:
      std::cout << "SSL_ERROR_SYSCALL";
      throw std::system_error( h_errno, std::system_category());
      break;
    default:
      std::cout << "SSL_ERROR_UNKNOW se=" << se << "\n";
      throw std::system_error( make_openssl_error_code());
      break;
    }

    //--------------------------------------------------------------------------
    std::cout << "reading\n" << std::flush;
    char reply[max_length];
    int const r = SSL_read( ssl, reply, max_length);
    switch( auto const se = SSL_get_error( ssl, r))
    {
    case SSL_ERROR_NONE:
      std::cout << "read " << w << "\n";
      throw std::system_error( std::make_error_code( std::errc::io_error), "Incomplete write");
      break;
    case SSL_ERROR_WANT_READ:
      break;
    case SSL_ERROR_WANT_WRITE:
      break;
    case SSL_ERROR_ZERO_RETURN:
      break;
    case SSL_ERROR_SYSCALL:
      break;
    default:
      break;
    }
    std::cout << "Reply: [";
    std::cout.write( reply, r);
    std::cout << "\n" << std::flush;

    std::cout << "SSL_shutdown:" << std::flush;
    SSL_shutdown( ssl);
    SSL_free( ssl);
    SSL_CTX_free( ctx);

    close(sock);

  }
  catch( const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n" << std::flush;
  }

  return 0;
}

/*
gcc -Wstrict-prototypes -Wall -Wno-sign-compare -Wpointer-arith -Wdeclaration-after-statement -Wformat-security -Wswitch-enum -Wunused-parameter -Wstrict-aliasing -Wbad-function-cast -Wcast-align -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-field-initializers -Wno-override-init \
           -g -O2 -export-dynamic -o utilities/ovs-dpctl utilities/ovs-dpctl.o \
                lib/libopenvswitch.a \
                     /home/jyyoo/src/dpdk/build/lib/librte_eal.a /home/jyyoo/src/dpdk/build/lib/libethdev.a /home/jyyoo/src/dpdk/build/lib/librte_cmdline.a /home/jyyoo/src/dpdk/build/lib/librte_hash.a /home/jyyoo/src/dpdk/build/lib/librte_lpm.a /home/jyyoo/src/dpdk/build/lib/librte_mbuf.a /home/jyyoo/src/dpdk/build/lib/librte_ring.a /home/jyyoo/src/dpdk/build/lib/librte_mempool.a /home/jyyoo/src/dpdk/build/lib/librte_malloc.a \
                          -lrt -lm -lpthread 
*/

// g++ -std=c++14 -Wall -Wextra -Wpedantic -O3 rawclient.cpp
// libcrypto provides the crypto used by libssl
// http://www.network-theory.co.uk/docs/gccintro/gccintro_18.html
// g++ -std=c++14 -Wall -Wextra -Wpedantic rawclient.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto 
// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

