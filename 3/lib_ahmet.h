#ifndef lib_ahmet
#define lib_ahmet
#include <fstream>
using namespace std;




class data_entry  
{
	public:
		data_entry(short node_id , char file_name[14]);
	public:
	short  node_id;
	char file_name[14];
};


class super_block
{
	
	public:

		super_block(int block_size,int inode_number,int total_blocks_number);
		super_block();
		void print();
	public:
		int block_size;
		int inode_number;
		int total_blocks_number;
		int blocks_inode_number;
		int superblock_and_spacem_block;
		int first_inodes_block_num;	
		int first_data_block_num;
		
};


class space_management
{
	public:
		int  free_block_num;
		char free_inode_blocks[200];
		char *free_data_blocks;
	public:
		void space_management_items(int total_blocks_number,int first_data_block_num);
};


class i_node
{
 	public:
 		i_node();
 	public:
	 	bool is_dir;
		int size;
		int blogs[13];
		char modified_time[20];
};

class creation_file_system{
	private:
		int block_size;
		int inode_number;
		int total_blocks_number ;
		FILE * fp;
		super_block superBlock;
		space_management spaceManagement;
		i_node inode;
		i_node root;
		void write_superblock();
		void write_space_management();
		void write_inodes();
		void write_inode_dir();
		void write_data();
		void read_information();
		

	public:
		creation_file_system(int block_size,int inode_number,int total_blocks_number,FILE *fp);
		
};

#endif