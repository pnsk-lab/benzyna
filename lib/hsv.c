#include <ba_runtime.h>

void ba_hsv_from_rgb(double hsv[3], double rgb[3]) {
	double h, s, v;

	double r = rgb[0];
	double g = rgb[1];
	double b = rgb[2];

	double max, min;

	max = r > g ? r : g;
	max = max > b ? max : b;
	min = r < g ? r : g;
	min = min < b ? min : b;

	h = max - min;

	if(h > 0) {
		if(max == r) {
			h = (g - b) / h;
			if(h < 0) h += 6;
		} else if(max == g) {
			h = 2 + (b - r) / h;
		} else {
			h = 4 + (r - g) / h;
		}
	}
	h /= 6;
	s = max - min;
	if(max != 0) s /= max;
	v = max;

	hsv[0] = h;
	hsv[1] = s;
	hsv[2] = v;
}

void ba_hsv_to_rgb(double rgb[3], double hsv[3]) {
	double r, g, b;

	double h = hsv[0];
	double s = hsv[1];
	double v = hsv[2];

	int    i;
	double f, p, q, t;

	i = floor(h * 6);
	f = h * 6 - i;
	p = v * (1 - s);
	q = v * (1 - f * s);
	t = v * (1 - (1 - f) * s);

	switch(i % 6) {
	case 0:
		r = v, g = t, b = p;
		break;
	case 1:
		r = q, g = v, b = p;
		break;
	case 2:
		r = p, g = v, b = t;
		break;
	case 3:
		r = p, g = q, b = v;
		break;
	case 4:
		r = t, g = p, b = v;
		break;
	case 5:
		r = v, g = p, b = q;
		break;
	}

	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}
