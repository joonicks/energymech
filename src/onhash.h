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
		heatmap[ki & HASHDATAMASK] += 1;
		//heatmap[(ki + h) & HASHDATAMASK] += 1;
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
