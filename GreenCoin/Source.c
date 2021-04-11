//#include "src/BlockChain/BlockChain.h"
//#include "src/ECDSA/Curve/Curve.h"


#include "src/ECDSA/ECDSA.h"
#include "src/ECDSA/Curve/Curve.h"

#include <stdio.h>

/*

p = 0x0043D7C1
q = 0x10EAD9BE
G = 0x00131DF7

*/

/*

p: 0x1C94A105DB921B7D
q: 0x00000000E4BE2961
G: 0x00573B04AAD786E3
Private key:     0x0000000000000357
Public key:      0x0290470EB37FDDC9

*/

/*

p: 0x1C94A105DB921B7D
q: 0x00000000E4BE2961
G: 0x00573B04AAD786E3
Private key:     0x000000000000300E
Public key:      0x0B353C5496AD016D

*/

int main_c() {

	/*uint2048_t aa = Init(2);
	uint2048_t ba = Init(3);
	uint2048_t ca = Init(5);
	uint2048_t da = Init(0);

	Pow(aa, ba, ca, da);
	Print_Hex(da);*/

	srand(time(NULL));

	//uint64_t private_key = -1;// 0x233116E25D6C4F37;

	
	uint2048_t h = Init(2);

	uint64_t p_t = 191096069;// 2059447976145853309;//0x0043D7C1;// Generate_Prime();
	uint64_t q_t = 53831;// 3837667681;// 0x10EAD9BE;// Generate_Prime();

	uint2048_t p = Init(p_t);
	uint2048_t q = Init(q_t);

	uint2048_t G = Generate_G(p, q, h);

	printf("p: ");
	Print_Hex(p);
	printf("q: ");
	Print_Hex(q);
	printf("G: ");
	Print_Hex(G);

	uint64_t c = -1;
	while (c > q - 2) {
		c = 0;
		for (int i = 0; i < 1; i++) {
			uint64_t aaa = rand();
			c += aaa << (16 * i);
		}
	}
	uint64_t private_key = c + 1;
	uint2048_t private_key_tmp = Init(private_key);
	uint2048_t public_key = Init(0);
	Pow(G, private_key_tmp, p, public_key);
	//Mod(public_key, p, public_key);
	printf("Private key:\t 0x%.16llX\nPublic key:\t ", private_key);
	Print_Hex(public_key);

	char * message = "abc";

	_Signature * signature = Create_Signature(message, 3, private_key, G, q, p);

	printf("Valid signature: %d\n", Validate_Signature(message, 3, signature, public_key, G, q, p));

	free(p);
	free(q);

}