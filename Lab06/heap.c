#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int staticvar;
const int constvar = 0;

int main(void)
{
        int stackvar;
        char buf[200];
        int *p;

        p = malloc(sizeof(int));
        sprintf(buf, "cat /proc/%d/maps", getpid());
        system(buf);

        printf("&staticvar=%p\n", &staticvar);
        printf("&constvar=%p\n", &constvar);
        printf("&stackvar=%p\n", &stackvar);
        printf("p=%p\n", p);
        printf("undefined behaviour: &p[500]=%p\n", &p[500]);
        printf("undefined behaviour: &p[50000000]=%p\n", &p[50000000]);

        p[500] = 999999; //undefined behaviour
        printf("undefined behaviour: p[500]=%d\n", p[500]);
        return 0;
}