# ft_malcolm
ARP Poisoning Software as part of Ecole 42's outer circle project.

# Description
This ARP poisoning software in default mode listens on a socket for ARP requests. When a request is received it is assessed to see if the target IP and MAC addresses match. Then it checks to see that the IP (i.e the IP address the target is looking for) matches the Source IP field set when the program is launched. If a match is found, the program sends an ARP response back to the target and exits.

# Installation
After cloning repository, navigate to directory and type
``` make ``` to build
``` make re ``` to rebuild
``` make clean ``` to remove unneccessary object files after build
``` make fclean ``` to remove object files and binary

# Usage
Must be launched as root or with sudo permissions
``` sudo ./ft_malcolm [Source IP] [Source MAC] [Target IP] [Target MAC] [Options] ```

Source IP - IP Address you want to spoof
Source MAC - MAC address you want the target to associate with the spoofed IP
Target IP - IP Address of target device on network
Target MAC - MAC address of target device on network

NOTE - The [Target MAC] accepts full or partial wildcard (*). If you do not know the mac address of the target you can set as ``` **:**:**:**:**:** ``` or if you know partial you can set wildcard for unknown fields, eg ``` aa:bb:**:dd:**:ff ```
The wildcard does not work if you use the ```-target``` option.

# Options
The program can be run with the following options:

```-verbose``` - Prints detailed information including packet information of sent payload
```-target``` - Actively sends a crafted ARP packet to the target IP instead of waiting and listening for the target to send a request.
```-broadcast``` - Sends a broadcast ARP packet to all devices on the network notifying them of the spoofed IP and MAC
```-interface <interface name>``` - Manually sets the interface to listen and respond from. Must be a valid network interface for it to work.

# Tips!
Linux by default has security features that prevent unsolicited ARP responses from being cached. If the following command is typed into a terminal on the target and the output is '0' then the security feature is active and the target must send a request in order to process the response. i.e using -target and -broadcast options will not work.
`sudo sysctl net.ipv4.conf.all.arp_accept `
Note - by setting `sudo sysctl net.ipv4.conf.all.arp_accept=1` you are allowing your device to accept unsolicited ARP responses!

If you are trying to spoof an existing IP on a network then it is likely the response from the existing device is sent before this program sends the response. The linux OS will process the packet which arrives first and ignores the packet which arrives second (eg this program). This is part of the above mentioned security feature of ignoring unsolicited responses.

You will not be able to poison yourself! This is as a result of RFC 5227 IP Conflict Detection.

If you want to verify the payload printing is correct as part of verbose mode you can run tcp dump while running ft_malcolm
```sudo tcpdump -i <your_interface_name> -s 0 -xx arp```

# Demonstration

[TARGET MACHINE]




# sysctl net.ipv4.conf.all.arp_accept=1

# ip link set dev eth0 arp off
#
# sudo sysctl net.ipv4.conf.all.arp_ignore=1


sudo tcpdump -i <your_interface_name> -s 0 -xx arp