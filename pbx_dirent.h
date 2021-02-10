#ifndef PBX_DIRENT_H
#define PBX_DIRENT_H

#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <string.h>

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
// pbx::dirent_flat_parsing( "./ips", []( int dirfd_, const std::string& path_, struct dirent *epdf_, struct stat& stat_)->void {} );
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

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

