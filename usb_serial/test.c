#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;
    while (time(0) < retTime);
}
int main()
{
    while (1) {

        waitFor(2);
        char c = getchar();
        
        printf("%c", c);
    }
     
   return 0;
}