#line 141 "bits.c"
int bitAnd(int x, int y) {
  return ~(~x|~y);
#line 31 "<command-line>"
#include "/usr/include/stdc-predef.h"
#line 143 "bits.c"
}
#line 152
int getByte(int x, int n)
{
  return (x >>( n << 3)) & 0xff;
}
#line 164
int logicalShift(int x, int n) {
  return (x>>n)&(~(((1<<n)+~1+1)<<(32+~n+1)));
}
#line 174
int bitCount(int x) {
  int msk1=0x55|(0x55<<8);
  int msk2=0x33|(0x33<<8);
  int msk3=0x0f|(0x0f<<8);
  int msk4=0xff|(0xff<<16);
  int n;
  msk1|=msk1<<16;
  msk2|=msk2<<16;
  msk3|=msk3<<16;
  n=(x&msk1)+((x>>1)&msk1);
  n=(n&msk2)+((n>>2)&msk2);
  n=(n&msk3)+((n>>4)&msk3);
  n=(n&msk4)+((n>>8)&msk4);
  n=(n&((1<<16)+~0))+(n>>16);

  return n;
}
#line 198
int bang(int x) {
  int a=(  x>>16)|x;
  a =( a>>8)|a;
  a =( a>>4)|a;
  a =( a>>2)|a;
  return (((a>>1)|a)&0x1)^1;
}
#line 211
int tmin(void) {
  return 1<<31;
}
#line 223
int fitsBits(int x, int n) {
  return (x>>31)&(x>>(n+~0))&1;
#line 228
}
#line 237
int divpwr2(int x, int n) {
    return (x+(((1<<n)+(~1+1))&(x>>31)))>>n;
}
#line 247
int negate(int x) {
  return ~x+1;
}
#line 257
int isPositive(int x) {
  return (((x>>31)&1)^1)&(!(!x));
}
#line 267
int isLessOrEqual(int x, int y) {
  return (((x^y)>>31)&(!(((x^y)&y)>>31)))+((!((x^y)>>31))&(!((y+(~x+1))>>31)));
}
#line 277
int ilog2(int x) {
  int n=0;

  n=(!(x>>1));
  n+=(!(x>>2));
  n+=(!(x>>3));
  n+=(!(x>>4));
  n+=(!(x>>5));
  n+=(!(x>>6));
  n+=(!(x>>7));
  n+=(!(x>>8));
  n+=(!(x>>9));
  n+=(!(x>>10));
  n+=(!(x>>11));
  n+=(!(x>>12));
  n+=(!(x>>13));
  n+=(!(x>>14));
  n+=(!(x>>15));
  n+=(!(x>>16));
  n+=(!(x>>17));
  n+=(!(x>>18));
  n+=(!(x>>19));
  n+=(!(x>>20));
  n+=(!(x>>21));
  n+=(!(x>>22));
  n+=(!(x>>23));
  n+=(!(x>>24));
  n+=(!(x>>25));
  n+=(!(x>>26));
  n+=(!(x>>27));
  n+=(!(x>>28));
  n+=(!(x>>29));

  return 30+(x >> 30)+(~n);
}
#line 323
unsigned float_neg(unsigned uf) {
  unsigned t;
  if ((((uf>>23)&0xff)==0xff)&&(uf&0x7fffff)) 
  {
    t=uf;
  }
  else 
  {
    t=uf^(1<<31);
  }

 return t;
}
#line 345
unsigned float_i2f(int x) {
    unsigned uf;unsigned s=0;unsigned e;
    unsigned i;unsigned t;
  if (x==0) 
  {
    uf = 0;
  }
  else if (x==-x) 
  {
    uf = 0xcf000000;
  }
  else 
  {
    if (x<0) 
     {
      uf=-x;
      s=(1<<31);
     }
    else 
      uf=x;
    i=30;
    while (!(uf&(1<<i))) 
    {
      i--;
    }
    uf=uf-(1<<i);
    if (i<24) 
    {
      uf=uf<<(23-i);
    }
    else 
    {
      t=i-23;
      uf+=(1<<(t-1));
      if (uf&((1<<(t+1))-1)) 
      {
        uf-=1;
      }
      uf=uf>>t;
    }
      e=(127+i)<<23;
      uf+=e|s;
  }

  return uf;
}
#line 402
unsigned float_twice(unsigned uf) {
    unsigned t=uf;unsigned a;
    a =( uf>>23)&0xff;
  if (a==0) 
  {
    t=(uf&0x80000000)|(uf<<1);
  }
  else if (a!=0xff) 
  {
    t=uf+(1<<23);
  }

  return t;
}
