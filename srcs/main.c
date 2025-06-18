#include "ft_malcolm.h"

t_info *global_info = NULL; 


static void    options_initializer(int argc, char** argv, t_info* info) {
	for (int index = 5; index < argc; index++) {
		if (global_info->stopProgram)
			return ;
		if (!info->stopProgram && ft_strcmp(argv[index], "-verbose") == 0)
			info->verboseFlag = 1;
		else if (!info->stopProgram && ft_strcmp(argv[index], "-broadcast") == 0)
			info->broadcastFlag = 1;
		else if (!info->stopProgram && ft_strcmp(argv[index], "-target") == 0)
			info->targetFlag = 1;
		else if (ft_strcmp(argv[index], "-interface") == 0) {
			if (index == argc - 1) {
				prepare_for_exit(EOPNOTSUPP, "You must specify interface name argument: -interface <interface name>");
				return ;
			}
			else {
				index++;
				info->interfaceFlag = 1;
				if (!assign_custom_interface(info, argv[index]))
					prepare_for_exit(EOPNOTSUPP, "Specify valid interface name and try again");
			}
		}
		else {
			char * prefix = "Option argument not recognized :";
			char * errorMsg = ft_calloc(sizeof(char), ft_strlen(argv[index]) + ft_strlen(prefix) + 1);
			if (!errorMsg)
				prepare_for_exit(ENOMEM, "Unable to allocate memory for error message..." );
			else {
				ft_memcpy(errorMsg, prefix,ft_strlen(prefix));
				ft_memcpy(errorMsg + ft_strlen(prefix),argv[index], ft_strlen(argv[index]));
				errorMsg[ft_strlen(argv[index]) + ft_strlen(prefix)] = '\0';
				info->error_msg = errorMsg;
				prepare_for_exit(EOPNOTSUPP, errorMsg );
			}
		}
	}
}

static t_info *info_initializer() {
	t_info* info = ft_calloc(sizeof(t_info), 1);
	if (!info) 
		return NULL ;

	ft_memset(info->source_ip,0, sizeof(info->source_ip));
	ft_memset(info->source_mac,0, sizeof(info->source_mac));
	ft_memset(info->target_ip,0, sizeof(info->target_ip));
	ft_memset(info->target_mac,0, sizeof(info->target_mac));

	info->verboseFlag = 0;
	info->broadcastFlag = 0;
	info->targetFlag = 0;
	info->targetMaskedMac = 0;
	info->stopProgram = 0;
	info->interfaceFlag = 0;
	info->nicName = NULL;
	info->error_msg = NULL;
	info->errorInt = 0;
	info->ifsPtr = NULL;
   
	return info;
}

int free_everything() {
	int errorCode = 0;
	if (global_info) {
		if (global_info->nicName)
			free(global_info->nicName);
		if (global_info->error_msg)
			free(global_info->error_msg);
		if (global_info->errorInt)
			errorCode = global_info->errorInt;
		if (global_info->ifsPtr)
			freeifaddrs(global_info->ifsPtr);
		free(global_info);
	}
	return errorCode;
}

static void handle_quit(int signal) {
	if (signal == SIGINT) {
		prepare_for_exit(ECANCELED,"Exiting program!!");
	}
}

void prepare_for_exit(int errint, char *msg) {
	errno = errint;
	if (errint == ECANCELED) {
		global_info->stopProgram = 1;
		global_info->errorInt = errint;
		printf("\rQuiting ft_malcolm!\n");
	}
	if (!global_info->stopProgram) {
		printf("%s", strerror(errint));
		global_info->stopProgram = 1;
		global_info->errorInt = errint;
		if (msg) {
			global_info->errorMsg = msg;
			printf(" - %s\n", global_info->errorMsg);
		}
		else
			printf("\n");
	}
}

int main(int argc, char** argv) {
	t_info* info = info_initializer();
	if (!info) {
		printf("Fatal Error!  %s - %s\n",strerror(ENOMEM), "Unable to create initialise information...");
		return ENOMEM;
	}

	global_info = info;
	if (getuid() != 0) {
		prepare_for_exit(EACCES, "You must launch program as root or sudo!");
		return (free_everything());
	} 
	if (argc < 5) {
		prepare_for_exit(EINVAL, "Format must be ./ft_malcolm [Source IP] [SourceMac] [TargetIP] [TargetMac] [Options]");
		return (free_everything());
	}

	struct sigaction sa;
	ft_memset(&sa, 0, sizeof(sa));
	
	sa.sa_handler = &handle_quit;
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);

	validate(argv);
	parse_data(argv, info);

	if (argc > 5 && !info->stopProgram)
		options_initializer(argc, argv, info);
	if (!info->stopProgram && info->targetFlag && ft_strchr(info->targetMac, '*'))
		prepare_for_exit(EOPNOTSUPP, "Cannot use wildcard target MAC if using -target option. You need the complete MAC address");
	if (!info->stopProgram && info->verboseFlag)
		print_info(info);
	if (!info->nicName)
		assign_nic_check_subnet(info, 0);
	if (!info->stopProgram && !info->nicName) {
		printf("[WARNING] Suitable network interface not found - allocating random...\n");
		assign_nic_check_subnet(info, 1);
		if (!info->nicName)
			prepare_for_exit(EBADFD, "Suitable network interface card not found!"); //fix this
		else {
			printf("[WARNING] Random network interface '%s' selected...\n", info->nicName);
			printf("[WARNING] If target is on subnet not accessible by NIC, ARP poisoning will not succeed...\n");
		}
	}
	if (!info->stopProgram)
		poison_arp(info);

	return (free_everything());
}

