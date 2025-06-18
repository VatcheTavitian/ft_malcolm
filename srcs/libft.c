#include "ft_malcolm.h"

void	*ft_calloc(size_t count, size_t size)
{
	unsigned char	*arr;
	size_t			i;

	i = 0;
	if (count == SIZE_MAX || size == SIZE_MAX)
		return (NULL);
	arr = malloc(count * size);
	if (!arr)
		return (NULL);
	while (i < count * size)
	{
		arr[i] = 0;
		i++;
	}
	return (arr);
}

size_t	ft_strlen(const char *str) {
	int		i;
	char	*str1;

	if (!str)
		return (0);
	str1 = (char *)str;
	i = 0;
	while (str1[i] != '\0')
		i++;
	return (i);
}

int	ft_isdigit(int c) {
	if (c >= '0' && c <= '9')
		return (2048);
	else
		return (0);
}

static int	ft_isalpha_modified(int c)
{
	if ((c >= 97 && c <= 102) || (c >= 65 && c <= 70))
		return (1);
	else
		return (0);
}


int	ft_isalnum_modified(int c) {
	if (ft_isdigit(c) || ft_isalpha_modified(c) || c == '*')
		return (1);
	else
		return (0);
}

void	*ft_memcpy(void *dst, const void *src, size_t n) {
	size_t		i;
	char		*s;
	char		*d;

	i = 0;
	s = (char *)src;
	d = (char *)dst;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (d);
}

void	*ft_memset(void *b, int c, size_t len) {
	size_t	i;

	i = 0;
	while (i < len)
	{
		((char *)b)[i] = c;
		i++;
	}
	return (b);
}

int	ft_strncmp_modified(const char *s1, const char *s2, size_t n) {
	unsigned int	i;

	i = 0;
	while ((s1[i] || s2[i]) && i < n)
	{	
		if (s1[i] != s2[i])
			return (((unsigned char *)s1)[i] - ((unsigned char *) s2)[i]);
		i++;
	}
	if ((s1[i] || s2[i]) && i > n)
		return 1;

	return (0);
}

int ft_strcmp(const char *s1, const char* s2) {
	size_t i = 0;
	if (!s1 || !s2)
		return (1);
	if (ft_strlen(s1) < ft_strlen(s2))
		return (-1);
	if (ft_strlen(s1) > ft_strlen(s2))
		return (1);
	
	for (; i < ft_strlen(s1); i++) {
		if (s1[i] != s2[i])
			return (1);
	}
	if (s1[i] != '\0' && s2[i] != '\0')
		return (1);
	return (0);
}

int	ft_strchr(const char *s, int c) {
	int		i;
	char	*s1;

	i = 0;
	s1 = (char *)s;
	while (s1[i])
	{
		if (s1[i] == (char)c)
			return (1);
		i++;
	}
	return (0);
}