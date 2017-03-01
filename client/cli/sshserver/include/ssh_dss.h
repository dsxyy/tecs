#include "ssh_mp.h"

typedef struct DSS_key 
{
	mp_int* p;
	mp_int* q;
	mp_int* g;
	mp_int* y;
	mp_int* x;
}dss_key;

dss_key * gen_dss_priv_key();
