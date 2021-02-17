
#include "pbx_net_scan.h"
#include <pbx_time.h>

#include <assert.h>
#include <fstream>
#include <iomanip>
#include <iostream>

template <typename T> void func(T t)
{
  std::cout << t << std::endl;
}
template <typename T, typename... Args> void func(T t, Args... args) // recursive variadic function
{
  std::cout << t << std::endl;
  func(args...);
}

template <class T> void func2(std::initializer_list<T> list)
{
  for (auto elem : list)
    std::cout << elem << std::endl;
}

//--------------------------------------------------------------------------------------------------
template <typename T> class logger
{
  std::string _file = {};
  std::vector<T> _v = {};

public:
  logger(const std::string &file_) : _file(file_)
  {
  }
  void log(const T &t_)
  {
    _v.push_back(t_);

    std::ofstream log;
    log.open(_file, std::ios_base::app);
    log << pbx::now();
    log << " ";
    for (auto &iter : _v)
      log << iter;
    log << std::endl;

    _v.clear();
  }
  template <typename... Args> void log(const T &t_, Args... args_)
  {
    _v.push_back(t_);
    return log(args_...);
  }
};

logger<std::string> LOG("jscan.txt");

void display(const std::vector<std::string> &v_)
{
  for (auto &i : v_)
    std::cout << i << std::endl;
}

typedef std::tuple<std::string, std::string, std::string> mac_ip_t;

//--------------------------------------------------------------------------------------------------
class db_t
{
  class elem_t
  {
    bool _open = true;
    std::string _mac = {};
    std::string _ip = {};
    std::string _what = {};
    std::string _time_in = pbx::now();
    std::string _time_out = {};
    uint32_t _time_in_int = pbx::now_as_int();
    uint32_t _time_out_int = 0;
    uint32_t _set_open_sz = 0;

  public:
    elem_t(const std::string mac_, const std::string ip_, const std::string &what_) : _mac(mac_), _ip(ip_), _what(what_)
    {
      LOG.log("35=D;48=", _mac, ";55=", _ip, ";52=", _time_in, ";106=", _what);
    }
    void set_close()
    {
      assert(_open == true);
      assert(_time_out_int < _time_in_int);
      _open = false;
      _time_out = pbx::now();
      _time_out_int = pbx::now_as_int();
      LOG.log("35=G;48=", _mac, ";55=", _ip, ";52=", _time_in, ";53=", _time_out);
    }
    void set_open(const std::string ip_)
    {
      assert(_open == false);
      assert(_time_in_int < _time_out_int);
      _open = true;
      _ip = ip_;
      _time_in = pbx::now();
      _time_in_int = pbx::now_as_int();
      ++_set_open_sz;
      LOG.log("35=F;48=", _mac, ";55=", _ip, ";53=", _time_out, ";52=", _time_in);
    }
    void display_elem(std::ostringstream &oss_) const
    {
      oss_ << _open;
      oss_ << " " << std::setw(15) << _ip;
      oss_ << " " << _mac << " ";
      if (_time_in_int < _time_out_int)
      {
        oss_ << "i" << _time_in;
        if (!_time_out.empty())
          oss_ << " o" << _time_out << " ";
        else
          oss_ << std::setw(21) << " ";
      }
      else
      {
        if (!_time_out.empty())
          oss_ << "o" << _time_out << " ";
        oss_ << "i" << _time_in << " ";
        if (_time_out.empty())
          oss_ << std::setw(21) << " ";
      }
      oss_ << std::setw(4) << _set_open_sz;
      oss_ << _what;
    }
    bool is_open() const
    {
      return _open;
    }
    const std::string &get_mac() const
    {
      return _mac;
    }
    const std::string &get_ip() const
    {
      return _ip;
    }
  };
  std::vector<elem_t> _elems = {};
  std::map<std::string, size_t> _mac_keys = {};
  std::map<std::string, size_t> _ip_keys = {};

public:
  bool add_mac_ip(const std::vector<mac_ip_t> &v_)
  {
    bool changed = {};
    // 1) on verifie que tous les elements open dans db sont dans la nouvelle liste
    //    sinon, on les ferme (ils ne sont plus vu par le scan de nmap)
    for (auto &elem : _elems)
    {
      if (elem.is_open())
      {
        auto found = std::find_if(v_.begin(), v_.end(), [&elem](const mac_ip_t &e_) {
          auto [mac, ip, what] = e_;
          return mac == elem.get_mac();
        });
        if (found == v_.end())
        {
          elem.set_close();
          changed = true;
        }
      }
    }
    for (auto &macip : v_)
    {
      auto [mac, ip, what] = macip;
      changed |= add_mac_ip(mac, ip, what);
    }
    return changed;
  }

private:
  bool add_mac_ip(const std::string &mac_, const std::string &ip_, const std::string &what_)
  {
    bool rtn = true;
    auto foundmac = _mac_keys.find(mac_);
    auto foundip = _ip_keys.find(ip_);
    if (foundmac != _mac_keys.end() && foundip != _ip_keys.end()) // MAC and IP are both know (1,1)
    {
      size_t idx_mac = foundmac->second;
      size_t idx_ip = foundip->second;
      if (idx_mac == idx_ip && _elems[idx_mac].is_open())
        rtn = false;
      else
      {
        _ip_keys[ip_] = idx_mac;
        _elems[idx_mac].set_open(ip_); // update IP
      }
    }
    else if (foundmac == _mac_keys.end()) // MAC is new (0,0) (0,1)
    {
      size_t idx = _elems.size();
      _elems.push_back(elem_t(mac_, ip_, what_));
      _mac_keys.insert({mac_, idx});
      if (foundip == _ip_keys.end())
        _ip_keys.insert({ip_, idx});
      else
        _ip_keys[ip_] = idx; // update IP
    }
    else if (foundmac != _mac_keys.end()) // MAC is know (1,0)
    {
      size_t idx = foundmac->second;
      if (foundip == _ip_keys.end())
        _ip_keys.insert({ip_, idx});
      else
        _ip_keys[ip_] = idx;

      _elems[idx].set_open(ip_); // update IP
      assert(_elems[idx].get_mac() == mac_);
    }
    else
    {
      // should never be here
    }
    return rtn;
  }

public:
  std::string display() const
  {
    int idx(0);
    std::string rtn;
    rtn += pbx::now();
    rtn += "\n";
    for (size_t i(0); i < _elems.size(); ++i)
    {
      if (!_elems[i].is_open())
        rtn += display(_elems[i], ++idx, i + 1);
    }
    idx = 0;
    for (size_t i(0); i < _elems.size(); ++i)
    {
      if (_elems[i].is_open())
        rtn += display(_elems[i], ++idx, i + 1);
    }
    return rtn;
  }

private:
  std::string display(const elem_t &elem_, int idx, int pos) const
  {
    std::ostringstream oss;
    oss << std::setw(2) << idx << ".";
    oss << std::setw(2) << pos << ".";
    elem_.display_elem(oss);
    oss << std::endl;
    return oss.str();
  }
};

//--------------------------------------------------------------------------------------------------
class scan_t
{
  db_t _db = {};

public:
  scan_t()
  {
    LOG.log("35=A;");
  }
  ~scan_t()
  {
    LOG.log("35=5;");
  }
  db_t &get_db()
  {
    return _db;
  }
  bool scan()
  {
    bool changed = {};
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
    std::vector<mac_ip_t> macips;
    bool bip = {}, bmac = {}, bwhat = {};
    std::string ip, mac, what;
    for (auto &iter : sub_net)
    {
      static const std::string ipstr("Nmap scan report for");
      if (iter.find(ipstr) != std::string::npos)
        std::tie(bip, ip) = pbx::get_ip_address(iter);
      static const std::string macstr("MAC Address");
      if (iter.find(macstr) != std::string::npos)
      {
        std::tie(bmac, mac) = pbx::get_mac_address(iter);
        std::tie(bwhat, what) = pbx::get_between_parenthesis(iter);
      }
      if (bip && bmac)
      {
        macips.push_back({mac, ip, what});
        bip = false;
        bmac = false;
        ip = {};
        mac = {};
      }
    }
    changed = _db.add_mac_ip(macips);
    return changed;
  }
  void display() const
  {
    std::cout << _db.display() << std::endl;
  }
  void loop()
  {
    while (true)
    {
      if (scan())
        display();
    }
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
void process_ifconfig(std::vector<mac_ip_t> &v_, const std::vector<std::string> interfaces_,
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
      std::transform(mac.begin(), mac.end(), mac.begin(), ::toupper);
      v_.push_back({mac, ip, "(me,ifconfig)"});
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
  scan_t scanner = {};
  std::vector<std::string> interfaces;
  std::vector<mac_ip_t> macips;

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
  process_ifconfig(macips, interfaces, ifconfig);

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
  /*
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
          macips.push_back({mac, ip, ""});
        }
      }
    }
  */

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
    bool bip = {}, bmac = {}, bwhat = {};
    std::string ip, mac, what;
    for (auto &iter : sub_net)
    {
      static const std::string ipstr("Nmap scan report for");
      if (iter.find(ipstr) != std::string::npos)
        std::tie(bip, ip) = pbx::get_ip_address(iter);
      static const std::string macstr("MAC Address");
      if (iter.find(macstr) != std::string::npos)
      {
        std::tie(bmac, mac) = pbx::get_mac_address(iter);
        std::tie(bwhat, what) = pbx::get_between_parenthesis(iter);
      }
      if (bip && bmac)
      {
        macips.push_back({mac, ip, what});
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
  scanner.get_db().add_mac_ip(macips);
  scanner.display();
  scanner.loop();
  return 0;
}

// g++ -Wall -fno-default-inline -Weffc++ -std=c++17 tst_net_scan.cpp
