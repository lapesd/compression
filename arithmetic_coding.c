/* ARITHMETIC ENCODING ALGORITHM. */
#include "arithmetic-coding.h"
static void bit_plus_follow(); 			/* Routine that follows 					*/
/* CURRENT STATE OF THE ENCODING. */
static code_value low, high; 			/* Ends of the current code region 			*/
static long bits_to_follow; 			/* Number of opposite bits to output after 	*/
										/* the next bit. 							*/
/* START ENCODING A STREAM OF SYMBOLS. */
start_encoding()
{
	low = 0;							/*Full code range							*/
	high = Top_value;					/*No bits to follow next					*/
	bits_to_follow = 0;
}

/*ENCODE A SYMBOL*/

encode_symbol(symbol, freq)
	int symbol;
	int freq[];
{
	long range = (long)(high - low) + 1;
	high = low + range*freq[symbol-1]/frq[0]-1;
	low = low + range*freq[symbol]/frq[0];
}