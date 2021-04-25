
#include "Curve.h"

int Is_on_curve(_Curve * curve, _Point * p) {
	return (pow(p->y, 2) - pow(p->x, 3) + (p->x * curve->a) - curve->b != 0);
}

int Is_Prime(_n_t n) {
	for (int i = 2; i < pow(n, 0.5) + 1; i++) {
		if (n%i == 0) {
			return 0;
		}
	}
	return 1;
}

uint64_t Generate_Prime() {
	int ind = rand() * rand();

	int last_num = 1;
	for (int i = 0; i < ind; i++) {
		// Calculate the prime number at this index
		int num = last_num + 1;

		while (!Is_Prime(num)) { num++; }
		last_num = num;
	}

	return last_num;
}

_n_t Generate_n() {
	srand(time(NULL));

	int ind = rand();

	int last_num = 1;
	for (int i = 0; i < ind; i++) {
		// Calculate the prime number at this index
		int num = last_num + 1;

		while (!Is_Prime(num)) { num++; }
		last_num = num;
	}

	printf("prime at index %d is %d\n", ind, last_num);

	_n_t n = 0;

	return last_num;
}