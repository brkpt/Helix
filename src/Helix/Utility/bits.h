#ifndef BITS_H
#define BITS_H

// ****************************************************************************
// Alignment macros
// ****************************************************************************
#define ALIGNUP(v,a)		((v) + ((a) - (v)%(a)))
#define ALIGNUP_POW2(v,a)	((a) + (((v) - 1) & ~((a) - 1)))

// ****************************************************************************
// Bit counting
//
// TODO: Unroll these to 4 bits at a time (16 value table)
// ****************************************************************************

//
//This function is the equivalent of the Divide by Two algorithm. Dividing a binary integer by 2 is the same as shifting it right one bit, and checking whether a binary integer is odd is the same as checking if its least significant bit is 1.
//
//int isPowerOfTwo (unsigned int x)
//{
// while (((x & 1) == 0) && x > 1) /* While x is even and > 1 */
//   x >>= 1;
// return (x == 1);
//}
//

// Excludes sign bit
inline int CountOnes32(int value)
{
	int bitsSet = 0;
	for(int bit=0;bit<31;bit++)
	{
		bitsSet += ((value & (1<<bit)) >> bit);
	}

	return bitsSet;
}

inline int CountOnes(unsigned int value)
{
	int bitsSet = 0;
	for(int bit=0;bit<32;bit++)
	{
		bitsSet += ((value & (1<<bit)) >> bit);
	}
	return bitsSet;
}

// ****************************************************************************
// Is value power of 2?
//
// Unsigned version takes advantage of the fact that if you subtract 1 from 
// a power of 2, all of the bits lower than the bit that is set become one, 
// and the set bit becomes 0
//
// 16 - 1 = 00010000 - 00000001 = 00001111
// 00010000 & 00001111 = 00000000
// ****************************************************************************
inline bool IsPow2(unsigned long value)
{
	return ((value != 0) && !(value & (value - 1)));
}

inline bool IsPow2(unsigned int value)
{
	return ((value != 0) && !(value & (value - 1)));
}

// ****************************************************************************
// Is value power of 2? -- Signed version
//
// NOTE: Negative values use the slower bit counting functions above
// TODO: Don't count all ones.  Stop once you find one.
// ****************************************************************************
inline bool IsPow2(int value)
{
	if( value > 0)
		return IsPow2(static_cast<unsigned int>(value));
	return CountOnes(value) == 1;
}

// ****************************************************************************
// ****************************************************************************
#define	IS_EVEN(v)		((v) & 1) == 0)
#define	IS_ODD(v)		((v) & 1) == 1)
#define IS_BIT_SET(v,n)	(((v) & 1<<(n)) ? true : false)

#endif // BITS_H
