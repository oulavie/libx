#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <vector>
#include <openssl/md5.h> //  -lssl -lcrypto
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <algorithm>
#include <typeinfo>
#include <fcntl.h>

/*
struct dirent 
{
  ino_t          d_ino;       // Inode number 
  off_t          d_off;       // Not an offset; see below 
  unsigned short d_reclen;    // Length of this record 
  unsigned char  d_type;      // Type of file; not supported by all filesystem types 
  char           d_name[256]; // Null-terminated filename 
};

struct stat 
{
  dev_t     st_dev;         // ID of device containing file 
  ino_t     st_ino;         // Inode number 
  mode_t    st_mode;        // File type and mode 
  nlink_t   st_nlink;       // Number of hard links 
  uid_t     st_uid;         // User ID of owner
  gid_t     st_gid;         // Group ID of owner 
  dev_t     st_rdev;        // Device ID (if special file) 
  off_t     st_size;        // Total size, in bytes 
  blksize_t st_blksize;     // Block size for filesystem I/O 
  blkcnt_t  st_blocks;      // Number of 512B blocks allocated 

  // Since Linux 2.6, the kernel supports nanosecond precision for the following timestamp fields.
  // For the details before Linux 2.6, see NOTES. 

  struct timespec st_atim;  // Time of last access 
  struct timespec st_mtim;  // Time of last modification 
  struct timespec st_ctim;  // Time of last status change 

#define st_atime st_atim.tv_sec      // Backward compatibility 
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec
};
*/

namespace pbx {

unsigned long get_size_by_fd( int fd_ )
{
  struct stat statbuf;
  if( fstat( fd_, &statbuf ) < 0 )
    exit(-1);
  return statbuf.st_size;
}

void print_md5( unsigned char* md_ )
{
  for( int i(0); i<MD5_DIGEST_LENGTH; ++i)
  {
    printf( "%02x", md_[i] );
  }
  std::cout << std::endl;
}

struct DT_REG_t
{
  std::string _name;
  off_t _st_size;
  time_t _st_mtime;
  std::string _md5;
};

class foo : public std::vector<foo*> 
{
  foo() = delete;
public:
  std::string _dir_name {};
  std::vector< DT_REG_t > _DT_REGs {};
  foo( const std::string& name_ ) : _dir_name( name_ ) {}
  virtual ~foo() noexcept
  {
    for( size_t i(0); i<this->size(); ++i)
    {
      delete this->operator[](i);
      this->operator[](i) = nullptr;
    }
  }
};

void build_tree( foo* p_, const std::string& path_ )
{
  DIR* dpdf = opendir( path_.data() );
  if( dpdf != nullptr )
  {
    try
    {
      struct dirent *epdf = nullptr;
      while( (epdf = readdir( dpdf ) ) != nullptr )
      {
        std::string name = epdf->d_name;
        if( name == "." or name == ".." or name == ".git" or name == ".hg" )
          continue;

        std::string path_d_name = path_;
        if( path_[ path_.length()-1 ] != '/' )
          path_d_name += "/";
        path_d_name += name;

        struct stat statbuf;
        stat( path_d_name.data(), &statbuf );

        switch( epdf->d_type )
        {
        case DT_DIR:
          {
            foo* p = new foo( path_d_name );
            p_->push_back( p );
            build_tree( p, path_d_name );
          }
          break;
        case DT_REG:
          {
            unsigned long file_size = statbuf.st_size;
            int fd = open( path_d_name.data(), O_RDONLY );
            if( fd > 0 )
            {
              char* file_buffer = (char*)mmap( 0, file_size, PROT_READ, MAP_SHARED, fd, 0 );
              unsigned char result[MD5_DIGEST_LENGTH+1];
              result[MD5_DIGEST_LENGTH] = '\0';
              MD5((unsigned char*)file_buffer, file_size, result );
              DT_REG_t a{ ._name = name,
                          ._st_size = statbuf.st_size,
                          ._st_mtime = statbuf.st_mtime,
                          ._md5 = std::string((char*)result)};
              p_->_DT_REGs.push_back( std::move( a ));
              munmap( file_buffer, file_size );
            }
            else
            {
              DT_REG_t a{ ._name = name,
                          ._st_size = statbuf.st_size,
                          ._st_mtime = statbuf.st_mtime,
                          ._md5 = std::string()};
              p_->_DT_REGs.push_back( std::move( a ));
            }
          }
          break;
        case DT_UNKNOWN:
        case DT_FIFO:
        case DT_SOCK:
        case DT_CHR:
        case DT_BLK:
        case DT_LNK:
        break;
        }
      }
    }
    catch(...)
    {
    }
    closedir( dpdf );
  }
  else
  {
    std::cout << "opendir() FAILED for: [" << path_ << "]" << std::endl; 
  }
}

void print_foo( foo* p_, const std::string& offset_ = "  " )
{
  for( auto& it : p_->_DT_REGs )
    std::cout << offset_ << it._name << std::endl;
  for( size_t i(0); i<p_->size(); ++i )
  {
    std::cout << offset_ << "[" << (*p_)[i]->_dir_name << "]" << std::endl;
    print_foo( (*p_)[i], offset_ + "  " );
  }
}

template< typename T >
void dirent_flat_parsing( const std::string& path_, T&& t_ )
{
  DIR* dpdf = opendir( path_.data() );
  if( dpdf != NULL )
  {
    try
    {
      struct dirent *epdf = nullptr;
      while( (epdf = readdir( dpdf ) ) != nullptr )
      {
        std::string name = epdf->d_name;
        if( name == "." or name == ".." or name == ".git" or name == ".hg" )
          continue;

        std::string path_d_name = path_;
        if( path_[ path_.length()-1 ] != '/' )
          path_d_name += "/";
        path_d_name += name;

        struct stat statbuf;
        stat( path_d_name.data(), &statbuf );

        switch( epdf->d_type )
        {
        case DT_REG:
          t_( dirfd( dpdf ), path_d_name, epdf, statbuf );
          break;
        case DT_DIR:
          dirent_flat_parsing( path_d_name, t_ );
          break;
        case DT_UNKNOWN:
        case DT_FIFO:
        case DT_SOCK:
        case DT_CHR:
        case DT_BLK:
        case DT_LNK:
        break;
        }
      }
    }
    catch(...)
    {
    }
    closedir( dpdf );
  }
}

}

std::vector<std::string> list_directory(const std::string &path_)
{
  std::vector<std::string> rtn;
  DIR *dir(NULL);
  struct dirent *ent(NULL);
  if ((dir = opendir(path_.data())) != NULL)
  {
    while ((ent = readdir(dir)) != NULL)
    {
      rtn.push_back(ent->d_name);
    }
    closedir(dir);
  }
  return rtn;
}

std::vector<std::string> list_directory_2(const std::string &path_)
{
  std::vector<std::string> rtn;
  for (const auto &entry : std::filesystem::directory_iterator(path_))
    std::cout << entry.path() << std::endl;
  return rtn;
}

std::vector<std::string> read_file(const std::string &infile_)
{
  std::vector<std::string> rtn;
  std::string line;
  std::ifstream myfile;
  myfile.open(infile_.c_str());
  if (myfile.is_open())
  {
    while (!myfile.eof())
    {
      std::getline(myfile, line);
    }
  }
  return rtn;
}

std::vector<std::string> get_envp(char *envp[])
{
  std::vector<std::string> rtn;
  while (*envp)
  {
    rtn.push_back(*envp++);
  }
  return rtn;
}

void print(const std::vector<std::string> &vect_)
{
  for (auto &a : vect_)
    std::cout << a << std::endl;
}

int main(int argc, char *argv[], char *envp[])
{
  for( int i(0); i<argc; ++i )
    std::cout << argv[i] << std::endl;

  auto s = get_envp( envp );
  print( s );

  // auto a = get_envp(envp);
  // print(a);
  //std::string dir("../../josco/whois");
  //auto a = list_directory_2(dir);
  //print(a);

  pbx::foo *p = new pbx::foo( "" );
  //std::string rep("." );
  //std::string rep(".." );
  //std::string rep("/home/fullname/tmp/libpbx/net" );
  std::string rep("/home/fullname/tmp" );
  pbx::build_tree( p, rep );
  pbx::print_foo( p );
 
  // ~/tmp/josco/whois
  return 0;
}

// clang-format-9 -i checking.cpp
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a -I..  checking.cpp -lssl -lcrypto


