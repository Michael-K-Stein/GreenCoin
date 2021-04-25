
#include "DSA.h"

g_t Generate_G(p_t p, q_t q, h_t h) {
	uint2048_t one = Init(1);

	uint2048_t tmp1 = Init(0);
	Sub(p, one, tmp1);

	uint2048_t e = Init(0);
	Div(tmp1, q, e);



	uint2048_t G = Init(1);
	while (Equ(G, one)) {

		Pow(h, e, p, G);

		//Mod(tmp1, p, G);

		Inc(h);
	}

	Dinit(tmp1);
	Dinit(one);
	Dinit(e);

	return G;
}