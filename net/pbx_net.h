#ifndef PBX_NET_H
#define PBX_NET_H

#include "pbx_macro.h" 
#include <iostream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <linux/if_packet.h> // struct packet_mreq
#include <ifaddrs.h>
#include <netdb.h>
#include <vector>
//#include <sys/types.h>
//#include <net/ethernet.h>
//#include <netpacket/packet.h>

#ifdef comment 
void set_promisc_mode( const char* nic_ )
bool set_promisc_mode( int fd_, const char* nic_ )
void set_promisc_mode( int fd, int interface_number=0 ) // 0=select all interfaces.

bool bind_fd_to_nic( int fd_, const char* nic_ ) // nic="eth0"
bool bind_fd_to_nic_2( int fd_, const char* nic_ ) 
bool bind_fd_to_nic_3( int fd_, const char* nic_ ) 

std::string ip_int_to_string( uint32_t ip, bool bigendian = false) // bigendian=true == network
uint32_t ip_string_to_int( const char* ip_ )

std::vector< std::pair<int,std::string> > get_all_nic()
std::string get_nic_mac( unsigned char* MAC_, const unsigned char* nic_ = (const unsigned char*)"eth0")
std::string get_nic_ip( uint32_t& IP_, const char* nic_ = (const char*)"eth0" )
std::string get_nic_ip( const std::string& if_)
bool is_nic_up( const std::string& if_)
std::string getInterfaceFromAddress( const std::string& bindAddr_ )
#endif

namespace pbx {

//--------------------------------------------------------------------------------------------------
void set_promisc_mode( const char* nic_ )
{
  struct ifreq eth;
  int fd = socket( AF_INET, SOCK_PACKET, htons(0x800) );
  strcpy( eth.ifr_name, nic_ );
  ioctl( fd, SIOCGIFFLAGS, &eth );
  eth.ifr_flags |= IFF_PROMISC;
  ioctl( fd, SIOCSIFFLAGS, &eth );
}

bool set_promisc_mode( int fd_, const char* nic_ )
{
  struct ifreq ethreq;
  strncpy( ethreq.ifr_name, nic_, IFNAMSIZ );
  if( ioctl( fd_, SIOCGIFFLAGS, &ethreq ) < 0 )
    return false;
  ethreq.ifr_flags |= IFF_PROMISC;
  if( ioctl( fd_, SIOCSIFFLAGS, &ethreq ) < 0 )
    return false;
  return true;
}

// To receive all frames, regardless of destination, it is necessary to put the interface into promiscuous mode. 
// This is done using the PACKET_ADD_MEMBERSHIP socket option, which accepts a structure of type packet_mreq:
// interface_number = if_nametoindex( "eth0")
void set_promisc_mode( int fd, int interface_number=0 ) // 0=select all interfaces.
{
  struct packet_mreq mreq;
  mreq.mr_ifindex = interface_number;
  mreq.mr_type = PACKET_MR_PROMISC;
  if( setsockopt( fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq) ) == -1 ) 
  {
    //perror("ERROR setsockopt failed");
    exit(1);
  }
}

//--------------------------------------------------------------------------------------------------
// Warning: You have to be root and have the CAP_NET_RAW capability in order to use this option.
// The second method is that you can resolv IP address tied to an interface with getifaddrs().
// sudo getcap ./a.out 
// sudo setcap CAP_NET_RAW+ep ./a.out
// sudo setcap CAP_NET_ADMIN+ep ./a.out
bool bind_fd_to_nic( int fd_, const char* nic_ ) // nic="eth0"
{
  struct ifreq ifr;
  memset( &ifr, 0, sizeof(ifr) );
  snprintf( ifr.ifr_name, sizeof(ifr.ifr_name), "%s", nic_ );
  ioctl( fd_, SIOCGIFINDEX, &ifr );
  if( setsockopt( fd_, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, sizeof(ifr)) < 0 ) 
  {
    std::cerr << "ERROR bind_fd_to_nic failed" << AT << std::endl;
    return false;
  }
  return true;
}

bool bind_fd_to_nic_2( int fd_, const char* nic_ ) 
{
  if( setsockopt( fd_, SOL_SOCKET, SO_BINDTODEVICE, (void*)nic_, strlen(nic_) ) < 0 ) 
  {
    std::cerr << "ERROR bind_fd_to_nic failed" << AT << std::endl;
    return false;
  }
  return true;
}

bool bind_fd_to_nic_3( int fd_, const char* nic_ ) 
{
  struct sockaddr_ll saddr;
  memset( &saddr, 0, sizeof(struct sockaddr_ll) );
  saddr.sll_family = AF_PACKET;
  saddr.sll_protocol = htons(0x0003); // ETH_P_ALL
  saddr.sll_ifindex = if_nametoindex( nic_ );
  if( bind( fd_, (struct sockaddr*)&saddr, sizeof(saddr)) < 0 ) 
  {
    std::cerr << "ERROR bind failed" << AT << std::endl;
    return false;
  }
  return true;
}

//--------------------------------------------------------------------------------------------------
/*
std::string ip_to_str( unsigned char *ip)
{
  char buffer[20];
  sprintf( buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  return std::string( buffer);
}
*/
std::string ip_int_to_string( uint32_t ip, bool bigendian = false) // bigendian=true == network
{
  unsigned char bytes[4];
  bytes[0] = (ip) & 0xFF;
  bytes[1] = (ip >> 8) & 0xFF;
  bytes[2] = (ip >> 16) & 0xFF;
  bytes[3] = (ip >> 24) & 0xFF;
  char buf[16];
  if( bigendian ) // network
    sprintf( buf, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3] );
  else 
    sprintf( buf, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0] );
  return std::string( buf );
}

uint32_t ip_string_to_int( const char* ip_ )
{
  uint32_t rtn = 0, tmp = 0;
  char* ip = (char*)ip_;
  while( true )
  {
    if( *ip != '\0' )
    {
      if( *ip != '.' )
      {
        tmp *= 10;
        tmp += *ip - '0';
      }
      else
      {
        rtn += tmp;
        rtn = rtn << 8;
        tmp = 0;
      }
      ++ip;
    }
    else
    {
      rtn += tmp;
      break;
    }
  }
  return rtn;
}

//--------------------------------------------------------------------------------------------------
std::vector< std::pair<int,std::string> > get_all_nic()
{
  // https://android.googlesource.com/platform/bionic/+/android-4.2_r1/libc/bionic/if_indextoname.c
  // ioctl(ctl_sock, SIOCGIFNAME, &ifr)
  
  std::vector< std::pair<int,std::string> > rtn;
  
  struct if_nameindex *head = if_nameindex();
  if (head == NULL)
  {
    perror("if_nameindex()");
    return rtn;
  }

  struct if_nameindex *ifni = head;
  while( ifni->if_index != 0)
  {
    rtn.push_back( { ifni->if_index, ifni->if_name } );
    ifni++;
  }
  if_freenameindex(head);
  head = NULL;
  ifni = NULL;
  return rtn;
}

//--------------------------------------------------------------------------------------------------
std::string get_nic_mac( unsigned char* MAC_, const unsigned char* nic_ = (const unsigned char*)"eth0" )
{
  int fd = socket( AF_INET, SOCK_DGRAM, 0 );
  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy( ifr.ifr_name , (const char *)nic_, IFNAMSIZ-1 );
  ioctl( fd, SIOCGIFHWADDR, &ifr );
  close( fd );
  unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;

  for( int i(0); i<6; ++i )
    MAC_[i] = mac[i];
  
  char buffer[20];
  sprintf( buffer, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return std::string( buffer);
}

//--------------------------------------------------------------------------------------------------
// Get IP address of an interface on Linux
std::string get_nic_ip( uint32_t& IP_, const char* nic_ = (const char*)"eth0" )
{
  struct ifaddrs *ifaddr, *ifa;
  char host[NI_MAXHOST];
  if( getifaddrs( &ifaddr ) == -1 )
  {
    perror("getifaddrs");
  }
  for( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next )
  {
    if( ifa->ifa_addr == NULL )
      continue;

    int s = getnameinfo( ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST );
    if( s != 0 )
      continue;

    if( (strcmp(ifa->ifa_name,nic_)==0) && (ifa->ifa_addr->sa_family==AF_INET) )
    {
      //struct sockaddr *p = ifa->ifa_addr;
      //unsigned char *ip = (unsigned char *)p->sa_data;
      //for( int i(0); i<4; ++i )
      //  IP_[i] = ip[i];
      //printf( "%d %d %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3], ip[4], ip[5]);

      //printf("\tInterface : %s",ifa->ifa_name );
      //printf("\t  Address : %s", host);
      IP_ = ip_string_to_int( host );
      //IP_ = *(ip + 2);
      break;
    }
  }
  freeifaddrs( ifaddr );
  return std::string( host );
}

//--------------------------------------------------------------------------------------------------
std::string get_nic_ip( const std::string& if_)
{
  if( if_ == "localhost" )
    return std::string( "127.0.0.1" );
  if( if_ == "any" )
    return std::string( "0.0.0.0" );
  struct ifreq ifr;
  memset( &ifr, 0, sizeof(ifr) );
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy( ifr.ifr_name, if_.c_str(), IFNAMSIZ-1 );
  int fd = socket( AF_INET, SOCK_DGRAM, 0 );
  if( ioctl( fd, SIOCGIFADDR, &ifr ) < 0 )
  {
    return "ERROR:\"" + std::string(strerror(errno)) + "\"";
  }
  close(fd);
  return inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);
}

//--------------------------------------------------------------------------------------------------
bool is_nic_up( const std::string& if_)
{
  if( if_ == "localhost" )
    return true; 
  if( if_ == "any" )
    return true;
  struct ifreq ifr;
  memset( &ifr, 0, sizeof(ifr) );
  //ifr.ifr_addr.sa_family = AF_INET;
  strncpy( ifr.ifr_name, if_.c_str(), IFNAMSIZ-1 );
  int fd = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );
  if( ioctl( fd, SIOCGIFFLAGS, &ifr ) < 0 )
  {
    //return "ERROR:" + std::string(strerror(errno));
    return false;
  }
  close(fd);
  return ( ifr.ifr_flags & IFF_UP ) && ( ifr.ifr_flags && IFF_RUNNING );
}

//--------------------------------------------------------------------------------------------------
bool is_valid_ip( const std::string& ip_ )
{
  struct in_addr addr;
  return inet_aton( ip_.data(), &addr ) != 0;
}

//--------------------------------------------------------------------------------------------------
std::string getInterfaceFromAddress( const std::string& bindAddr_ )
{
  hostent* ph = gethostbyname( bindAddr_.data() );
  if( ph == NULL )
    return "ERROR";
  in_addr* paddr = (in_addr*)( ph->h_addr );
  std::string IPaddr = inet_ntoa( *paddr );
  std::cout << IPaddr << " // ";
  
  struct ifaddrs *alladdrs, *ptr;
  getifaddrs( &alladdrs );
  ptr = alladdrs;
  while( ptr )
  {
    if( ptr->ifa_addr ) 
    {
      std::cout << std::string( inet_ntoa( ((sockaddr_in*)(ptr->ifa_addr))->sin_addr ) ) << " / " ;
      if( ptr->ifa_addr->sa_family == AF_INET &&
          std::string( inet_ntoa( ((sockaddr_in*)(ptr->ifa_addr))->sin_addr ) ) == IPaddr )
      {
        return std::string( ptr->ifa_name );
      }
    }
    ptr = ptr->ifa_next;
  }
  return std::string();
}


}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

