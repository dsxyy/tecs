#include <stdlib.h>
#include "pub_tmp.h"          /*#include "pub_oss.h"*/
#include "ssh_dss.h"

static char dsskey_p1[] = "A2AA91406C57F063015906797D97B270ACF8E7D04D73E179DC871FFFF36F0CC777FDD34ED42CAD93EF0FECF096918FE6962746A5D8A7B5EA4716B65CF9ECD4FB829C7EB63ACDA7666ACC843F0252A84D014BB5C51587B6A07188524EE0526EFE83DD2079A74D7A3C77290A76C8F1A602B6B1A87AF6C2FF3B207E9EDF09B24EF9";
static char dsskey_q1[] = "C986AA8919D159A97E77C42DAF108D7E733A6A57";
static char dsskey_g1[] = "5458B9349104F88B4CBF5E8F7855FF2AB35D90FA5EFEB916FF21B5662ADDFB342FDDE19150E47171C2FEEDB183E80E4F6609126F0FCD23084F38D7E824DAC8D0B46A2B65CA017C013C51FF404FC2FA9CA558B527DCECC1FF68B87A62BDEF68E555D80676EDE7215E0AD73469DCAFAA387F45E5BFE6002A3B986B5613DB4CEACD";
static char dsskey_x1[] = "25F4AF15F41627F712551E558E6D83F40C293128";
static char dsskey_y1[] = "A1D07E8404116C81C6F1D6C61CB9ABA2D86A6DDC72F5437090BFE00A79070C5CC51136BB38CECBF85015470860587775CD6D9CBB098B82D1D1A901F7F353DA369FE0E14AE2F6DFBD5840FD54DF65B8D10043E5C5F08E940D59DAF5F3BD610DABDC634B54202F48BCC8E2430C82D9DC560F649B0508E5846C2A90863C567EE043";

static char *decode16(char *in,int *lenout)
{
    int inlen, outlen, reslen;
    char *out;
    int i;
    char t1,t2;
    inlen = strlen(in);
    reslen = inlen%2;
    if(reslen==0)
    {
        outlen = inlen/2+1;
    }
    else
    {
        outlen = inlen/2+2;
    }
    out = (char *)XOS_GetUB(outlen);

    memset(out,0,outlen);
    if(reslen == 0)
    {
        t1 = in[0];
        t2 = in[1];
        if((t1>=48)&&(t1<=57))
            t1 = t1 - 48;
        else  if((t1>=65)&&(t1<=70))
            t1 =t1 - 55;
        else  if((t1>=97)&&(t1<=104))
            t1 = t1 - 87;
        if((t2>=48)&&(t2<=57))
            t2 = t2 - 48;
        else  if((t2>=65)&&(t2<=70))
            t2 =t2 - 55;
        else  if((t2>=97)&&(t2<=104))
            t2 = t2 - 87;
        out[0] = t1<<4|t2;
    }
    else
    {
        t1 = in[0];
        if((t1>=48)&&(t1<=57))
            t1 = t1 - 48;
        else  if((t1>=65)&&(t1<=70))
            t1 =t1 - 55;
        else  if((t1>=97)&&(t1<=104))
            t1 = t1 - 87;
        out[0] = t1;
    }
    for(i=1;i<outlen-1;i++)
    {
        t1 = in[2*i-reslen];
        t2 = in[2*i+1-reslen];
        if((t1>=48)&&(t1<=57))
            t1 = t1 - 48;
        else  if((t1>=65)&&(t1<=70))
            t1 =t1 - 55;
        else  if((t1>=97)&&(t1<=104))
            t1 = t1 - 87;
        if((t2>=48)&&(t2<=57))
            t2 = t2 - 48;
        else  if((t2>=65)&&(t2<=70))
            t2 =t2 - 55;
        else  if((t2>=97)&&(t2<=104))
            t2 = t2 - 87;
        out[i] = t1<<4|t2;
    }
    *lenout = outlen-1;
    return out;
}

/* Éú³ÉdssÃÜÔ¿ */
dss_key * gen_dss_priv_key()
{
    dss_key *key;
    char *dsskey_p,*dsskey_q,*dsskey_g,*dsskey_x,*dsskey_y;
    int len_p,len_q,len_g,len_x,len_y;
    key = (dss_key*)XOS_GetUB(sizeof(dss_key));
    key->p = (mp_int*)XOS_GetUB(sizeof(mp_int));
    mp_init(key->p);
    key->q = (mp_int*)XOS_GetUB(sizeof(mp_int));
    mp_init(key->q);
    key->g = (mp_int*)XOS_GetUB(sizeof(mp_int));
    mp_init(key->g);
    key->y = (mp_int*)XOS_GetUB(sizeof(mp_int));
    mp_init(key->y);
    key->x = (mp_int*)XOS_GetUB(sizeof(mp_int));
    mp_init(key->x);
    /* seedrandom(); */
    dsskey_p = decode16(dsskey_p1,&len_p);
    dsskey_q = decode16(dsskey_q1,&len_q);
    dsskey_g = decode16(dsskey_g1,&len_g);
    dsskey_x = decode16(dsskey_x1,&len_x);
    dsskey_y = decode16(dsskey_y1,&len_y);
	
    mp_read_unsigned_bin(key->p, (BYTE*)dsskey_p, len_p);
    mp_read_unsigned_bin(key->q, (BYTE*)dsskey_q, len_q);
    mp_read_unsigned_bin(key->g, (BYTE*)dsskey_g, len_g);
    mp_read_unsigned_bin(key->x, (BYTE*)dsskey_x, len_x);
    mp_read_unsigned_bin(key->y, (BYTE*)dsskey_y, len_y);
	
    XOS_RetUB((BYTE *)dsskey_p);
    XOS_RetUB((BYTE *)dsskey_q);
    XOS_RetUB((BYTE *)dsskey_g);
    XOS_RetUB((BYTE *)dsskey_x);
    XOS_RetUB((BYTE *)dsskey_y);
    return key;
}
