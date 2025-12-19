#include <sys/prctl.h>

int main(int argc, char **argv)
{
	int r = prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
	return r;
}
