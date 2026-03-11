#ifndef UTILS_H
#define UTILS_H


#include <stdint.h>
#include <stdbool.h>


/* Macros */

/*
    Safety macros to set/clear a bit from a uint8_t value.

    Safety: avoids undefined shifts; out-of-range positions (bit_pos < 0 or bit_pos > 7)
            produce a zero mask.
    Contract: 'num' must be a uint8_t lvalue; 'bit_pos' may be any integer.
    Evaluation: 'bit_pos' is evaluated once per macro; 'num' is evaluated once as an lvalue.
    Semantics: set/clear are no-ops when 'bit_pos' is outside [0..7].

    Example:
        uint8_t x = 0;
        SET_BIT_U8(x, 3);   // x = 0b00001000
        CLEAR_BIT_U8(x, 3); // x = 0
*/
#define MASK_U8(bit_pos)			( (uint8_t)( ((unsigned)(bit_pos) < 8u) ? (1u << (unsigned)(bit_pos)) : 0u ) )
#define SET_BIT_U8(num, bit_pos)		do { (num) |= MASK_U8(bit_pos); } while (0)
#define CLEAR_BIT_U8(num, bit_pos)         do { (num) &= (uint8_t)~MASK_U8(bit_pos); } while (0)

/*
	Safety macro to read a bit from a uint8_t value.
	
	Returns 0 or 1.
	Safety: positions >= 8 are read as 0.
	Evaluation: both 'num' and 'bit_pos' are evaluated once.
*/
#define TEST_BIT_U8(num, bit_pos)		( ( ((uint8_t)(num)) & MASK_U8(bit_pos) ) ? 1u : 0u )


/* Global functions */

/* --------- Implement these functions --------- */
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
uint32_t get_ms(void);
uint32_t get_us(void);


#endif  // UTILS_H
