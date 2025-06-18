#include "ft_malcolm.h"

static void assign_ips(char** ipaddr, t_info* info) {
	size_t seg = 0; 
	for (size_t i = 0; i < ft_strlen(ipaddr[1]); i++) {
		if (ipaddr[1][i] == '.') 
			seg++;
		else
			info->source_ip[seg] = info->source_ip[seg] * 10 + (ipaddr[1][i] - 48);
	}
	seg = 0;
	for (size_t i = 0; i < ft_strlen(ipaddr[3]); i++) {
		if (ipaddr[3][i] == '.') 
			seg++;
		else
			info->target_ip[seg] = info->target_ip[seg] * 10 + (ipaddr[3][i] - 48);
	}
}


static unsigned char hex_to_unsignedchar(char *hex) {
	unsigned char result = 0;

	for (int i = 0; i < 2; i++) {
		char c = hex[i];
		unsigned char value = 0;
		if (c >= '0' && c <= '9') {
			value = c - '0';
		} else if (c >= 'A' && c <= 'F') {
			value = c - 'A' + 10;
		} else if (c >= 'a' && c <= 'f') {
			value = c - 'a' + 10;
		}
		result = (result << 4) | value;
	
	}
	return result;
}


static void assign_macs(char** mac, t_info* info) {
	size_t seg = 0;
	char * numstr = NULL;
	for (size_t i = 0; i < ft_strlen(mac[2]); i+=3) {
	   numstr = ft_calloc(3, sizeof(char));
	   	if (!numstr) {
			prepare_for_exit(ENOMEM, "Unable to allocate memory for to assign source mac addresses..." );
			return ;
		}
		ft_memcpy(numstr, &(mac[2][i]), 2);
		numstr[2] = '\0';
		info->source_mac[seg] = hex_to_unsignedchar(numstr);
		free(numstr);
		seg++;
	}
	seg = 0;

	for (size_t i = 0; i < ft_strlen(mac[4]); i+=3) {
		numstr = ft_calloc(3, sizeof(char));
		if (!numstr) {
			prepare_for_exit(ENOMEM, "Unable to allocate memory for to assign target mac addresses..." );
			return ;
		}
		ft_memcpy(numstr, &(mac[4][i]), 2);
		numstr[2] = '\0';
		if (!ft_strcmp(numstr,"**")) {
			info->target_mac[seg] = hex_to_unsignedchar("2a");
			info->targetMaskedMac = 1;
		}
		else
			info->target_mac[seg] = hex_to_unsignedchar(numstr);
		free(numstr);
		seg++;
	}
}

void print_info(t_info* info) {
	printf("[INPUT INFO]\n");
	printf("Source IP  = %s\n", info->sourceIP);
	printf("Source MAC = %s\n", info->sourceMac);
	printf("Target IP  = %s\n", info->targetIP);
	printf("Target MAC = %s\n", info->targetMac);

	printf("[SOURCE INFO CRAFTED FOR PACKET]\n");
	printf("IP  : ");
	for (int i = 0; i < 3; i++)
		 printf("%d.", info->source_ip[i]);
	printf("%d\n", info->source_ip[3]);
	printf("MAC : ");
	for (int i = 0; i < 5; i++)
		 printf("%02x:", info->source_mac[i]);
	printf("%02x", info->source_mac[5]);
	printf("\n");

	printf("[TARGET INFO CRAFTED FOR PACKET]\n");
	printf("IP  : ");
	for (int i = 0; i < 3; i++)
		 printf("%d.", info->target_ip[i]);
	printf("%d\n", info->target_ip[3]);
	printf("MAC : ");
	for (int i = 0; i < 5; i++) {
		if (info->target_mac[i] != 42)
			printf("%02x:", info->target_mac[i]);
		else
		 	printf("**:");

	}
	if (info->target_mac[5] != 42)
			printf("%02x\n", info->target_mac[5]);
		else
		 	printf("**\n");
	
	printf("Verbose mode - ON\n");
	if (info->targetFlag)
		 printf("Target mode - ON\n");
	else
		 printf("Target mode - OFF\n");
	if (info->broadcastFlag)
		 printf("Broadcast mode - ON\n");
	else
		 printf("Broadcast mode - OFF\n");
	if (info->nicName)
		 printf("Interface selected - '%s'\n", info->nicName);
	else
		 printf("Interface not specified - autoselecting...\n");

}

void parse_data(char** argv, t_info* info) {
	if (global_info ->stopProgram)
		return ;
	info->sourceIP = argv[1];
	info->sourceMac = argv[2];
	info->targetIP = argv[3];
	info->targetMac = argv[4];
	assign_ips(argv, info);
	assign_macs(argv, info);
}