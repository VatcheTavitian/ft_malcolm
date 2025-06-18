#include "ft_malcolm.h"


static int ip_range_check(char* ipaddr) {
	size_t i = 0, digits = 0, num = 0, seg = 0;
	while (ipaddr[i] && i < ft_strlen(ipaddr)) {
		if ((i == 0 && ipaddr[i] == '.' ) || (i == ft_strlen(ipaddr) - 1 && ipaddr[i] == '.'))
			return (1);
		if (ipaddr[i] != '.')
			num = num * 10 + (ipaddr[i] - 48);
		if (ipaddr[i] == '.' || i == ft_strlen(ipaddr) - 1) {
			seg++;
			if (num > 255)
				return (1);
			if (num < 1 && ((seg == 1 )))
				return (1);
			digits = 0;
			num = 0;
		}
		else
			digits++;
		if (digits > 3)
			return (1);
		i++;
	}
	return (0);
}

static int leading_zero_check(char* ipaddr) {
	for (size_t i = 0; i < ft_strlen(ipaddr); i++) {
		if (i == 0 && ipaddr[i] == '0') {
			return (1);
		}
		if (ipaddr[i] == '.' && (ipaddr[i + 1] == '0')) {
			if (i < ft_strlen(ipaddr) - 1) {
				if (ipaddr[i + 2] != '.' && ipaddr[i + 2] != '\0') {
					return (1);
				}
			}
		}
	}
	return (0);
}

static int ip_dot_check(char* ipaddr) {
	size_t i = 0, dot = 0;
	while (ipaddr[i] && i < ft_strlen(ipaddr)) {
		if (!ft_isdigit(ipaddr[i]) && ipaddr[i] != '.')
			return (1);
		if (ipaddr[i] == '.' && ipaddr[i + 1] == '.' )
			return (1);
		if (ipaddr[i] == '.')
			dot++;
		i++;
	}
	if (dot != 3)
		return (1);
	return (0);
}

static int mac_colon_check(char* mac) {
	size_t i = 0, colon = 0;
	while (mac[i] && i < ft_strlen(mac)) {
		if (mac[i] == ':') {
			if (i % 3 != 2)
				return (1);
			colon++;
		}
		i++;
	}
	if (colon != 5)
		return (1);
	return (0);
}

static int mac_hex_check(char* mac) {
	for (size_t i = 0; i < ft_strlen(mac); i++) {
		if (mac[i] != ':') {
			if (!ft_isalnum_modified(mac[i]))
				return(1);
		}
	}
	return (0);
}


static void validate_ip(char* ipaddr) {
	char * msgIfError = NULL;
	if (!msgIfError && (ft_strlen(ipaddr) < 7 || ft_strlen(ipaddr) >= 16))
		msgIfError = "IPv4 Address incorrect format - Check IP";
	if (!msgIfError && leading_zero_check(ipaddr))
		msgIfError = "IPv4 Address incorrect format - Check IP leading zeros and dot placement";
	if (!msgIfError && ip_dot_check(ipaddr))
		msgIfError = "IPv4 Address incorrect format - Must be format 'XXX.XXX.XXX.XXX'";
	if (!msgIfError && ip_range_check(ipaddr))
		msgIfError = "IPv4 Address incorrect format - Must be format 'XXX.XXX.XXX.XXX' and in valid range";
	if (msgIfError) {
		printf("Invalid IP address - [%s]\n", ipaddr);
		prepare_for_exit(EINVAL, msgIfError);  
	}
}

static void validate_mac(char* mac) {
	char * msgIfError = NULL;
	if (!msgIfError && ft_strlen(mac) != 17)
		msgIfError =  "MAC Address incorrect format - Check MAC length";
	if (!msgIfError && mac_colon_check(mac))
		msgIfError = "MAC Address incorrect format - Must be format 'XX:XX:XX:XX:XX:XX'";
	if (!msgIfError && mac_hex_check(mac))
		msgIfError =  "MAC Address incorrect format - Must be valid hex range'";
	if (msgIfError) {
		printf("Invalid MAC address - [%s]\n", mac);
		prepare_for_exit(EINVAL, msgIfError);  
	}

}

static void validate_wildcard(char* mac) {
	for (size_t i = 0; i < ft_strlen(mac); i+=3) {
		if ((mac[i] == '*' && mac[i + 1] != '*') || (mac[i] != '*' && mac[i + 1] == '*'))
		 	prepare_for_exit(EINVAL,"Target MAC Address wildcard format incorrect\nYou can only wildcard entire byte, not partial...\neg. CORRECT [aa:bb:**:dd:**:ff] INCORRECT [aa:b*:cc:dd:*e:ff]");
	}
}

void validate(char** argv) {
	if (ft_strchr(argv[2], '*')) {
		 prepare_for_exit(EINVAL,"Source MAC Address incorrect format - cannot contain wildcard '*'");
	}
	validate_ip(argv[1]);
	validate_ip(argv[3]);
	validate_mac(argv[2]);
	validate_mac(argv[4]);
	if (!global_info->stopProgram && ft_strchr(argv[4], '*'))
		validate_wildcard(argv[4]);
}

