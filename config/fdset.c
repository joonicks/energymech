#include <stdio.h>
#include <sys/select.h>

int main(int argc, char **argv, char **envp)
{
	fd_set readfds;
	int sz;

	sz = sizeof(readfds);
	printf("%i\n",sz);
	return(0);
}
