#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
	char * sr = (char *)malloc(50);
	char * sr1 = (char *)malloc(10);
	char * sr2 ;
//	memset(sr+5,0,10);
	printf("sizeof int :	%d 	%d	%d\n",sizeof(sr1),sizeof(sr2),sizeof(sr));
}
