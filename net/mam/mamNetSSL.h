
#pragma once
#ifndef mam_net_ssl_h
#define mam_net_ssl_h

// ssl include ...
#include <memory>
#include <mutex>
#include <openssl/conf.h>
#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/x509_vfy.h>
#include <openssl/x509v3.h>
#include <sys/syscall.h>
#include <system_error>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <vector>

namespace mam
{

//------------------------------------------------------------------------------
//--- SSL implementation
//------------------------------------------------------------------------------
std::vector<std::unique_ptr<std::mutex>> static_locks;
std::once_flag init_static_locks_flag;
void init_static_locks()
{
  // static_locks.resize( ::CRYPTO_num_locks() );
  for (auto &mp : static_locks)
    mp.reset(new std::mutex);
}

extern "C" unsigned long id_fn()
{
  static thread_local auto const tid = ::syscall(SYS_gettid);
  return (unsigned long)tid;
}
extern "C" void locker_fn(int mode, int n, const char *, int)
{
  if (CRYPTO_LOCK & mode)
    static_locks[n]->lock();
  else
    static_locks[n]->unlock();
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
    if (_ninstances++ == 0)
    {
      std::lock_guard<std::mutex> lock(_mutex);
      if (_nthreads++ == 0)
      {
        // CRYPTO_malloc_debug_init();
        //::CRYPTO_dbg_set_options( V_CRYPTO_MDEBUG_ALL );
        ::CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);

        ::SSL_library_init();       // 1
        ::SSL_load_error_strings(); // 2
        ::OpenSSL_add_all_algorithms();
        std::call_once(init_static_locks_flag, init_static_locks);
        //::CRYPTO_set_id_callback( &id_fn ); // 4
        //::CRYPTO_set_locking_callback( &locker_fn ); // 5
      }
    }
  }
  ~SSLThread()
  {
    // while (make_openssl_error_code())
    //{
    //}

    if (--_ninstances == 0)
    {
      ERR_remove_state(id_fn());

      std::lock_guard<std::mutex> lock(_mutex);
      if (--_nthreads == 0)
      {
        // FIPS_mode_set(0); 1
        //::CRYPTO_set_locking_callback( nullptr ); // 2
        //::CRYPTO_set_id_callback( nullptr ); // 3
        ENGINE_cleanup();             // 4
        CONF_modules_unload(1);       // 5
        ERR_free_strings();           // 6
        EVP_cleanup();                // 7
        CRYPTO_cleanup_all_ex_data(); // 8
        ERR_remove_state(0);
        // CRYPTO_mem_leaks_fp( stdout );
      }
    }
  }
  SSLThread(const SSLThread &) = delete;
  const SSLThread &operator=(const SSLThread &) = delete;
  SSLThread(SSLThread &&) = delete;
  const SSLThread &operator=(SSLThread &&) = delete;
};

std::mutex SSLThread::_mutex;
size_t SSLThread::_nthreads = 0;
thread_local size_t SSLThread::_ninstances = 0;

} // namespace mam

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
