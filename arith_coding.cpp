#include <cstdio>
#include <stdlib.h>
//An infinite precision implementation of Arithmetic Coding.
typedef struct {
	double first;
	double second;
	void set(char c) {
		if (c >= (int)'A' && c <= (int)'Z') {
      		first = (c - (int)'A') * 0.01;
      		second = (c - (int)'A') * 0.01 + 0.01;
      	}
	}
	static char getSymbol( double d) {
		if ( d >= 0.0 && d < 0.26)
			return 'A' + static_cast<int>(d*100);
	}
} model;

double encode(char* input, int n) {
	double high = 1.0;
	double low = 0.0;
	char c;
	int i = 0;
	model p;
	while ( i < n ) {
	  c = input[i];
	  i++;
	  p.set(c);
	  double range = high - low;
	  high = low + range * p.second;
	  low = low + range * p.first; 
	}
	double out = low + (high-low)/2;
	return out;
}

void decode(double message) {
	double high = 1.0;
	double low = 0.0;
	model p;
	for ( ; ; ) {
		double range = high - low;
		char c = p.getSymbol((message - low)/range);
		printf("%c", c);
		if ( c == 'Z' )
		  return;
		p.set(c);
		high = low + range * p.second;
		low = low + range * p.first; 
	}
}

int main() {
	char* n = (char*) malloc(sizeof(char)*4);
	for( int i = 0; i < 4; i++) {
		n[i] = (char) (((int)'W') + i);
	}
	double m = encode(n, 4);
	decode(m);

}