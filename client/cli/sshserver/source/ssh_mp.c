#include <stdlib.h>
#include "pub_tmp.h"          /*#include "pub_oss.h"*/
#include "ssh_mp.h"

/*
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
*/
/* 初始化一个mp_int数 */
int mp_init (mp_int *a)
{
    /* allocate memory required and clear it */
    a->dp = (mp_digit*)XOS_GetUB(sizeof(mp_digit)*MP_PREC);
    if (a->dp == NULL)
    {
        return MP_ERROR;
    }
    /* set the used to zero, allocated digits to the default precision
    * and sign to positive */
    a->used  = 0;
    a->alloc = MP_PREC;
    a->sign  = MP_ZPOS;

    return MP_OKAY;
}

/* copy, b = a */
int mp_copy (mp_int * a, mp_int * b)
{
    int     res;
    unsigned int n;
    /* if dst == src do nothing */
    if (a == b)
    {
        return MP_OKAY;
    }

    /* grow dest */
    if (b->alloc < a->used)
    {
        if ((res = mp_grow (b, a->used)) != MP_OKAY)
        {
            return res;
        }
    }

    /* zero b and copy the parameters over */
    {
        mp_digit *tmpa, *tmpb;

        /* pointer aliases */

        /* source */
        tmpa = a->dp;

        /* destination */
        tmpb = b->dp;

        /* copy all the digits */
        for (n = 0; n < a->used; n++)
        {
            *tmpb++ = *tmpa++;
        }

        /* clear high digits */
        for (; n < b->used; n++)
        {
            *tmpb++ = 0;
        }
    }

    /* copy used count and sign */
    b->used = a->used;
    b->sign = a->sign;
    return MP_OKAY;
}

/* grow as required */
int mp_grow (mp_int * a, int size)
{
    int     i;
    mp_digit *tmp;
    mp_int  newa;

    /* if the alloc size is smaller alloc more ram */
    if ((int)(a->alloc) < size)
    {
        /* ensure there are always at least MP_PREC digits extra on top */
        size += (MP_PREC * 2) - (size % MP_PREC);
        /* reallocate the array a->dp
        *
        * We store the return in a temporary variable
        * in case the operation failed we don't want
        * to overwrite the dp member of a.
        */
        mp_init_copy(&newa,a);
        tmp = (mp_digit*)XOS_GetUB(sizeof(mp_digit) * size);
        if (tmp == NULL)
        {
            /* reallocation failed but "a" is still valid [can be freed] */
            return MP_ERROR;
        }
        if (a->dp!=NULL)
        {
            XOS_RetUB((BYTE *)(a->dp));
        }
        /* reallocation succeeded so set a->dp */
        a->dp = tmp;
        mp_copy(&newa,a);
        mp_clear(&newa);
        /* zero excess digits */
        i        = a->alloc;
        a->alloc = size;
        for (; i < (int)(a->alloc); i++)
        {
            a->dp[i] = 0;
        }
    }
    return MP_OKAY;
}

/* creates "a" then copies b into it */
int mp_init_copy (mp_int * a, mp_int * b)
{
    int     res;

    if ((res = mp_init (a)) != MP_OKAY)
    {
        return res;
    }
    return mp_copy (b, a);
}

/* clear one (frees)  */
void mp_clear (mp_int * a)
{
    /* only do anything if a hasn't been freed previously */
    if (a->dp != NULL)
    {
        /* first zero the digits */
        memset (a->dp, 0, sizeof (mp_digit) * a->used);

        /* free ram */
        XOS_RetUB ((BYTE *)(a->dp));

        /* reset members to make debugging easier */
        a->dp    = NULL;
        a->alloc = a->used = 0;
        a->sign  = MP_ZPOS;
    }
}

/* trim unused digits
*
* This is used to ensure that leading zero digits are
* trimed and the leading "used" digit will be non-zero
* Typically very fast.  Also fixes the sign if there
* are no more leading digits
*/
void mp_clamp (mp_int * a)
{
    /* decrease used while the most significant digit is zero. */
    while (a->used > 0 && a->dp[a->used - 1] == 0)
    {
        --(a->used);
    }

    /* reset the sign flag if used == 0 */
    if (a->used == 0)
    {
        a->sign = MP_ZPOS;
    }
}

/* shift left by a certain bit count */
int mp_mul_2d (mp_int * a, int b, mp_int * c)
{
    mp_digit d;
    int      res;

    /* copy */
    if (a != c)
    {
        if ((res = mp_copy (a, c)) != MP_OKAY)
        {
            return res;
        }
    }

    if (c->alloc < (c->used + b/DIGIT_BIT + 1))
    {
        if ((res = mp_grow (c, c->used + b / DIGIT_BIT + 1)) != MP_OKAY)
        {
            return res;
        }
    }

    /* shift by as many digits in the bit count */
    if (b >= (int)DIGIT_BIT)
    {
        if ((res = mp_lshd (c, b / DIGIT_BIT)) != MP_OKAY)
        {
            return res;
        }
    }

    /* shift any bit count < DIGIT_BIT */
    d = (mp_digit) (b % DIGIT_BIT);
    if (d != 0)
    {
        register mp_digit *tmpc, shift, mask, r, rr;
        register int x;

        /* bitmask for carries */
        mask = (((mp_digit)1) << d) - 1;

        /* shift for msbs */
        shift = DIGIT_BIT - d;

        /* alias */
        tmpc = c->dp;

        /* carry */
        r    = 0;
        for (x = 0; x < (int)(c->used); x++)
        {
            /* get the higher bits of the current word */
            rr = (*tmpc >> shift) & mask;

            /* shift the current word and OR in the carry */
            *tmpc = ((*tmpc << d) | r) & MP_MASK;
            ++tmpc;

            /* set the carry to the carry bits of the current word */
            r = rr;
        }

        /* set final carry */
        if (r != 0)
        {
            c->dp[(c->used)++] = r;
        }
    }
    mp_clamp (c);
    return MP_OKAY;
}

/* shift left a certain amount of digits */
int mp_lshd (mp_int * a, int b)
{
    int     x, res;

    /* if its less than zero return */
    if (b <= 0)
    {
        return MP_OKAY;
    }

    /* grow to fit the new digits */
    if (a->alloc < a->used + b)
    {
        if ((res = mp_grow (a, a->used + b)) != MP_OKAY)
        {
            return res;
        }
    }

    {
        register mp_digit *top, *bottom;

        /* increment the used by the shift amount then copy upwards */
        a->used += b;

        /* top */
        top = a->dp + a->used - 1;

        /* base */
        bottom = a->dp + a->used - 1 - b;

        /* much like mp_rshd this is implemented using a sliding window
        * except the window goes the otherway around.  Copying from
        * the bottom to the top.  see bn_mp_rshd.c for more info.
        */
        for (x = a->used - 1; x >= b; x--)
        {
            *top-- = *bottom--;
        }

        /* zero the lower digits */
        top = a->dp;
        for (x = 0; x < b; x++)
        {
            *top++ = 0;
        }
    }
    return MP_OKAY;
}

/* set to zero */
void mp_zero (mp_int * a)
{
    a->sign = MP_ZPOS;
    a->used = 0;
    memset (a->dp, 0, sizeof (mp_digit) * a->alloc);
}

/* reads a unsigned char array, assumes the msb is stored first [big endian] */
int mp_read_unsigned_bin (mp_int * a, unsigned char *b, int c)
{
    int     res;
    /* make sure there are at least two digits */
    if (a->alloc < 2)
    {
        if ((res = mp_grow(a, 2)) != MP_OKAY)
        {
            return res;
        }
    }

    /* zero the int */
    mp_zero (a);

    /* read the bytes in */
    while (c-- > 0)
    {
        if ((res = mp_mul_2d (a, 8, a)) != MP_OKAY)
        {
            return res;
        }
        a->dp[0] |= *b++;
        a->used += 1;
    }
    mp_clamp (a);
    return MP_OKAY;
}

/* set a 32-bit const */
int mp_set_int (mp_int * a, unsigned int b)
{
    int     x, res;

    mp_zero (a);

    /* set four bits at a time */
    for (x = 0; x < 8; x++)
    {
        /* shift the number up four bits */
        if ((res = mp_mul_2d (a, 4, a)) != MP_OKAY)
        {
            return res;
        }

        /* OR in the top four bits of the source */
        a->dp[0] |= (b >> 28) & 15;

        /* shift the source up to the next four bits */
        b <<= 4;

        /* ensure that digits are not clamped off */
        a->used += 1;
    }
    mp_clamp (a);
    return MP_OKAY;
}

/* single digit subtraction */
int mp_sub_d (mp_int * a, mp_digit b, mp_int * c)
{
    mp_digit *tmpa, *tmpc, mu;
    int       res, ix, oldused;

    /* grow c as required */
    if (c->alloc < a->used + 1)
    {
        if ((res = mp_grow(c, a->used + 1)) != MP_OKAY)
        {
            return res;
        }
    }

    /* if a is negative just do an unsigned
    * addition [with fudged signs]
    */
    if (a->sign == MP_NEG)
    {
        a->sign = MP_ZPOS;
        res     = mp_add_d(a, b, c);
        a->sign = c->sign = MP_NEG;
        return res;
    }

    /* setup regs */
    oldused = c->used;
    tmpa    = a->dp;
    tmpc    = c->dp;

    /* if a <= b simply fix the single digit */
    if ((a->used == 1 && a->dp[0] <= b) || a->used == 0)
    {
        if (a->used == 1)
        {
            *tmpc++ = b - *tmpa;
        }
        else
        {
            *tmpc++ = b;
        }
        ix      = 1;

        /* negative/1digit */
        c->sign = MP_NEG;
        c->used = 1;
    }
    else
    {
        /* positive/size */
        c->sign = MP_ZPOS;
        c->used = a->used;

        /* subtract first digit */
        *tmpc    = *tmpa++ - b;
        mu       = *tmpc >> (sizeof(mp_digit) * SSH_CHAR_BIT - 1);
        *tmpc++ &= MP_MASK;

        /* handle rest of the digits */
        for (ix = 1; ix < (int)(a->used); ix++)
        {
            *tmpc    = *tmpa++ - mu;
            mu       = *tmpc >> (sizeof(mp_digit) * SSH_CHAR_BIT - 1);
            *tmpc++ &= MP_MASK;
        }
    }

    /* zero excess digits */
    while (ix++ < oldused)
    {
        *tmpc++ = 0;
    }
    mp_clamp(c);
    return MP_OKAY;
}

/* single digit addition */
int mp_add_d (mp_int * a, mp_digit b, mp_int * c)
{
    unsigned int ix;
    int     res, oldused;
    mp_digit *tmpa, *tmpc, mu;

    /* grow c as required */
    if (c->alloc < a->used + 1)
    {
        if ((res = mp_grow(c, a->used + 1)) != MP_OKAY)
        {
            return res;
        }
    }
    /* if a is negative and |a| >= b, call c = |a| - b */
    if (a->sign == MP_NEG && (a->used > 1 || a->dp[0] >= b))
    {
        /* temporarily fix sign of a */
        a->sign = MP_ZPOS;

        /* c = |a| - b */
        res = mp_sub_d(a, b, c);

        /* fix sign  */
        a->sign = c->sign = MP_NEG;

        return res;
    }

    /* old number of used digits in c */
    oldused = c->used;

    /* sign always positive */
    c->sign = MP_ZPOS;

    /* source alias */
    tmpa    = a->dp;

    /* destination alias */
    tmpc    = c->dp;

    /* if a is positive */
    if (a->sign == MP_ZPOS)
    {
        /* add digit, after this we're propagating
        * the carry.
        	*/
        *tmpc   = *tmpa++ + b;
        mu      = *tmpc >> DIGIT_BIT;
        *tmpc++ &= MP_MASK;

        /* now handle rest of the digits */
        for (ix = 1; ix < a->used; ix++)
        {
            *tmpc   = *tmpa++ + mu;
            mu      = *tmpc >> DIGIT_BIT;
            *tmpc++ &= MP_MASK;
        }
        /* set final carry */
        ix++;
        *tmpc++  = mu;

        /* setup size */
        c->used = a->used + 1;
    }
    else
    {
        /* a was negative and |a| < b */
        c->used  = 1;

        /* the result is a single digit */
        if (a->used == 1)
        {
            *tmpc++  =  b - a->dp[0];
        }
        else
        {
            *tmpc++  =  b;
        }

        /* setup count so the clearing of oldused
        * can fall through correctly
        */
        ix       = 1;
    }

    /* now zero to oldused */
    while ((int)ix++ < oldused)
    {
        *tmpc++ = 0;
    }
    mp_clamp(c);

    return MP_OKAY;
}

/* b = a/2 */
int mp_div_2 (mp_int * a, mp_int * b)
{
    int     x, res, oldused;

    /* copy */
    if (b->alloc < a->used)
    {
        if ((res = mp_grow (b, a->used)) != MP_OKAY)
        {
            return res;
        }
    }

    oldused = b->used;
    b->used = a->used;
    {
        register mp_digit r, rr, *tmpa, *tmpb;

        /* source alias */
        tmpa = a->dp + b->used - 1;

        /* dest alias */
        tmpb = b->dp + b->used - 1;

        /* carry */
        r = 0;
        for (x = b->used - 1; x >= 0; x--)
        {
            /* get the carry for the next iteration */
            rr = *tmpa & 1;

            /* shift the current digit, add in carry and store */
            *tmpb-- = (*tmpa-- >> 1) | (r << (DIGIT_BIT - 1));

            /* forward carry to next iteration */
            r = rr;
        }

        /* zero excess digits */
        tmpb = b->dp + b->used;
        for (x = b->used; x < oldused; x++)
        {
            *tmpb++ = 0;
        }
    }
    b->sign = a->sign;
    mp_clamp (b);
    return MP_OKAY;
}

/* get the size for an unsigned equivalent */
int mp_unsigned_bin_size (mp_int * a)
{
    int     size = mp_count_bits (a);
    return (size / 8 + ((size & 7) != 0 ? 1 : 0));
}

/* returns the number of bits in an int */
int mp_count_bits (mp_int * a)
{
    int     r;
    mp_digit q;

    /* shortcut */
    if (a->used == 0)
    {
        return 0;
    }

    /* get number of digits and add that */
    r = (a->used - 1) * DIGIT_BIT;

    /* take the last digit and count the bits in it */
    q = a->dp[a->used - 1];
    while (q > ((mp_digit) 0))
    {
        ++r;
        q >>= ((mp_digit) 1);
    }
    return r;
}

/* compare two ints (signed)*/
int mp_cmp (mp_int * a, mp_int * b)
{
    /* compare based on sign */
    if (a->sign != b->sign)
    {
        if (a->sign == MP_NEG)
        {
            return MP_LT;
        }
        else
        {
            return MP_GT;
        }
    }

    /* compare digits */
    if (a->sign == MP_NEG)
    {
        /* if negative compare opposite direction */
        return mp_cmp_mag(b, a);
    }
    else
    {
        return mp_cmp_mag(a, b);
    }
}

/* compare maginitude of two ints (unsigned) */
int mp_cmp_mag (mp_int * a, mp_int * b)
{
    unsigned int     n;
    mp_digit *tmpa, *tmpb;

    /* compare based on # of non-zero digits */
    if (a->used > b->used)
    {
        return MP_GT;
    }

    if (a->used < b->used)
    {
        return MP_LT;
    }

    /* alias for a */
    tmpa = a->dp + (a->used - 1);

    /* alias for b */
    tmpb = b->dp + (a->used - 1);

    /* compare based on digits  */
    for (n = 0; n < a->used; ++n, --tmpa, --tmpb)
    {
        if (*tmpa > *tmpb)
        {
            return MP_GT;
        }

        if (*tmpa < *tmpb)
        {
            return MP_LT;
        }
    }
    return MP_EQ;
}

/* compare a digit */
int mp_cmp_d (mp_int * a, mp_digit b)
{
    /* compare based on sign */
    if (a->sign == MP_NEG)
    {
        return MP_LT;
    }

    /* compare based on magnitude */
    if (a->used > 1)
    {
        return MP_GT;
    }

    /* compare the only digit of a to b */
    if (a->dp[0] > b)
    {
        return MP_GT;
    }
    else if (a->dp[0] < b)
    {
        return MP_LT;
    }
    else
    {
        return MP_EQ;
    }
}

/* this is a shell function that calls either the normal or Montgomery
* exptmod functions.  Originally the call to the montgomery code was
* embedded in the normal function but that wasted alot of stack space
* for nothing (since 99% of the time the Montgomery code would be called)
*/
int mp_exptmod (mp_int * G, mp_int * X, mp_int * P, mp_int * Y)
{
    int dr;

    /* modulus P must be positive */
    if (P->sign == MP_NEG)
    {
        return MP_VAL;
    }

    /* if exponent X is negative we have to recurse */
    if (X->sign == MP_NEG)
    {
        mp_int tmpG, tmpX;
        int err;

        /* first compute 1/G mod P */
        if ((err = mp_init(&tmpG)) != MP_OKAY)
        {
            return err;
        }
        if ((err = mp_invmod(G, P, &tmpG)) != MP_OKAY)
        {
            mp_clear(&tmpG);
            return err;
        }

        /* now get |X| */
        if ((err = mp_init(&tmpX)) != MP_OKAY)
        {
            mp_clear(&tmpG);
            return err;
        }
        if ((err = mp_abs(X, &tmpX)) != MP_OKAY)
        {
            mp_clear_multi(&tmpG, &tmpX, NULL);
            return err;
        }

        /* and now compute (1/G)**|X| instead of G**X [X < 0] */
        err = mp_exptmod(&tmpG, &tmpX, P, Y);
        mp_clear_multi(&tmpG, &tmpX, NULL);
        return err;
    }

    /* is it a DR modulus? */
    dr = mp_dr_is_modulus(P);

    /* if not, is it a uDR modulus? */
    if (dr == 0)
    {
        dr = mp_reduce_is_2k(P) << 1;
    }


    /* if the modulus is odd or dr != 0 use the fast method */
    if (mp_isodd (P) == 1 || dr !=  0)
    {
        return mp_exptmod_fast (G, X, P, Y, dr);
    }
    else
    {
        /* otherwise use the generic Barrett reduction technique */
        return s_mp_exptmod (G, X, P, Y);
    }
}

int mp_invmod (mp_int * a, mp_int * b, mp_int * c)
{
    mp_int  x, y, u, v, A, B, C, D;
    int     res;

    /* b cannot be negative */
    if (b->sign == MP_NEG || mp_iszero(b) == 1)
    {
        return MP_VAL;
    }

    /* if the modulus is odd we can use a faster routine instead */
    if (mp_isodd (b) == 1)
    {
        return fast_mp_invmod (a, b, c);
    }

    /* init temps */
    if ((res = mp_init_multi(&x, &y, &u, &v,
                             &A, &B, &C, &D, NULL)) != MP_OKAY)
    {
        return res;
    }

    /* x = a, y = b */
    if ((res = mp_copy (a, &x)) != MP_OKAY)
    {
        goto __ERR;
    }
    if ((res = mp_copy (b, &y)) != MP_OKAY)
    {
        goto __ERR;
    }

    /* 2. [modified] if x,y are both even then return an error! */
    if (mp_iseven (&x) == 1 && mp_iseven (&y) == 1)
    {
        res = MP_VAL;
        goto __ERR;
    }

    /* 3. u=x, v=y, A=1, B=0, C=0,D=1 */
    if ((res = mp_copy (&x, &u)) != MP_OKAY)
    {
        goto __ERR;
    }
    if ((res = mp_copy (&y, &v)) != MP_OKAY)
    {
        goto __ERR;
    }
    mp_set (&A, 1);
    mp_set (&D, 1);

top:
    /* 4.  while u is even do */
    while (mp_iseven (&u) == 1)
    {
        /* 4.1 u = u/2 */
        if ((res = mp_div_2 (&u, &u)) != MP_OKAY)
        {
            goto __ERR;
        }
        /* 4.2 if A or B is odd then */
        if (mp_isodd (&A) == 1 || mp_isodd (&B) == 1)
        {
            /* A = (A+y)/2, B = (B-x)/2 */
            if ((res = mp_add (&A, &y, &A)) != MP_OKAY)
            {
                goto __ERR;
            }
            if ((res = mp_sub (&B, &x, &B)) != MP_OKAY)
            {
                goto __ERR;
            }
        }
        /* A = A/2, B = B/2 */
        if ((res = mp_div_2 (&A, &A)) != MP_OKAY)
        {
            goto __ERR;
        }
        if ((res = mp_div_2 (&B, &B)) != MP_OKAY)
        {
            goto __ERR;
        }
    }

    /* 5.  while v is even do */
    while (mp_iseven (&v) == 1)
    {
        /* 5.1 v = v/2 */
        if ((res = mp_div_2 (&v, &v)) != MP_OKAY)
        {
            goto __ERR;
        }
        /* 5.2 if C or D is odd then */
        if (mp_isodd (&C) == 1 || mp_isodd (&D) == 1)
        {
            /* C = (C+y)/2, D = (D-x)/2 */
            if ((res = mp_add (&C, &y, &C)) != MP_OKAY)
            {
                goto __ERR;
            }
            if ((res = mp_sub (&D, &x, &D)) != MP_OKAY)
            {
                goto __ERR;
            }
        }
        /* C = C/2, D = D/2 */
        if ((res = mp_div_2 (&C, &C)) != MP_OKAY)
        {
            goto __ERR;
        }
        if ((res = mp_div_2 (&D, &D)) != MP_OKAY)
        {
            goto __ERR;
        }
    }

    /* 6.  if u >= v then */
    if (mp_cmp (&u, &v) != MP_LT)
    {
        /* u = u - v, A = A - C, B = B - D */
        if ((res = mp_sub (&u, &v, &u)) != MP_OKAY)
        {
            goto __ERR;
        }

        if ((res = mp_sub (&A, &C, &A)) != MP_OKAY)
        {
            goto __ERR;
        }

        if ((res = mp_sub (&B, &D, &B)) != MP_OKAY)
        {
            goto __ERR;
        }
    }
    else
    {
        /* v - v - u, C = C - A, D = D - B */
        if ((res = mp_sub (&v, &u, &v)) != MP_OKAY)
        {
            goto __ERR;
        }

        if ((res = mp_sub (&C, &A, &C)) != MP_OKAY)
        {
            goto __ERR;
        }

        if ((res = mp_sub (&D, &B, &D)) != MP_OKAY)
        {
            goto __ERR;
        }
    }

    /* if not zero goto step 4 */
    if (mp_iszero (&u) == 0)
        goto top;

    /* now a = C, b = D, gcd == g*v */

    /* if v != 1 then there is no inverse */
    if (mp_cmp_d (&v, 1) != MP_EQ)
    {
        res = MP_VAL;
        goto __ERR;
    }

    /* if its too low */
    while (mp_cmp_d(&C, 0) == MP_LT)
    {
        if ((res = mp_add(&C, b, &C)) != MP_OKAY)
        {
            goto __ERR;
        }
    }

    /* too big */
    while (mp_cmp_mag(&C, b) != MP_LT)
    {
        if ((res = mp_sub(&C, b, &C)) != MP_OKAY)
        {
            goto __ERR;
        }
    }

    /* C is now the inverse */
    mp_exch (&C, c);
    res = MP_OKAY;
__ERR:
    mp_clear_multi (&x, &y, &u, &v, &A, &B, &C, &D, NULL);
    return res;
}

/* computes the modular inverse via binary extended euclidean algorithm,
 * that is c = 1/a mod b
 *
 * Based on mp_invmod except this is optimized for the case where b is
 * odd as per HAC Note 14.64 on pp. 610
 */
int
fast_mp_invmod (mp_int * a, mp_int * b, mp_int * c)
{
    mp_int  x, y, u, v, B, D;
    int     res, neg;

    /* 2. [modified] if a,b are both even then return an error!
     *
     * That is if gcd(a,b) = 2**k * q then obviously there is no inverse.
     */
    if (mp_iseven (a) == 1 && mp_iseven (b) == 1)
    {
        return MP_VAL;
    }

    /* init all our temps */
    if ((res = mp_init_multi(&x, &y, &u, &v, &B, &D, NULL)) != MP_OKAY)
    {
        return res;
    }

    /* x == modulus, y == value to invert */
    if ((res = mp_copy (b, &x)) != MP_OKAY)
    {
        goto __ERR;
    }

    /* we need y = |a| */
    if ((res = mp_abs (a, &y)) != MP_OKAY)
    {
        goto __ERR;
    }

    /* 3. u=x, v=y, A=1, B=0, C=0,D=1 */
    if ((res = mp_copy (&x, &u)) != MP_OKAY)
    {
        goto __ERR;
    }
    if ((res = mp_copy (&y, &v)) != MP_OKAY)
    {
        goto __ERR;
    }
    mp_set (&D, 1);

top:
    /* 4.  while u is even do */
    while (mp_iseven (&u) == 1)
    {
        /* 4.1 u = u/2 */
        if ((res = mp_div_2 (&u, &u)) != MP_OKAY)
        {
            goto __ERR;
        }
        /* 4.2 if B is odd then */
        if (mp_isodd (&B) == 1)
        {
            if ((res = mp_sub (&B, &x, &B)) != MP_OKAY)
            {
                goto __ERR;
            }
        }
        /* B = B/2 */
        if ((res = mp_div_2 (&B, &B)) != MP_OKAY)
        {
            goto __ERR;
        }
    }

    /* 5.  while v is even do */
    while (mp_iseven (&v) == 1)
    {
        /* 5.1 v = v/2 */
        if ((res = mp_div_2 (&v, &v)) != MP_OKAY)
        {
            goto __ERR;
        }
        /* 5.2 if D is odd then */
        if (mp_isodd (&D) == 1)
        {
            /* D = (D-x)/2 */
            if ((res = mp_sub (&D, &x, &D)) != MP_OKAY)
            {
                goto __ERR;
            }
        }
        /* D = D/2 */
        if ((res = mp_div_2 (&D, &D)) != MP_OKAY)
        {
            goto __ERR;
        }
    }

    /* 6.  if u >= v then */
    if (mp_cmp (&u, &v) != MP_LT)
    {
        /* u = u - v, B = B - D */
        if ((res = mp_sub (&u, &v, &u)) != MP_OKAY)
        {
            goto __ERR;
        }

        if ((res = mp_sub (&B, &D, &B)) != MP_OKAY)
        {
            goto __ERR;
        }
    }
    else
    {
        /* v - v - u, D = D - B */
        if ((res = mp_sub (&v, &u, &v)) != MP_OKAY)
        {
            goto __ERR;
        }

        if ((res = mp_sub (&D, &B, &D)) != MP_OKAY)
        {
            goto __ERR;
        }
    }

    /* if not zero goto step 4 */
    if (mp_iszero (&u) == 0)
    {
        goto top;
    }

    /* now a = C, b = D, gcd == g*v */

    /* if v != 1 then there is no inverse */
    if (mp_cmp_d (&v, 1) != MP_EQ)
    {
        res = MP_VAL;
        goto __ERR;
    }

    /* b is now the inverse */
    neg = a->sign;
    while (D.sign == MP_NEG)
    {
        if ((res = mp_add (&D, b, &D)) != MP_OKAY)
        {
            goto __ERR;
        }
    }
    mp_exch (&D, c);
    c->sign = neg;
    res = MP_OKAY;

__ERR:
    mp_clear_multi (&x, &y, &u, &v, &B, &D, NULL);
    return res;
}

/* b = |a|
*
* Simple function copies the input and fixes the sign to positive
*/
int
mp_abs (mp_int * a, mp_int * b)
{
    int     res;

    /* copy a to b */
    if (a != b)
    {
        if ((res = mp_copy (a, b)) != MP_OKAY)
        {
            return res;
        }
    }

    /* force the sign of b to positive */
    b->sign = MP_ZPOS;

    return MP_OKAY;
}

void mp_clear_multi(mp_int *mp, ...)
{
    mp_int* next_mp = mp;
    va_list args;
    va_start(args, mp);
    while (next_mp != NULL)
    {
        mp_clear(next_mp);
        next_mp = va_arg(args, mp_int*);
    }
    va_end(args);
}

/* determines if a number is a valid DR modulus */
int mp_dr_is_modulus(mp_int *a)
{
    unsigned int ix;

    /* must be at least two digits */
    if (a->used < 2)
    {
        return 0;
    }

    /* must be of the form b**k - a [a <= b] so all
       * but the first digit must be equal to -1 (mod b).
       */
    for (ix = 1; ix < a->used; ix++)
    {
        if (a->dp[ix] != MP_MASK)
        {
            return 0;
        }
    }
    return 1;
}

/* determines if mp_reduce_2k can be used */
int
mp_reduce_is_2k(mp_int *a)
{
    int ix, iy;

    if (a->used == 0)
    {
        return 0;
    }
    else if (a->used == 1)
    {
        return 1;
    }
    else if (a->used > 1)
    {
        iy = mp_count_bits(a);
        for (ix = DIGIT_BIT; ix < iy; ix++)
        {
            if ((a->dp[ix/DIGIT_BIT] &
                    ((mp_digit)1 << (mp_digit)(ix % DIGIT_BIT))) == 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

int mp_exptmod_fast (mp_int * G, mp_int * X, mp_int * P, mp_int * Y, int redmode)
{
    mp_int * M;
    mp_int  res;
    mp_digit buf, mp;
    int     err, bitbuf, bitcpy, bitcnt, mode, digidx, x, y, winsize;

    /* use a pointer to the reduction algorithm.  This allows us to use
     * one of many reduction algorithms without modding the guts of
     * the code with if statements everywhere.
     */
    int     (*redux)(mp_int*,mp_int*,mp_digit);

    /*改为动态申请*/
    M = (mp_int*)XOS_GetUB(TAB_SIZE * sizeof(mp_int));
    if (NULL == M)
    {
        return  0;
    }
    memset((mp_int*)M, 0, TAB_SIZE * sizeof(mp_int));

    /* find window size */
    x = mp_count_bits (X);
    if (x <= 7)
    {
        winsize = 2;
    }
    else if (x <= 36)
    {
        winsize = 3;
    }
    else if (x <= 140)
    {
        winsize = 4;
    }
    else if (x <= 450)
    {
        winsize = 5;
    }
    else if (x <= 1303)
    {
        winsize = 6;
    }
    else if (x <= 3529)
    {
        winsize = 7;
    }
    else
    {
        winsize = 8;
    }

#ifdef MP_LOW_MEM
    if (winsize > 5)
    {
        winsize = 5;
    }
#endif

    /* init M array */
    /* init first cell */
    if ((err = mp_init(&M[1])) != MP_OKAY)
    {
        XOS_RetUB((BYTE *)M);
        return err;
    }

    /* now init the second half of the array */
    for (x = 1<<(winsize-1); x < (1 << winsize); x++)
    {
        if ((err = mp_init(&M[x])) != MP_OKAY)
        {
            for (y = 1<<(winsize-1); y < x; y++)
            {
                mp_clear (&M[y]);
            }
            mp_clear(&M[1]);
            XOS_RetUB((BYTE *)M);
            return err;
        }
    }

    /* determine and setup reduction code */
    if (redmode == 0)
    {
        /* now setup montgomery  */
        if ((err = mp_montgomery_setup (P, &mp)) != MP_OKAY)
        {
            goto __M;
        }

        /* automatically pick the comba one if available (saves quite a few calls/ifs) */
        if (((P->used * 2 + 1) < MP_WARRAY) &&
                P->used < (1 << ((SSH_CHAR_BIT * sizeof (WORD64)) - (2 * DIGIT_BIT))))
        {
            redux = fast_mp_montgomery_reduce;
        }
        else
        {
            /* use slower baseline Montgomery method */
            redux = mp_montgomery_reduce;
        }
    }
    else if (redmode == 1)
    {
        /* setup DR reduction for moduli of the form B**k - b */
        mp_dr_setup(P, &mp);
        redux = mp_dr_reduce;
    }
    else
    {
        /* setup DR reduction for moduli of the form 2**k - b */
        if ((err = mp_reduce_2k_setup(P, &mp)) != MP_OKAY)
        {
            goto __M;
        }
        redux = mp_reduce_2k;
    }

    /* setup result */
    if ((err = mp_init (&res)) != MP_OKAY)
    {
        goto __M;
    }

    /* create M table
     *
     * The M table contains powers of the input base, e.g. M[x] = G^x mod P
     *
     * The first half of the table is not computed though accept for M[0] and M[1]
     */

    if (redmode == 0)
    {
        /* now we need R mod m */
        if ((err = mp_montgomery_calc_normalization (&res, P)) != MP_OKAY)
        {
            goto __RES;
        }

        /* now set M[1] to G * R mod m */
        if ((err = mp_mulmod (G, &res, P, &M[1])) != MP_OKAY)
        {
            goto __RES;
        }
    }
    else
    {
        mp_set(&res, 1);
        if ((err = mp_mod(G, P, &M[1])) != MP_OKAY)
        {
            goto __RES;
        }
    }

    /* compute the value at M[1<<(winsize-1)] by squaring M[1] (winsize-1) times */
    if ((err = mp_copy (&M[1], &M[1 << (winsize - 1)])) != MP_OKAY)
    {
        goto __RES;
    }

    for (x = 0; x < (winsize - 1); x++)
    {
        if ((err = mp_sqr (&M[1 << (winsize - 1)], &M[1 << (winsize - 1)])) != MP_OKAY)
        {
            goto __RES;
        }
        if ((err = redux (&M[1 << (winsize - 1)], P, mp)) != MP_OKAY)
        {
            goto __RES;
        }
    }

    /* create upper table */
    for (x = (1 << (winsize - 1)) + 1; x < (1 << winsize); x++)
    {
        if ((err = mp_mul (&M[x - 1], &M[1], &M[x])) != MP_OKAY)
        {
            goto __RES;
        }
        if ((err = redux (&M[x], P, mp)) != MP_OKAY)
        {
            goto __RES;
        }
    }

    /* set initial mode and bit cnt */
    mode   = 0;
    bitcnt = 1;
    buf    = 0;
    digidx = X->used - 1;
    bitcpy = 0;
    bitbuf = 0;

    for (;;)
    {
        /* grab next digit as required */
        if (--bitcnt == 0)
        {
            /* if digidx == -1 we are out of digits so break */
            if (digidx == -1)
            {
                break;
            }
            /* read next digit and reset bitcnt */
            buf    = X->dp[digidx--];
            bitcnt = (int)DIGIT_BIT;
        }

        /* grab the next msb from the exponent */
        y     = (mp_digit)(buf >> (DIGIT_BIT - 1)) & 1;
        buf <<= (mp_digit)1;

        /* if the bit is zero and mode == 0 then we ignore it
         * These represent the leading zero bits before the first 1 bit
         * in the exponent.  Technically this opt is not required but it
         * does lower the # of trivial squaring/reductions used
         */
        if (mode == 0 && y == 0)
        {
            continue;
        }

        /* if the bit is zero and mode == 1 then we square */
        if (mode == 1 && y == 0)
        {
            if ((err = mp_sqr (&res, &res)) != MP_OKAY)
            {
                goto __RES;
            }
            if ((err = redux (&res, P, mp)) != MP_OKAY)
            {
                goto __RES;
            }
            continue;
        }

        /* else we add it to the window */
        bitbuf |= (y << (winsize - ++bitcpy));
        mode    = 2;

        if (bitcpy == winsize)
        {
            /* ok window is filled so square as required and multiply  */
            /* square first */
            for (x = 0; x < winsize; x++)
            {
                if ((err = mp_sqr (&res, &res)) != MP_OKAY)
                {
                    goto __RES;
                }
                if ((err = redux (&res, P, mp)) != MP_OKAY)
                {
                    goto __RES;
                }
            }

            /* then multiply */
            if ((err = mp_mul (&res, &M[bitbuf], &res)) != MP_OKAY)
            {
                goto __RES;
            }
            if ((err = redux (&res, P, mp)) != MP_OKAY)
            {
                goto __RES;
            }

            /* empty window and reset */
            bitcpy = 0;
            bitbuf = 0;
            mode   = 1;
        }
    }

    /* if bits remain then square/multiply */
    if (mode == 2 && bitcpy > 0)
    {
        /* square then multiply if the bit is set */
        for (x = 0; x < bitcpy; x++)
        {
            if ((err = mp_sqr (&res, &res)) != MP_OKAY)
            {
                goto __RES;
            }
            if ((err = redux (&res, P, mp)) != MP_OKAY)
            {
                goto __RES;
            }

            /* get next bit of the window */
            bitbuf <<= 1;
            if ((bitbuf & (1 << winsize)) != 0)
            {
                /* then multiply */
                if ((err = mp_mul (&res, &M[1], &res)) != MP_OKAY)
                {
                    goto __RES;
                }
                if ((err = redux (&res, P, mp)) != MP_OKAY)
                {
                    goto __RES;
                }
            }
        }
    }

    if (redmode == 0)
    {
        /* fixup result if Montgomery reduction is used
         * recall that any value in a Montgomery system is
         * actually multiplied by R mod n.  So we have
         * to reduce one more time to cancel out the factor
         * of R.
         */
        if ((err = mp_montgomery_reduce (&res, P, mp)) != MP_OKAY)
        {
            goto __RES;
        }
    }

    /* swap res with Y */
    mp_exch (&res, Y);
    err = MP_OKAY;
__RES:
    mp_clear (&res);
__M:
    mp_clear(&M[1]);
    for (x = 1<<(winsize-1); x < (1 << winsize); x++)
    {
        mp_clear (&M[x]);
    }

    XOS_RetUB((BYTE *)M);
    return err;
}

int
s_mp_exptmod (mp_int * G, mp_int * X, mp_int * P, mp_int * Y)
{
    mp_int  M[TAB_SIZE], res, mu;
    mp_digit buf;
    int     err, bitbuf, bitcpy, bitcnt, mode, digidx, x, y, winsize;

    /* find window size */
    x = mp_count_bits (X);
    if (x <= 7)
    {
        winsize = 2;
    }
    else if (x <= 36)
    {
        winsize = 3;
    }
    else if (x <= 140)
    {
        winsize = 4;
    }
    else if (x <= 450)
    {
        winsize = 5;
    }
    else if (x <= 1303)
    {
        winsize = 6;
    }
    else if (x <= 3529)
    {
        winsize = 7;
    }
    else
    {
        winsize = 8;
    }

#ifdef MP_LOW_MEM
    if (winsize > 5)
    {
        winsize = 5;
    }
#endif

    /* init M array */
    /* init first cell */
    if ((err = mp_init(&M[1])) != MP_OKAY)
    {
        return err;
    }

    /* now init the second half of the array */
    for (x = 1<<(winsize-1); x < (1 << winsize); x++)
    {
        if ((err = mp_init(&M[x])) != MP_OKAY)
        {
            for (y = 1<<(winsize-1); y < x; y++)
            {
                mp_clear (&M[y]);
            }
            mp_clear(&M[1]);
            return err;
        }
    }

    /* create mu, used for Barrett reduction */
    if ((err = mp_init (&mu)) != MP_OKAY)
    {
        goto __M;
    }
    if ((err = mp_reduce_setup (&mu, P)) != MP_OKAY)
    {
        goto __MU;
    }

    /* create M table
     *
     * The M table contains powers of the base,
     * e.g. M[x] = G**x mod P
     *
     * The first half of the table is not
     * computed though accept for M[0] and M[1]
     */
    if ((err = mp_mod (G, P, &M[1])) != MP_OKAY)
    {
        goto __MU;
    }

    /* compute the value at M[1<<(winsize-1)] by squaring
     * M[1] (winsize-1) times
     */
    if ((err = mp_copy (&M[1], &M[1 << (winsize - 1)])) != MP_OKAY)
    {
        goto __MU;
    }

    for (x = 0; x < (winsize - 1); x++)
    {
        if ((err = mp_sqr (&M[1 << (winsize - 1)],
                           &M[1 << (winsize - 1)])) != MP_OKAY)
        {
            goto __MU;
        }
        if ((err = mp_reduce (&M[1 << (winsize - 1)], P, &mu)) != MP_OKAY)
        {
            goto __MU;
        }
    }

    /* create upper table, that is M[x] = M[x-1] * M[1] (mod P)
     * for x = (2**(winsize - 1) + 1) to (2**winsize - 1)
     */
    for (x = (1 << (winsize - 1)) + 1; x < (1 << winsize); x++)
    {
        if ((err = mp_mul (&M[x - 1], &M[1], &M[x])) != MP_OKAY)
        {
            goto __MU;
        }
        if ((err = mp_reduce (&M[x], P, &mu)) != MP_OKAY)
        {
            goto __MU;
        }
    }

    /* setup result */
    if ((err = mp_init (&res)) != MP_OKAY)
    {
        goto __MU;
    }
    mp_set (&res, 1);

    /* set initial mode and bit cnt */
    mode   = 0;
    bitcnt = 1;
    buf    = 0;
    digidx = X->used - 1;
    bitcpy = 0;
    bitbuf = 0;

    for (;;)
    {
        /* grab next digit as required */
        if (--bitcnt == 0)
        {
            /* if digidx == -1 we are out of digits */
            if (digidx == -1)
            {
                break;
            }
            /* read next digit and reset the bitcnt */
            buf    = X->dp[digidx--];
            bitcnt = (int) DIGIT_BIT;
        }

        /* grab the next msb from the exponent */
        y     = (buf >> (mp_digit)(DIGIT_BIT - 1)) & 1;
        buf <<= (mp_digit)1;

        /* if the bit is zero and mode == 0 then we ignore it
         * These represent the leading zero bits before the first 1 bit
         * in the exponent.  Technically this opt is not required but it
         * does lower the # of trivial squaring/reductions used
         */
        if (mode == 0 && y == 0)
        {
            continue;
        }

        /* if the bit is zero and mode == 1 then we square */
        if (mode == 1 && y == 0)
        {
            if ((err = mp_sqr (&res, &res)) != MP_OKAY)
            {
                goto __RES;
            }
            if ((err = mp_reduce (&res, P, &mu)) != MP_OKAY)
            {
                goto __RES;
            }
            continue;
        }

        /* else we add it to the window */
        bitbuf |= (y << (winsize - ++bitcpy));
        mode    = 2;

        if (bitcpy == winsize)
        {
            /* ok window is filled so square as required and multiply  */
            /* square first */
            for (x = 0; x < winsize; x++)
            {
                if ((err = mp_sqr (&res, &res)) != MP_OKAY)
                {
                    goto __RES;
                }
                if ((err = mp_reduce (&res, P, &mu)) != MP_OKAY)
                {
                    goto __RES;
                }
            }

            /* then multiply */
            if ((err = mp_mul (&res, &M[bitbuf], &res)) != MP_OKAY)
            {
                goto __MU;
            }
            if ((err = mp_reduce (&res, P, &mu)) != MP_OKAY)
            {
                goto __MU;
            }

            /* empty window and reset */
            bitcpy = 0;
            bitbuf = 0;
            mode   = 1;
        }
    }

    /* if bits remain then square/multiply */
    if (mode == 2 && bitcpy > 0)
    {
        /* square then multiply if the bit is set */
        for (x = 0; x < bitcpy; x++)
        {
            if ((err = mp_sqr (&res, &res)) != MP_OKAY)
            {
                goto __RES;
            }
            if ((err = mp_reduce (&res, P, &mu)) != MP_OKAY)
            {
                goto __RES;
            }

            bitbuf <<= 1;
            if ((bitbuf & (1 << winsize)) != 0)
            {
                /* then multiply */
                if ((err = mp_mul (&res, &M[1], &res)) != MP_OKAY)
                {
                    goto __RES;
                }
                if ((err = mp_reduce (&res, P, &mu)) != MP_OKAY)
                {
                    goto __RES;
                }
            }
        }
    }

    mp_exch (&res, Y);
    err = MP_OKAY;
__RES:
    mp_clear (&res);
__MU:
    mp_clear (&mu);
__M:
    mp_clear(&M[1]);
    for (x = 1<<(winsize-1); x < (1 << winsize); x++)
    {
        mp_clear (&M[x]);
    }
    return err;
}

/* store in unsigned [big endian] format */
int mp_to_unsigned_bin (mp_int * a, unsigned char *b)
{
    int     x, res;
    mp_int  t;

    if ((res = mp_init_copy (&t, a)) != MP_OKAY)
    {
        return res;
    }

    x = 0;
    while (mp_iszero (&t) == 0)
    {
        b[x++] = (unsigned char) (t.dp[0] & 255);
        if ((res = mp_div_2d (&t, 8, &t, NULL)) != MP_OKAY)
        {
            mp_clear (&t);
            return res;
        }
    }
    bn_reverse (b, x);
    mp_clear (&t);
    return MP_OKAY;
}
/* reverse an array, used for radix code */
void bn_reverse (unsigned char *s, int len)
{
    int     ix, iy;
    unsigned char t;

    ix = 0;
    iy = len - 1;
    while (ix < iy)
    {
        t     = s[ix];
        s[ix] = s[iy];
        s[iy] = t;
        ++ix;
        --iy;
    }
}

/* swap the elements of two integers, for cases where you can't simply swap the
* mp_int pointers around
*/
void
mp_exch (mp_int * a, mp_int * b)
{
    mp_int  t;

    t  = *a;
    *a = *b;
    *b = t;
}
/* high level subtraction (handles signs) */
int
mp_sub (mp_int * a, mp_int * b, mp_int * c)
{
    int     sa, sb, res;

    sa = a->sign;
    sb = b->sign;

    if (sa != sb)
    {
        /* subtract a negative from a positive, OR */
        /* subtract a positive from a negative. */
        /* In either case, ADD their magnitudes, */
        /* and use the sign of the first number. */
        c->sign = sa;
        res = s_mp_add (a, b, c);
    }
    else
    {
        /* subtract a positive from a positive, OR */
        /* subtract a negative from a negative. */
        /* First, take the difference between their */
        /* magnitudes, then... */
        if (mp_cmp_mag (a, b) != MP_LT)
        {
            /* Copy the sign from the first */
            c->sign = sa;
            /* The first has a larger or equal magnitude */
            res = s_mp_sub (a, b, c);
        }
        else
        {
            /* The result has the *opposite* sign from */
            /* the first number. */
            c->sign = (sa == MP_ZPOS) ? MP_NEG : MP_ZPOS;
            /* The second has a larger magnitude */
            res = s_mp_sub (b, a, c);
        }
    }
    return res;
}

/* high level addition (handles signs) */
int
mp_add (mp_int * a, mp_int * b, mp_int * c)
{
    int     sa, sb, res;

    /* get sign of both inputs */
    sa = a->sign;
    sb = b->sign;

    /* handle two cases, not four */
    if (sa == sb)
    {
        /* both positive or both negative */
        /* add their magnitudes, copy the sign */
        c->sign = sa;
        res = s_mp_add (a, b, c);
    }
    else
    {
        /* one positive, the other negative */
        /* subtract the one with the greater magnitude from */
        /* the one of the lesser magnitude.  The result gets */
        /* the sign of the one with the greater magnitude. */
        if (mp_cmp_mag (a, b) == MP_LT)
        {
            c->sign = sb;
            res = s_mp_sub (b, a, c);
        }
        else
        {
            c->sign = sa;
            res = s_mp_sub (a, b, c);
        }
    }
    return res;
}

/* set to a digit */
void
mp_set (mp_int * a, mp_digit b)
{
    mp_zero (a);
    a->dp[0] = b & MP_MASK;
    a->used  = (a->dp[0] != 0) ? 1 : 0;
}

int mp_init_multi(mp_int *mp, ...)
{
    int res = MP_OKAY;      /* Assume ok until proven otherwise */
    int n = 0;                 /* Number of ok inits */
    mp_int* cur_arg = mp;
    va_list args;

    va_start(args, mp);        /* init args to next argument from caller */
    while (cur_arg != NULL)
    {
        if (mp_init(cur_arg) != MP_OKAY)
        {
            /* Oops - error! Back-track and mp_clear what we already
            succeeded in init-ing, then return error.
                      */
            va_list clean_args;

            /* end the current list */
            va_end(args);

            /* now start cleaning up */
            cur_arg = mp;
            va_start(clean_args, mp);
            while (n--)
            {
                mp_clear(cur_arg);
                cur_arg = va_arg(clean_args, mp_int*);
            }
            va_end(clean_args);
            res = MP_MEM;
            break;
        }
        n++;
        cur_arg = va_arg(args, mp_int*);
    }
    va_end(args);
    return res;                /* Assumed ok, if error flagged above. */
}

/* configured for a AMD XP Thoroughbred core with etc/tune.c */
int     KARATSUBA_MUL_CUTOFF = 109,      /* Min. number of digits before Karatsuba multiplication is used. */
                               KARATSUBA_SQR_CUTOFF = 127,      /* Min. number of digits before Karatsuba squaring is used. */

                                                      TOOM_MUL_CUTOFF      = 350,      /* no optimal values of these are known yet so set em high */
                                                                             TOOM_SQR_CUTOFF      = 400;

/* high level multiplication (handles sign) */
int
mp_mul (mp_int * a, mp_int * b, mp_int * c)
{
    int     res, neg;
    neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;

    /* use Toom-Cook? */
    if (MIN ((int)(a->used), (int)(b->used)) >= TOOM_MUL_CUTOFF)
    {
        res = mp_toom_mul(a, b, c);
        /* use Karatsuba? */
    }
    else if (MIN ((int)(a->used), (int)(b->used)) >= KARATSUBA_MUL_CUTOFF)
    {
        res = mp_karatsuba_mul (a, b, c);
    }
    else
    {
        /* can we use the fast multiplier?
        *
        * The fast multiplier can be used if the output will
        * have less than MP_WARRAY digits and the number of
        * digits won't affect carry propagation
        */
        int     digs = a->used + b->used + 1;

        if ((digs < MP_WARRAY) &&
                MIN(a->used, b->used) <=
                (1 << ((SSH_CHAR_BIT * sizeof (WORD64)) - (2 * DIGIT_BIT))))
        {
            res = fast_s_mp_mul_digs (a, b, c, digs);
        }
        else
        {
            res = s_mp_mul (a, b, c);
        }

    }
    c->sign = neg;
    return res;
}

/* computes b = a*a */
int
mp_sqr (mp_int * a, mp_int * b)
{
    int     res;
    /* use Toom-Cook? */
    if ((int)(a->used) >= TOOM_SQR_CUTOFF)
    {
        res = mp_toom_sqr(a, b);
        /* Karatsuba? */
    }
    else if ((int)(a->used) >= KARATSUBA_SQR_CUTOFF)
    {
        res = mp_karatsuba_sqr (a, b);
    }
    else
    {
        /* can we use the fast comba multiplier? */
        if ((a->used * 2 + 1) < MP_WARRAY &&
                a->used <
                (1 << (sizeof(WORD64) * SSH_CHAR_BIT - 2*DIGIT_BIT - 1)))
        {
            res = fast_s_mp_sqr (a, b);
        }
        else
        {
            res = s_mp_sqr (a, b);
        }
    }
    b->sign = MP_ZPOS;
    return res;
}

/* c = a mod b, 0 <= c < b */
int
mp_mod (mp_int * a, mp_int * b, mp_int * c)
{
    mp_int  t;
    int     res;

    if ((res = mp_init (&t)) != MP_OKAY)
    {
        return res;
    }

    if ((res = mp_div (a, b, NULL, &t)) != MP_OKAY)
    {
        mp_clear (&t);
        return res;
    }

    if (t.sign == MP_NEG)
    {
        res = mp_add (b, &t, c);
    }
    else
    {
        res = MP_OKAY;
        mp_exch (&t, c);
    }

    mp_clear (&t);
    return res;
}

/* d = a * b (mod c) */
int
mp_mulmod (mp_int * a, mp_int * b, mp_int * c, mp_int * d)
{
    int     res;
    mp_int  t;

    if ((res = mp_init (&t)) != MP_OKAY)
    {
        return res;
    }

    if ((res = mp_mul (a, b, &t)) != MP_OKAY)
    {
        mp_clear (&t);
        return res;
    }
    res = mp_mod (&t, c, d);
    mp_clear (&t);
    return res;
}

/* calculates a = B^n mod b for Montgomery reduction
* Where B is the base [e.g. 2^DIGIT_BIT].
* B^n mod b is computed by first computing
* A = B^(n-1) which doesn't require a reduction but a simple OR.
* then C = A * B = B^n is computed by performing upto DIGIT_BIT
* shifts with subtractions when the result is greater than b.
*
* The method is slightly modified to shift B unconditionally upto just under
* the leading bit of b.  This saves alot of multiple precision shifting.
*/
int
mp_montgomery_calc_normalization (mp_int * a, mp_int * b)
{
    int     x, bits, res;

    /* how many bits of last digit does b use */
    bits = mp_count_bits (b) % DIGIT_BIT;

    /* compute A = B^(n-1) * 2^(bits-1) */
    if ((res = mp_2expt (a, (b->used - 1) * DIGIT_BIT + bits - 1)) != MP_OKAY)
    {
        return res;
    }

    /* now compute C = A * B mod b */
    for (x = bits - 1; x < (int)DIGIT_BIT; x++)
    {
        if ((res = mp_mul_2 (a, a)) != MP_OKAY)
        {
            return res;
        }
        if (mp_cmp_mag (a, b) != MP_LT)
        {
            if ((res = s_mp_sub (a, b, a)) != MP_OKAY)
            {
                return res;
            }
        }
    }

    return MP_OKAY;
}

/* reduces a modulo n where n is of the form 2**p - k */
int
mp_reduce_2k(mp_int *a, mp_int *n, mp_digit k)
{
    mp_int q;
    int    p, res;

    if ((res = mp_init(&q)) != MP_OKAY)
    {
        return res;
    }

    p = mp_count_bits(n);
top:
    /* q = a/2**p, a = a mod 2**p */
    if ((res = mp_div_2d(a, p, &q, a)) != MP_OKAY)
    {
        goto ERR;
    }

    if (k != 1)
    {
        /* q = q * k */
        if ((res = mp_mul_d(&q, k, &q)) != MP_OKAY)
        {
            goto ERR;
        }
    }

    /* a = a + q */
    if ((res = s_mp_add(a, &q, a)) != MP_OKAY)
    {
        goto ERR;
    }

    if (mp_cmp_mag(a, n) != MP_LT)
    {
        s_mp_sub(a, n, a);
        goto top;
    }

ERR:
    mp_clear(&q);
    return res;
}

/* determines the setup value */
int
mp_reduce_2k_setup(mp_int *a, mp_digit *d)
{
    int res, p;
    mp_int tmp;

    if ((res = mp_init(&tmp)) != MP_OKAY)
    {
        return res;
    }

    p = mp_count_bits(a);
    if ((res = mp_2expt(&tmp, p)) != MP_OKAY)
    {
        mp_clear(&tmp);
        return res;
    }

    if ((res = s_mp_sub(&tmp, a, &tmp)) != MP_OKAY)
    {
        mp_clear(&tmp);
        return res;
    }

    *d = tmp.dp[0];
    mp_clear(&tmp);
    return MP_OKAY;
}

/* reduce "x" in place modulo "n" using the Diminished Radix algorithm.
*
* Based on algorithm from the paper
*
* "Generating Efficient Primes for Discrete Log Cryptosystems"
*                 Chae Hoon Lim, Pil Loong Lee,
*          POSTECH Information Research Laboratories
*
* The modulus must be of a special format [see manual]
*
* Has been modified to use algorithm 7.10 from the LTM book instead
*
* Input x must be in the range 0 <= x <= (n-1)**2
*/
int
mp_dr_reduce (mp_int * x, mp_int * n, mp_digit k)
{
    int      err, i, m;
    WORD64  r;
    mp_digit mu, *tmpx1, *tmpx2;

    /* m = digits in modulus */
    m = n->used;

    /* ensure that "x" has at least 2m digits */
    if ((int)(x->alloc) < m + m)
    {
        if ((err = mp_grow (x, m + m)) != MP_OKAY)
        {
            return err;
        }
    }

    /* top of loop, this is where the code resumes if
    * another reduction pass is required.
    */
top:
    /* aliases for digits */
    /* alias for lower half of x */
    tmpx1 = x->dp;

    /* alias for upper half of x, or x/B**m */
    tmpx2 = x->dp + m;

    /* set carry to zero */
    mu = 0;

    /* compute (x mod B**m) + k * [x/B**m] inline and inplace */
    for (i = 0; i < m; i++)
    {
        r         = ((WORD64)*tmpx2++) * ((WORD64)k) + *tmpx1 + mu;
        *tmpx1++  = (mp_digit)(r & MP_MASK);
        mu        = (mp_digit)(r >> ((WORD64)DIGIT_BIT));
    }

    /* set final carry */
    *tmpx1++ = mu;

    /* zero words above m */
    for (i = m + 1; i < (int)(x->used); i++)
    {
        *tmpx1++ = 0;
    }

    /* clamp, sub and return */
    mp_clamp (x);

    /* if x >= n then subtract and reduce again
    * Each successive "recursion" makes the input smaller and smaller.
    */
    if (mp_cmp_mag (x, n) != MP_LT)
    {
        s_mp_sub(x, n, x);
        goto top;
    }
    return MP_OKAY;
}

/* determines the setup value */
void mp_dr_setup(mp_int *a, mp_digit *d)
{
    /* the casts are required if DIGIT_BIT is one less than
    * the number of bits in a mp_digit [e.g. DIGIT_BIT==31]
        */
    *d = (mp_digit)((((WORD64)1) << ((WORD64)DIGIT_BIT)) -
                    ((WORD64)a->dp[0]));
}

/* computes xR**-1 == x (mod N) via Montgomery Reduction */
int
mp_montgomery_reduce (mp_int * x, mp_int * n, mp_digit rho)
{
    int     ix, res, digs;
    mp_digit mu;

    /* can the fast reduction [comba] method be used?
    *
    * Note that unlike in mp_mul you're safely allowed *less*
    * than the available columns [255 per default] since carries
    * are fixed up in the inner loop.
    */
    digs = n->used * 2 + 1;
    if ((digs < MP_WARRAY) &&
            n->used <
            (1 << ((SSH_CHAR_BIT * sizeof (WORD64)) - (2 * DIGIT_BIT))))
    {
        return fast_mp_montgomery_reduce (x, n, rho);
    }

    /* grow the input as required */
    if ((int)(x->alloc) < digs)
    {
        if ((res = mp_grow (x, digs)) != MP_OKAY)
        {
            return res;
        }
    }
    x->used = digs;

    for (ix = 0; ix < (int)(n->used); ix++)
    {
        /* mu = ai * rho mod b
        *
        * The value of rho must be precalculated via
        * bn_mp_montgomery_setup() such that
        * it equals -1/n0 mod b this allows the
        * following inner loop to reduce the
        * input one digit at a time
        */
        mu = ((mp_word)x->dp[ix]) * ((mp_word)rho) & MP_MASK;

        /* a = a + mu * m * b**i */
        {
            register int iy;
            register mp_digit *tmpn, *tmpx, u;
            register WORD64 r;

            /* alias for digits of the modulus */
            tmpn = n->dp;

            /* alias for the digits of x [the input] */
            tmpx = x->dp + ix;

            /* set the carry to zero */
            u = 0;

            /* Multiply and add in place */
            for (iy = 0; iy < (int)(n->used); iy++)
            {
                /* compute product and sum */
                r       = ((WORD64)mu) * ((WORD64)*tmpn++) +
                          ((WORD64) u) + ((WORD64) * tmpx);

                /* get carry */
                u       = (mp_digit)(r >> ((WORD64) DIGIT_BIT));

                /* fix digit */
                *tmpx++ = (mp_digit)(r & ((WORD64) MP_MASK));
            }
            /* At this point the ix'th digit of x should be zero */


            /* propagate carries upwards as required*/
            while (u)
            {
                *tmpx   += u;
                u        = *tmpx >> DIGIT_BIT;
                *tmpx++ &= MP_MASK;
            }
        }
    }

    /* at this point the n.used'th least
    * significant digits of x are all zero
    * which means we can shift x to the
    * right by n.used digits and the
    * residue is unchanged.
    */

    /* x = x/b**n.used */
    mp_clamp(x);
    mp_rshd (x, n->used);

    /* if x >= n then x = x - n */
    if (mp_cmp_mag (x, n) != MP_LT)
    {
        return s_mp_sub (x, n, x);
    }

    return MP_OKAY;
}

/* computes xR**-1 == x (mod N) via Montgomery Reduction
 *
 * This is an optimized implementation of mp_montgomery_reduce
 * which uses the comba method to quickly calculate the columns of the
 * reduction.
 *
 * Based on Algorithm 14.32 on pp.601 of HAC.
*/
int
fast_mp_montgomery_reduce (mp_int * x, mp_int * n, mp_digit rho)
{
    unsigned int     ix;
    int res, olduse;
    WORD64 W[MP_WARRAY];

    /* get old used count */
    olduse = x->used;

    /* grow a as required */
    if (x->alloc < n->used + 1)
    {
        if ((res = mp_grow (x, n->used + 1)) != MP_OKAY)
        {
            return res;
        }
    }

    /* first we have to get the digits of the input into
     * an array of double precision words W[...]
     */
    {
        register WORD64 *_W;
        register mp_digit *tmpx;

        /* alias for the W[] array */
        _W   = W;

        /* alias for the digits of  x*/
        tmpx = x->dp;

        /* copy the digits of a into W[0..a->used-1] */
        for (ix = 0; ix < x->used; ix++)
        {
            *_W++ = *tmpx++;
        }

        /* zero the high words of W[a->used..m->used*2] */
        for (; ix < n->used * 2 + 1; ix++)
        {
            *_W++ = 0;
        }
    }

    /* now we proceed to zero successive digits
     * from the least significant upwards
     */
    for (ix = 0; ix < n->used; ix++)
    {
        /* mu = ai * m' mod b
         *
         * We avoid a double precision multiplication (which isn't required)
         * by casting the value down to a mp_digit.  Note this requires
         * that W[ix-1] have  the carry cleared (see after the inner loop)
         */
        register mp_digit mu;
        mu = ((W[ix] & MP_MASK) * rho) & MP_MASK;

        /* a = a + mu * m * b**i
         *
         * This is computed in place and on the fly.  The multiplication
         * by b**i is handled by offseting which columns the results
         * are added to.
         *
         * Note the comba method normally doesn't handle carries in the
         * inner loop In this case we fix the carry from the previous
         * column since the Montgomery reduction requires digits of the
         * result (so far) [see above] to work.  This is
         * handled by fixing up one carry after the inner loop.  The
         * carry fixups are done in order so after these loops the
         * first m->used words of W[] have the carries fixed
         */
        {
            unsigned int iy;
            register mp_digit *tmpn;
            register WORD64 *_W;

            /* alias for the digits of the modulus */
            tmpn = n->dp;

            /* Alias for the columns set by an offset of ix */
            _W = W + ix;

            /* inner loop */
            for (iy = 0; iy < n->used; iy++)
            {
                *_W++ += ((WORD64)mu) * ((WORD64)*tmpn++);
            }
        }

        /* now fix carry for next digit, W[ix+1] */
        W[ix + 1] += W[ix] >> ((WORD64) DIGIT_BIT);
    }

    /* now we have to propagate the carries and
     * shift the words downward [all those least
     * significant digits we zeroed].
     */
    {
        register mp_digit *tmpx;
        register WORD64 *_W, *_W1;

        /* nox fix rest of carries */

        /* alias for current word */
        _W1 = W + ix;

        /* alias for next word, where the carry goes */
        _W = W + ++ix;

        for (; ix <= n->used * 2 + 1; ix++)
        {
            *_W++ += *_W1++ >> ((WORD64) DIGIT_BIT);
        }

        /* copy out, A = A/b**n
         *
         * The result is A/b**n but instead of converting from an
         * array of mp_word to mp_digit than calling mp_rshd
         * we just copy them in the right order
         */

        /* alias for destination word */
        tmpx = x->dp;

        /* alias for shifted double precision result */
        _W = W + n->used;

        for (ix = 0; ix < n->used + 1; ix++)
        {
            *tmpx++ = (mp_digit)(*_W++ & ((WORD64) MP_MASK));
        }

        /* zero oldused digits, if the input a was larger than
         * m->used+1 we'll have to clear the digits
         */
        for (; (int)ix < olduse; ix++)
        {
            *tmpx++ = 0;
        }
    }

    /* set the max used and clamp */
    x->used = n->used + 1;
    mp_clamp (x);

    /* if A >= m then A = A - m */
    if (mp_cmp_mag (x, n) != MP_LT)
    {
        return s_mp_sub (x, n, x);
    }
    return MP_OKAY;
}

/* setups the montgomery reduction stuff */
int
mp_montgomery_setup (mp_int * n, mp_digit * rho)
{
    mp_digit x, b;

    /* fast inversion mod 2**k
    *
    * Based on the fact that
    *
    * XA = 1 (mod 2**n)  =>  (X(2-XA)) A = 1 (mod 2**2n)
    *                    =>  2*X*A - X*X*A*A = 1
    *                    =>  2*(1) - (1)     = 1
    */
    b = n->dp[0];

    if ((b & 1) == 0)
    {
        return MP_VAL;
    }

    x = (((b + 2) & 4) << 1) + b; /* here x*a==1 mod 2**4 */
    x *= 2 - b * x;               /* here x*a==1 mod 2**8 */
#if !defined(MP_8BIT)

    x *= 2 - b * x;               /* here x*a==1 mod 2**16 */
#endif
#if defined(MP_64BIT) || !(defined(MP_8BIT) || defined(MP_16BIT))

    x *= 2 - b * x;               /* here x*a==1 mod 2**32 */
#endif
#ifdef MP_64BIT

    x *= 2 - b * x;               /* here x*a==1 mod 2**64 */
#endif

    /* rho = -1/m mod b */
    *rho = (((mp_digit) 1 << ((mp_digit) DIGIT_BIT)) - x) & MP_MASK;

    return MP_OKAY;
}

/* reduces x mod m, assumes 0 < x < m**2, mu is
* precomputed via mp_reduce_setup.
* From HAC pp.604 Algorithm 14.42
*/
int
mp_reduce (mp_int * x, mp_int * m, mp_int * mu)
{
    mp_int  q;
    int     res, um = m->used;

    /* q = x */
    if ((res = mp_init_copy (&q, x)) != MP_OKAY)
    {
        return res;
    }

    /* q1 = x / b**(k-1)  */
    mp_rshd (&q, um - 1);

    /* according to HAC this optimization is ok */
    if (((unsigned long) um) > (((mp_digit)1) << (DIGIT_BIT - 1)))
    {
        if ((res = mp_mul (&q, mu, &q)) != MP_OKAY)
        {
            goto CLEANUP;
        }
    }
    else
    {
        if ((res = s_mp_mul_high_digs (&q, mu, &q, um - 1)) != MP_OKAY)
        {
            goto CLEANUP;
        }
    }

    /* q3 = q2 / b**(k+1) */
    mp_rshd (&q, um + 1);

    /* x = x mod b**(k+1), quick (no division) */
    if ((res = mp_mod_2d (x, DIGIT_BIT * (um + 1), x)) != MP_OKAY)
    {
        goto CLEANUP;
    }

    /* q = q * m mod b**(k+1), quick (no division) */
    if ((res = s_mp_mul_digs (&q, m, &q, um + 1)) != MP_OKAY)
    {
        goto CLEANUP;
    }

    /* x = x - q */
    if ((res = mp_sub (x, &q, x)) != MP_OKAY)
    {
        goto CLEANUP;
    }

    /* If x < 0, add b**(k+1) to it */
    if (mp_cmp_d (x, 0) == MP_LT)
    {
        mp_set (&q, 1);
        if ((res = mp_lshd (&q, um + 1)) != MP_OKAY)
            goto CLEANUP;
        if ((res = mp_add (x, &q, x)) != MP_OKAY)
            goto CLEANUP;
    }

    /* Back off if it's too big */
    while (mp_cmp (x, m) != MP_LT)
    {
        if ((res = s_mp_sub (x, m, x)) != MP_OKAY)
        {
            goto CLEANUP;
        }
    }

CLEANUP:
    mp_clear (&q);

    return res;
}

/* pre-calculate the value required for Barrett reduction
* For a given modulus "b" it calulates the value required in "a"
*/
int
mp_reduce_setup (mp_int * a, mp_int * b)
{
    int     res;

    if ((res = mp_2expt (a, b->used * 2 * DIGIT_BIT)) != MP_OKAY)
    {
        return res;
    }
    return mp_div (a, b, a, NULL);
}

/* shift right by a certain bit count (store quotient in c, optional remainder in d) */
int
mp_div_2d (mp_int * a, int b, mp_int * c, mp_int * d)
{
    mp_digit D, r, rr;
    int     x, res;
    mp_int  t;


    /* if the shift count is <= 0 then we do no work */
    if (b <= 0)
    {
        res = mp_copy (a, c);
        if (d != NULL)
        {
            mp_zero (d);
        }
        return res;
    }

    if ((res = mp_init (&t)) != MP_OKAY)
    {
        return res;
    }

    /* get the remainder */
    if (d != NULL)
    {
        if ((res = mp_mod_2d (a, b, &t)) != MP_OKAY)
        {
            mp_clear (&t);
            return res;
        }
    }

    /* copy */
    if ((res = mp_copy (a, c)) != MP_OKAY)
    {
        mp_clear (&t);
        return res;
    }

    /* shift by as many digits in the bit count */
    if (b >= (int)DIGIT_BIT)
    {
        mp_rshd (c, b / DIGIT_BIT);
    }

    /* shift any bit count < DIGIT_BIT */
    D = (mp_digit) (b % DIGIT_BIT);
    if (D != 0)
    {
        register mp_digit *tmpc, mask, shift;

        /* mask */
        mask = (((mp_digit)1) << D) - 1;

        /* shift for lsb */
        shift = DIGIT_BIT - D;

        /* alias */
        tmpc = c->dp + (c->used - 1);

        /* carry */
        r = 0;
        for (x = c->used - 1; x >= 0; x--)
        {
            /* get the lower  bits of this word in a temp */
            rr = *tmpc & mask;

            /* shift the current word and mix in the carry bits from the previous word */
            *tmpc = (*tmpc >> D) | (r << shift);
            --tmpc;

            /* set the carry to the carry bits of the current word found above */
            r = rr;
        }
    }
    mp_clamp (c);
    if (d != NULL)
    {
        mp_exch (&t, d);
    }
    mp_clear (&t);
    return MP_OKAY;
}

/* low level subtraction (assumes |a| > |b|), HAC pp.595 Algorithm 14.9 */
int
s_mp_sub (mp_int * a, mp_int * b, mp_int * c)
{
    int     olduse, res, min, max;

    /* find sizes */
    min = b->used;
    max = a->used;

    /* init result */
    if ((int)(c->alloc) < max)
    {
        if ((res = mp_grow (c, max)) != MP_OKAY)
        {
            return res;
        }
    }
    olduse = c->used;
    c->used = max;

    {
        register mp_digit u, *tmpa, *tmpb, *tmpc;
        register int i;

        /* alias for digit pointers */
        tmpa = a->dp;
        tmpb = b->dp;
        tmpc = c->dp;

        /* set carry to zero */
        u = 0;
        for (i = 0; i < min; i++)
        {
            /* T[i] = A[i] - B[i] - U */
            *tmpc = *tmpa++ - *tmpb++ - u;

            /* U = carry bit of T[i]
            * Note this saves performing an AND operation since
            * if a carry does occur it will propagate all the way to the
            * MSB.  As a result a single shift is enough to get the carry
            */
            u = *tmpc >> ((mp_digit)(SSH_CHAR_BIT * sizeof (mp_digit) - 1));

            /* Clear carry from T[i] */
            *tmpc++ &= MP_MASK;
        }

        /* now copy higher words if any, e.g. if A has more digits than B  */
        for (; i < max; i++)
        {
            /* T[i] = A[i] - U */
            *tmpc = *tmpa++ - u;

            /* U = carry bit of T[i] */
            u = *tmpc >> ((mp_digit)(SSH_CHAR_BIT * sizeof (mp_digit) - 1));

            /* Clear carry from T[i] */
            *tmpc++ &= MP_MASK;
        }

        /* clear digits above used (since we may not have grown result above) */
        for (i = c->used; i < olduse; i++)
        {
            *tmpc++ = 0;
        }
    }

    mp_clamp (c);
    return MP_OKAY;
}

/* low level addition, based on HAC pp.594, Algorithm 14.7 */
int
s_mp_add (mp_int * a, mp_int * b, mp_int * c)
{
    mp_int *x;
    int     olduse, res, min, max;

    /* find sizes, we let |a| <= |b| which means we have to sort
    * them.  "x" will point to the input with the most digits
    */
    if (a->used > b->used)
    {
        min = b->used;
        max = a->used;
        x = a;
    }
    else
    {
        min = a->used;
        max = b->used;
        x = b;
    }

    /* init result */
    if ((int)(c->alloc) < max + 1)
    {
        if ((res = mp_grow (c, max + 1)) != MP_OKAY)
        {
            return res;
        }
    }

    /* get old used digit count and set new one */
    olduse = c->used;
    c->used = max + 1;

    {
        register mp_digit u, *tmpa, *tmpb, *tmpc;
        register int i;

        /* alias for digit pointers */

        /* first input */
        tmpa = a->dp;

        /* second input */
        tmpb = b->dp;

        /* destination */
        tmpc = c->dp;

        /* zero the carry */
        u = 0;
        for (i = 0; i < min; i++)
        {
            /* Compute the sum at one digit, T[i] = A[i] + B[i] + U */
            *tmpc = *tmpa++ + *tmpb++ + u;

            /* U = carry bit of T[i] */
            u = *tmpc >> ((mp_digit)DIGIT_BIT);

            /* take away carry bit from T[i] */
            *tmpc++ &= MP_MASK;
        }

        /* now copy higher words if any, that is in A+B
        * if A or B has more digits add those in
        */
        if (min != max)
        {
            for (; i < max; i++)
            {
                /* T[i] = X[i] + U */
                *tmpc = x->dp[i] + u;

                /* U = carry bit of T[i] */
                u = *tmpc >> ((mp_digit)DIGIT_BIT);

                /* take away carry bit from T[i] */
                *tmpc++ &= MP_MASK;
            }
        }

        /* add carry */
        *tmpc++ = u;

        /* clear digits above oldused */
        for (i = c->used; i < olduse; i++)
        {
            *tmpc++ = 0;
        }
    }

    mp_clamp (c);
    return MP_OKAY;
}

/* multiplies |a| * |b| and only computes upto digs digits of result
* HAC pp. 595, Algorithm 14.12  Modified so you can control how
* many digits of output are created.
*/
int
s_mp_mul_digs (mp_int * a, mp_int * b, mp_int * c, int digs)
{
    mp_int  t;
    int     res, pa, pb, ix, iy;
    mp_digit u;
    WORD64 r;
    mp_digit tmpx, *tmpt, *tmpy;

    /* can we use the fast multiplier? */
    if (((digs) < MP_WARRAY) &&
            MIN (a->used, b->used) <
            (1 << ((SSH_CHAR_BIT * sizeof (WORD64)) - (2 * DIGIT_BIT))))
    {
        return fast_s_mp_mul_digs (a, b, c, digs);
    }

    if ((res = mp_init_size (&t, digs)) != MP_OKAY)
    {
        return res;
    }
    t.used = digs;

    /* compute the digits of the product directly */
    pa = a->used;
    for (ix = 0; ix < pa; ix++)
    {
        /* set the carry to zero */
        u = 0;

        /* limit ourselves to making digs digits of output */
        pb = MIN ((int)(b->used), digs - ix);

        /* setup some aliases */
        /* copy of the digit from a used within the nested loop */
        tmpx = a->dp[ix];

        /* an alias for the destination shifted ix places */
        tmpt = t.dp + ix;

        /* an alias for the digits of b */
        tmpy = b->dp;

        /* compute the columns of the output and propagate the carry */
        for (iy = 0; iy < pb; iy++)
        {
            /* compute the column as a mp_word */
            r       = ((WORD64)*tmpt) +
                      ((WORD64)tmpx) * ((WORD64)*tmpy++) +
                      ((WORD64) u);

            /* the new column is the lower part of the result */
            *tmpt++ = (mp_digit) (r & ((WORD64) MP_MASK));

            /* get the carry word from the result */
            u       = (mp_digit) (r >> ((WORD64) DIGIT_BIT));
        }
        /* set carry if it is placed below digs */
        if (ix + iy < digs)
        {
            *tmpt = u;
        }
    }

    mp_clamp (&t);
    mp_exch (&t, c);

    mp_clear (&t);
    return MP_OKAY;
}

/* Fast (comba) multiplier
 *
 * This is the fast column-array [comba] multiplier.  It is
 * designed to compute the columns of the product first
 * then handle the carries afterwards.  This has the effect
 * of making the nested loops that compute the columns very
 * simple and schedulable on super-scalar processors.
 *
 * This has been modified to produce a variable number of
 * digits of output so if say only a half-product is required
 * you don't have to compute the upper half (a feature
 * required for fast Barrett reduction).
 *
 * Based on Algorithm 14.12 on pp.595 of HAC.
 *
 */
int
fast_s_mp_mul_digs (mp_int * a, mp_int * b, mp_int * c, int digs)
{
    int     olduse, res, pa, ix;
    mp_word W[MP_WARRAY];

    /* grow the destination as required */
    if ((int)(c->alloc) < digs)
    {
        if ((res = mp_grow (c, digs)) != MP_OKAY)
        {
            return res;
        }
    }

    /* clear temp buf (the columns) */
    memset (W, 0, sizeof (mp_word) * digs);

    /* calculate the columns */
    pa = a->used;
    for (ix = 0; ix < pa; ix++)
    {
        /* this multiplier has been modified to allow you to
         * control how many digits of output are produced.
         * So at most we want to make upto "digs" digits of output.
         *
         * this adds products to distinct columns (at ix+iy) of W
         * note that each step through the loop is not dependent on
         * the previous which means the compiler can easily unroll
         * the loop without scheduling problems
         */
        {
            register mp_digit tmpx, *tmpy;
            register mp_word *_W;
            register int iy, pb;

            /* alias for the the word on the left e.g. A[ix] * A[iy] */
            tmpx = a->dp[ix];

            /* alias for the right side */
            tmpy = b->dp;

            /* alias for the columns, each step through the loop adds a new
               term to each column
             */
            _W = W + ix;

            /* the number of digits is limited by their placement.  E.g.
               we avoid multiplying digits that will end up above the # of
               digits of precision requested
             */
            pb = MIN ((int)(b->used), digs - ix);

            for (iy = 0; iy < pb; iy++)
            {
                *_W++ += ((mp_word)tmpx) * ((mp_word)*tmpy++);
            }
        }

    }

    /* setup dest */
    olduse = c->used;
    c->used = digs;

    {
        register mp_digit *tmpc;

        /* At this point W[] contains the sums of each column.  To get the
         * correct result we must take the extra bits from each column and
         * carry them down
         *
         * Note that while this adds extra code to the multiplier it
         * saves time since the carry propagation is removed from the
         * above nested loop.This has the effect of reducing the work
         * from N*(N+N*c)==N**2 + c*N**2 to N**2 + N*c where c is the
         * cost of the shifting.  On very small numbers this is slower
         * but on most cryptographic size numbers it is faster.
         *
         * In this particular implementation we feed the carries from
         * behind which means when the loop terminates we still have one
         * last digit to copy
         */
        tmpc = c->dp;
        for (ix = 1; ix < digs; ix++)
        {
            /* forward the carry from the previous temp */
            W[ix] += (W[ix - 1] >> ((mp_word) DIGIT_BIT));

            /* now extract the previous digit [below the carry] */
            *tmpc++ = (mp_digit) (W[ix - 1] & ((mp_word) MP_MASK));
        }
        /* fetch the last digit */
        *tmpc++ = (mp_digit) (W[digs - 1] & ((mp_word) MP_MASK));

        /* clear unused digits [that existed in the old copy of c] */
        for (; ix < olduse; ix++)
        {
            *tmpc++ = 0;
        }
    }
    mp_clamp (c);
    return MP_OKAY;
}

/* c = |a| * |b| using Karatsuba Multiplication using
 * three half size multiplications
 *
 * Let B represent the radix [e.g. 2**DIGIT_BIT] and
 * let n represent half of the number of digits in
 * the min(a,b)
 *
 * a = a1 * B**n + a0
 * b = b1 * B**n + b0
 *
 * Then, a * b =>
   a1b1 * B**2n + ((a1 - a0)(b1 - b0) + a0b0 + a1b1) * B + a0b0
 *
 * Note that a1b1 and a0b0 are used twice and only need to be
 * computed once.  So in total three half size (half # of
 * digit) multiplications are performed, a0b0, a1b1 and
 * (a1-b1)(a0-b0)
 *
 * Note that a multiplication of half the digits requires
 * 1/4th the number of single precision multiplications so in
 * total after one call 25% of the single precision multiplications
 * are saved.  Note also that the call to mp_mul can end up back
 * in this function if the a0, a1, b0, or b1 are above the threshold.
 * This is known as divide-and-conquer and leads to the famous
 * O(N**lg(3)) or O(N**1.584) work which is asymptopically lower than
 * the standard O(N**2) that the baseline/comba methods use.
 * Generally though the overhead of this method doesn't pay off
 * until a certain size (N ~ 80) is reached.
 */
int
mp_karatsuba_mul (mp_int * a, mp_int * b, mp_int * c)
{
    mp_int  x0, x1, y0, y1, t1, x0y0, x1y1;
    int     B, err;

    /* default the return code to an error */
    err = MP_MEM;

    /* min # of digits */
    B = MIN (a->used, b->used);

    /* now divide in two */
    B = B / 2;

    /* init copy all the temps */
    if (mp_init_size (&x0, B) != MP_OKAY)
        goto ERR;
    if (mp_init_size (&x1, a->used - B) != MP_OKAY)
        goto X0;
    if (mp_init_size (&y0, B) != MP_OKAY)
        goto X1;
    if (mp_init_size (&y1, b->used - B) != MP_OKAY)
        goto Y0;

    /* init temps */
    if (mp_init_size (&t1, B * 2) != MP_OKAY)
        goto Y1;
    if (mp_init_size (&x0y0, B * 2) != MP_OKAY)
        goto T1;
    if (mp_init_size (&x1y1, B * 2) != MP_OKAY)
        goto X0Y0;

    /* now shift the digits */
    x0.sign = x1.sign = a->sign;
    y0.sign = y1.sign = b->sign;

    x0.used = y0.used = B;
    x1.used = a->used - B;
    y1.used = b->used - B;

    {
        register int x;
        register mp_digit *tmpa, *tmpb, *tmpx, *tmpy;

        /* we copy the digits directly instead of using higher level functions
         * since we also need to shift the digits
         */
        tmpa = a->dp;
        tmpb = b->dp;

        tmpx = x0.dp;
        tmpy = y0.dp;
        for (x = 0; x < B; x++)
        {
            *tmpx++ = *tmpa++;
            *tmpy++ = *tmpb++;
        }

        tmpx = x1.dp;
        for (x = B; x < (int)(a->used); x++)
        {
            *tmpx++ = *tmpa++;
        }
        tmpy = y1.dp;
        for (x = B; x < (int)(b->used); x++)
        {
            *tmpy++ = *tmpb++;
        }
    }

    /* only need to clamp the lower words since by definition the
     * upper words x1/y1 must have a known number of digits
     */
    mp_clamp (&x0);
    mp_clamp (&y0);

    /* now calc the products x0y0 and x1y1 */
    /* after this x0 is no longer required, free temp [x0==t2]! */
    if (mp_mul (&x0, &y0, &x0y0) != MP_OKAY)
        goto X1Y1;          /* x0y0 = x0*y0 */
    if (mp_mul (&x1, &y1, &x1y1) != MP_OKAY)
        goto X1Y1;          /* x1y1 = x1*y1 */

    /* now calc x1-x0 and y1-y0 */
    if (mp_sub (&x1, &x0, &t1) != MP_OKAY)
        goto X1Y1;          /* t1 = x1 - x0 */
    if (mp_sub (&y1, &y0, &x0) != MP_OKAY)
        goto X1Y1;          /* t2 = y1 - y0 */
    if (mp_mul (&t1, &x0, &t1) != MP_OKAY)
        goto X1Y1;          /* t1 = (x1 - x0) * (y1 - y0) */

    /* add x0y0 */
    if (mp_add (&x0y0, &x1y1, &x0) != MP_OKAY)
        goto X1Y1;          /* t2 = x0y0 + x1y1 */
    if (mp_sub (&x0, &t1, &t1) != MP_OKAY)
        goto X1Y1;          /* t1 = x0y0 + x1y1 - (x1-x0)*(y1-y0) */

    /* shift by B */
    if (mp_lshd (&t1, B) != MP_OKAY)
        goto X1Y1;          /* t1 = (x0y0 + x1y1 - (x1-x0)*(y1-y0))<<B */
    if (mp_lshd (&x1y1, B * 2) != MP_OKAY)
        goto X1Y1;          /* x1y1 = x1y1 << 2*B */

    if (mp_add (&x0y0, &t1, &t1) != MP_OKAY)
        goto X1Y1;          /* t1 = x0y0 + t1 */
    if (mp_add (&t1, &x1y1, c) != MP_OKAY)
        goto X1Y1;          /* t1 = x0y0 + t1 + x1y1 */

    /* Algorithm succeeded set the return code to MP_OKAY */
    err = MP_OKAY;

X1Y1:
    mp_clear (&x1y1);
X0Y0:
    mp_clear (&x0y0);
T1:
    mp_clear (&t1);
Y1:
    mp_clear (&y1);
Y0:
    mp_clear (&y0);
X1:
    mp_clear (&x1);
X0:
    mp_clear (&x0);
ERR:
    return err;
}

/* multiplication using the Toom-Cook 3-way algorithm */
int
mp_toom_mul(mp_int *a, mp_int *b, mp_int *c)
{
    mp_int w0, w1, w2, w3, w4, tmp1, tmp2, a0, a1, a2, b0, b1, b2;
    int res, B;

    /* init temps */
    if ((res = mp_init_multi(&w0, &w1, &w2, &w3, &w4,
                             &a0, &a1, &a2, &b0, &b1,
                             &b2, &tmp1, &tmp2, NULL)) != MP_OKAY)
    {
        return res;
    }

    /* B */
    B = MIN(a->used, b->used) / 3;

    /* a = a2 * B**2 + a1 * B + a0 */
    if ((res = mp_mod_2d(a, DIGIT_BIT * B, &a0)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_copy(a, &a1)) != MP_OKAY)
    {
        goto ERR;
    }
    mp_rshd(&a1, B);
    mp_mod_2d(&a1, DIGIT_BIT * B, &a1);

    if ((res = mp_copy(a, &a2)) != MP_OKAY)
    {
        goto ERR;
    }
    mp_rshd(&a2, B*2);

    /* b = b2 * B**2 + b1 * B + b0 */
    if ((res = mp_mod_2d(b, DIGIT_BIT * B, &b0)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_copy(b, &b1)) != MP_OKAY)
    {
        goto ERR;
    }
    mp_rshd(&b1, B);
    mp_mod_2d(&b1, DIGIT_BIT * B, &b1);

    if ((res = mp_copy(b, &b2)) != MP_OKAY)
    {
        goto ERR;
    }
    mp_rshd(&b2, B*2);

    /* w0 = a0*b0 */
    if ((res = mp_mul(&a0, &b0, &w0)) != MP_OKAY)
    {
        goto ERR;
    }

    /* w4 = a2 * b2 */
    if ((res = mp_mul(&a2, &b2, &w4)) != MP_OKAY)
    {
        goto ERR;
    }

    /* w1 = (a2 + 2(a1 + 2a0))(b2 + 2(b1 + 2b0)) */
    if ((res = mp_mul_2(&a0, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_mul_2(&tmp1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a2, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_mul_2(&b0, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp2, &b1, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_mul_2(&tmp2, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp2, &b2, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_mul(&tmp1, &tmp2, &w1)) != MP_OKAY)
    {
        goto ERR;
    }

    /* w3 = (a0 + 2(a1 + 2a2))(b0 + 2(b1 + 2b2)) */
    if ((res = mp_mul_2(&a2, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_mul_2(&tmp1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a0, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_mul_2(&b2, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp2, &b1, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_mul_2(&tmp2, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp2, &b0, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_mul(&tmp1, &tmp2, &w3)) != MP_OKAY)
    {
        goto ERR;
    }


    /* w2 = (a2 + a1 + a0)(b2 + b1 + b0) */
    if ((res = mp_add(&a2, &a1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a0, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&b2, &b1, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp2, &b0, &tmp2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_mul(&tmp1, &tmp2, &w2)) != MP_OKAY)
    {
        goto ERR;
    }

    /* now solve the matrix

       0  0  0  0  1
       1  2  4  8  16
       1  1  1  1  1
       16 8  4  2  1
       1  0  0  0  0

       using 12 subtractions, 4 shifts,
              2 small divisions and 1 small multiplication
     */

    /* r1 - r4 */
    if ((res = mp_sub(&w1, &w4, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3 - r0 */
    if ((res = mp_sub(&w3, &w0, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1/2 */
    if ((res = mp_div_2(&w1, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3/2 */
    if ((res = mp_div_2(&w3, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r2 - r0 - r4 */
    if ((res = mp_sub(&w2, &w0, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w2, &w4, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1 - r2 */
    if ((res = mp_sub(&w1, &w2, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3 - r2 */
    if ((res = mp_sub(&w3, &w2, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1 - 8r0 */
    if ((res = mp_mul_2d(&w0, 3, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w1, &tmp1, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3 - 8r4 */
    if ((res = mp_mul_2d(&w4, 3, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w3, &tmp1, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* 3r2 - r1 - r3 */
    if ((res = mp_mul_d(&w2, 3, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w2, &w1, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w2, &w3, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1 - r2 */
    if ((res = mp_sub(&w1, &w2, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3 - r2 */
    if ((res = mp_sub(&w3, &w2, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1/3 */
    if ((res = mp_div_3(&w1, &w1, NULL)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3/3 */
    if ((res = mp_div_3(&w3, &w3, NULL)) != MP_OKAY)
    {
        goto ERR;
    }

    /* at this point shift W[n] by B*n */
    if ((res = mp_lshd(&w1, 1*B)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_lshd(&w2, 2*B)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_lshd(&w3, 3*B)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_lshd(&w4, 4*B)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_add(&w0, &w1, c)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&w2, &w3, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&w4, &tmp1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, c, c)) != MP_OKAY)
    {
        goto ERR;
    }

ERR:
    mp_clear_multi(&w0, &w1, &w2, &w3, &w4,
                   &a0, &a1, &a2, &b0, &b1,
                   &b2, &tmp1, &tmp2, NULL);
    return res;
}

/* low level squaring, b = a*a, HAC pp.596-597, Algorithm 14.16 */
int
s_mp_sqr (mp_int * a, mp_int * b)
{
    mp_int  t;
    int     res, ix, iy, pa;
    mp_word r;
    mp_digit u, tmpx, *tmpt;

    pa = a->used;
    if ((res = mp_init_size (&t, 2*pa + 1)) != MP_OKAY)
    {
        return res;
    }
    t.used = 2*pa + 1;

    for (ix = 0; ix < pa; ix++)
    {
        /* first calculate the digit at 2*ix */
        /* calculate double precision result */
        r = ((mp_word) t.dp[2*ix]) +
            ((mp_word)a->dp[ix])*((mp_word)a->dp[ix]);

        /* store lower part in result */
        t.dp[2*ix] = (mp_digit) (r & ((mp_word) MP_MASK));

        /* get the carry */
        u = (mp_digit)(r >> ((mp_word) DIGIT_BIT));

        /* left hand side of A[ix] * A[iy] */
        tmpx = a->dp[ix];

        /* alias for where to store the results */
        tmpt = t.dp + (2*ix + 1);

        for (iy = ix + 1; iy < pa; iy++)
        {
            /* first calculate the product */
            r = ((mp_word)tmpx) * ((mp_word)a->dp[iy]);

            /* now calculate the double precision result, note we use
            * addition instead of *2 since it's easier to optimize
            */
            r       = ((mp_word) *tmpt) + r + r + ((mp_word) u);

            /* store lower part */
            *tmpt++ = (mp_digit) (r & ((mp_word) MP_MASK));

            /* get carry */
            u       = (mp_digit)(r >> ((mp_word) DIGIT_BIT));
        }
        /* propagate upwards */
        while (u != ((mp_digit) 0))
        {
            r       = ((mp_word) *tmpt) + ((mp_word) u);
            *tmpt++ = (mp_digit) (r & ((mp_word) MP_MASK));
            u       = (mp_digit)(r >> ((mp_word) DIGIT_BIT));
        }
    }

    mp_clamp (&t);
    mp_exch (&t, b);
    mp_clear (&t);
    return MP_OKAY;
}

/* fast squaring
 *
 * This is the comba method where the columns of the product
 * are computed first then the carries are computed.  This
 * has the effect of making a very simple inner loop that
 * is executed the most
 *
 * W2 represents the outer products and W the inner.
 *
 * A further optimizations is made because the inner
 * products are of the form "A * B * 2".  The *2 part does
 * not need to be computed until the end which is good
 * because 64-bit shifts are slow!
 *
 * Based on Algorithm 14.16 on pp.597 of HAC.
 *
 */
int
fast_s_mp_sqr (mp_int * a, mp_int * b)
{
    int     olduse, newused, res, ix, pa;
    mp_word W2[MP_WARRAY], W[MP_WARRAY];

    /* calculate size of product and allocate as required */
    pa = a->used;
    newused = pa + pa + 1;
    if ((int)(b->alloc) < newused)
    {
        if ((res = mp_grow (b, newused)) != MP_OKAY)
        {
            return res;
        }
    }

    /* zero temp buffer (columns)
     * Note that there are two buffers.  Since squaring requires
     * a outer and inner product and the inner product requires
     * computing a product and doubling it (a relatively expensive
     * op to perform n**2 times if you don't have to) the inner and
     * outer products are computed in different buffers.  This way
     * the inner product can be doubled using n doublings instead of
     * n**2
     */
    memset (W,  0, newused * sizeof (mp_word));
    memset (W2, 0, newused * sizeof (mp_word));

    /* This computes the inner product.  To simplify the inner N**2 loop
     * the multiplication by two is done afterwards in the N loop.
     */
    for (ix = 0; ix < pa; ix++)
    {
        /* compute the outer product
         *
         * Note that every outer product is computed
         * for a particular column only once which means that
         * there is no need todo a double precision addition
         * into the W2[] array.
         */
        W2[ix + ix] = ((mp_word)a->dp[ix]) * ((mp_word)a->dp[ix]);

        {
            register mp_digit tmpx, *tmpy;
            register mp_word *_W;
            register int iy;

            /* copy of left side */
            tmpx = a->dp[ix];

            /* alias for right side */
            tmpy = a->dp + (ix + 1);

            /* the column to store the result in */
            _W = W + (ix + ix + 1);

            /* inner products */
            for (iy = ix + 1; iy < pa; iy++)
            {
                *_W++ += ((mp_word)tmpx) * ((mp_word)*tmpy++);
            }
        }
    }

    /* setup dest */
    olduse  = b->used;
    b->used = newused;

    /* now compute digits
     *
     * We have to double the inner product sums, add in the
     * outer product sums, propagate carries and convert
     * to single precision.
     */
    {
        register mp_digit *tmpb;

        /* double first value, since the inner products are
         * half of what they should be
         */
        W[0] += W[0] + W2[0];

        tmpb = b->dp;
        for (ix = 1; ix < newused; ix++)
        {
            /* double/add next digit */
            W[ix] += W[ix] + W2[ix];

            /* propagate carry forwards [from the previous digit] */
            W[ix] = W[ix] + (W[ix - 1] >> ((mp_word) DIGIT_BIT));

            /* store the current digit now that the carry isn't
             * needed
             */
            *tmpb++ = (mp_digit) (W[ix - 1] & ((mp_word) MP_MASK));
        }
        /* set the last value.  Note even if the carry is zero
         * this is required since the next step will not zero
         * it if b originally had a value at b->dp[2*a.used]
         */
        *tmpb++ = (mp_digit) (W[(newused) - 1] & ((mp_word) MP_MASK));

        /* clear high digits of b if there were any originally */
        for (; ix < olduse; ix++)
        {
            *tmpb++ = 0;
        }
    }

    mp_clamp (b);
    return MP_OKAY;
}

/* Karatsuba squaring, computes b = a*a using three
* half size squarings
*
* See comments of mp_karatsuba_mul for details.  It
* is essentially the same algorithm but merely
* tuned to perform recursive squarings.
*/
int
mp_karatsuba_sqr (mp_int * a, mp_int * b)
{
    mp_int  x0, x1, t1, t2, x0x0, x1x1;
    int     B, err;

    err = MP_MEM;

    /* min # of digits */
    B = a->used;

    /* now divide in two */
    B = B / 2;

    /* init copy all the temps */
    if (mp_init_size (&x0, B) != MP_OKAY)
        goto ERR;
    if (mp_init_size (&x1, a->used - B) != MP_OKAY)
        goto X0;

    /* init temps */
    if (mp_init_size (&t1, a->used * 2) != MP_OKAY)
        goto X1;
    if (mp_init_size (&t2, a->used * 2) != MP_OKAY)
        goto T1;
    if (mp_init_size (&x0x0, B * 2) != MP_OKAY)
        goto T2;
    if (mp_init_size (&x1x1, (a->used - B) * 2) != MP_OKAY)
        goto X0X0;

    {
        register int x;
        register mp_digit *dst, *src;

        src = a->dp;

        /* now shift the digits */
        dst = x0.dp;
        for (x = 0; x < B; x++)
        {
            *dst++ = *src++;
        }

        dst = x1.dp;
        for (x = B; x < (int)(a->used); x++)
        {
            *dst++ = *src++;
        }
    }

    x0.used = B;
    x1.used = a->used - B;

    mp_clamp (&x0);

    /* now calc the products x0*x0 and x1*x1 */
    if (mp_sqr (&x0, &x0x0) != MP_OKAY)
        goto X1X1;           /* x0x0 = x0*x0 */
    if (mp_sqr (&x1, &x1x1) != MP_OKAY)
        goto X1X1;           /* x1x1 = x1*x1 */

    /* now calc (x1-x0)**2 */
    if (mp_sub (&x1, &x0, &t1) != MP_OKAY)
        goto X1X1;           /* t1 = x1 - x0 */
    if (mp_sqr (&t1, &t1) != MP_OKAY)
        goto X1X1;           /* t1 = (x1 - x0) * (x1 - x0) */

    /* add x0y0 */
    if (s_mp_add (&x0x0, &x1x1, &t2) != MP_OKAY)
        goto X1X1;           /* t2 = x0x0 + x1x1 */
    if (mp_sub (&t2, &t1, &t1) != MP_OKAY)
        goto X1X1;           /* t1 = x0x0 + x1x1 - (x1-x0)*(x1-x0) */

    /* shift by B */
    if (mp_lshd (&t1, B) != MP_OKAY)
        goto X1X1;           /* t1 = (x0x0 + x1x1 - (x1-x0)*(x1-x0))<<B */
    if (mp_lshd (&x1x1, B * 2) != MP_OKAY)
        goto X1X1;           /* x1x1 = x1x1 << 2*B */

    if (mp_add (&x0x0, &t1, &t1) != MP_OKAY)
        goto X1X1;           /* t1 = x0x0 + t1 */
    if (mp_add (&t1, &x1x1, b) != MP_OKAY)
        goto X1X1;           /* t1 = x0x0 + t1 + x1x1 */

    err = MP_OKAY;

X1X1:
    mp_clear (&x1x1);
X0X0:
    mp_clear (&x0x0);
T2:
    mp_clear (&t2);
T1:
    mp_clear (&t1);
X1:
    mp_clear (&x1);
X0:
    mp_clear (&x0);
ERR:
    return err;
}

/* squaring using Toom-Cook 3-way algorithm */
int
mp_toom_sqr(mp_int *a, mp_int *b)
{
    mp_int w0, w1, w2, w3, w4, tmp1, a0, a1, a2;
    int res, B;

    /* init temps */
    if ((res = mp_init_multi(&w0, &w1, &w2, &w3, &w4, &a0, &a1, &a2, &tmp1, NULL)) != MP_OKAY)
    {
        return res;
    }

    /* B */
    B = a->used / 3;

    /* a = a2 * B**2 + a1 * B + a0 */
    if ((res = mp_mod_2d(a, DIGIT_BIT * B, &a0)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_copy(a, &a1)) != MP_OKAY)
    {
        goto ERR;
    }
    mp_rshd(&a1, B);
    mp_mod_2d(&a1, DIGIT_BIT * B, &a1);

    if ((res = mp_copy(a, &a2)) != MP_OKAY)
    {
        goto ERR;
    }
    mp_rshd(&a2, B*2);

    /* w0 = a0*a0 */
    if ((res = mp_sqr(&a0, &w0)) != MP_OKAY)
    {
        goto ERR;
    }

    /* w4 = a2 * a2 */
    if ((res = mp_sqr(&a2, &w4)) != MP_OKAY)
    {
        goto ERR;
    }

    /* w1 = (a2 + 2(a1 + 2a0))**2 */
    if ((res = mp_mul_2(&a0, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_mul_2(&tmp1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a2, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_sqr(&tmp1, &w1)) != MP_OKAY)
    {
        goto ERR;
    }

    /* w3 = (a0 + 2(a1 + 2a2))**2 */
    if ((res = mp_mul_2(&a2, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_mul_2(&tmp1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a0, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_sqr(&tmp1, &w3)) != MP_OKAY)
    {
        goto ERR;
    }


    /* w2 = (a2 + a1 + a0)**2 */
    if ((res = mp_add(&a2, &a1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, &a0, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sqr(&tmp1, &w2)) != MP_OKAY)
    {
        goto ERR;
    }

    /* now solve the matrix

       0  0  0  0  1
       1  2  4  8  16
       1  1  1  1  1
       16 8  4  2  1
       1  0  0  0  0

       using 12 subtractions, 4 shifts, 2 small divisions and 1 small multiplication.
     */

    /* r1 - r4 */
    if ((res = mp_sub(&w1, &w4, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3 - r0 */
    if ((res = mp_sub(&w3, &w0, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1/2 */
    if ((res = mp_div_2(&w1, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3/2 */
    if ((res = mp_div_2(&w3, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r2 - r0 - r4 */
    if ((res = mp_sub(&w2, &w0, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w2, &w4, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1 - r2 */
    if ((res = mp_sub(&w1, &w2, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3 - r2 */
    if ((res = mp_sub(&w3, &w2, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1 - 8r0 */
    if ((res = mp_mul_2d(&w0, 3, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w1, &tmp1, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3 - 8r4 */
    if ((res = mp_mul_2d(&w4, 3, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w3, &tmp1, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* 3r2 - r1 - r3 */
    if ((res = mp_mul_d(&w2, 3, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w2, &w1, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_sub(&w2, &w3, &w2)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1 - r2 */
    if ((res = mp_sub(&w1, &w2, &w1)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3 - r2 */
    if ((res = mp_sub(&w3, &w2, &w3)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r1/3 */
    if ((res = mp_div_3(&w1, &w1, NULL)) != MP_OKAY)
    {
        goto ERR;
    }
    /* r3/3 */
    if ((res = mp_div_3(&w3, &w3, NULL)) != MP_OKAY)
    {
        goto ERR;
    }

    /* at this point shift W[n] by B*n */
    if ((res = mp_lshd(&w1, 1*B)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_lshd(&w2, 2*B)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_lshd(&w3, 3*B)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_lshd(&w4, 4*B)) != MP_OKAY)
    {
        goto ERR;
    }

    if ((res = mp_add(&w0, &w1, b)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&w2, &w3, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&w4, &tmp1, &tmp1)) != MP_OKAY)
    {
        goto ERR;
    }
    if ((res = mp_add(&tmp1, b, b)) != MP_OKAY)
    {
        goto ERR;
    }

ERR:
    mp_clear_multi(&w0, &w1, &w2, &w3, &w4, &a0, &a1, &a2, &tmp1, NULL);
    return res;
}

/* integer signed division.
 * c*b + d == a [e.g. a/b, c=quotient, d=remainder]
 * HAC pp.598 Algorithm 14.20
 *
 * Note that the description in HAC is horribly
 * incomplete.  For example, it doesn't consider
 * the case where digits are removed from 'x' in
 * the inner loop.  It also doesn't consider the
 * case that y has fewer than three digits, etc..
 *
 * The overall algorithm is as described as
 * 14.20 from HAC but fixed to treat these cases.
*/

int
mp_div (mp_int * a, mp_int * b, mp_int * c, mp_int * d)
{
    mp_int  q, x, y, t1, t2;
    int     res, n, t, i, norm, neg;

    /* is divisor zero ? */
    if (mp_iszero (b) == 1)
    {
        return MP_VAL;
    }

    /* if a < b then q=0, r = a */
    if (mp_cmp_mag (a, b) == MP_LT)
    {
        if (d != NULL)
        {
            res = mp_copy (a, d);
        }
        else
        {
            res = MP_OKAY;
        }
        if (c != NULL)
        {
            mp_zero (c);
        }
        return res;
    }

    if ((res = mp_init_size (&q, a->used + 2)) != MP_OKAY)
    {
        return res;
    }
    q.used = a->used + 2;

    if ((res = mp_init (&t1)) != MP_OKAY)
    {
        goto __Q;
    }

    if ((res = mp_init (&t2)) != MP_OKAY)
    {
        goto __T1;
    }

    if ((res = mp_init_copy (&x, a)) != MP_OKAY)
    {
        goto __T2;
    }

    if ((res = mp_init_copy (&y, b)) != MP_OKAY)
    {
        goto __X;
    }

    /* fix the sign */
    neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;
    x.sign = y.sign = MP_ZPOS;

    /* normalize both x and y, ensure that y >= b/2, [b == 2**DIGIT_BIT] */
    norm = mp_count_bits(&y) % DIGIT_BIT;
    if (norm < (int)(DIGIT_BIT-1))
    {
        norm = (DIGIT_BIT-1) - norm;
        if ((res = mp_mul_2d (&x, norm, &x)) != MP_OKAY)
        {
            goto __Y;
        }
        if ((res = mp_mul_2d (&y, norm, &y)) != MP_OKAY)
        {
            goto __Y;
        }
    }
    else
    {
        norm = 0;
    }

    /* note hac does 0 based, so if used==5 then its 0,1,2,3,4, e.g. use 4 */
    n = x.used - 1;
    t = y.used - 1;

    /* while (x >= y*b**n-t) do { q[n-t] += 1; x -= y*b**{n-t} } */
    if ((res = mp_lshd (&y, n - t)) != MP_OKAY)
    { /* y = y*b**{n-t} */
        goto __Y;
    }

    while (mp_cmp (&x, &y) != MP_LT)
    {
        ++(q.dp[n - t]);
        if ((res = mp_sub (&x, &y, &x)) != MP_OKAY)
        {
            goto __Y;
        }
    }

    /* reset y by shifting it back down */
    mp_rshd (&y, n - t);

    /* step 3. for i from n down to (t + 1) */
    for (i = n; i >= (t + 1); i--)
    {
        if (i > (int)(x.used))
        {
            continue;
        }

        /* step 3.1 if xi == yt then set q{i-t-1} to b-1,
         * otherwise set q{i-t-1} to (xi*b + x{i-1})/yt */
        if (x.dp[i] == y.dp[t])
        {
            q.dp[i - t - 1] = ((((mp_digit)1) << DIGIT_BIT) - 1);
        }
        else
        {
            mp_word tmp;
            tmp = ((mp_word) x.dp[i]) << ((mp_word) DIGIT_BIT);
            tmp |= ((mp_word) x.dp[i - 1]);
            tmp /= ((mp_word) y.dp[t]);
            if (tmp > (mp_word) MP_MASK)
                tmp = MP_MASK;
            q.dp[i - t - 1] = (mp_digit) (tmp & (mp_word) (MP_MASK));
        }

        /* while (q{i-t-1} * (yt * b + y{t-1})) >
                 xi * b**2 + xi-1 * b + xi-2

           do q{i-t-1} -= 1;
        */
        q.dp[i - t - 1] = (q.dp[i - t - 1] + 1) & MP_MASK;
        do
        {
            q.dp[i - t - 1] = (q.dp[i - t - 1] - 1) & MP_MASK;

            /* find left hand */
            mp_zero (&t1);
            t1.dp[0] = (t - 1 < 0) ? 0 : y.dp[t - 1];
            t1.dp[1] = y.dp[t];
            t1.used = 2;
            if ((res = mp_mul_d (&t1, q.dp[i - t - 1], &t1)) != MP_OKAY)
            {
                goto __Y;
            }

            /* find right hand */
            t2.dp[0] = (i - 2 < 0) ? 0 : x.dp[i - 2];
            t2.dp[1] = (i - 1 < 0) ? 0 : x.dp[i - 1];
            t2.dp[2] = x.dp[i];
            t2.used = 3;
        }
        while (mp_cmp_mag(&t1, &t2) == MP_GT);

        /* step 3.3 x = x - q{i-t-1} * y * b**{i-t-1} */
        if ((res = mp_mul_d (&y, q.dp[i - t - 1], &t1)) != MP_OKAY)
        {
            goto __Y;
        }

        if ((res = mp_lshd (&t1, i - t - 1)) != MP_OKAY)
        {
            goto __Y;
        }

        if ((res = mp_sub (&x, &t1, &x)) != MP_OKAY)
        {
            goto __Y;
        }

        /* if x < 0 then { x = x + y*b**{i-t-1}; q{i-t-1} -= 1; } */
        if (x.sign == MP_NEG)
        {
            if ((res = mp_copy (&y, &t1)) != MP_OKAY)
            {
                goto __Y;
            }
            if ((res = mp_lshd (&t1, i - t - 1)) != MP_OKAY)
            {
                goto __Y;
            }
            if ((res = mp_add (&x, &t1, &x)) != MP_OKAY)
            {
                goto __Y;
            }

            q.dp[i - t - 1] = (q.dp[i - t - 1] - 1UL) & MP_MASK;
        }
    }

    /* now q is the quotient and x is the remainder
     * [which we have to normalize]
     */

    /* get sign before writing to c */
    x.sign = a->sign;

    if (c != NULL)
    {
        mp_clamp (&q);
        mp_exch (&q, c);
        c->sign = neg;
    }

    if (d != NULL)
    {
        mp_div_2d (&x, norm, &x, NULL);
        mp_exch (&x, d);
    }

    res = MP_OKAY;

__Y:
    mp_clear (&y);
__X:
    mp_clear (&x);
__T2:
    mp_clear (&t2);
__T1:
    mp_clear (&t1);
__Q:
    mp_clear (&q);
    return res;
}

/* b = a*2 */
int
mp_mul_2 (mp_int * a, mp_int * b)
{
    int     x, res, oldused;

    /* grow to accomodate result */
    if (b->alloc < a->used + 1)
    {
        if ((res = mp_grow (b, a->used + 1)) != MP_OKAY)
        {
            return res;
        }
    }

    oldused = b->used;
    b->used = a->used;

    {
        register mp_digit r, rr, *tmpa, *tmpb;

        /* alias for source */
        tmpa = a->dp;

        /* alias for dest */
        tmpb = b->dp;

        /* carry */
        r = 0;
        for (x = 0; x < (int)(a->used); x++)
        {

            /* get what will be the *next* carry bit from the
            * MSB of the current digit
            	*/
            rr = *tmpa >> ((mp_digit)(DIGIT_BIT - 1));

            /* now shift up this digit, add in the carry [from the previous] */
            *tmpb++ = ((*tmpa++ << ((mp_digit)1)) | r) & MP_MASK;

            /* copy the carry that would be from the source
            * digit into the next iteration
            */
            r = rr;
        }

        /* new leading digit? */
        if (r != 0)
        {
            /* add a MSB which is always 1 at this point */
            *tmpb = 1;
            ++(b->used);
        }

        /* now zero any excess digits on the destination
        * that we didn't write to
        */
        tmpb = b->dp + b->used;
        for (x = b->used; x < oldused; x++)
        {
            *tmpb++ = 0;
        }
    }
    b->sign = a->sign;
    return MP_OKAY;
}

/* computes a = 2**b
*
* Simple algorithm which zeroes the int, grows it then just sets one bit
* as required.
*/
int
mp_2expt (mp_int * a, int b)
{
    int     res;

    /* zero a as per default */
    mp_zero (a);

    /* grow a to accomodate the single bit */
    if ((res = mp_grow (a, b / DIGIT_BIT + 1)) != MP_OKAY)
    {
        return res;
    }

    /* set the used count of where the bit will go */
    a->used = b / DIGIT_BIT + 1;

    /* put the single bit in its place */
    a->dp[b / DIGIT_BIT] = 1 << (b % DIGIT_BIT);

    return MP_OKAY;
}

/* multiply by a digit */
int
mp_mul_d (mp_int * a, mp_digit b, mp_int * c)
{
    mp_digit u, *tmpa, *tmpc;
    mp_word  r;
    int      ix, res, olduse;

    /* make sure c is big enough to hold a*b */
    if (c->alloc < a->used + 1)
    {
        if ((res = mp_grow (c, a->used + 1)) != MP_OKAY)
        {
            return res;
        }
    }

    /* get the original destinations used count */
    olduse = c->used;

    /* set the sign */
    c->sign = a->sign;

    /* alias for a->dp [source] */
    tmpa = a->dp;

    /* alias for c->dp [dest] */
    tmpc = c->dp;

    /* zero carry */
    u = 0;

    /* compute columns */
    for (ix = 0; ix < (int)(a->used); ix++)
    {
        /* compute product and carry sum for this term */
        r       = ((mp_word) u) + ((mp_word)*tmpa++) * ((mp_word)b);

        /* mask off higher bits to get a single digit */
        *tmpc++ = (mp_digit) (r & ((mp_word) MP_MASK));

        /* send carry into next iteration */
        u       = (mp_digit) (r >> ((mp_word) DIGIT_BIT));
    }

    /* store final carry [if any] */
    *tmpc++ = u;

    /* now zero digits above the top */
    while (ix++ < olduse)
    {
        *tmpc++ = 0;
    }

    /* set used count */
    c->used = a->used + 1;
    mp_clamp(c);

    return MP_OKAY;
}

/* shift right a certain amount of digits */
void
mp_rshd (mp_int * a, int b)
{
    int     x;

    /* if b <= 0 then ignore it */
    if (b <= 0)
    {
        return;
    }

    /* if b > used then simply zero it and return */
    if ((int)(a->used) <= b)
    {
        mp_zero (a);
        return;
    }

    {
        register mp_digit *bottom, *top;

        /* shift the digits down */

        /* bottom */
        bottom = a->dp;

        /* top [offset into digits] */
        top = a->dp + b;

        /* this is implemented as a sliding window where
        * the window is b-digits long and digits from
        * the top of the window are copied to the bottom
        *
        * e.g.

          b-2 | b-1 | b0 | b1 | b2 | ... | bb |   ---->
          /\                   |      ---->
          \-------------------/      ---->
        */
        for (x = 0; x < (int)(a->used - b); x++)
        {
            *bottom++ = *top++;
        }

        /* zero the top digits */
        for (; x < (int)(a->used); x++)
        {
            *bottom++ = 0;
        }
    }

    /* remove excess digits */
    a->used -= b;
}

/* calc a value mod 2**b */
int
mp_mod_2d (mp_int * a, int b, mp_int * c)
{
    int     x, res;

    /* if b is <= 0 then zero the int */
    if (b <= 0)
    {
        mp_zero (c);
        return MP_OKAY;
    }

    /* if the modulus is larger than the value than return */
    if (b > (int) (a->used * DIGIT_BIT))
    {
        res = mp_copy (a, c);
        return res;
    }

    /* copy */
    if ((res = mp_copy (a, c)) != MP_OKAY)
    {
        return res;
    }

    /* zero digits above the last digit of the modulus */
    for (x = (b / DIGIT_BIT) + ((b % DIGIT_BIT) == 0 ? 0 : 1); x < (int)(c->used); x++)
    {
        c->dp[x] = 0;
    }
    /* clear the digit that is not completely outside/inside the modulus */
    c->dp[b / DIGIT_BIT] &=
        (mp_digit) ((((mp_digit) 1) << (((mp_digit) b) % DIGIT_BIT)) - ((mp_digit) 1));
    mp_clamp (c);
    return MP_OKAY;
}

/* multiplies |a| * |b| and does not compute the lower digs digits
* [meant to get the higher part of the product]
*/
int
s_mp_mul_high_digs (mp_int * a, mp_int * b, mp_int * c, int digs)
{
    mp_int  t;
    int     res, pa, pb, ix, iy;
    mp_digit u;
    mp_word r;
    mp_digit tmpx, *tmpt, *tmpy;

    /* can we use the fast multiplier? */
    if (((a->used + b->used + 1) < MP_WARRAY)
            && MIN (a->used, b->used) < (1 << ((SSH_CHAR_BIT * sizeof (mp_word)) - (2 * DIGIT_BIT))))
    {
        return fast_s_mp_mul_high_digs (a, b, c, digs);
    }

    if ((res = mp_init_size (&t, a->used + b->used + 1)) != MP_OKAY)
    {
        return res;
    }
    t.used = a->used + b->used + 1;

    pa = a->used;
    pb = b->used;
    for (ix = 0; ix < pa; ix++)
    {
        /* clear the carry */
        u = 0;

        /* left hand side of A[ix] * B[iy] */
        tmpx = a->dp[ix];

        /* alias to the address of where the digits will be stored */
        tmpt = &(t.dp[digs]);

        /* alias for where to read the right hand side from */
        tmpy = b->dp + (digs - ix);

        for (iy = digs - ix; iy < pb; iy++)
        {
            /* calculate the double precision result */
            r       = ((mp_word)*tmpt) +
                      ((mp_word)tmpx) * ((mp_word)*tmpy++) +
                      ((mp_word) u);

            /* get the lower part */
            *tmpt++ = (mp_digit) (r & ((mp_word) MP_MASK));

            /* carry the carry */
            u       = (mp_digit) (r >> ((mp_word) DIGIT_BIT));
        }
        *tmpt = u;
    }
    mp_clamp (&t);
    mp_exch (&t, c);
    mp_clear (&t);
    return MP_OKAY;
}

/* init an mp_init for a given size */
int
mp_init_size (mp_int * a, int size)
{
    /* pad size so there are always extra digits */
    size += (MP_PREC * 2) - (size % MP_PREC);

    /* alloc mem */
    a->dp = (mp_digit*)XOS_GetUB(sizeof(mp_digit)*size);
    if (a->dp == NULL)
    {
        return MP_MEM;
    }
    a->used  = 0;
    a->alloc = size;
    a->sign  = MP_ZPOS;

    return MP_OKAY;
}

/* divide by three (based on routine from MPI and the GMP manual) */
int
mp_div_3 (mp_int * a, mp_int *c, mp_digit * d)
{
    mp_int   q;
    mp_word  w, t;
    mp_digit b;
    int      res, ix;

    /* b = 2**DIGIT_BIT / 3 */
    b = (((mp_word)1) << ((mp_word)DIGIT_BIT)) / ((mp_word)3);

    if ((res = mp_init_size(&q, a->used)) != MP_OKAY)
    {
        return res;
    }

    q.used = a->used;
    q.sign = a->sign;
    w = 0;
    for (ix = a->used - 1; ix >= 0; ix--)
    {
        w = (w << ((mp_word)DIGIT_BIT)) | ((mp_word)a->dp[ix]);

        if (w >= 3)
        {
            /* multiply w by [1/3] */
            mp_word tmp1=(mp_word)DIGIT_BIT;
            t = (w * ((mp_word)b)) >> (tmp1);

            /* now subtract 3 * [w/3] from w, to get the remainder */
            w -= (t << ((mp_word)1)) + t;

            /* fixup the remainder as required since
            * the optimization is not exact.
            */
            while (w >= 3)
            {
                t += 1;
                w -= 3;
            }
        }
        else
        {
            t = 0;
        }
        q.dp[ix] = (mp_digit)t;
    }

    /* [optional] store the remainder */
    if (d != NULL)
    {
        *d = (mp_digit)w;
    }

    /* [optional] store the quotient */
    if (c != NULL)
    {
        mp_clamp(&q);
        mp_exch(&q, c);
    }
    mp_clear(&q);

    return res;
}

/* this is a modified version of fast_s_mp_mul_digs that only produces
* output digits *above* digs.  See the comments for fast_s_mp_mul_digs
* to see how it works.
*
* This is used in the Barrett reduction since for one of the multiplications
* only the higher digits were needed.  This essentially halves the work.
*
* Based on Algorithm 14.12 on pp.595 of HAC.
*/
int
fast_s_mp_mul_high_digs (mp_int * a, mp_int * b, mp_int * c, int digs)
{
    int     oldused, newused, res, pa, pb, ix;
    mp_word W[MP_WARRAY];

    /* calculate size of product and allocate more space if required */
    newused = a->used + b->used + 1;
    if ((int)(c->alloc) < newused)
    {
        if ((res = mp_grow (c, newused)) != MP_OKAY)
        {
            return res;
        }
    }

    /* like the other comba method we compute the columns first */
    pa = a->used;
    pb = b->used;
    memset (W + digs, 0, (pa + pb + 1 - digs) * sizeof (mp_word));
    for (ix = 0; ix < pa; ix++)
    {
        {
            register mp_digit tmpx, *tmpy;
            register int iy;
            register mp_word *_W;

            /* work todo, that is we only calculate digits that are at "digs" or above  */
            iy = digs - ix;

            /* copy of word on the left of A[ix] * B[iy] */
            tmpx = a->dp[ix];

            /* alias for right side */
            tmpy = b->dp + iy;

            /* alias for the columns of output.  Offset to be equal to or above the
            * smallest digit place requested
            */
            _W = W + digs;

            /* skip cases below zero where ix > digs */
            if (iy < 0)
            {
                iy    = abs(iy);
                tmpy += iy;
                _W   += iy;
                iy    = 0;
            }

            /* compute column products for digits above the minimum */
            for (; iy < pb; iy++)
            {
                *_W++ += ((mp_word) tmpx) * ((mp_word)*tmpy++);
            }
        }
    }

    /* setup dest */
    oldused = c->used;
    c->used = newused;

    /* now convert the array W downto what we need
    *
    * See comments in bn_fast_s_mp_mul_digs.c
    */
    for (ix = digs + 1; ix < newused; ix++)
    {
        W[ix] += (W[ix - 1] >> ((mp_word) DIGIT_BIT));
        c->dp[ix - 1] = (mp_digit) (W[ix - 1] & ((mp_word) MP_MASK));
    }
    c->dp[newused - 1] = (mp_digit) (W[newused - 1] & ((mp_word) MP_MASK));

    for (; ix < oldused; ix++)
    {
        c->dp[ix] = 0;
    }
    mp_clamp (c);
    return MP_OKAY;
}

/* d = a + b (mod c) */
int
mp_addmod (mp_int * a, mp_int * b, mp_int * c, mp_int * d)
{
    int     res;
    mp_int  t;

    if ((res = mp_init (&t)) != MP_OKAY)
    {
        return res;
    }

    if ((res = mp_add (a, b, &t)) != MP_OKAY)
    {
        mp_clear (&t);
        return res;
    }
    res = mp_mod (&t, c, d);
    mp_clear (&t);
    return res;
}
