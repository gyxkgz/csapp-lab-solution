#include <stdio.h>

void main()
{
    float a,b;
    int i;
    for(i=1;i<=3;i++)
    {
        a+=2.0/i;
    }
    for(i=4;i<=64;i++)
    {
        b+=2.0/i;
    }
    printf("a=%f,b=%f\n",a,b);

}