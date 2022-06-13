#include "stdio.h"

int stringLength(char *str)  
{  
    int i=0;  
     
    while(str[i] != '\0') i++;  
     
    return i;  
} 

char data[] = "string";

int main ()
{
  int a = stringLength(data);
  printf(data);
  return 0;
}