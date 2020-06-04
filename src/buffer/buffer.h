#define BLOCK_SIZE 4096
#define MAX_ACTIVE_FILE 5
#define MAX_BLOCK 40
#include<string>
#include<forward_list>
#include<ctime>
#include<iostream>
#include<fstream>
using namespace std;

typedef struct fileInfo *FileInfo;
typedef struct blockInfo *BlockInfo;

struct fileInfo {
	int type;				// 0-> 数据文件， 1 -> 索引文件
	string fileName;		// 文件名
	int recordAmount;		// 文件中的记录数
	int freeNum;			// 文件中可用空块的数量
	int recordLength;		// 文件中记录的长度
	FileInfo next;			// 链表指针
	BlockInfo firstBlock;	// 该文件下，块链表的表头
};


struct blockInfo {
	int blockNum;		// 表示这个块是对应文件下的第num个块
	bool dirtyBit;		// 0 不需要写回文件，1需要
	BlockInfo next;  	// 链表指针
	FileInfo file;     	// 块所属的文件
	int charNum;	   	// 块的已用空间大小
	char *cBlock;	   	// 块的储存空间
	int call_times;		// 块的使用次数 
	int lock;			// 加锁的情况下不能被替换
	bool isfree;		// 是否为空块
};


//寻找空块
BlockInfo findBlock();

//在filelist中找一个文件
FileInfo get_file_info(string fileName, int fileType);

//在buffer中查找一个块
BlockInfo get_block_in_file(FileInfo file, int blockNum);

//在buffer中把一个块移出对应文件
void remove_block_from_file(BlockInfo block_to_remove);

//缓冲区操作：把一个块挂到文件上
void add_block_to_file(BlockInfo block, FileInfo file);

//从磁盘读一个块
BlockInfo read_block_from_disk(FileInfo file, string db_name, int blocknum, BlockInfo tmpblock);

//把一个块写到磁盘
void write_block_to_disk(string db_name, BlockInfo block);

//从文件中读取某个块
BlockInfo get_block_info(string db_name, string table_name, int file_type, int BlockNum);

//关闭文件
void closefile(string db_name, FileInfo file);

//把一个文件加入filelist
FileInfo add_file_to_list(string table_name, int type, int recordAmount, int recordlen);

//锁住一个块
void lock_block(BlockInfo block);

//解锁一个块
void unlock_block(BlockInfo block);

void write_to_block(BlockInfo block, char to_write[]);

void add_to_block(BlockInfo block, char to_write[]);

//把to_write[]数据的length长度的数据，写入从block开始的第index个字节上，注意把后面的数据后移，防止覆盖，如果index<0那么表示将数据插入最后
bool writeToIndex(string fileName, int index, char* to_write, int length, string db_name, int filetype);

//给出fileName和index（从block开始的第index个字节），后面读取length长度的数据，保存到to_read指针上，输入保证这个指针能存下，如果数据读取读不到那么长的数据，则修改length，返回false
bool readData(string fileName, string db_name, int index, char to_read[], int& length, int filetype);

