
#include "pbx_net_scan.h"

#include <iostream>

void display(const std::vector<std::string> &v_)
{
  for (auto &i : v_)
    std::cout << i << std::endl;
}

int main()
{
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
    int i(0);
    bool b_lo = {};
    for (auto &iter : netstat)
    {
      auto [bnic, nic] = pbx::get_first_word(iter); // kate gregory, std::tie(b, ip)=
      if (bnic)
      {
        if (b_lo || nic == "lo")
        {
          b_lo = true;
          std::cout << i++ << " " << nic << std::endl;
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
    int i(0);
    for (auto &iter : arp)
    {
      auto [bip, ip] = pbx::get_ip_address(iter); // kate gregory, std::tie(b, ip)=
      if (bip)
        std::cout << i++ << ": " << ip;
      auto [bmac, mac] = pbx::get_mac_address(iter); // kate gregory, std::tie(b, ip)=
      if (bmac)
      {
        std::transform(mac.begin(), mac.end(), mac.begin(), ::toupper);
        std::cout << " " << mac;
      }
      std::cout << std::endl;
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
    int i(0);
    for (auto &iter : sub_net)
    {
      const std::string ipstr("Nmap scan report for ");
      if (iter.find(ipstr) != std::string::npos)
      {
        auto [bip, ip] = pbx::get_ip_address(iter); // kate gregory, std::tie(b, ip)=
        std::cout << std::endl;
        std::cout << i++;
        if (bip)
          std::cout << ": " << ip;
      }
      const std::string macstr("MAC Address");
      if (iter.find(macstr) != std::string::npos)
      {
        auto [bmac, mac] = pbx::get_mac_address(iter); // kate gregory, std::tie(b, ip)=
        if (bmac)
          std::cout << " " << mac;
      }
    }
    std::cout << std::endl;
  }

  // auto sub_net2 = pbx::execute_command("sudo nmap -A 192.168.0.*");
  // display(sub_net2);

  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  auto date_end = pbx::execute_command("date");
  std::cout << "start: ";
  display(date_begin);
  std::cout << "stop : ";
  display(date_end);
  return 0;
}

// g++ -Wall -fno-default-inline -Weffc++ -std=c++17 tst_net_scan.cpp
