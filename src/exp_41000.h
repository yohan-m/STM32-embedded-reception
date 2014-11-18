
// Exponential filter for the frequency 39,5 kHz

#ifndef _EXP410_H__
#define _EXP410_H__


#include "global.h"

const int32_t cos_41000[256]= {
-1752,
-2599,
3973,
-800,
-3290,
3612,
200,
-3785,
3034,
1189,
-4052,
2275,
2105,
-4077,
1379,
2896,
-3857,
401,
3513,
-3406,
-602,
3919,
-2751,
-1568,
4091,
-1931,
-2440,
4017,
-996,
-3167,
3702,
0,
-3703,
3166,
995,
-4018,
2439,
1930,
-4092,
1567,
2750,
-3920,
601,
3405,
-3514,
-402,
3856,
-2897,
-1380,
4076,
-2106,
-2276,
4051,
-1190,
-3035,
3784,
-201,
-3613,
3289,
799,
-3974,
2598,
1751,
-4096,
1751,
2598,
-3974,
799,
3289,
-3613,
-201,
3784,
-3035,
-1190,
4051,
-2276,
-2106,
4076,
-1380,
-2897,
3856,
-402,
-3514,
3405,
601,
-3920,
2750,
1567,
-4092,
1930,
2439,
-4018,
995,
3166,
-3703,
-1,
3702,
-3167,
-996,
4017,
-2440,
-1931,
4091,
-1568,
-2751,
3919,
-602,
-3406,
3513,
401,
-3857,
2896,
1379,
-4077,
2105,
2275,
-4052,
1189,
3034,
-3785,
200,
3612,
-3290,
-800,
3973,
-2599,
-1752,
4096,
-1752,
-2599,
3973,
-800,
-3290,
3612,
200,
-3785,
3034,
1189,
-4052,
2275,
2105,
-4077,
1379,
2896,
-3857,
401,
3513,
-3406,
-602,
3919,
-2751,
-1568,
4091,
-1931,
-2440,
4017,
-996,
-3167,
3702,
-1,
-3703,
3166,
995,
-4018,
2439,
1930,
-4092,
1567,
2750,
-3920,
601,
3405,
-3514,
-402,
3856,
-2897,
-1380,
4076,
-2106,
-2276,
4051,
-1190,
-3035,
3784,
-201,
-3613,
3289,
799,
-3974,
2598,
1751,
-4096,
1751,
2598,
-3974,
799,
3289,
-3613,
-201,
3784,
-3035,
-1190,
4051,
-2276,
-2106,
4076,
-1380,
-2897,
3856,
-402,
-3514,
3405,
601,
-3920,
2750,
1567,
-4092,
1930,
2439,
-4018,
995,
3166,
-3703,
-1,
3702,
-3167,
-996,
4017,
-2440,
-1931,
4091,
-1568,
-2751,
3919,
-602,
-3406,
3513,
401,
-3857,
2896,
1379,
-4077,
2105,
2275,
-4052,
1189,
3034,
-3785,
200,
3612,
-3290,
-800,
3973,
-2599,
-1752,
4096,
};
const int32_t sin_41000[256]= {
3702,
-3167,
-996,
4017,
-2440,
-1931,
4091,
-1568,
-2751,
3919,
-602,
-3406,
3513,
401,
-3857,
2896,
1379,
-4077,
2105,
2275,
-4052,
1189,
3034,
-3785,
200,
3612,
-3290,
-800,
3973,
-2599,
-1752,
4096,
-1752,
-2599,
3973,
-800,
-3290,
3612,
200,
-3785,
3034,
1189,
-4052,
2275,
2105,
-4077,
1379,
2896,
-3857,
401,
3513,
-3406,
-602,
3919,
-2751,
-1568,
4091,
-1931,
-2440,
4017,
-996,
-3167,
3702,
0,
-3703,
3166,
995,
-4018,
2439,
1930,
-4092,
1567,
2750,
-3920,
601,
3405,
-3514,
-402,
3856,
-2897,
-1380,
4076,
-2106,
-2276,
4051,
-1190,
-3035,
3784,
-201,
-3613,
3289,
799,
-3974,
2598,
1751,
-4096,
1751,
2598,
-3974,
799,
3289,
-3613,
-201,
3784,
-3035,
-1190,
4051,
-2276,
-2106,
4076,
-1380,
-2897,
3856,
-402,
-3514,
3405,
601,
-3920,
2750,
1567,
-4092,
1930,
2439,
-4018,
995,
3166,
-3703,
-1,
3702,
-3167,
-996,
4017,
-2440,
-1931,
4091,
-1568,
-2751,
3919,
-602,
-3406,
3513,
401,
-3857,
2896,
1379,
-4077,
2105,
2275,
-4052,
1189,
3034,
-3785,
200,
3612,
-3290,
-800,
3973,
-2599,
-1752,
4096,
-1752,
-2599,
3973,
-800,
-3290,
3612,
200,
-3785,
3034,
1189,
-4052,
2275,
2105,
-4077,
1379,
2896,
-3857,
401,
3513,
-3406,
-602,
3919,
-2751,
-1568,
4091,
-1931,
-2440,
4017,
-996,
-3167,
3702,
0,
-3703,
3166,
995,
-4018,
2439,
1930,
-4092,
1567,
2750,
-3920,
601,
3405,
-3514,
-402,
3856,
-2897,
-1380,
4076,
-2106,
-2276,
4051,
-1190,
-3035,
3784,
-201,
-3613,
3289,
799,
-3974,
2598,
1751,
-4096,
1751,
2598,
-3974,
799,
3289,
-3613,
-201,
3784,
-3035,
-1190,
4051,
-2276,
-2106,
4076,
-1380,
-2897,
3856,
-402,
-3514,
3405,
601,
-3920,
2750,
1567,
-4092,
1930,
2439,
-4018,
995,
3166,
-3703,
-1,
};

#endif