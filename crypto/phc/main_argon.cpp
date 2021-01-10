#include "argon2.h"
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>

#define HASHLEN 32
#define SALTLEN 16

//--------------------------------------------------------------------------------------------------
std::string encode(const std::string &word_)
{
  char *PWD = new char[word_.length()];
  strcpy(PWD, word_.c_str());

  uint8_t hash1[HASHLEN];

  uint8_t salt[SALTLEN];
  memset(salt, 0x00, SALTLEN);

  uint8_t *pwd = (uint8_t *)strdup(PWD);
  uint32_t pwdlen = strlen((char *)pwd);

  uint32_t t_cost = 2;         // 1-pass computation
  uint32_t m_cost = (1 << 16); // 64 mebibytes memory usage
  uint32_t parallelism = 1;    // number of threads and lanes

  // high-level API
  argon2i_hash_raw(t_cost, m_cost, parallelism, pwd, pwdlen, salt, SALTLEN, hash1, HASHLEN);

  free(pwd);
  delete[] PWD;

  std::string encoded;
  for (int i(0); i < HASHLEN; ++i)
  {
    encoded += hash1[i];
  }
  return encoded;
}

//--------------------------------------------------------------------------------------------------
bool verify(const std::string &password_, const std::string &encoded_)
{
  bool status = {};

  char *pwd = new char[password_.length()];
  char *hashed = new char[encoded_.length()];

  strcpy(pwd, password_.c_str());
  strcpy(hashed, encoded_.c_str());

  // int vp = argon2i_verify( hashed, pwd, strlen(pwd));
  int vp = argon2i_verify(pwd, hashed, strlen(hashed));
  if (vp == ARGON2_OK)
  {
    status = true;
  }
  else
  {
    std::cout << "ERROR: " << vp << std::endl;
  }

  delete[] pwd;
  delete[] hashed;

  return status;
}

//--------------------------------------------------------------------------------------------------
#define OUT_LEN 32
#define ENCODED_LEN 108
void hashtest(uint32_t version, uint32_t t, uint32_t m, uint32_t p, char *pwd, char *salt, char *hexref, char *mcfref,
              argon2_type type)
{
  unsigned char out[OUT_LEN];
  unsigned char hex_out[OUT_LEN * 2 + 4];
  char encoded[ENCODED_LEN];
  int ret, i;

  printf("Hash test: $v=%d t=%d, m=%d, p=%d, pass=%s, salt=%s: ", version, t, m, p, pwd, salt);

  ret = argon2_hash(t, 1 << m, p, pwd, strlen(pwd), salt, strlen(salt), out, OUT_LEN, encoded, ENCODED_LEN, type,
                    version);
  assert(ret == ARGON2_OK);

  for (i = 0; i < OUT_LEN; ++i)
    sprintf((char *)(hex_out + i * 2), "%02x", out[i]);
  assert(memcmp(hex_out, hexref, OUT_LEN * 2) == 0);

  if (ARGON2_VERSION_NUMBER == version)
  {
    assert(memcmp(encoded, mcfref, strlen(mcfref)) == 0);
  }

  ret = argon2_verify(encoded, pwd, strlen(pwd), type);
  assert(ret == ARGON2_OK);
  ret = argon2_verify(mcfref, pwd, strlen(pwd), type);
  assert(ret == ARGON2_OK);

  printf("PASS\n");
}

//--------------------------------------------------------------------------------------------------
int test(char *PWD)
{
  uint8_t hash1[HASHLEN];
  uint8_t hash2[HASHLEN];

  uint8_t salt[SALTLEN];
  memset(salt, 0x00, SALTLEN);

  uint8_t *pwd = (uint8_t *)strdup(PWD);
  uint32_t pwdlen = strlen((char *)pwd);

  uint32_t t_cost = 2;         // 1-pass computation
  uint32_t m_cost = (1 << 16); // 64 mebibytes memory usage
  uint32_t parallelism = 1;    // number of threads and lanes

  // high-level API
  argon2i_hash_raw(t_cost, m_cost, parallelism, pwd, pwdlen, salt, SALTLEN, hash1, HASHLEN);

  // low-level API
  argon2_context context = {hash2,   /* output array, at least HASHLEN in size */
                            HASHLEN, /* digest length */
                            pwd,     /* password array */
                            pwdlen,  /* password length */
                            salt,    /* salt array */
                            SALTLEN, /* salt length */
                            NULL, 0, /* optional secret data */
                            NULL, 0, /* optional associated data */
                            t_cost, m_cost, parallelism, parallelism, ARGON2_VERSION_13, /* algorithm version */
                            NULL, NULL, /* custom memory allocation / deallocation functions */
                            /* by default only internal memory is cleared (pwd is not wiped) */
                            ARGON2_DEFAULT_FLAGS};

  int rc = argon2i_ctx(&context);
  if (ARGON2_OK != rc)
  {
    printf("Error: %s\n", argon2_error_message(rc));
    exit(1);
  }
  free(pwd);

  for (int i = 0; i < HASHLEN; ++i)
    printf("%02x", hash1[i]);
  printf("\n");
  if (memcmp(hash1, hash2, HASHLEN))
  {
    for (int i = 0; i < HASHLEN; ++i)
    {
      printf("%02x", hash2[i]);
    }
    printf("\nfail\n");
  }
  else
    printf("ok\n");
  return 0;
}

//--------------------------------------------------------------------------------------------------
int main(void)
{
  std::string pwd("this is my password");
  std::string encoded_pwd = encode(pwd);
  std::cout << pwd << std::endl;
  std::cout << "[" << encoded_pwd << "]" << std::endl;
  std::cout << verify(pwd, encoded_pwd) << std::endl;

  std::cout << "---------------------------------------------------------" << std::endl;
  test(pwd.data());

  std::cout << "---------------------------------------------------------" << std::endl;
  int version = ARGON2_VERSION_10;
  hashtest(version, 2, 16, 1, "password", "somesalt",
           "09316115d5cf24ed5a15a31a3ba326e5cf32edc24702987c02b6566f61913cf7",
           "$argon2id$v=19$m=65536,t=2,p=1$c29tZXNhbHQ"
           "$CTFhFdXPJO1aFaMaO6Mm5c8y7cJHAph8ArZWb2GRPPc",
           Argon2_id);
}

// clang-format off
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++17 -I.. sputnic.cpp 
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a -I.. sputnic.cpp
// g++ -I. -std=c89 -O3 -Wall -g -Iinclude -Isrc -pthread -march=native argon2.c core.c blake2/blake2b.c thread.c encoding.c opt.c run.c main.cpp
// g++ -std=c++2a -O3 -W -Wall -g -Iargon2 -pthread -march=native argon2/argon2.c argon2/core.c argon2/blake2/blake2b.c argon2/thread.c argon2/encoding.c argon2/opt.c main_argon.cpp


