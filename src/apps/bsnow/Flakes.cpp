#include "Flakes.h"

#define TRAN 0xFF
#define WHIT 0x3F

const uchar gFlakeBits[NUM_PATTERNS][64] = {
{
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, 
TRAN, TRAN, WHIT, TRAN, WHIT, TRAN, TRAN, TRAN, 
TRAN, TRAN, TRAN, WHIT, TRAN, TRAN, TRAN, TRAN, 
TRAN, TRAN, WHIT, TRAN, WHIT, TRAN, TRAN, TRAN, 
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, 
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, 
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, 
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN 
},
{
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, 
TRAN, TRAN, TRAN, WHIT, TRAN, TRAN, TRAN, TRAN, 
TRAN, WHIT, TRAN, WHIT, TRAN, WHIT, TRAN, TRAN, 
TRAN, TRAN, WHIT, WHIT, WHIT, TRAN, TRAN, TRAN, 
TRAN, WHIT, TRAN, WHIT, TRAN, WHIT, TRAN, TRAN, 
TRAN, TRAN, TRAN, WHIT, TRAN, TRAN, TRAN, TRAN, 
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, 
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN 
}
/*
{
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, 
TRAN, WHIT, TRAN, TRAN, TRAN, WHIT, TRAN, TRAN, 
TRAN, TRAN, WHIT, TRAN, WHIT, TRAN, TRAN, TRAN, 
TRAN, WHIT, WHIT, WHIT, WHIT, WHIT, TRAN, TRAN, 
TRAN, TRAN, WHIT, TRAN, WHIT, TRAN, TRAN, TRAN, 
TRAN, WHIT, TRAN, TRAN, TRAN, WHIT, TRAN, TRAN, 
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, 
TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN, TRAN 
}
*/
};

