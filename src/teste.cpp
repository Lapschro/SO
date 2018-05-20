#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

int main (){
	for(int i = 0; i < 15; i++){
		printf("2- Slept %d\n", i);
		sleep(1);
	}
	printf("Acabou!\n");
	return 0;
}
