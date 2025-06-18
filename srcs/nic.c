#include "ft_malcolm.h"

static int check_nic_exists(char *name) {
	struct ifaddrs* ifs;
	
	getifaddrs(&ifs);
	if (!ifs) {
		prepare_for_exit(ENOMEM, "Unable to create struct to locate NIC...Exiting...");
		return (0);
	}
	global_info->ifsPtr = ifs;
	struct ifaddrs* current = ifs;

	while (current != NULL) {
	   if (ft_strcmp(current->ifa_name, name) == 0) {
			printf("Interface with name '%s' located\n", name);
			freeifaddrs(ifs);
			global_info->ifsPtr = NULL;
			return (1);
		}
		current = current->ifa_next;
	}
	if (!global_info->stopProgram)
		printf("[WARNING] Unable to locate interface with name '%s'\n", name);
	freeifaddrs(ifs);
	global_info->ifsPtr = NULL;
	return (0);
}

int assign_custom_interface(t_info* info, char* name) {
	info->nicName = ft_calloc(sizeof(char), ft_strlen(name) + 1);
	if (!info->nicName) {
		prepare_for_exit(ENOMEM, "Unable to allocate memory for custom interface name..." );
		return 0;
	}
	ft_memcpy(info->nicName, name, ft_strlen(name));
	info->nicName[ft_strlen(name)] = '\0';

	return(check_nic_exists(name));
}

static int subnet_match(int loc, char* ip, char* target) {
	int i = 0;
  
	while (ip[i] && loc > 0) {
		if (ip[i] == '.')
			loc--;
		if (loc == 0)
			break;
		i++;
	}
	if (ft_strncmp_modified(target, ip, i) == 0) 
		return (1);
	return (0);
}

// Matching logic only valid for CIDRs 8/16/24/32
static int get_ip_dot_location(int cidr) {
 
	if (8 % cidr == 0 || 8 % cidr < 8 )
		return 1;
	else if (16 % cidr == 0 || (16 % cidr > 8  && 16 % cidr < 16))
		return 2;
	else if (24 % cidr == 0 || (24 % cidr > 16  && 24 % cidr < 24))
		return 3;
	else
		return 4;
}

static int check_subnet(t_info* info, char* nic_name, char* ip, int cidr) {

	int ip_dot_location = get_ip_dot_location(cidr);

	if (subnet_match(ip_dot_location, ip, info->targetIP)) {
		info->nicName = ft_calloc(sizeof(char), ft_strlen(nic_name) + 1);
		if (!info->nicName) {
			prepare_for_exit(ENOMEM, "Unable to allocate memory for custom interface name..." );
			return 0;
		}
		ft_memcpy(info->nicName, nic_name, ft_strlen(nic_name));
		info->nicName[ft_strlen(nic_name)] = '\0';
		info->nicMask = cidr;
		return (1);
	}
	return (0);
}

void assign_nic_check_subnet(t_info* info, int backup) {
	if (info->stopProgram)
		return ;
	struct ifaddrs* ifs;
	
	getifaddrs(&ifs);
	if (!ifs) {
		prepare_for_exit(ENOMEM, "Unable to create struct to locate NIC...Exiting!...");
		return ;
	}
	global_info->ifsPtr = ifs;
	struct ifaddrs* current = ifs;

	while (current != NULL) {
		if (info->stopProgram)
			return ;
  
	   if (current->ifa_addr->sa_family == AF_INET) {
			// Need to cast values into sockaddr_in struct to read sin address of each. Otherwise sin_addr field doesnt exist
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)current->ifa_addr;
			struct sockaddr_in *mask = (struct sockaddr_in *)current->ifa_netmask;
			char ip[INET_ADDRSTRLEN];
			char netmask[INET_ADDRSTRLEN];
			int cidr_count = 0;
			// s_addr is big endian - puting in raw_mask without converting can cause issues on little endian machine if not converted!!
			uint32_t raw_mask = mask->sin_addr.s_addr;

			// Func copies addresses from network format into string
			inet_ntop(AF_INET, &ipv4->sin_addr, ip, sizeof(ip));
			inet_ntop(AF_INET, &mask->sin_addr, netmask, sizeof(netmask));

			while (raw_mask) {
				cidr_count += raw_mask & 1;
				raw_mask >>= 1;
			}

			if (info->verboseFlag)
				printf("Checking interface '%s' for suitability...\n", current->ifa_name);
			if (backup && ft_strcmp(current->ifa_name,"lo") != 0)  {
				info->nicName = ft_calloc(sizeof(char), ft_strlen(current->ifa_name) + 1);
				if (!info->nicName) {
					prepare_for_exit(ENOMEM, "Unable to allocate memory for custom interface name..." );
					freeifaddrs(ifs);
					global_info->ifsPtr = NULL;
					return ;
				}
				ft_memcpy(info->nicName, current->ifa_name, ft_strlen(current->ifa_name));
				info->nicName[ft_strlen(current->ifa_name)] = '\0';
			}
			if (check_subnet(info, current->ifa_name, ip, cidr_count)) {
			   printf("Potential matching subnet located on interface %s \n", info->nicName);
			   break ;
			}
		}
		current = current->ifa_next;
	}
	freeifaddrs(ifs);
	global_info->ifsPtr = NULL;
}