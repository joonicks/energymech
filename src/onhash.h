/*
 *
 *  This file is included in ons.c right before on_msg
 *  It is also used in gencmd.c where its used with different
 *  random hashdata in order to find a hashmap with no
 *  collisions. Unfortunatly there is no way of handling
 *  hasmaps with collisions currently so if it gets stuck
 *  trying hashdata forever you are out of luck.
 *
 */
int mkhash(const char *cmd)
{
	const char *s;
	int h,t,ki,kk;
	uint8_t a,b;

	s = cmd;
	ki = 15 * strlen(cmd);
	kk = hashdata[ki & HASHDATAMASK];
	while(*s)
	{
		a = tolowertab[(uchar)s[0]];
		b = tolowertab[(uchar)s[1]];
		h = (a - b) & HASHDATAMASK;
		ki += a + hashdata[h];
#ifdef HEATMAP
		/* used when refining the algorithm */
		heatmap[ki & HASHDATAMASK] += 1;
#endif
		kk ^= hashdata[ki & HASHDATAMASK] ^ (hashdata[(h + ki) & HASHDATAMASK] << 8);
		s++;
	}
	t = kk & 511;
#ifndef COM_ONS_C
	if (hash[t] > 0)
		collis += 1;
	hash[t] += 1;
#endif
	return(t);
}
