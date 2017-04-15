#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main() {
	int fd;
	char str[10];

//	fd = open("/dev/charMod", O_RDWR);
	
	if (fd < 0) {
		printf("Failed to open the device\n");
		return -1;
	}

	char five_mb_string[4 * 1024 * 1024];

	for (int i = 0; i < 4 * 1024; i++) {
		sprintf(str, "%d", i + 10000);
		//printf("%s\n", str);
		for (int k = 0; k < 5 ; k++) {
			five_mb_string[i * 1024 + k] = str[k];
		}		
		for (int j = 0; j < 1024 - 5; j++) {
			five_mb_string[i * 1024 + j + 5] = 'a';
		}
	}
	five_mb_string[4 * 1024 * 1024 - 1] = '\0';
	printf("%lu\n", sizeof(five_mb_string));
	return 0;
}
