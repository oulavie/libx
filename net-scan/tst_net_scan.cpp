
#include "pbx_net_scan.h"

#include <assert.h>
#include <iomanip>
#include <iostream>

void display(const std::vector<std::string> &v_)
{
  for (auto &i : v_)
    std::cout << i << std::endl;
}

class db_t
{
  struct elem_t
  {
    bool _open = {};
    std::string _mac = {};
    std::string _ip = {};
  };
  std::vector<elem_t> _elems = {};
  std::map<std::string, int> _elem_keys = {};

public:
  void add_mac_ip(const std::string mac_, const std::string ip_)
  {
    auto foundmac = _elem_keys.find(mac_);
    auto foundip = _elem_keys.find(ip_);
    if (foundmac == _elem_keys.end() && foundip == _elem_keys.end()) // MAC and IP are new (0,0)
    {
      size_t idx = _elems.size();
      _elems.push_back(elem_t());

      _elem_keys.insert({mac_, idx});
      _elem_keys.insert({ip_, idx});

      _elems[idx]._open = true;
      _elems[idx]._mac = mac_;
      _elems[idx]._ip = ip_;
    }
    else if (foundmac != _elem_keys.end()) // MAC is know (1,0) (1,1)
    {
      size_t idx = foundmac->second;
      if (foundip == _elem_keys.end())
        _elem_keys.insert({ip_, idx});
      else
        _elem_keys[ip_] = idx;

      _elems[idx]._open = true;
      assert(_elems[idx]._mac == mac_);
      _elems[idx]._ip = ip_; // update IP
    }
    else if (foundmac == _elem_keys.end()) // MAC is new (0,1)
    {
      size_t idx = _elems.size();
      _elems.push_back(elem_t());

      _elem_keys.insert({mac_, idx});
      _elem_keys[ip_] = idx;

      _elems[idx]._open = true;
      _elems[idx]._mac = mac_;
      _elems[idx]._ip = ip_;
    }
    else
    {
      // should never be here
    }
  }
  std::string display() const
  {
    int idx(0);
    std::string rtn;
    for (auto &iter : _elems)
    {
      if (!iter._open)
        rtn += display(iter);
    }
    for (auto &iter : _elems)
    {
      if (iter._open)
        rtn += display(iter, ++idx);
    }
    return rtn;
  }
  std::string display(const elem_t &elem_, int idx = 0) const
  {
    std::ostringstream oss;
    if (idx > 0)
      oss << std::setw(2) << idx << " : ";
    else
      oss << "     ";
    oss << elem_._open;
    oss << " " << std::setw(15) << elem_._ip;
    oss << " " << elem_._mac;
    oss << std::endl;
    return oss.str();
  }
};

//--------------------------------------------------------------------------------------------------
/*
$ ifconfig -a
wlp1s0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.0.108  netmask 255.255.255.0  broadcast 192.168.0.255
        inet6 fe80::4b2f:fa20:77ab:587c  prefixlen 64  scopeid 0x20<link>
        ether 70:66:55:cd:cc:d5  txqueuelen 1000  (Ethernet)
        RX packets 564352  bytes 2560010149 (2.5 GB)
        RX errors 0  dropped 104330  overruns 0  frame 0
        TX packets 164956  bytes 147323803 (147.3 MB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
 */
void process_ifconfig(db_t &db_, const std::vector<std::string> interfaces_,
                      const std::vector<std::string> &ifconfig_output_)
{
  bool bip = {}, bmac = {};
  std::string ip, mac;
  for (auto &iter : ifconfig_output_)
  {
    for (auto &nic : interfaces_)
    {
      if (iter.find(nic) != std::string::npos)
      {
        bip = false;
        bmac = false;
        ip = {};
        mac = {};
        break;
      }
    }
    static const std::string ipstr("inet ");
    if (iter.find(ipstr) != std::string::npos)
      std::tie(bip, ip) = pbx::get_ip_address(iter);
    static const std::string macstr("ether ");
    if (iter.find(macstr) != std::string::npos)
      std::tie(bmac, mac) = pbx::get_mac_address(iter);
    if (bip && bmac)
    {
      db_.add_mac_ip(mac, ip);
      bip = false;
      bmac = false;
      ip = {};
      mac = {};
    }
  }
}

//--------------------------------------------------------------------------------------------------
int main()
{
  db_t db = {};
  std::vector<std::string> interfaces;

  std::string iprouteur("192.168.0.1");

  auto date_begin = pbx::execute_command("date");
  display(date_begin);

  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  /*
  netstat -i
  Kernel Interface table
  Iface      MTU    RX-OK RX-ERR RX-DRP RX-OVR    TX-OK TX-ERR TX-DRP TX-OVR Flg
  lo       65536   706910      0      0 0        706910      0      0      0 LRU
  wlp1s0    1500   568002      0 104493 0        166269      0      0      0 BMRU
   */
  auto netstat = pbx::execute_command("netstat -i"); // --interfaces, -i Display a table of all network interfaces.
  display(netstat);
  {
    bool b_lo = {};
    for (auto &iter : netstat)
    {
      auto [bnic, nic] = pbx::get_first_word(iter); // kate gregory, std::tie(b, ip)=
      if (bnic)
      {
        if (b_lo || nic == "lo")
        {
          b_lo = true;
          interfaces.push_back(nic);
        }
      }
    }
  }

  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  /*
  ifconfig -a
  wlp1s0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
          inet 192.168.0.108  netmask 255.255.255.0  broadcast 192.168.0.255
          inet6 fe80::4b2f:fa20:77ab:587c  prefixlen 64  scopeid 0x20<link>
          ether 70:66:55:cd:cc:d5  txqueuelen 1000  (Ethernet)
          RX packets 564352  bytes 2560010149 (2.5 GB)
          RX errors 0  dropped 104330  overruns 0  frame 0
          TX packets 164956  bytes 147323803 (147.3 MB)
          TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
   */
  auto ifconfig = pbx::execute_command("ifconfig -a");
  display(ifconfig);
  process_ifconfig(db, interfaces, ifconfig);

  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  auto ip_a = pbx::execute_command("ip a");
  display(ip_a);

  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  /*
  $ sudo route -n
  Kernel IP routing table
  Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
  0.0.0.0         192.168.0.1     0.0.0.0         UG    600    0        0 wlp1s0
  169.254.0.0     0.0.0.0         255.255.0.0     U     1000   0        0 wlp1s0
  192.168.0.0     0.0.0.0         255.255.255.0   U     600    0        0 wlp1s0
  */
  auto route_n = pbx::execute_command("sudo route -n");
  display(route_n);

  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  /*
  $ arp
  Address                  HWtype  HWaddress           Flags Mask            Iface
  192.168.0.116            ether   b0:05:94:1c:c0:8f   C                     wlp1s0
  192.168.0.172            ether   74:31:70:19:96:4f   C                     wlp1s0
  192.168.0.123            ether   44:e4:ee:f9:6a:52   C                     wlp1s0
  192.168.0.119            ether   f0:ee:10:70:76:16   C                     wlp1s0
  _gateway                 ether   b8:ec:a3:cf:69:c6   C                     wlp1s0
  192.168.0.106            ether   70:ce:8c:46:66:88   C                     wlp1s0
   */
  // auto arp = pbx::execute_command("arp");
  auto arp = pbx::execute_command("arp -a");
  display(arp);
  {
    for (auto &iter : arp)
    {
      auto [bip, ip] = pbx::get_ip_address(iter);    // kate gregory
      auto [bmac, mac] = pbx::get_mac_address(iter); // kate gregory
      if (bip && bmac)
      {
        std::transform(mac.begin(), mac.end(), mac.begin(), ::toupper);
        db.add_mac_ip(mac, ip);
      }
    }
  }

  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  /*
  $ sudo nmap -sn 192.168.0.1/24
  Nmap scan report for _gateway (192.168.0.1)
  Host is up (0.0061s latency).
  MAC Address: B8:EC:A3:CF:69:C6 (Zyxel Communications)

  Nmap scan report for 192.168.0.106
  Host is up (0.061s latency).
  MAC Address: 70:CE:8C:46:66:88 (Samsung Electronics)
  */
  auto sub_net = pbx::execute_command("sudo nmap -sn 192.168.0.1/24"); // routeur same as -sP (No port scan)
  {
    display(sub_net);
    bool bip = {}, bmac = {};
    std::string ip, mac;
    for (auto &iter : sub_net)
    {
      static const std::string ipstr("Nmap scan report for");
      if (iter.find(ipstr) != std::string::npos)
        std::tie(bip, ip) = pbx::get_ip_address(iter);
      static const std::string macstr("MAC Address");
      if (iter.find(macstr) != std::string::npos)
        std::tie(bmac, mac) = pbx::get_mac_address(iter);
      if (bip && bmac)
      {
        db.add_mac_ip(mac, ip);
        bip = false;
        bmac = false;
        ip = {};
        mac = {};
      }
    }
  }

  // auto sub_net2 = pbx::execute_command("sudo nmap -A 192.168.0.*");
  // display(sub_net2);

  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  auto date_end = pbx::execute_command("date");
  std::cout << "start: ";
  display(date_begin);
  std::cout << "stop : ";
  display(date_end);
  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  std::cout << db.display() << std::endl;
  return 0;
}

// g++ -Wall -fno-default-inline -Weffc++ -std=c++17 tst_net_scan.cpp
