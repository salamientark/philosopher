#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int	ft_strncmp(char *s1, char *s2, unsigned int n)
{
	unsigned int	cnt;

	cnt = 0;
	while (cnt < n)
	{
		if (s1[cnt] != s2[cnt])
			return (s1[cnt] - s1[cnt]);
		cnt++;
	}
	return (0);
}

int	main(int ac, char **av)
{
	(void) ac;
	char			*s1;
	char			*s2;
	unsigned int	n;

	s1 = av[1];
	s2 = av[2];
	n = atoi(av[3]);
	printf("ft_strncmp(%s, %s, %d) = %d\n", s1, s2, n, ft_strncmp(s1, s2, n));
	s1 = NULL;
	printf("strncmp(%s, %s, %d) = %d\n", s1, s2, n, strncmp(s1, s2, n));
	return (0);
}
