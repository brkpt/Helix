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
// NOTE: Uses the bit counting functions above
// ****************************************************************************
inline bool IsPow2(int value)
{
	return CountOnes(value) == 1;
}

inline bool IsPow2(unsigned int value)
{
	return CountOnes(value) == 1;
}

// ****************************************************************************
// ****************************************************************************
#define	IS_EVEN(v)		((v) & 1) == 0)
#define	IS_ODD(v)		((v) & 1) == 1)
#define IS_BIT_SET(v,n)	(((v) & 1<<(n)) ? true : false)

#endif // BITS_H
