//need to be able to represent R*whole
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define precision 16 /*bits to use to represent numbers*/
#define whole (1 << precision) /*the whole value*/
#define half whole/2
#define quarter whole/4

void define_values(int* c, int* d, int* r, int n) {
	c[0] = 0;
	int j, i;
	d[0] = c[0] + r[0];
	for(j = 1; j < n; j++) {
		c[j] = 0;
		for(i = 0; i < j; i++) {
			c[j] += r[i];
		}
		d[j] = c[j] + r[j];
		printf("d %i c %i\n", d[j], c[j]);
	}
}

char* arith_coding(int* x, int* c, int* d, int R, int n) {
	int a = 0, b = whole, s = 0;
	int i, j, w;
	char* cod = (char*) malloc(sizeof(char)*32);
	int t = 0;
	for(i = 0; i < n; i++) {
		w = b-a;
		b = a + w*d[x[i]]/R;
		a = a + w*c[x[i]]/R;
		while(b<half || a > half) {
			if(b<half) {
				printf("0");
				cod[t++] = '0';
				for(j = 0; j < s; j++) {
					printf("1");
					cod[t++] = '1';
				}
				s = 0;
				a = 2*a;
				b = 2*b;
			} else if(a > half) {
				printf("1");
				cod[t++] = '1';
				for(j = 0; j < s; j++) {
					printf("0");
					cod[t++] = '0';
				}
				s = 0;
				a = 2*(a-half);
				b = 2*(b-half);
			}
		}
		while(a>quarter && b < 3*quarter) {
			s = s+1;
			a = 2*(a - quarter);
			b = 2*(b - quarter);
		}
	}
	s = s+1;
	if (a <= quarter) {
		printf("0");
		cod[t++] = '0';
		for(j = 0; j < s; j++) {
			printf("1");
			cod[t++] = '1';
		}
		s = 0;
		a = 2*a;
		b = 2*b;
	} else {
		printf("1");
		cod[t++] = '1';
		for(j = 0; j < s; j++) {
			printf("0");
			cod[t++] = '0';
		}
		s = 0;
		a = 2*a;
		b = 2*b;
	}
	return cod;
}

void arith_decoding(char* cod, int* c, int* d, int R, int n) {
	int a = 0, b = whole, z = 0, i = 0;
	while(i+1 <= precision && i+1<16) {
		if (cod[i] == '1')
			z = z + pow(2, precision - i);
		i++;
	}
	int j, ac, bc, keep = 1, w;
	while(1) {
		for(j = 0; j < n; j++) {
			w = b - a;
			bc = a + w*d[j]/R;
			ac = a + w*c[j]/R;
		printf("%i %i %i\n", ac, bc, z);
			if(ac <= z && z < bc) {
				printf("%i", j);
				a = ac;
				b = bc;
				if (j == 0) {
					keep = 0;
					break;
				}
			}
		}
		if(!keep)
			break;
		while(b < half || a > half) {
			if(b < half) {
				a = 2*a;
				b = 2*b;
				z = 2*z;
			} else if ( a > half) {
				a = 2*(a - half);
				b = 2*(b - half);
				z = 3*(z - half);
			}
			if (i <= 32 && cod[i] == '1')
				z++;
			i++;
		}
		while(a > quarter && b < 3*quarter) {
			a = 2*(a - quarter);
			b = 2*(b - quarter);
			z = 2*(z - quarter);
			if (i <= 32 && cod[i] == '1')
				z++;
			i++;
		}
	}
}

int main() {
	int*c, *d, *r, *x;
	int n = 3;
	r = (int*) malloc (sizeof(int)*(n));
	x = (int*) malloc (sizeof(int)*n);
	c = (int*) malloc(sizeof(int)*(n));
	d = (int*) malloc(sizeof(int)*(n));
	int sum = 3;
	r[0] = 20*sum;
	r[1] = 40*sum;
	r[2] = 40*sum;
	define_values(c, d, r, n);
	x[0] = 2;
	x[1] = 1;
	x[2] = 0;
	printf("hi%i\n", d[0]);
	int i;
	for(i = 0; i < 1000000; i++);
	arith_coding(x, c, d, sum, n);
	char cod[6] = {'0', '0', '1', '1', '0', '1'};
	// char cod[6] = {'1', '0', '1', '1', '0', '0'};
printf("\n" );
	arith_decoding(cod, c, d, sum, n);
	
}