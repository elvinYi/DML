/**
*  @file endian.h
*  Functions for reading and writing endian-specific values
*/

#ifndef ENDIAN_H_
#define ENDIAN_H_

#include <BaseDataTypes.h>

/** @name ENDIANs
*  The two types of endianness
*/
/*@{*/
#define LIL_ENDIAN	1234
#define BIG_ENDIAN	4321
/*@}*/

//FOR FLV WE USE BIG ENDIAN
#define BYTEORDER	LIL_ENDIAN


/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

    /**
    *  @name Swap Functions
    *  Use inline functions for compilers that support them, and static
    *  functions for those that do not.  Because these functions become
    *  static for compilers that do not support inline functions, this
    *  header should only be included in files that actually use them.
    */
    /*@{*/

    static  UInt16 Swap16(UInt16 x) {
        return((x << 8) | (x >> 8));
    }


    static  UInt32 Swap32(UInt32 x) {
        return((x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24));
    }


    static  UInt64 Swap64(UInt64 x)
    {
        UInt32 hi, lo;

        /* Separate into high and low 32-bit values and swap them */
        lo = x & 0xFFFFFFFF;
        x >>= 32;
        hi = x & 0xFFFFFFFF;
        x = Swap32(lo);
        x <<= 32;
        x |= Swap32(hi);
        return(x);
    }

    /**
    *  @name SwapLE and SwapBE Functions
    *  Byteswap item from the specified endianness to the native endianness
    */
    /*@{*/
#if BYTEORDER == LIL_ENDIAN
#define SwapLE16(X)	(X)
#define SwapLE32(X)	(X)
#define SwapLE64(X)	(X)
#define SwapBE16(X)	Swap16(X)
#define SwapBE32(X)	Swap32(X)
#define SwapBE64(X)	Swap64(X)
#else
#define SwapLE16(X)	Swap16(X)
#define SwapLE32(X)	Swap32(X)
#define SwapLE64(X)	Swap64(X)
#define SwapBE16(X)	(X)
#define SwapBE32(X)	(X)
#define SwapBE64(X)	(X)
#endif
    /*@}*/

    /* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* ENDIAN_H_ */
