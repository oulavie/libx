
https://goteleport.com/blog/how-to-ssh-properly/

nmap -sP 192.168.0.1/24           // to discover the sub-network
nmap -sV 192.168.0.107 -p 1-65535 // to know which ports are open
sudo nmap -Pn 192.168.0.191

nmap -A -T4 scanme.nmap.org 

TARGET SPECIFICATION 

HOST DISCOVERY
-sn (No port scan)
-Pn (No ping)

-PS port list (TCP SYN Ping)
-PA port list (TCP ACK Ping) 
-PU port list (UDP Ping)
-PY port list (SCTP INIT Ping) 
-PE; -PP; -PM (ICMP Ping Types) 
-PO protocol list (IP Protocol Ping) 
-n (No DNS resolution)    
-R (DNS resolution for all targets)
--resolve-all (Scan each resolved address) 
--system-dns (Use system DNS resolver)
--dns-servers server1[,server2[,...]]  (Servers to use for reverse DNS queries)

PORT SCANNING BASICS
While Nmap has grown in functionality over the years, it began as an
PORT SCANNING TECHNIQUES  
-sS (TCP SYN scan)
-sT (TCP connect scan)
-sU (UDP scans) 
-sY (SCTP INIT scan)
-sN; -sF; -sX (TCP NULL, FIN, and Xmas scans) 
-sA (TCP ACK scan)
-sW (TCP Window scan) 
-sM (TCP Maimon scan)
-sO (IP protocol scan) 


LoadAll.sh 2>&1 | tee -a Load.log

https://securitytrails.com/blog/top-15-nmap-commands-to-scan-remote-hosts

https://www.yeahhub.com/top-30-basic-nmap-commands-beginners/

https://www.yeahhub.com/5-commonly-used-nmap-commands/

https://www.yeahhub.com/most-useful-nmap-commands/

https://phoenixnap.com/kb/nmap-command-linux-examples

https://www.tecmint.com/nmap-command-examples/

In addition to the interesting ports table, Nmap can provide further information on targets, 
including reverse DNS names, operating system guesses, device types, and MAC addresses.

NetBIOS, MAC and IP address, computer name, and hostname. 

Target Selection
1   Scan a single IP  nmap 192.168.20.128   Nmap Commands
2   Scan a host   nmap www.example.com  Nmap Commands
3   Scan a range of IPs   nmap 192.168.20.120-128   Nmap Commands
4   Scan a subnet   nmap 192.168.20.2/24  Nmap Commands
5   Scan targets from Text file   nmap -iL ips.txt  Nmap Commands

Port Selection
6   Scan a single port  nmap -p 22 192.168.20.128   Nmap Commands
7   Scan a range of ports   nmap -p 1-100 192.168.20.128  Nmap Commands
8   Scan 100 common ports   nmap -F 192.168.20.128  Nmap Commands
9   Scan all ports  nmap -p- 192.168.20.128   Nmap Commands
10  Specify UDP or TCP scan   nmap -p U:137,T:139 192.168.20.128

Scan Types
11  Scan using TCP connect  nmap -sT 192.168.20.128   Nmap Commands
12  Scan using TCP SYN scan   nmap -sS 192.168.20.128   Nmap Commands
13  Scan UDP ports  nmap -sU -p 123,161,162 192.168.20.128  Nmap Commands
14  Scan Selected ports (Ignore Discovery)  nmap -Pn -F 192.168.20.128

Service and OS Detection
15  Detect OS and Services  nmap -A 192.168.20.128  Nmap Commands
16  Standard service detection  nmap -sV 192.168.20.128   Nmap Commands
17  Aggressive service detection  nmap -sV –version-intensity 5 192.168.20.128

Output Formats
18  Save default output to file   nmap -oN result.txt 192.168.20.128  Nmap Commands
19  Save results as XML   nmap -oX resultxml.xml 192.168.20.128   Nmap Commands
20  Save formatted results (Grep)   nmap -oG formattable.txt 192.168.20.128   Nmap Commands
21  Save in all formats   nmap -oA allformats 192.168.20.128  Nmap Commands

Scripting Engine
22  Scan using default safe scripts   nmap -sV -sC 192.168.20.128   Nmap Commands
23  Get help for a script   nmap –script-help=ssl-heartbleed  Nmap Commands
24  Scan using a specific script  nmap -sV -p 443 -script=ssl-heartbleed 192.168.20.133   Nmap Commands
25  Update script database  nmap –script-updatedb   Nmap Commands

Some Useful NSE Scripts
26  Scan for UDP DDOS reflectors  nmap -sU -A -PN -n -pU:19,53,123,161 -script=ntp-monlist,dns-recursion,snmp-sysdescr 192.168.20.2/24  Nmap Commands
27  Gather page titles from HTTP Servers  nmap –script=http-title 192.168.20.128  Nmap Commands
28  Get HTTP headers of web services  nmap –script=http-headers 192.168.20.128  Nmap Commands
29  Find web apps from known paths  nmap –script=http-enum 192.168.20.128   Nmap Commands
30  Find exposed Netbios servers  nmap -sU –script nbtstat.nse -p 137 192.168.20.128

----------------------------------------------------------------------------------------------------
Example 1 – Ping Scan
Command: nmap -sP <Target>
nmap -sP 192.168.0.1/24

Sometimes you just want to know which hosts are running on the network at this time. 
Nmap can do this by sending an ICMP echo request packet to each IP address in the network you specify.
It will respond if the host is running. Unfortunately, some sites such as microsoft.com block ICMP echo request packets.
However, by default, nmap can also send TCP ack packets to port 80. If you receive a RST packet, it means the host is running.
The third technique usd by nmap is to send a SYN packet and then wait for an RST or SYN/ACK packet. 
For non-root users, nmap uses the connect() method.

----------------------------------------------------------------------------------------------------
Example 2- Standard Service Detection'
Command: nmap -sV <Target> -p 1-65535

This is the command to scan for running service. 
Nmap contains a database of more than 2000+ well-known services and associated ports.

----------------------------------------------------------------------------------------------------
Example 3 – TCP Syn Scan
Command: nmap -sS <Target> -p 80
sudo nmap -sS 192.168.0.107 -p 80

Since it is not necessary to open all TCP connections, it is usually called half-open. 
You can issue a TCP sync packet (SYN) and wait for a response.

    If the other party returns a SYN|ACK (response) packet, it indicates that the target port is listening;
    if the RST packet is returned, it means that the target port has no listener;
    if a SYN|ACK packet is received, the source host will immediately issue a RST (reset).

The packet is disconnected from the target host, which is actually done automatically by our operating system kernel. 
The biggest benefit of this technology is that few systems can log this into the system log. 
However, you always need root privileges to customize the SYN packet.

----------------------------------------------------------------------------------------------------
Example 4 – Detect Operating System
Command: nmap -O <Target>

This is the command to scan and search for the OS (and the OS version) on a host. 
This command will provide valuable information for the enumeration phase of your network security assessment.

----------------------------------------------------------------------------------------------------
Example 5 – Detect OS and Services
Command: nmap -A <Target>

Nmap is one of the most popular tools used for the enumeration of a targeted host. 
Nmap can use scans that provide the OS, version, and service detection for individual or multiple devices.

Detection scans are critical to the enumeration process when conducting penetration testing of a network. 
It is important to know where vulnerable machines are located on the network so they can be fixed or replaced before they are attacked.

Many attackers will use these scans to figure out what payloads would be most effective on a victim’s device. 
The OS scan works by using the TCP/IP stack fingerprinting method. 
The services scan works by using the Nmap-service-probes database to enumerate details of services running on a targeted host.



----------------------------------------------------------------------------------------------------
fullname@pop-os:~/tmp/oulavie/libx/net-scan$ arp
Address                  HWtype  HWaddress           Flags Mask            Iface
192.168.0.116            ether   b0:05:94:1c:c0:8f   C                     wlp1s0
192.168.0.123            ether   44:e4:ee:f9:6a:52   C                     wlp1s0
_gateway                 ether   b8:ec:a3:cf:69:c6   C                     wlp1s0
192.168.0.159            ether   2a:e7:4e:f3:e7:4b   C                     wlp1s0
192.168.0.106            ether   70:ce:8c:46:66:88   C                     wlp1s0
192.168.0.102            ether   88:75:98:00:72:9e   C                     wlp1s0
192.168.0.191            ether   c0:d0:12:7a:07:8d   C                     wlp1s0
192.168.0.142            ether   6c:00:6b:21:77:8f   C                     wlp1s0
192.168.0.170            ether   fc:a6:21:69:53:bb   C                     wlp1s0
192.168.0.190            ether   50:65:f3:bf:57:54   C                     wlp1s0

----------------------------------------------------------------------------------------------------
fullname@pop-os:~/tmp/oulavie/libx/net-scan$ nmap -sP 192.168.0.1/24
Starting Nmap 7.91 ( https://nmap.org ) at 2021-02-13 19:58 EST
Nmap scan report for _gateway (192.168.0.1)
Host is up (0.0011s latency).
Nmap scan report for 192.168.0.106
Host is up (0.034s latency).
Nmap scan report for pop-os (192.168.0.107)
Host is up (0.00016s latency).
Nmap scan report for 192.168.0.116
Host is up (0.078s latency).
Nmap scan report for 192.168.0.123
Host is up (0.033s latency).
Nmap scan report for 192.168.0.142
Host is up (0.067s latency).
Nmap scan report for 192.168.0.170
Host is up (0.091s latency).
Nmap scan report for 192.168.0.172
Host is up (0.0044s latency).
Nmap scan report for 192.168.0.173
Host is up (0.091s latency).
Nmap scan report for 192.168.0.190
Host is up (0.0080s latency).
Nmap done: 256 IP addresses (10 hosts up) scanned in 5.15 seconds

----------------------------------------------------------------------------------------------------
fullname@pop-os:~/tmp/oulavie/libx/net-scan$ sudo nmap -Pn 192.168.0.191
Host discovery disabled (-Pn). All addresses will be marked 'up' and scan times will be slower.
Starting Nmap 7.91 ( https://nmap.org ) at 2021-02-13 19:51 EST
Nmap scan report for 192.168.0.191
Host is up (0.0074s latency).
Not shown: 764 closed ports, 235 filtered ports
PORT      STATE SERVICE
62078/tcp open  iphone-sync
MAC Address: C0:D0:12:7A:07:8D (Apple)

----------------------------------------------------------------------------------------------------
fullname@pop-os:~/tmp/oulavie/libx/net-scan$ sudo nmap -Pn 192.168.0.106
Host discovery disabled (-Pn). All addresses will be marked 'up' and scan times will be slower.
Starting Nmap 7.91 ( https://nmap.org ) at 2021-02-13 20:01 EST
Nmap scan report for 192.168.0.106
Host is up (0.016s latency).
All 1000 scanned ports on 192.168.0.106 are closed
MAC Address: 70:CE:8C:46:66:88 (Samsung Electronics)

Nmap done: 1 IP address (1 host up) scanned in 7.98 seconds

----------------------------------------------------------------------------------------------------
fullname@pop-os:~/tmp/oulavie/libx/net-scan$ sudo nmap -A 192.168.0.116
Starting Nmap 7.91 ( https://nmap.org ) at 2021-02-13 20:03 EST
Nmap scan report for 192.168.0.116
Host is up (0.0095s latency).
All 1000 scanned ports on 192.168.0.116 are closed
MAC Address: B0:05:94:1C:C0:8F (Liteon Technology)
Device type: firewall|general purpose|game console
Running: Cisco AsyncOS 7.X, FreeBSD 10.X|6.X|7.X|8.X|9.X, Sony embedded
OS CPE: cpe:/h:cisco:ironport_c650 cpe:/o:cisco:asyncos:7.0.1 cpe:/o:freebsd:freebsd:10.2 cpe:/h:sony:playstation_4 cpe:/o:freebsd:freebsd:6.2 cpe:/o:freebsd:freebsd:7.0:beta2 cpe:/o:freebsd:freebsd:8.2 cpe:/o:freebsd:freebsd:9.1
Too many fingerprints match this host to give specific OS details
Network Distance: 1 hop

TRACEROUTE
HOP RTT     ADDRESS
1   9.45 ms 192.168.0.116

OS and Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 7.64 seconds

----------------------------------------------------------------------------------------------------
fullname@pop-os:~/tmp/oulavie/libx/net-scan$ sudo nmap -O 192.168.0.190
Starting Nmap 7.91 ( https://nmap.org ) at 2021-02-13 20:10 EST
Nmap scan report for 192.168.0.190
Host is up (0.0047s latency).
Not shown: 992 closed ports
PORT     STATE SERVICE
80/tcp   open  http
139/tcp  open  netbios-ssn
443/tcp  open  https
445/tcp  open  microsoft-ds
631/tcp  open  ipp
8080/tcp open  http-proxy
9100/tcp open  jetdirect
9220/tcp open  unknown
MAC Address: 50:65:F3:BF:57:54 (Hewlett Packard)
No exact OS matches for host (If you know what OS is running on it, see https://nmap.org/submit/ ).
TCP/IP fingerprint:
OS:SCAN(V=7.91%E=4%D=2/13%OT=80%CT=1%CU=38302%PV=Y%DS=1%DC=D%G=Y%M=5065F3%T
OS:M=602878A9%P=x86_64-pc-linux-gnu)SEQ(SP=107%GCD=1%ISR=107%II=I%TS=A)OPS(
OS:O1=M5B4NW0NNT11%O2=M578NW0NNSNNT11%O3=M280NW0NNT11%O4=M5B4NW0NNSNNT11%O5
OS:=M218NW0NNSNNT11%O6=M109NNSNNT11)WIN(W1=21F0%W2=2088%W3=2258%W4=21F0%W5=
OS:20C0%W6=209D)ECN(R=Y%DF=N%T=40%W=2238%O=M5B4NW0NNS%CC=N%Q=)T1(R=Y%DF=N%T
OS:=40%S=O%A=S+%F=AS%RD=0%Q=)T2(R=N)T3(R=N)T4(R=Y%DF=N%T=40%W=0%S=A%A=Z%F=R
OS:%O=%RD=0%Q=)T5(R=Y%DF=N%T=40%W=0%S=Z%A=S+%F=AR%O=%RD=0%Q=)T6(R=Y%DF=N%T=
OS:40%W=0%S=A%A=Z%F=R%O=%RD=0%Q=)T7(R=Y%DF=N%T=40%W=0%S=Z%A=S+%F=AR%O=%RD=0
OS:%Q=)U1(R=Y%DF=N%T=FF%IPL=38%UN=0%RIPL=G%RID=G%RIPCK=G%RUCK=G%RUD=G)IE(R=
OS:Y%DFI=N%T=FF%CD=S)

Network Distance: 1 hop

OS detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 53.19 seconds

----------------------------------------------------------------------------------------------------
fullname@pop-os:~/tmp/oulavie/libx/net-scan$ sudo nmap -A 192.168.0.190
Starting Nmap 7.91 ( https://nmap.org ) at 2021-02-13 20:06 EST
Nmap scan report for 192.168.0.190
Host is up (0.0040s latency).
Not shown: 992 closed ports
PORT     STATE SERVICE      VERSION
80/tcp   open  http         HP ENVY 5640 series printer http config (Serial TH5336W0QW05ZC)
| http-methods: 
|_  Potentially risky methods: PUT DELETE
| http-robots.txt: 1 disallowed entry 
|_/
|_http-server-header: HP HTTP Server; HP ENVY 5640 series - B9S56A; Serial Number: TH5336W0QW05ZC; Built:Fri Jun 05, 2020 11:49:11AM {NLM5CN2023AR}
|_http-title: Site doesn't have a title (text/html).
139/tcp  open  tcpwrapped
443/tcp  open  ssl/http     HP ENVY 5640 series printer http config (Serial TH5336W0QW05ZC)
| http-robots.txt: 1 disallowed entry 
|_/
|_http-server-header: HP HTTP Server; HP ENVY 5640 series - B9S56A; Serial Number: TH5336W0QW05ZC; Built:Fri Jun 05, 2020 11:49:11AM {NLM5CN2023AR}
|_http-title: Site doesn't have a title (text/html).
| ssl-cert: Subject: commonName=HPBF5754/organizationName=HP/stateOrProvinceName=Washington/countryName=US
| Not valid before: 2014-09-10T16:42:22
|_Not valid after:  2034-09-05T16:42:22
445/tcp  open  microsoft-ds HP Officejet Pro 8600 printer smbd
631/tcp  open  http         HP ENVY 5640 series printer http config (Serial TH5336W0QW05ZC)
| http-methods: 
|_  Potentially risky methods: PUT DELETE
|_http-server-header: HP HTTP Server; HP ENVY 5640 series - B9S56A; Serial Number: TH5336W0QW05ZC; Built:Fri Jun 05, 2020 11:49:11AM {NLM5CN2023AR}
8080/tcp open  http         HP ENVY 5640 series printer http config (Serial TH5336W0QW05ZC)
| http-methods: 
|_  Potentially risky methods: PUT DELETE
| http-robots.txt: 1 disallowed entry 
|_/
|_http-server-header: HP HTTP Server; HP ENVY 5640 series - B9S56A; Serial Number: TH5336W0QW05ZC; Built:Fri Jun 05, 2020 11:49:11AM {NLM5CN2023AR}
|_http-title: Site doesn't have a title (text/html).
9100/tcp open  jetdirect?
9220/tcp open  hp-gsg       HP Generic Scan Gateway 1.0
MAC Address: 50:65:F3:BF:57:54 (Hewlett Packard)
No exact OS matches for host (If you know what OS is running on it, see https://nmap.org/submit/ ).
TCP/IP fingerprint:
OS:SCAN(V=7.91%E=4%D=2/13%OT=80%CT=1%CU=39599%PV=Y%DS=1%DC=D%G=Y%M=5065F3%T
OS:M=602877ED%P=x86_64-pc-linux-gnu)SEQ(SP=104%GCD=1%ISR=10F%II=I%TS=A)SEQ(
OS:SP=107%GCD=1%ISR=10B%CI=I%II=I%TS=A)SEQ(SP=106%GCD=1%ISR=105%TS=A)OPS(O1
OS:=M5B4NW0NNT11%O2=M578NW0NNSNNT11%O3=M280NW0NNT11%O4=M5B4NW0NNSNNT11%O5=M
OS:218NW0NNSNNT11%O6=M109NNSNNT11)WIN(W1=21F0%W2=2088%W3=2258%W4=21F0%W5=20
OS:C0%W6=209D)ECN(R=Y%DF=N%T=40%W=2238%O=M5B4NW0NNS%CC=N%Q=)T1(R=Y%DF=N%T=4
OS:0%S=O%A=S+%F=AS%RD=0%Q=)T2(R=N)T3(R=N)T4(R=Y%DF=N%T=40%W=0%S=A%A=Z%F=R%O
OS:=%RD=0%Q=)T5(R=Y%DF=N%T=40%W=0%S=Z%A=S+%F=AR%O=%RD=0%Q=)T6(R=Y%DF=N%T=40
OS:%W=0%S=A%A=Z%F=R%O=%RD=0%Q=)T7(R=Y%DF=N%T=40%W=0%S=Z%A=S+%F=AR%O=%RD=0%Q
OS:=)U1(R=Y%DF=N%T=FF%IPL=38%UN=0%RIPL=G%RID=G%RIPCK=G%RUCK=G%RUD=G)IE(R=Y%
OS:DFI=N%T=FF%CD=S)

Network Distance: 1 hop
Service Info: Device: printer; CPE: cpe:/h:hp:envy_5640_series, cpe:/h:hp:officejet_pro_8600

Host script results:
|_nbstat: NetBIOS name: HP5065F3BF5754, NetBIOS user: <unknown>, NetBIOS MAC: <unknown> (unknown)
|_smb2-time: Protocol negotiation failed (SMB2)

TRACEROUTE
HOP RTT     ADDRESS
1   4.03 ms 192.168.0.190

OS and Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 117.16 seconds

----------------------------------------------------------------------------------------------------
-sS (TCP SYN scan)
-sT (TCP connect scan)
-sU (UDP scans) 
-sY (SCTP INIT scan)
-sN; -sF; -sX (TCP NULL, FIN, and Xmas scans) 
-sA (TCP ACK scan)
-sW (TCP Window scan) 
-sM (TCP Maimon scan)
-sO (IP protocol scan) 
----------------------------------------------------------------------------------------------------
fullname@pop-os:~/tmp/oulavie/libx/net-scan$ sudo nmap -sS 192.168.0.1
Starting Nmap 7.91 ( https://nmap.org ) at 2021-02-14 09:52 EST
Nmap scan report for EMG2926 (192.168.0.1)
Host is up (0.0042s latency).
Not shown: 992 closed ports
PORT      STATE    SERVICE
53/tcp    open     domain
80/tcp    open     http
443/tcp   open     https
2002/tcp  filtered globe
5000/tcp  open     upnp
9100/tcp  open     jetdirect
49152/tcp open     unknown
49153/tcp open     unknown
MAC Address: B8:EC:A3:CF:69:C6 (Zyxel Communications)

Nmap done: 1 IP address (1 host up) scanned in 1.58 seconds




