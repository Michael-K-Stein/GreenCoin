#pragma once

#ifndef __CURVE_H
#define __CURVE_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "../../BNMath/BNMath.h"

typedef uint32_t _n_t;

typedef struct _Point _Point;
struct _Point {
	double x;
	double y;
};

typedef struct _Curve _Curve;
struct _Curve {
	uint64_t a;
	uint64_t b;
};

int Is_on_curve(_Curve * curve, _Point * p);


int Is_Prime(_n_t n);

/*
_Point * Generate_G(_Curve * curve);
_Point * Generate_G(_Curve * curve) {
	srand(time(NULL));
	int n = 4;

	_Point * pp = (_Point*)malloc(sizeof(_Point));

	while (!Is_Prime(n)) {

		double x = rand();
		double y = pow(pow(x, 3) - (curve->a * x) + curve->b, 0.5);

		_Point p;
		p.x = x;
		p.y = y;

		if (!Is_on_curve(curve, &p)) { continue; }

		// Point p is on the curve
		_Point tmp_p = p;
		n = 1;
		while (!(tmp_p.x == 1 && tmp_p.y == 1)) {
			n++;
			tmp_p.x += p.x;
			tmp_p.y += p.y;
		}

		pp->x = p.x;
		pp->y = p.y;
		printf("G: %f, %f | n: %d\n", p.x, p.y, n);
	}

	return pp;
}*/

uint64_t Generate_Prime();

_n_t Generate_n();

#endif // !__CURVE_H