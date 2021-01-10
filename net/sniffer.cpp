
#include <pbx.h>
#include "pbx_net.h" 
#include "pbx_whois.h" 

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <map>
#include <set>
//#include <filesystem>
//#include <unistd.h>
//#include <sys/types.h>
//#include <asm/types.h>
//#include <assert.h>

#include <net/ethernet.h>    // struct ethhdr
#include <net/if_arp.h>      // struct arphdr
#include <netinet/ip.h>      // struct iphdr
#include <netinet/ip_icmp.h> // struct icmp
#include <netinet/igmp.h>    // struct igmp
#include <netinet/tcp.h>     // struct tcphdr
#include <netinet/udp.h>     // struct udphdr
#include <linux/ipv6.h>      // struct ipv6hdr
//#include <net/if.h>
//#include <netinet/in.h>
//#include <linux/if.h>
//#include <linux/netlink.h>
//#include <linux/rtnetlink.h>
//#include <arpa/inet.h>
/*
#include <netlink/netlink.h>
#include <netlink/cache.h>
#include <netlink/utils.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <netlink/route/link.h>
#include <netlink/route/tc.h>
#include <netlink/route/qdisc.h>
#include <netlink/route/class.h>
#include <netlink/route/classifier.h>
#include <netlink/route/qdisc/htb.h>
#include <netlink-private/netlink.h>
#include <netlink-private/socket.h>
#include <netlink-private/utils.h>
*/
// /usr/include/netinet/in.h

static uint32_t MYIP;
static unsigned char MYMAC[6];
// https://www.whois.com/whois/205.206.70.7

// https://cpp.hotexamples.com/examples/-/-/recvmsg/cpp-recvmsg-function-examples.html
// http://man7.org/linux/man-pages/man7/netlink.7.html
// https://androidjapane.blogspot.com/2013/06/code-raw-sockets-in-c-on-linux.html
// https://blog.leetsys.com/
// https://krebsonsecurity.com/
// https://sattia.blogspot.com/
// http://www.irongeek.com/i.php?page=videos/derbycon8/mainlist
// https://www.binarytides.com/category/programming/sockets/c-sockets/
// https://www.binarytides.com/c-code-to-perform-ip-whois/
// https://www.binarytides.com/packet-sniffer-code-c-linux/
// https://www.binarytides.com/packet-sniffer-code-in-c-using-linux-sockets-bsd-part-2/
// https://cs.baylor.edu/~donahoo/tools/sniffer/sniffingFAQ.htm
// 
//
// All tcp ports w/ program name that opened them: netstat -pat
// List all internet ports open:                   lsof -i
// Ethernet Header

/*
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);

The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.
The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
The ntohl() function converts the unsigned integer netlong from network byte order to host byte order.
The ntohs() function converts the unsigned short integer netshort from network byte order to host byte order.

On the i386 the host byte order is Least Significant Byte first, 
whereas the network byte order, as used on the Internet, 
is Most Significant Byte first. 
----------------------------------------------------------------------------------------------------
#include <if_ether.h>
Ethernet Header 
typedef struct ethhdr         (14 bytes)
{
  u_char h_dest[6];
  u_char h_source[6];
  u_short h_proto;     // ETH_P_ARP, ETH_P_IP, ETH_P_IPV6,...  
};

These are the defined Ethernet Protocol ID's. 
#define ETH_P_IP    0x0800    Internet Protocol packet 
#define ETH_P_ARP   0x0806    Address Resolution packet  
#define ETH_P_LOOP  0x0060    Ethernet Loopback packet 
#define ETH_P_ECHO  0x0200    Ethernet Echo packet   
#define ETH_P_PUP   0x0400    Xerox PUP packet   
#define ETH_P_RARP  0x8035    Reverse Addr Res packet  
#define ETH_P_X25   0x0805    CCITT X.25     
#define ETH_P_IPX   0x8137    IPX over DIX     
#define ETH_P_802_3 0x0001    Dummy type for 802.3 frames  
#define ETH_P_AX25  0x0002    Dummy protocol id for AX.25  
#define ETH_P_ALL   0x0003    Every packet (be careful!!!) 
#define ETH_P_IPV6  0x86DD    IPv6 over bluebook

----------------------------------------------------------------------------------------------------
// ARP Header, IPv4
struct  arphdr {
  u_short ar_hrd;           // format of hardware address 
#define ARPHRD_ETHER  1     // ethernet hardware format 
#define ARPHRD_FRELAY   15  // frame relay hardware format 
  u_short ar_pro;           // format of protocol address 
  u_char  ar_hln;           // length of hardware address 
  u_char  ar_pln;           // length of protocol address 
  u_short ar_op;            // one of: 
#define ARPOP_REQUEST 1     // request to resolve address 
#define ARPOP_REPLY 2       // response to previous request 
#define ARPOP_REVREQUEST 3  // request protocol address given hardware 
#define ARPOP_REVREPLY  4   // response giving protocol address 
#define ARPOP_INVREQUEST 8  // request to identify peer 
#define ARPOP_INVREPLY  9   // response identifying peer 
The remaining fields are variable in size, according to the sizes above.
#ifdef COMMENT_ONLY
  u_char  ar_sha[]; // sender hardware address 
  u_char  ar_spa[]; // sender protocol address 
  u_char  ar_tha[]; // target hardware address 
  u_char  ar_tpa[]; // target protocol address 
#endif
};
the ARP message, see RFC 826 ("Packet format")
struct etharp_hdr {
  PACK_STRUCT_FIELD(u16_t hwtype);
  PACK_STRUCT_FIELD(u16_t proto);
  PACK_STRUCT_FLD_8(u8_t  hwlen);
  PACK_STRUCT_FLD_8(u8_t  protolen);
  PACK_STRUCT_FIELD(u16_t opcode);
  PACK_STRUCT_FLD_S(struct eth_addr shwaddr);
  PACK_STRUCT_FLD_S(struct ip4_addr_wordaligned sipaddr);
  PACK_STRUCT_FLD_S(struct eth_addr dhwaddr);
  PACK_STRUCT_FLD_S(struct ip4_addr_wordaligned dipaddr);
} PACK_STRUCT_STRUCT;

----------------------------------------------------------------------------------------------------
https://tools.ietf.org/html/rfc791
The structure of IP Header as given by RFC 791
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |Version|  IHL  |Type of Service|          Total Length         |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |         Identification        |Flags|      Fragment Offset    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |  Time to Live |    Protocol   |         Header Checksum       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Source Address                          |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Destination Address                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Options                    |    Padding    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct iphdr 
{
#if defined(__LITTLE_ENDIAN_BITFIELD)
  __u8  ihl:4,
  version:4;
#elif defined (__BIG_ENDIAN_BITFIELD)
  __u8  version:4,
  ihl:4;
#else
  #error  "Please fix <asm/byteorder.h>"
#endif
  __u8  tos;            // type of service
  __u16 tot_len;        // total length
  __u16 id;             // 
  __u16 frag_off;
  __u8  ttl;
  __u8  protocol;       // IPPROTO_UDP
  __u16 check;
  __u32 saddr; // s_addr is always in network (big endian) byte order on all platforms.
  __u32 daddr;
};

enum {
IPPROTO_IP = 0,           Dummy protocol for TCP    
IPPROTO_ICMP = 1,         Internet Control Message Protocol  
IPPROTO_IGMP = 2,         Internet Group Management Protocol  
IPPROTO_IPIP = 4,         IPIP tunnels (older KA9Q tunnels use 94) 
IPPROTO_TCP = 6,          Transmission Control Protocol  
IPPROTO_EGP = 8,          Exterior Gateway Protocol    
IPPROTO_PUP = 12,         PUP protocol        
IPPROTO_UDP = 17,         User Datagram Protocol    
IPPROTO_IDP = 22,         XNS IDP protocol     
IPPROTO_DCCP = 33,        Datagram Congestion Control Protocol 
IPPROTO_RSVP = 46,        RSVP protocol      
IPPROTO_GRE = 47,         Cisco GRE tunnels (rfc 1701,1702)  
IPPROTO_IPV6 = 41,        IPv6-in-IPv4 tunnelling    
IPPROTO_ESP = 50,         Encapsulation Security Payload protocol 
IPPROTO_AH = 51,          Authentication Header protocol      
IPPROTO_BEETPH = 94,      IP option pseudo header for BEET 
IPPROTO_PIM    = 103,     Protocol Independent Multicast 
IPPROTO_COMP   = 108,     Compression Header protocol 
IPPROTO_SCTP   = 132,     Stream Control Transport Protocol 
IPPROTO_UDPLITE = 136,    UDP-Lite (RFC 3828)    
IPPROTO_RAW   = 255,      Raw IP packets    
IPPROTO_MAX
};

----------------------------------------------------------------------------------------------------
The structure of a TCP header as given by RFC 793
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |          Source Port          |       Destination Port        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                        Sequence Number                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Acknowledgment Number                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |  Data |           |U|A|P|R|S|F|                               |
  | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
  |       |           |G|K|H|T|N|N|                               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           Checksum            |         Urgent Pointer        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Options                    |    Padding    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                             data                              |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct tcphdr 
{
  __u16   source;
  __u16   dest;
  __u32   seq;
  __u32   ack_seq;
#if defined(__LITTLE_ENDIAN_BITFIELD)
  __u16   res1:4,
  doff:4,
  fin:1,
  syn:1,
  rst:1,
  psh:1,
  ack:1,
  urg:1,
  ece:1,
  cwr:1;
#elif defined(__BIG_ENDIAN_BITFIELD)
  __u16   doff:4,
  res1:4,
  cwr:1,
  ece:1,
  urg:1,
  ack:1,
  psh:1,
  rst:1,
  syn:1,
  fin:1;
#else
  #error  "Adjust your <asm/byteorder.h> defines"
#endif  
  __u16   window;
  __u16   check;
  __u16   urg_ptr;
};

----------------------------------------------------------------------------------------------------
struct ipv6hdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8			priority:4,
				version:4;
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u8			version:4,
				priority:4;
#else
#error	"Please fix <asm/byteorder.h>"
#endif
	__u8			flow_lbl[3];

	__be16		payload_len;
	__u8			nexthdr;
	__u8			hop_limit;

	struct	in6_addr	saddr;
	struct	in6_addr	daddr;
};

----------------------------------------------------------------------------------------------------
struct sockaddr_ll 
{
  unsigned short sll_family;   // Always AF_PACKET 
  unsigned short sll_protocol; // Physical layer protocol 
  int            sll_ifindex;  // Interface number 
  unsigned short sll_hatype;   // ARP hardware type 
  unsigned char  sll_pkttype;  // Packet type 
  unsigned char  sll_halen;    // Length of address 
  unsigned char  sll_addr[8];  // Physical layer address 
};

struct sockaddr_in 
{
  sa_family_t    sin_family; // address family: AF_INET 
  in_port_t      sin_port;   // port in network byte order 
  struct in_addr sin_addr;   // internet address 
};
struct sockaddr 
{ 
  unsigned short sa_family;   // address family 
  char           sa_data[14]; // protocol address 
};
struct in_addr // Internet address.
{
  uint32_t       s_addr;     // address in network byte order 
};

*/

//----------------------------------------------------------------------------------------
std::pair< std::string, std::string> convert2( uint32_t address_)
{
  struct sockaddr_in addr;
  memset( &addr, 0, sizeof(addr) );
  addr.sin_addr.s_addr = address_;
  char chararray[INET_ADDRSTRLEN];
  inet_ntop( AF_INET, &(addr.sin_addr), chararray, INET_ADDRSTRLEN );
  //struct hostent *hp = gethostbyaddr((char *) &(addr.sin_addr), 4, AF_INET);
  //return std::make_pair( chararray, (hp != 0) ? hp->h_name : "(null)");
  return std::make_pair( chararray, "(null)" );
}

//----------------------------------------------------------------------------------------
namespace packet {

std::string start( std::ostringstream& oss, size_t recv_data_size_ )
{
  std::string date = pbx::now();
  oss << date;
  oss << ", " << std::right << std::setfill(' ') << std::setw(5) << recv_data_size_;
  oss << "|";
  return date;
}

void append_ethhdr( std::ostringstream& oss, struct ethhdr * eth, size_t &size_ )
{
  size_ = size_ - sizeof(struct ethhdr);

  bool bMacSource = {};
  bool bMacDest = {};

  const unsigned char* s = (const unsigned char*)&( eth->h_source[0] );
  if( MYMAC[0] == s[0] and MYMAC[1] == s[1] and MYMAC[2] == s[2] and MYMAC[3] == s[3] and MYMAC[4] == s[4] and MYMAC[5] == s[5] )
    bMacSource = true;

  const unsigned char* d = (const unsigned char*)&( eth->h_dest[0] );
  if( MYMAC[0] == d[0] and MYMAC[1] == d[1] and MYMAC[2] == d[2] and MYMAC[3] == d[3] and MYMAC[4] == d[4] and MYMAC[5] == d[5] )
    bMacDest = true;

  oss << (bMacSource ? "T" : " "); 
  oss << (bMacDest   ? "R" : " ");
  oss << " ";

#ifdef COLOR
  if( bMacSource )
    oss << color::set(color::FG_BLUE) << pbx::mac_to_str( &(eth->h_source[0])) << color::set(color::FG_DEFAULT);
  else
#endif
    oss << pbx::mac_to_str( &(eth->h_source[0]));

  oss << "->";

#ifdef COLOR
  if( bMacDest )
    oss << color::set(color::FG_BLUE) << pbx::mac_to_str( &(eth->h_dest[0])) << color::set(color::FG_DEFAULT);
  else
#endif
    oss << pbx::mac_to_str( &(eth->h_dest[0]));

  uint16_t hproto = ntohs(eth->h_proto);
  oss << ", 0x" << pbx::uint16_to_hex( hproto );
  switch( hproto )
  {
    case 0x0806: oss << " ETH_P_ARP  "; break;
    case 0x0800: oss << " ETH_P_IP   "; break;
    case 0x86DD: oss << " ETH_P_IPV6 "; break;
    
    case 0x0060: oss << " ETH_P_LOOP "; break;
    case 0x0200: oss << " ETH_P_ECHO "; break;
    case 0x0400: oss << " ETH_P_PUP  "; break;
    case 0x8035: oss << " ETH_P_RARP "; break;
    case 0x0805: oss << " ETH_P_X25  "; break;
    case 0x8137: oss << " ETH_P_IPX  "; break;
    case 0x0001: oss << " ETH_P_802_3"; break;
    case 0x0002: oss << " ETH_P_AX25 "; break;
    case 0x0003: oss << " ETH_P_ALL  "; break;
    default:     oss << " ?????????? "; break;
  }
  oss << "|";
}

void append_arphdr( std::ostringstream& oss, arphdr * arp, size_t &size_ )
{
  size_ = size_ - sizeof(struct arphdr);
  
  oss << "ar_hrd=0x" << pbx::uint16_to_hex( ntohs( arp->ar_hrd ) );
  oss << ", ar_pro=0x" << pbx::uint16_to_hex( ntohs( arp->ar_pro ) );
  oss << ", ar_op=0x"  << pbx::uint16_to_hex( ntohs( arp->ar_op ) );
  //oss << ", sMAC="     << pbx::mac_to_str( arp->ar_sha );   // Source MAC Address 
  //u_char srcIP[4];      	// Source IP Address     
  //u_char arp_dstMAC[6];   // Destination MAC Address
  //u_char dstIP[4];      	// Destination IP Address   
  oss << "|";
}

size_t append_iphdr( std::ostringstream& oss, iphdr * iph_, size_t &size_ )
{
  //size_ = size_ - sizeof(struct iphdr);
  size_ = size_ - ntohs( iph_->tot_len );
/*  
  auto source = convert2( iph_->saddr);
  oss << std::left << std::setfill(' ') << std::setw(15) << source.first;
  oss << "/";
  auto destination = convert2( iph_->daddr);
  oss << std::left << std::setfill(' ') << std::setw(15) << destination.first;
*/
  oss << std::right << std::setfill(' ') << std::setw(15) << pbx::ip_int_to_string( iph_->saddr, true ); 
  oss << "->";
  oss << std::left << std::setfill(' ') << std::setw(15) << pbx::ip_int_to_string( iph_->daddr, true ); 

  oss << ", check=";
  oss << std::left << std::setfill(' ') << std::setw(5) << ntohs( iph_->check );
  oss << ", tos=";
  oss << std::left << std::setfill(' ') << std::setw(5) << ntohs( iph_->tos );
  oss << ", len=";
  oss << std::left << std::setfill(' ') << std::setw(5) << ntohs( iph_->tot_len );
  //oss << ", proto=";
  //oss << uint8_to_hex( ntohs( iph_->protocol ) );
  oss << "|";

  return ntohs( iph_->tot_len ) - sizeof( struct iphdr );
}
/*
  auto src_str2 = convert2( iph->saddr);
  
  struct sockaddr_in source;
  memset(&source, 0, sizeof(source));
  source.sin_addr.s_addr = iph->saddr;
  char src_str[INET_ADDRSTRLEN];
  inet_ntop( AF_INET, &(source.sin_addr), src_str, INET_ADDRSTRLEN);

  struct sockaddr_in dst;
  memset(&dst, 0, sizeof(dst));
  dst.sin_addr.s_addr = iph->daddr;
  char dst_str[INET_ADDRSTRLEN];
  inet_ntop( AF_INET, &(dst.sin_addr), dst_str, INET_ADDRSTRLEN);

  struct hostent *hp = gethostbyaddr((char *) &(source.sin_addr), 4, AF_INET);

  char buffer[1024];
  sprintf( buffer, "%15s -> %15s; %5u -> %5u %5u %s %s", 
    src_str, //inet_ntoa(name.sin_addr),
    dst_str, //inet_ntoa(dst.sin_addr), 
    ntohs(tcph->source), 
    ntohs(tcph->dest), 
    len,
    (hp != 0) ? hp->h_name : "(null)",
    src_str2.first.data()
    );
  
  std::cout << buffer << AT << std::endl();
*/

}

//--------------------------------------------------------------------------------------------------
/*
you can use the socket option IP_HDRINCL to tell that you will construct the header and write both header and payload to the socket: 
sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
int on = 1;
setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
*/

#include <linux/sockios.h> // SIOCGSTAMP
//--------------------------------------------------------------------------------------------------
template< typename T>
void loop_recvfrom( int sock_fd_, T t_)
{
  //struct sockaddr_in wire;
  struct sockaddr_ll wire;
  unsigned char packet[65536];
  while(1)
  {
    socklen_t len = sizeof(struct sockaddr);
    int recv_data_size = recvfrom( sock_fd_, packet, sizeof(packet), 0, (struct sockaddr*)&wire, &len);
    if( recv_data_size > 0)
    {
      // Return a struct timeval with the receive timestamp of the last packet passed to the user. 
      // This is useful for accurate round trip time measurements. 
      // This ioctl should only be used if the socket option SO_TIMESTAMP is not set on the socket.
      // SO_TIMESTAMP Generates a timestamp for each incoming packet in (not necessarily monotonic) system time.
      // Note that SIOCGSTAMP and SO_TIMESTAMP are mutually exclusive - 
      // if you're going to use SIOCGSTAMP you should disable SO_TIMESTAMP (with enabled = 0). 
      // This is because SO_TIMESTAMP directs the kernel to make the timestamp available via recvmmsg ancillary data instead of via SIOCGSTAMP.

      // https://stackoverflow.com/questions/36041740/obtain-packet-timestamp-through-ioctl-call-on-socket-file-descriptor
      // I've discovered why it doesn't work. SIOCGSTAMP only works for UDP packets or RAW sockets, but does not work for TCP. – Gio Mar 17 '16 at 9:33
      // @kirbo it doesn't make sense to ask for timestamps for TCP, 
      // because there's no direct correlation between arriving packets and data becoming available. 
      // If you really want timestamps for TCP you'll have to use RAW sockets and implement your own TCP stack (or use a userspace TCP library). – ecatmur Jul 4 '16 at 10:39

      struct timeval time;
      uint64_t tns={};
      if( ioctl( sock_fd_, SIOCGSTAMP, &time) < 0)
      {
      }
      else
      {
        //std::cout << time.tv_sec << " " << time.tv_usec << std::endl;
        tns = time.tv_sec * 1'000'000'000 + time.tv_usec * 1'000;
      }
      //bool Tx = (wire.sll_pkttype == PACKET_OUTGOING);
      t_( packet, recv_data_size);
    }
    else
    {
      return;
    }
  }
}

//--------------------------------------------------------------------------------------------------
// https://stackoverflow.com/questions/13304672/socket-reading-and-timestamps
// https://man7.org/linux/man-pages/man2/recvmmsg.2.html
template< typename T>
void loop_recvmmsg( int sock_fd_, T t_)
{
  #define VLEN 10
  #define BUFSIZE 200
  #define TIMEOUT 1
  char bufs[VLEN][BUFSIZE+1];
  struct iovec iovecs[VLEN];
  struct mmsghdr msgs[VLEN];
  struct timespec timeout;
  timeout.tv_sec = TIMEOUT;
  timeout.tv_nsec = 0;

//#ifdef neverdefined
/* 
  struct iovec {                  // Scatter/gather array items
    void  *iov_base;              // Starting address
    size_t iov_len;               // Number of bytes to transfer
  };

  struct msghdr {
    void         *msg_name;       // optional address
    socklen_t     msg_namelen;    // size of address
    struct iovec *msg_iov;        // scatter/gather array
    size_t        msg_iovlen;     // # elements in msg_iov
    void         *msg_control;    // ancillary data, see below
    size_t        msg_controllen; // ancillary data buffer len
    int           msg_flags;      // flags on received message
  };

  // The mmsghdr structure is defined in <sys/socket.h> as:
  struct mmsghdr {
     struct msghdr msg_hdr;  // Message header
     unsigned int  msg_len;  // Number of received bytes for header
  };
 */
//#endif

  while(1)
  {  
    memset( msgs, 0, sizeof(msgs) );
    for( int i = 0; i < VLEN; i++) 
    {
      iovecs[i].iov_base         = bufs[i];
      iovecs[i].iov_len          = BUFSIZE;
      msgs[i].msg_hdr.msg_iov    = &iovecs[i];
      msgs[i].msg_hdr.msg_iovlen = 1;
    }
    // recvmmsg - receive multiple messages on a socket
    int retval = recvmmsg( sock_fd_, msgs, VLEN, 0, &timeout);
    if (retval != -1) 
    {
      printf("%d messages received\n", retval);
      for (int i = 0; i < retval; i++) 
      {
        bufs[i][ msgs[i].msg_len ] = 0;
        printf("%d %s", i+1, bufs[i]); 
      }
    }
  }
} 

/*
void listFiles( const std::string& path)
{
  for( auto& dirEntry: std::filesystem::recursive_directory_iterator(path) )
  {
    if (!dirEntry.is_regular_file()) 
    {
      std::cout << "Directory: " << dirEntry.path() << std::endl;
      continue;
    }
    std::filesystem::path file = dirEntry.path();
    std::cout << "Filename: " << file.filename() << " extension: " << file.extension() << std::endl;
  }
}
void listFiles( const std::string& path)
{
  for( const auto& entry : std::filesystem::directory_iterator( path ) ) 
  {
    const auto filenameStr = entry.path().filename().string();
    if( entry.is_directory() ) 
    {
      std::cout << "dir:  " << filenameStr << '\n';
    }
    else if( entry.is_regular_file() )
    {
      std::cout << "file: " << filenameStr << '\n';
    }
    else
      std::cout << "??    " << filenameStr << '\n';
  }
}
*/

//--------------------------------------------------------------------------------------------------
void get_interfaces()
{
  // https://android.googlesource.com/platform/bionic/+/android-4.2_r1/libc/bionic/if_indextoname.c
  // ioctl(ctl_sock, SIOCGIFNAME, &ifr)
  struct if_nameindex *head = if_nameindex();
  if( head == NULL) 
  {
    perror("if_nameindex()");
    return;
  }   
                                                
  struct if_nameindex *ifni = head;
  while( ifni->if_index != 0) 
  {
    printf("Interface %d : %s\n", ifni->if_index, ifni->if_name);
    ifni++;
  }   
  if_freenameindex( head );
  head = NULL;
  ifni = NULL;
}

//--------------------------------------------------------------------------------------------------
struct ipcsv_t
{
  uint64_t _timestamp = {};
  std::string _date, _ip, _iplow, _iphigh;
  std::string _OrgName;
  std::string _Country;
  std::string _Customer;
};

void load_ipcsv_file( const char* csv_file_)
{
  std::vector<ipcsv_t> rtn;
  auto str = std::ifstream( csv_file_, std::ios::in );
  auto tokens = pbx::parse_csv_header( str );
  auto push_ipscsv_t = [&]( pbx::Tokens& tokens_ )
  {
    ipcsv_t data 
    {
      ._date     = tokens_.at(0),
      ._ip       = tokens_.at(1),
      ._iplow    = tokens_.at(2),
      ._iphigh   = tokens_.at(3),
      ._OrgName  = tokens_.at(4),
      ._Country  = tokens_.at(5),
      ._Customer = tokens_.at(6),
    };
    rtn.push_back( data );
  };
  pbx::parse_csv_file( push_ipscsv_t, str );
}

//--------------------------------------------------------------------------------------------------
void process_ip( const std::string& date_, uint32_t ip_, std::map< uint32_t, ipcsv_t >& ipmap_ )
{
  auto found = ipmap_.find( ip_ );
  if( found != ipmap_.end() )
    return;

  std::string ipstr = pbx::ip_int_to_string( ip_ );
  pbx::whois_t whois;
  pbx::execute_whois( ipstr, whois );
  std::cout << "whois requete for: " << ipstr << " " << whois._OrgName << ", " << whois._Country << ", " << whois._Customer << AT << std::endl;
 
  ipcsv_t ipcsv = 
    {
      ._timestamp = pbx::now_as_nanosec(),
      ._date      = date_,
      ._ip        = ipstr,
      ._iplow     = whois._NetRangeLow,
      ._iphigh    = whois._NetRangeHigh,
      ._OrgName   = whois._OrgName,
      ._Country   = whois._Country,
      ._Customer  = whois._Customer,
    };
 
  ipmap_.insert( { ip_, ipcsv } );
}

std::vector<uint32_t> process_iphdr( const std::string& date_, iphdr * iph_,  uint32_t myip_, std::map< uint32_t, ipcsv_t >& ipmap_ )
{
  std::vector<uint32_t> rtn;
  uint32_t source = ntohl( iph_->saddr );
  uint32_t dest = ntohl( iph_->daddr );
  if( source != myip_ )
  {
    process_ip( date_, source, ipmap_ );
    rtn.push_back( source );
  }
  if( dest != myip_ )
  {
    process_ip( date_, dest, ipmap_ );
    rtn.push_back( dest );
  }
  return rtn;
}


//--------------------------------------------------------------------------------------------------
int main()
{
  std::map< uint32_t, ipcsv_t > _ipmap;

  std::ofstream ofs;
  ofs.open ("log.txt", std::ofstream::app);

  int uid = geteuid();
  if( uid != 0)
  {
    std::cerr << "You're not root ! " <<  uid << std::endl;
    exit(1);
  }

  get_interfaces();
  std::cout << "IP: " << pbx::get_nic_ip( MYIP, (const char*)"enp2s0" ) << std::endl;
  std::cout << "MAC: " << pbx::get_nic_mac( (unsigned char*)MYMAC, (const unsigned char *)"enp2s0" ) << std::endl;
  std::cout << "if_nametoindex = " << if_nametoindex( "enp2s0") << std::endl;

  std::cout << "struct ethhdr      = " << sizeof(struct ethhdr) << std::endl;
  std::cout << "  struct arphdr    = " << sizeof(struct arphdr) << std::endl;
  std::cout << "  struct iphdr     = " << sizeof(struct iphdr) << std::endl;
  std::cout << "    struct icmp    = " << sizeof(struct icmp) << std::endl;
  std::cout << "    struct igmp    = " << sizeof(struct igmp) << std::endl;
  std::cout << "    struct tcphdr  = " << sizeof(struct tcphdr) << std::endl;
  std::cout << "    struct udphdr  = " << sizeof(struct udphdr) << std::endl;
  std::cout << "  struct ipv6hdr   = " << sizeof(struct ipv6hdr) << std::endl;
  //return 0;

//  CAP_NET_RAW 
  
  //uint16_t protocol = htons(ETH_P_ALL); // ETH_P_IP

  //int sock_fd = socket( AF_NETLINK, SOCK_RAW, NETLINK_ROUTE); // rien
  //int sock_fd = socket( AF_NETLINK, SOCK_DGRAM, 0); // rien
  
  //20200202:224243.659 dst=00:16:36:9c:81:19 src=00:13:10:09:4b:08 hproto=0x806 ptype=2048
  //int sock_fd = socket( PF_PACKET, SOCK_RAW , htons(ETH_P_ARP)); // #define  ETH_P_ARP   0x0806 /* Address Resolution packet */
  //int sock_fd = socket( PF_PACKET, SOCK_RAW, htons(ETH_P_IP)); // OK  #define ETH_P_IP   0x0800 /* Internet Protocol packet */
  //If you want to sniff only IP and ARP packets for example then you can try this :
  //int sock_fd = socket( PF_PACKET , SOCK_RAW , htons(ETH_P_IP|ETH_P_ARP)); // ouais ... mais marche pas!

  //int sock_fd = socket( AF_INET, SOCK_RAW, IPPROTO_IP); // error 93 
  //int sock_fd = socket( AF_INET, SOCK_RAW, IPPROTO_ICMP); // rien 
  //int sock_fd = socket( AF_INET, SOCK_RAW, IPPROTO_RAW); // rien
  //int sock_fd = socket( AF_INET, SOCK_RAW, htons (ETH_P_ARP)); // error 22
  //int sock_fd = socket( AF_INET, SOCK_RAW, IPPROTO_TCP|IPPROTO_RAW); // rien
  //int sock_fd = socket( AF_INET, SOCK_RAW, 0); // error 93
  
  int sock_fd = socket( PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); // OK
  //int sock_fd = socket( AF_INET, SOCK_RAW, IPPROTO_TCP); // OK
 
  if( sock_fd < 0)
  {
    std::cerr << "ERROR " << errno << AT << std::endl;
    return 0;
  }

  if( not pbx::set_promisc_mode( sock_fd, "enp2s0"))
  {
    std::cerr << "set_promisc_mode() FAILED" << AT << std::endl;
    return 0;
  }

  if( not pbx::bind_fd_to_nic( sock_fd, "enp2s0"))
  //if( not bind_fd_to_nic_2( sock_fd, "enp2s0"))
  //if( not bind_fd_to_nic_3( sock_fd, "enp2s0"))
  {
    std::cerr << "bind_fd_to_nic() FAILED" << AT << std::endl;
    return 0;
  }

  //--------------------------------------------------------------------------------------
  auto fctor0 = [&]( unsigned char *packet_, size_t recv_data_size_)->void
  {
    struct ethhdr *eth = ( struct ethhdr *)packet_; // Find Ethernet header in packet 

    std::ostringstream oss;
    std::string date = packet::start( oss, recv_data_size_ );
    packet::append_ethhdr( oss, eth, recv_data_size_ );
    
    //if( eth->h_proto == htons( 0x0806 ) ) // ETH_P_ARP

    bool bTCP = {};
    size_t szAfterIP = 0;

    uint16_t hproto = ntohs(eth->h_proto);
    switch( hproto )
    {
      case 0x0806: // ETH_P_ARP
        {
          struct arphdr* arp = (struct arphdr *)( packet_ + sizeof( *eth ) );
          packet::append_arphdr( oss, arp, recv_data_size_ );
        }
        break;
      case 0x0800: // ETH_P_IP
        {
          struct iphdr *ip = (struct iphdr *)( packet_ + sizeof( *eth ) );
          szAfterIP = packet::append_iphdr( oss, ip, recv_data_size_ );
          
          std::vector<uint32_t> ips = process_iphdr( date, ip, MYIP, _ipmap );

          // https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers

          switch( ntohs( ip->protocol ) )
          {
            case 0x0100: // ICMP vol.2 p.308, RFC 792
              {
                oss << " ICMP";
                struct icmp *p = (struct icmp *)( packet_ + sizeof( *eth ) + sizeof( *ip ) );
                oss << " type=" << htons( p->icmp_type );
                oss << " code=" << htons( p->icmp_code );
                oss << " sum=" << htons( p->icmp_cksum );
              }
              break;
            case 0x0200: // IGMP vol.2 p.385, RFC 1112
              {
                oss << " IGMP";
                struct igmp *p = (struct igmp *)( packet_ + sizeof( *eth ) + sizeof( *ip ) );
                oss << " type=" << htons( p->igmp_type );
                oss << " code=" << htons( p->igmp_code );
                oss << " sum=" << htons( p->igmp_cksum );
              }
              break;
            case 0x0600: // TCP, RFC 793
              {
                bTCP = true;
                oss << " TCP";
                struct tcphdr *tcp = (struct tcphdr *)( packet_ + sizeof( *eth ) + sizeof( *ip ) );
                oss << " sp=" << htons( tcp->source );
                oss << " dp=" << htons( tcp->dest );
                oss << "|";

                for( auto& ip : ips )
                {
                  auto found = _ipmap.find( ip );
                  if( found != _ipmap.end() )
                  {
                    oss << found->second._OrgName << ", " << found->second._Country << "; ";
                  }
                }
              }
              break;
            case 0x1100: // UDP vol.2 p.759, RFC 768
              {
                oss << " UDP";
                struct udphdr *p = (struct udphdr *)( packet_ + sizeof( *eth ) + sizeof( *ip ) );
                oss << " sp=" << htons( p->uh_sport ); // source port
                oss << " dp=" << htons( p->uh_dport ); // destination port
                oss << " len=" << htons( p->uh_ulen );  // udp length
                oss << " sum=" << htons( p->uh_sum);    // udp checksum
              }
              break;
            default:
              {
                oss << "unknow";
              }
              break;
          }
        }
        break;
      case 0x0060: // ETH_P_LOOP
        {
          oss << " ETH_P_LOOP";
        }
        break;
      case 0x0200: // ETH_P_ECHO
        {
          oss << " ETH_P_ECHO";
        }
        break;
      case 0x0400: // ETH_P_PUP
        {
          oss << " ETH_P_PUP";
        }
        break;
      case 0x8035: // ETH_P_RARP
        {
          oss << " ETH_P_RARP";
        }
        break;
      case 0x0805: // ETH_P_X25
        {
          oss << " ETH_P_X25";
        }
        break;
      case 0x8137: // ETH_P_IPX
        {
          oss << " ETH_P_IPX";
        }
        break;
      case 0x0001: // ETH_P_802_3
        {
          oss << " ETH_P_802_3";
        }
        break;
      case 0x0002: // ETH_P_AX25
        {
          oss << " ETH_P_AX25";
        }
        break;
      case 0x0003: // ETH_P_ALL
        {
          oss << " ETH_P_ALL";
        }
        break;
      case 0x86DD: // ETH_P_IPV6
        {
          oss << " ETH_P_IPV6";
          struct ipv6hdr *p = (struct ipv6hdr *)( packet_ + sizeof(*eth) );
          recv_data_size_ = recv_data_size_ - htons( p->payload_len );
        }
        break;
      default:
        {
          oss << " unknown";
        }
        break;
    }

    //if( eth->h_proto == htons( 0x0800 ) ) // ETH_P_IP
    std::cout << oss.str()  << " | " << recv_data_size_;
    if( bTCP )
    {
      size_t szAfterTCP = szAfterIP - sizeof( struct tcphdr );

      unsigned char* tcpload = packet_ + sizeof( struct ethhdr ) + sizeof( struct iphdr ) + sizeof( struct tcphdr );
      std::cout << " | ";
      std::cout << pbx::char_to_hex( tcpload, szAfterTCP ).str();

      //std::cout << " | ";
      std::ostringstream osstcp;
      pbx::render_printable_chars( osstcp, (char*)tcpload, szAfterTCP );
      std::cout << osstcp.str();
    }
    std::cout << std::endl;

    ofs << oss.str() << std::endl;
  };

  loop_recvfrom( sock_fd, fctor0);
  ofs.close();
}
// git clone https://github.com/poudlar/sniffer
// git commit -m "first commit"
// git remote add origin https://github.com/poudlar/sniffer.git
// git push -u origin master
//
// sudo iptables -L -n -v
// netstat -an | grep LISTEN
// socklist
//
// man 7 raw
// man 7 capabilities
// sudo getcap ./a.out 
// sudo setcap cap_net_raw+ep ./a.out
// sudo setcap CAP_NET_ADMIN+ep ./a.out
// sudo setcap CAP_NET_RAW+ep ./a.out
// sudo setcap CAP_NET_ADMIN+ep ./a.out
// sudo getcap ./a.out 

// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++17 -I.. sniffer.cpp 
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a -I.. sniffer.cpp 
// sudo ./a.out

