////////////////////////////////////////////////////////////////////
///
/// \file locate3.h
/// All the code in lookup3.c is written by Bob Jenkins, May 2006.
/// this header file is written by acording to locate3.c 
///
///////////////////////////////////////////////////////////////////

#ifndef locate3_h
#define locate3_h
#include <stdint.h>
#include <stdio.h>

/// @brief hash a variable-length key into a 32-bit value
/*! @fn
* hashlittle() -- hash a variable-length key into a 32-bit value <br>
*   **k**       : the key (the unaligned variable-length array of bytes) <br>
*   **length**  : the length of the key, counting by bytes <br>
*   **initval** : can be any 4-byte value <br>
* Returns a 32-bit value.  Every bit of the key affects every bit of
* the return value.  Two keys differing by one or two bits will have
* totally different hash values.
* 
* The best hash table sizes are powers of 2.  There is no need to do
* mod a prime (mod is sooo slow!).  If you need less than 32 bits,
* use a bitmask.  For example, if you need only 10 bits, do 
*   __h = (h & hashmask(10));__ <br>
* In which case, the hash table should have hashsize(10) elements. <br>
* <br>
* If you are hashing n strings (uint8_t **)k, do it like this: <br>
*
*   for (i=0, h=0; i<n; ++i) h = hashlittle( k[i], len[i], h);
*/
/*! @note
* Made by Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net. <br>
* Use for hash table lookup, or anything where one collision in 2^^32 is
* acceptable.  Do NOT use for cryptographic purposes.
*/ 
extern uint32_t hashlittle(const void *key, size_t length, uint32_t initval);
/// @brief return 2 32-bit hash values
/*! @fn
 * hashlittle2: return 2 32-bit hash values
 *
 * This is identical to hashlittle(), except it returns two 32-bit hash
 * values instead of just one.  This is good enough for hash table
 * lookup with 2^^64 buckets, or if you want a second hash if you're not
 * happy with the first, or if you want a probably-unique 64-bit ID for
 * the key.  *pc is better mixed than *pb, so use *pc first.  If you want
 * a 64-bit value do something like __"*pc + (((uint64_t)*pb)<<32)"__.
 */
/*! @note
* Made by Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net. <br>
* Use for hash table lookup, or anything where one collision in 2^^32 is
* acceptable.  Do NOT use for cryptographic purposes.
*/ 
extern void hashlittle2(const void *key, size_t length, uint32_t *pc, uint32_t *pb);
/*! @fn
 * hashbig():
 * This is the same as hashword() on big-endian machines.  It is different
 * from hashlittle() on all machines.  hashbig() takes advantage of
 * big-endian byte ordering.
 */
/*! @note
* Made by Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net. <br>
* Use for hash table lookup, or anything where one collision in 2^^32 is
* acceptable.  Do NOT use for cryptographic purposes.
*/ 
extern uint32_t hashbig( const void *key, size_t length, uint32_t initval);
#endif