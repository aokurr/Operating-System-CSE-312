#include <iostream>
#include <string.h>
#include <math.h>
#include "lib_ahmet.h"
using namespace std;

#define FILE_SIZE 1048576*8

#define INODE_NUMBER 200

int main (int argc, char const *argv[]){

	
	if(argc != 3){
		perror("Invalid parameters number\n");
		exit(1);
	}

	
	FILE *fp;
	fp=fopen(argv[2],"w+b");

	int block_size = (atoi(argv[1]) )* 1024;
	int total_blocks_number = ceil((float)(FILE_SIZE / block_size));
	creation_file_system file_system(block_size,INODE_NUMBER,total_blocks_number,fp);
	
	return 0;
}