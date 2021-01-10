
#include "evp_gcm_encryp.h"
#include "hexdump.h"
#include <string.h>
#include <openssl/rand.h>

// generate 256bit/32byte key
std::string generate_encryption_key( const char* pwd_ )
{
  unsigned char key[32];
  if( !PKCS5_PBKDF2_HMAC_SHA1( pwd_, strlen( pwd_ ), NULL, 0, 1000, 32, key ) )
    return std::string();
  return std::string( (const char*)key );
}

//--------------------------------------------------------------------------------------------------
#define KEY_LEN      32
#define KEK_KEY_LEN  20
#define ITERATION     1 

void uchar_to_hex( unsigned char* out, size_t len )
{
  for(size_t i(0); i < len; ++i) 
    printf("%02x ", out[i]); 
  printf("\n");
}

// https://stackoverflow.com/questions/9771212/how-to-use-pkcs5-pbkdf2-hmac-sha1
void test_0()
{
  const char pwd[] = "password";
  unsigned char salt_value[] = {'s','a','l','t'};

  unsigned char *out = (unsigned char *) malloc(sizeof(unsigned char) * KEK_KEY_LEN);

  printf( "pass: %s\n", pwd );
  printf( "ITERATION: %u\n", ITERATION );
  printf( "salt: " ); for( int i=0; i<sizeof(salt_value); i++) { printf("%02x", salt_value[i]); }
  printf( "\n" );

  if( PKCS5_PBKDF2_HMAC_SHA1( pwd, strlen(pwd), salt_value, sizeof(salt_value), ITERATION, KEK_KEY_LEN, out) != 0 )
  {
    printf("out: "); 
    uchar_to_hex( out, KEK_KEY_LEN );
    std::cout << "out: " << pbx::to_hex( (const unsigned char*)out, KEK_KEY_LEN ) << std::endl;
    std::cout << "exp: " << "0c 60 c8 0f 96 1f 0e 71 f3 a9 b5 24 af 60 12 06 2f e0 37 a6" << std::endl;      
  }
  else
  {
    fprintf(stderr, "PKCS5_PBKDF2_HMAC_SHA1 failed\n");
  }
  free(out);
}


//--------------------------------------------------------------------------------------------------
void test_1()
{
  const unsigned char *plainText =
      (const unsigned char *)( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(1)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(2)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(3)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(4)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(5)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(6)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(7)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(8)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(9)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(A)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(B)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(C)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(D)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(E)"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678(F)"
          );

  // const unsigned char* plainText = (const unsigned char*)("secret");

  int plainTextLen = strlen((const char *)plainText);

  //printf("Plain text input:\n");
  //HexDump(plainText, plainTextLen);

  // 256bit/32byte key
  const unsigned char *key = (const unsigned char *)("12345678901234567890123456789012");

  const unsigned char *aad = (const unsigned char *)("1234");
  int aadLen = 4;

  unsigned char *IV = (unsigned char *)("123456789012");
  int IVLen = 12;

  //unsigned char encryptedText[128] = {0};
  unsigned char *encryptedText = new unsigned char[ plainTextLen ];
  memset( encryptedText, 0, plainTextLen );

  unsigned char tag[16] = {0};

  int encryptedLen = pbx::GCMEncrypt( plainText, plainTextLen, aad, aadLen, key, IV, IVLen, encryptedText, tag);

  //printf("Encrypted message:\n");
  //HexDump(encryptedText, encryptedLen);


  //unsigned char decryptedText[128] = {0};
  //unsigned char decryptedText[ plainTextLen ] = {0};
  unsigned char *decryptedText = new unsigned char[ plainTextLen ];
  memset( decryptedText, 0, plainTextLen );

  int decryptedLen = pbx::GCMDecrypt( encryptedText, encryptedLen, aad, aadLen, tag, key, IV, IVLen, decryptedText);

  printf("Decrypted text: %s\n", decryptedText);

  if (decryptedLen > -1)
  {
    HexDump( decryptedText, decryptedLen);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << pbx::to_hex( (const char*)decryptedText, decryptedLen ) << std::endl;
  }
  else
  {
    printf("Decryption error\n");
  }
  delete [] encryptedText;
  delete [] decryptedText;
}

//--------------------------------------------------------------------------------------------------
void test_2()
{
	unsigned char plaintext[1024],key[32],ciphertext[1024+EVP_MAX_BLOCK_LENGTH],tag[100],pt[1024+EVP_MAX_BLOCK_LENGTH];
	unsigned char iv[16];
	unsigned char aad[17]="abcdefghijklmnop";	//dummy
	int k;

	printf("Enter key: ");
	scanf("%s",key);
	
	/* generate encryption key from user entered key */
	if(!PKCS5_PBKDF2_HMAC_SHA1((const char*)key, strlen((const char*)key),NULL,0,1000,32,key))
	{
		printf("Error in key generation\n");
		exit(1);
	}

	/* generate random IV */
	while(!RAND_bytes(iv,sizeof(iv)));

	/* get plaintext input */
	printf("Enter plaintext: ");
	scanf("%s",plaintext);

	/* encrypt the text and print on STDOUT */
	k = pbx::GCMEncrypt( plaintext, strlen((const char*)plaintext), aad, sizeof(aad)-1, 
                       key, iv, strlen((const char*)iv), ciphertext, tag);
	printf("%s\n",ciphertext);

	/* decrypt the text and print on STDOUT */
	k = pbx::GCMDecrypt( ciphertext, k, aad, sizeof(aad)-1, 
                       tag, key, iv, strlen((const char*)iv), pt);
	if(k>0)
	{
		pt[k]='\0';
		printf("%s\n",pt);
	}
	else
		printf("Unreliable Decryption, maybe the encrypted data was tampered\n");
}


//--------------------------------------------------------------------------------------------------
int main( int argc, const char *argv[] )
{
  //test_0();
  //test_1();
  test_2();
  return 0;
}

// The fragility of AES-GCM authentication algorithm
// https://eprint.iacr.org/2013/157.pdf

// stack smashing detected
// -fno-stack-protector -lssp
// objdump -D a.out > toto             _stack_chk_fail

// Latest installation of openssl and libssl-dev which can be done using sudo apt-get install command in terminal

// g++ main_evp_gcm_encryp.cpp -lcrypto -lssl -W -Wall

