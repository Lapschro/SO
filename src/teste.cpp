#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

int main (){
	for(int j = 0; j < 3; j++)
		for(unsigned int i = 0; i < 0xffffffff;i++);
	return 0;
}
