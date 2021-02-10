#ifndef EVP_GCM_ENCRYPT_H
#define EVP_GCM_ENCRYPT_H

#include <openssl/evp.h>

namespace pbx {

int GCMEncrypt( const unsigned char *plainText, int plainTextLen, const unsigned char *aad, int aadLen,
                const unsigned char *key, const unsigned char *IV, int IVLen, unsigned char *cipherText,
                unsigned char *tag )
{
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
/*
  const EVP_CIPHER *cipher = nullptr;
  switch(key_len)
  {
    case 128: cipher = EVP_aes_128_gcm(); break;
    case 192: cipher = EVP_aes_192_gcm(); break;
    case 256: cipher = EVP_aes_256_gcm(); break;
    default: break;
  }
  EVP_EncryptInit (ctx, cipher, KEY, IV);
*/
  EVP_EncryptInit_ex( ctx, EVP_aes_256_gcm(), NULL, NULL, NULL ); // because expected key len = 256bit/32byte 

  EVP_CIPHER_CTX_ctrl( ctx, EVP_CTRL_GCM_SET_IVLEN, IVLen, NULL );

  EVP_EncryptInit_ex( ctx, NULL, NULL, key, IV );

  int len = 0;

  EVP_EncryptUpdate( ctx, NULL, &len, aad, aadLen );

  //EVP_EncryptUpdate( ctx, cipherText, &len, plainText, plainTextLen );
  int inc = 0;
  while( inc <= plainTextLen - 16 )
  {
    EVP_EncryptUpdate( ctx, cipherText + inc, &len, plainText + inc, 16 );
    inc += len;
  }
  EVP_EncryptUpdate( ctx, cipherText + inc, &len, plainText + inc, plainTextLen - inc );
  inc += len;

  //int encryptedLen = len;
  int encryptedLen = inc;

  //EVP_EncryptFinal_ex( ctx, cipherText + len, &len );
  EVP_EncryptFinal_ex( ctx, cipherText + inc, &len );
  encryptedLen += len;

  EVP_CIPHER_CTX_ctrl( ctx, EVP_CTRL_GCM_GET_TAG, 16, tag );

  EVP_CIPHER_CTX_free( ctx );

  return encryptedLen;
}

int GCMDecrypt( const unsigned char *cipherText, int cipherTextLen, const unsigned char *aad, int aadLen,
                unsigned char *tag, const unsigned char *key, const unsigned char *IV, int IVLen,
                unsigned char *plainText )
{
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

  EVP_DecryptInit_ex( ctx, EVP_aes_256_gcm(), NULL, NULL, NULL ); // because expected key len = 256bit/32byte

  EVP_CIPHER_CTX_ctrl( ctx, EVP_CTRL_GCM_SET_IVLEN, IVLen, NULL );

  EVP_DecryptInit_ex( ctx, NULL, NULL, key, IV );

  int len = 0;

  EVP_DecryptUpdate( ctx, NULL, &len, aad, aadLen );

  //EVP_DecryptUpdate( ctx, plainText, &len, cipherText, cipherTextLen );
  int inc = 0;
  while( inc <= cipherTextLen - 16 )
  {
    EVP_DecryptUpdate( ctx, plainText + inc, &len, cipherText + inc, 16 );
    inc += len;
  }
  EVP_DecryptUpdate( ctx, plainText + inc, &len, cipherText + inc, cipherTextLen - inc );
  inc += len;

  //int plainTextLen = len;
  int plainTextLen = inc;

  EVP_CIPHER_CTX_ctrl( ctx, EVP_CTRL_GCM_SET_TAG, 16, tag );

  //int result = EVP_DecryptFinal_ex( ctx, plainText + len, &len );
  int result = EVP_DecryptFinal_ex( ctx, plainText + inc, &len );

  EVP_CIPHER_CTX_free( ctx );

  if( result > 0)
  {
    plainTextLen += len;
    return plainTextLen;
  }
  return -1;
}

}
#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

