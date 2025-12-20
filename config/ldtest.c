#include <stdio.h>
#include <unistd.h>

#if defined(__GNUC__) && defined(__ELF__)
#define __page(x)               __attribute__ (( __section__ (x) ))
#else
#define __page(x)               /* nothing */
#endif

#define	S(x)	x,sizeof(x)

#if 0
    *(.text.e)  /* RARE */
    *(.text.d)  /* INIT */ main
    *(.text.b)  /* CFG1 */ func2
    *(.text.c)  /* CMD1 */ func1
    *(.text.a)  /* CORE */
    *(.text.f)  /* DBUG */
#endif

__page(".text.e")
int function1(int a)
{
	return a + 1;
}

__page(".text.c")
int function2(int a)
{
	return a + 2;
}

__page(".text.a")
int main(int argc, char **argv)
{
#if defined(__GNUC__) && defined(__ELF__)
	if (((void*)main < (void*)function2) && ((void*)function2 < (void*)function1))
		write(1,S("yes\n"));
	else
		write(1,S("no\n"));
#else
	write(1,S("no (non ELF/non GNU)\n"));
#endif
	return(0);
}
