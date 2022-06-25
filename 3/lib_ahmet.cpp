#include "lib_ahmet.h"
#include <iostream>
#include <string>
#include <math.h>
#include <string.h>
using namespace std;

/***************************I NODE *******************************/

i_node::i_node(){
	
	is_dir = false;
	size = -1;
	for(int i = 0;i < 13 ;i++){
		blogs[i] = -1; 
	}
    time_t     now = time(0);
    struct tm  tstruct;
    tstruct = *localtime(&now);
    strftime(modified_time, sizeof(char)*20,"%B %d %I:%M", &tstruct);

}

/***************************DATA ENTRY************************/

data_entry::data_entry(short node_id , char file_name[14]){
	this->node_id=node_id;
	strcpy(this->file_name,file_name);
}
			
/******************************SUPER BLOCK******************************/

super_block::super_block(int block_size,int inode_number , int total_blocks_number ){
	this->block_size=block_size;
	this->inode_number=inode_number;
	this->total_blocks_number=total_blocks_number;
	
	blocks_inode_number = ceil(((float)(inode_number * (sizeof(bool) + (sizeof(int) * 14) +  20) )) / block_size);
    
    superblock_and_spacem_block = ceil(((float)inode_number + total_blocks_number + (sizeof(int) * 8) ) / block_size);

    first_inodes_block_num = superblock_and_spacem_block;

    first_data_block_num = superblock_and_spacem_block + blocks_inode_number;
}
super_block::super_block(){}

/***********************SPACE MANAGEMENT*****************************/
void space_management :: space_management_items(int total_blocks_number,int first_data_block_num){
	
	free_block_num=total_blocks_number - first_data_block_num;
	int i;
	
	for(i = 0; i < 200 ;i++){
		free_inode_blocks[i] = 'f';
	}

	free_data_blocks = new char [total_blocks_number];
	
	for (i = 0; i < first_data_block_num; i++){
		free_data_blocks[i] = 'u';
	}

	
	for(i = first_data_block_num; i < total_blocks_number;i++){
		free_data_blocks[i] = 'f';
		
	}
}


/*********************************WRİTE FILE************************/

creation_file_system::creation_file_system(int block_size,int inode_number,int total_blocks_number,FILE *fp){
	
	this->block_size=block_size;
	this->inode_number=inode_number;
	this->total_blocks_number=total_blocks_number;
	
	super_block superBlock(block_size,inode_number,total_blocks_number);
	this->superBlock=superBlock;
	this->fp=fp;

	spaceManagement.space_management_items(total_blocks_number,superBlock.first_data_block_num);

	char temp[block_size];
	memset(temp, 0, block_size); 
	for (int i = 0; i < total_blocks_number; ++i)
		fwrite(temp, 1, block_size, fp);
	
	
	write_superblock();
	write_space_management();
	write_inodes();
	write_inode_dir();
	write_data();
	read_information();

}


void creation_file_system :: write_superblock(){
	
	fseek(fp,0,SEEK_SET);
	/* 7 değişken tutuluyor*/
	fwrite(&superBlock,(sizeof(int) * 7),1,fp);

}
void creation_file_system :: write_space_management(){
	fwrite(&(spaceManagement.free_block_num),sizeof(int),1,fp);
	fwrite(spaceManagement.free_inode_blocks,sizeof(char),200,fp);
	int i;
	
	for(i=0;i<superBlock.first_data_block_num;i++)
		fwrite(&(spaceManagement.free_data_blocks[i]),sizeof(char),1,fp);
	
	for(i = superBlock.first_data_block_num; i <superBlock.total_blocks_number;i++)
		fwrite(&(spaceManagement.free_data_blocks[i]),sizeof(char),1,fp);
	


}
void creation_file_system:: write_inodes(){
	
	fseek(fp, superBlock.first_inodes_block_num * block_size, SEEK_SET );
	
	for(int i =0;i < inode_number;i++){
		fwrite(&(inode.is_dir), sizeof(bool), 1, fp);
		fwrite(&(inode.size), sizeof(int), 1, fp);
		for(int j = 0;j < 13 ;j++){
			fwrite(&(inode.blogs[j]), sizeof(int), 1, fp);
		}
		
		fwrite(&inode.modified_time,sizeof(char),20,fp);
	
	}
}
void creation_file_system::write_inode_dir(){
	fseek(fp, superBlock.first_inodes_block_num * block_size, SEEK_SET );
	root.is_dir = true;
 	root.size = block_size;
 	root.blogs[0] = 0;
 	fwrite(&(root.is_dir), sizeof(bool), 1, fp);
 	fwrite(&(root.size), sizeof(int), 1, fp);
 	fwrite(&(root.blogs[0]), sizeof(int), 1, fp);

 	for(int i = 1 ; i < 13;i++){
 		root.blogs[i]=-1;
 		fwrite(&(root.blogs[i]), sizeof(int), 1, fp);
 	}

 	fwrite(&root.modified_time,sizeof(char),20,fp);
}
void creation_file_system::write_data(){
	fseek(fp, superBlock.first_data_block_num * block_size, SEEK_SET);
	
	char root[14]="..\0";
	char current[14]=".\0";
	
	data_entry root_entry(0,root);
	fwrite(&root_entry,16, 1, fp);

	data_entry current_entry(0,current);
 	fwrite(&current_entry,16, 1, fp);

 	char free[14]="\0";
 	data_entry empty_entry(-1,free);

 	for(int i = 0 ; i < (block_size / 16) - 2; i++){
		fwrite(&empty_entry,16, 1, fp);		
 	}

 	
 	/*1. inode artık root ile dolu*/
 	
 	fseek(fp, 7 * sizeof(int), SEEK_SET );
 	int tempA = spaceManagement.free_block_num - 1;
 	fwrite(&tempA, sizeof(int), 1, fp);
 	
 	fseek(fp, ( 8 * sizeof(int)), SEEK_SET );
 	char temp = 'u';
 	fwrite(&temp, sizeof(char), 1, fp); 
}
void creation_file_system :: read_information(){
	
	int block_size;
	int inode_number;
	int total_blocks_number;
	int blocks_inode_number;
	int superblock_and_spacem_block;
	int first_inodes_block_num;	
	int first_data_block_num;	
	int free_block_num;
	fseek(fp,0,SEEK_SET);
	
	fread(&block_size,sizeof(int), 1, fp);
	fread(&inode_number,sizeof(int), 1, fp);
	fread(&total_blocks_number, sizeof(int), 1, fp);
	fread(&blocks_inode_number,   sizeof(int), 1, fp);
	fread(&superblock_and_spacem_block, sizeof(int), 1, fp);
	fread(&first_inodes_block_num,   sizeof(int), 1, fp);
	fread(&first_data_block_num, sizeof(int), 1, fp);
	
	
	fread(&free_block_num, sizeof(int), 1, fp);

	char *free_inode_blocks = new char[inode_number]; 
	memset(free_inode_blocks, 0, inode_number); 
	
	char *free_data_blocks = new char[total_blocks_number];  
	memset(free_data_blocks, 0, total_blocks_number); 

	int i ;

	for(i = 0; i < inode_number;i++){
		fread(&free_inode_blocks[i], 1,1 , fp);
	}
	for(i = 0; i < total_blocks_number;i++){
		fread(&free_data_blocks[i], 1,1 , fp);
	}
	cout<<"\n************SUPERBLOCK SECTION READING FROM DISK..******************"<<endl;
	cout<<"BLOCK SIZE ="<<block_size<<endl;
	
	cout<<"NUMBER OF I NODE ="<<inode_number<<endl;
	
	cout<<"NUMBER OF TOTAL BLOCK ="<<total_blocks_number<<endl;
	
	cout<<"NUMBER OF INODE BLOCK ="<<blocks_inode_number<<endl;
	
	cout<<"SUPERBLOCK + SPACE MANAGEMENT BLOCK="<<superblock_and_spacem_block<<endl;
	
	cout<<"FIRST INODE BLOCK NUMBER="<<first_inodes_block_num<<endl;
	
	cout<<"FIRS DATA BLOCK NUMBER="<<first_data_block_num<<endl;

	cout<<"\n\n************SPACE MANAGEMENT SECTION READING FROM DISK..***********"<<endl;
	cout<<"FREE BLOCK NUMBER ="<<free_block_num<<endl;
	cout<<endl;
	cout<<"free node list"<<endl;
	cout<<"{";
	for(i = 0; i < inode_number;i++){
		cout<<free_inode_blocks[i]<<',';
	}
	cout<<"}";
	cout<<endl;
	cout<<"free block list"<<endl;
	cout<<"{";
	for(i = 0; i < total_blocks_number;i++){
		cout<<free_data_blocks[i]<<',';
	}
	cout<<"}";
	cout<<endl;
}
