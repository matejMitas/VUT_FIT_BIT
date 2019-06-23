// bit_array.h
// Řešení IJC-DU1, příklad a), 08.03.2017
// Autor: Matěj Mitaš, FIT

#ifndef bit_array_h
#define bit_array_h

#include <limits.h>

#include "error.h"

// define typedef of unsigned long
typedef unsigned long bit_array_t;

// determining length of data type
#define DATA_TYPE unsigned long
#define ULSIZE (int)(sizeof(unsigned long)*CHAR_BIT)


// macro for bit_array_t
#define ba_create(jmeno_pole, velikost) \
    bit_array_t jmeno_pole[((velikost + ULSIZE - 1) / ULSIZE) + 1] = {0, }; \
    jmeno_pole[0] = velikost;\

#define ba_size(jmeno_pole) ( jmeno_pole[0] )

// support macro for working with unknow size of item
#define BIT_COUNT(A) (int)(sizeof(A[0])*CHAR_BIT)

#define BA_GET_BIT_(jmeno_pole, index) ( (jmeno_pole[1 + index/BIT_COUNT(jmeno_pole)] & (1UL << (index%BIT_COUNT(jmeno_pole)) )) != 0 ? 1 : 0)

#define BA_SET_BIT_(jmeno_pole, index, vyraz) ( (vyraz) == 0 ? (jmeno_pole[1 + index/BIT_COUNT(jmeno_pole)] &= ~(1UL << (index%BIT_COUNT(jmeno_pole))))\
                                                             : (jmeno_pole[1 + index/BIT_COUNT(jmeno_pole)] |= 1UL << (index%BIT_COUNT(jmeno_pole))))



// functions are defined both as macro and as inlines
#ifndef USE_INLINE // USE_INLINE not defined

#define ba_set_bit(jmeno_pole, index, vyraz) ( (index < ba_size(jmeno_pole) && index >= 0) ? (BA_SET_BIT_(jmeno_pole, index, vyraz))\
                                            : (error_msg("Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)(ba_size(jmeno_pole)-1))))


#define ba_get_bit(jmeno_pole, index) ({ \
    DATA_TYPE retval; \
    DATA_TYPE max = ba_size(jmeno_pole);\
    if (index < max && index >= 0) {\
        retval = BA_GET_BIT_(jmeno_pole, index);\
    } else {\
        error_msg("Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)(max-1));\
        exit(1);\
    }\
    retval;\
})



#else // USE_INLINE defined

inline int ba_get_bit(bit_array_t jmeno_pole, unsigned long index)
{
    if (index < ba_size(jmeno_pole) && index >= 0) {
        error_msg("Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)(ba_size(jmeno_pole)-1));
    } else {
        return BA_GET_BIT_(jmeno_pole, index);
    }
}


inline void ba_set_bit(bit_array_t jmeno_pole, unsigned long index, int vyraz)
{
    if (index < ba_size(jmeno_pole) && index >= 0) {
        error_msg("Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)(ba_size(jmeno_pole)-1));
    } else {
        BA_SET_BIT_(jmeno_pole, index, vyraz);
    }
}


#endif // endif for USE_INLINE

#endif /* bit_array_h */

