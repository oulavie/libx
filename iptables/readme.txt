
http://tinyproxy.github.io/
https://nxnjz.net/2019/10/how-to-setup-a-simple-proxy-server-with-tinyproxy-debian-10-buster/

https://danielmiessler.com/study/iptables/
https://www.linode.com/docs/guides/control-network-traffic-with-iptables/
https://www.digitalocean.com/community/tutorials/iptables-essentials-common-firewall-rules-and-commands
https://web.mit.edu/rhel-doc/4/RH-DOCS/rhel-rg-en-4/s1-iptables-options.html
https://www.baeldung.com/linux/iptables-intro
https://www.tecmint.com/basic-guide-on-iptables-linux-firewall-tips-commands/
https://nxnjz.net/2019/10/how-to-setup-a-simple-proxy-server-with-tinyproxy-debian-10-buster/
https://en.wikipedia.org/wiki/Iptables
https://www.redhat.com/sysadmin/iptables

git.netfilter.org/iptables

$ sudo apt-get install iptables

$ man iptables

$ sudo iptables-save > ~/iptables.txt
$ sudo iptables-restore < ~/iptables.txt

$ iptables -L -v
$ sudo ip6tables -L -nv
$ sudo iptables -L --line-numbers
$ sudo iptables -L
The line numbers are important when you attempt to delete a rule.

Issue the following command to remove the second rule:
$ sudo iptables -D INPUT 2

Adding the -v option will give you packet and byte information, and
adding -n will list everything numerically. In other words – hostnames, protocols, and networks are listed as numbers.

$ iptables -L -v -n
$ iptables -t filter -L -v -n
$ iptables -t nat -L -v -n

$ sudo iptables -A INPUT -j DROP
This entry appends the DENY ALL rule to the bottom of whatever list of rules already exists.
Remember, iptables rules are read from the top down, so you always want the DENY ALL rule to be the last one on the list. If this rule falls anywhere else in the list, nothing below it will process.


There are two ways that I add iptables rules.
One is with append (-A), but I only use that one time per system.
The second is insert (-I), which is the way I add all other rules to a system.
My explanation is that append puts the rule at the bottom of the list, and there's only one rule I want at the very bottom: the classic DENY ALL. It's simple to add.

$ sudo iptables -A INPUT -j DROP
$ iptables -I INPUT -s 192.168.1.0/24 -p tcp --dport 22 -j ACCEPT
$ iptables -A INPUT -p tcp --dport ssh -s 10.10.10.10 -m state --state NEW,ESTABLISHED -j ACCEPT
$ iptables -A OUTPUT -p tcp --sport 22 -d 10.10.10.10 -m state --state ESTABLISHED -j ACCEPT

-A will add or append the rule to the end of the chain.

To clear all the currently configured rules, you can issue the flush command.
$ iptables -F

ufw                     iptables
sudo ufw allow ssh      sudo iptables -A INPUT -p tcp --dport 22 -j ACCEPT
sudo ufw allow http     sudo iptables -A INPUT -p tcp --dport 80 -j ACCEPT
sudo ufw allow https    sudo iptables -A INPUT -p tcp --dport 443 -j ACCEPT
sudo ufw allow 25       sudo iptables -A INPUT -p tcp --dport 25 -j ACCEPT

#!/bin/bash
iptables -A OUTPUT -p tcp -d bigmart.com -j ACCEPT
iptables -A OUTPUT -p tcp -d bigmart-data.com -j ACCEPT
iptables -A OUTPUT -p tcp -d ubuntu.com -j ACCEPT
iptables -A OUTPUT -p tcp -d ca.archive.ubuntu.com -j ACCEPT
iptables -A OUTPUT -p tcp --dport 80 -j DROP
iptables -A OUTPUT -p tcp --dport 443 -j DROP
iptables -A INPUT -p tcp -s 10.0.3.1 --dport 22 -j ACCEPT
iptables -A INPUT -p tcp -s 0.0.0.0/0 --dport 22 -j DROP

*filter

# Allows all loopback (lo0) traffic and drop all traffic to 127/8 that doesn't use lo0
-A INPUT -i lo -j ACCEPT
-A INPUT ! -i lo -d 127.0.0.0/8 -j REJECT

# Accepts all established inbound connections
-A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

# Allows all outbound traffic
# You could modify this to only allow certain traffic
-A OUTPUT -j ACCEPT

# Allows HTTP and HTTPS connections from anywhere (the normal ports for websites)
-A INPUT -p tcp --dport 80 -j ACCEPT
-A INPUT -p tcp --dport 443 -j ACCEPT

# Allows SSH connections
# The --dport number is the same as in /etc/ssh/sshd_config
-A INPUT -p tcp -m state --state NEW --dport 22 -j ACCEPT

# Now you should read up on iptables rules and consider whether ssh access
# for everyone is really desired. Most likely you will only allow access from certain IPs.

# Allow ping
#  note that blocking other types of icmp packets is considered a bad idea by some
#  remove -m icmp --icmp-type 8 from this line to allow all kinds of icmp:
#  https://security.stackexchange.com/questions/22711
-A INPUT -p icmp -m icmp --icmp-type 8 -j ACCEPT

# log iptables denied calls (access via 'dmesg' command)
-A INPUT -m limit --limit 5/min -j LOG --log-prefix "iptables denied: " --log-level 7

# Reject all other inbound - default deny unless explicitly allowed policy:
-A INPUT -j REJECT
-A FORWARD -j REJECT

COMMIT

At present, there are total four chains:

    INPUT : Default chain originating to system.
    OUTPUT : Default chain generating from system.
    FORWARD : Default chain packets are send through another interface.
    RH-Firewall-1-INPUT : The user-defined custom chain.

There are at present three tables.

    Filter
    NAT
    Mangle

The iptables commands are as follows:

 -A — Appends the iptables rule to the end of the specified chain. This is the command used to add a rule when rule order in the chain does not matter.

 -C — Checks a particular rule before adding it to the user-specified chain. This command can help you construct complicated iptables rules by prompting you for additional parameters and options.

 -D — Deletes a rule in a particular chain by number (such as 5 for the fifth rule in a chain). You can also type the entire rule, and iptables deletes the rule in the chain that matches it.

 -E — Renames a user-defined chain. This does not affect the structure of the table.

 -F — Flushes the selected chain, which effectively deletes every rule in the the chain. If no chain is specified, this command flushes every rule from every chain.

 -h — Provides a list of command structures, as well as a quick summary of command parameters and options.

 -I — Inserts a rule in a chain at a point specified by a user-defined integer value. If no number is specified, iptables places the command at the top of the chain.






