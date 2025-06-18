#ifndef FT_MALCOLM_H
# define FT_MALCOLM_H


	# include <stdio.h>
	# include <unistd.h>
	# include <stdlib.h>
	# include <string.h>
	# include <errno.h>
	# include <stdint.h>
	# include <sys/socket.h>
	# include <netinet/ip.h>
	# include <linux/if_packet.h>
	# include <arpa/inet.h>
	# include <net/if.h>
	# include <netinet/if_ether.h>
	# include <netdb.h>
	# include <ifaddrs.h>
	# include <signal.h>

	typedef struct s_info {
		char*   sourceIP;
		char*   sourceMac;
		char*   targetIP;
		char*   targetMac;

		unsigned char source_ip[4];
		unsigned char source_mac[6];
		unsigned char target_ip[4];
		unsigned char target_mac[6];

		int     verboseFlag;
		int     broadcastFlag;
		int     targetFlag;
		int		interfaceFlag;

		int		targetMaskedMac;
		int		stopProgram;
		int		errorInt;
		char *	errorMsg;

		char*   nicName;
		int     nicMask;
		char * error_msg;
		struct ifaddrs* ifsPtr;
	}   t_info;

	struct arp_packet {
		struct arphdr header;
		unsigned char sender_mac[6];
		unsigned char sender_ip[4];
		unsigned char target_mac[6];
		unsigned char target_ip[4];
	};

	extern t_info *global_info;

	//main.c
	int		free_everything();
	void	prepare_for_exit(int errint, char *msg);

	// libft.c
	size_t	ft_strlen(const char *str);
	int     ft_isdigit(int c);
	int     ft_isalnum_modified(int c);
	void	*ft_calloc(size_t count, size_t size);
	void	*ft_memcpy(void *dst, const void *src, size_t n);
	void	*ft_memset(void *b, int c, size_t len);
	int     ft_strncmp_modified(const char *s1, const char *s2, size_t n);
	int     ft_strcmp(const char *s1, const char* s2);
	int		ft_strchr(const char *s, int c);

	// validate.c
	void    validate(char** argv);

	// parse.c
	void    parse_data(char** argv, t_info* info);
	void 	print_info(t_info* info);

	// poison.c
	void    poison_arp(t_info* info);

	// nic.c
	int		assign_custom_interface(t_info* info, char* name);
	void	assign_nic_check_subnet(t_info* info, int backup);


#endif