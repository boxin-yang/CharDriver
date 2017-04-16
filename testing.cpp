#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main() {
	int fd, ret;
	char str[10];

	fd = open("/dev/charMod", O_RDWR);
	
	if (fd < 0) {
		printf("Failed to open the device\n");
		return -1;
	}

	char five_mb_string[5 * 1024 * 1024];
	char string_to_receive[5 * 1024 * 1024];

	for (int i = 0; i < 5 * 1024; i++) {
		sprintf(str, "%d", i + 10000);
		//printf("%s\n", str);
		for (int k = 0; k < 5 ; k++) {
			five_mb_string[i * 1024 + k] = str[k];
		}		
		for (int j = 0; j < 1024 - 5; j++) {
			five_mb_string[i * 1024 + j + 5] = 'a';
		}
	}
	five_mb_string[5 * 1024 * 1024 - 1] = '\0';
	// printf("%lu\n", sizeof(five_mb_string));
//	printf("%s\n", five_mb_string);

	printf("Writing to device with data of size %lu\n", sizeof(five_mb_string));
	ret = write(fd, five_mb_string, strlen(five_mb_string));
	if (ret < 0) {
		printf("Error writing the device driver\n");
		return -1;
	}
	printf("Successfully wrote %d bytes of data to device\n", ret);	

//	printf("Reading from device\n");

//	ret = read(fd, string_to_receive, 10000);
	/*
	if (ret < 0) {
		printf("Error reading the device driver\n");
	}
	printf("Successfully read %d bytes of data from device\n", ret);
	printf("Received data of size %lu\n", strlen(string_to_receive));
	
	char head_tag[6], tail_tag[6];
	
	strncpy(string_to_receive, head_tag, 5);
	head_tag[5] = '\0';
	printf("First block has tag %s\n", head_tag);
	
	int blocks_copied = strlen(string_to_receive) / 1024;
	for (int i = 0; i < 5; i++) {
		tail_tag[i] = string_to_receive[(blocks_copied - 1) * 1024 + i];
	}
	tail_tag[5] = '\0';
	printf("Last block has tag %s\n", tail_tag);
	*/
	return 0;
}
