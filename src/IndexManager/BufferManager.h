#pragma once
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
struct blockInfo;
typedef struct blockInfo* BlockInfo;
struct fileInfo;
typedef struct fileInfo* FileInfo;

#define BLOCK_SIZE 4096
struct blockInfo {
	int blockNum;
	bool dirtyBit;
	BlockInfo next;
	FileInfo file;
	int charNum;
	char *cBlock;
	int call_times;
	int lock;
	bool isfree;
};

struct fileInfo {
	int type;
	string fileName;
	int recordAmount;
	int freeNum;
	int recordLength;
	FileInfo next;
	BlockInfo firstBlock;
};


class BufferManager {
public:
	BufferManager() { BIList.push_back(nullptr); listNum = 0; }
	//BlockInfo get_block_info(string db_name, string table_name, int file_type, int BlockNum) { return new blockInfo(); }
	BlockInfo get_block_info(string db_name,string table_name,int type , int blockNum) { 
		if (blockNum > listNum) {
			listNum++;
			BlockInfo tmp = new blockInfo();
			tmp->blockNum = listNum;
			tmp->cBlock = (char*)malloc(sizeof(char) * 4096);
			tmp->cBlock[0] = 0;
			BIList.push_back(tmp);
			return tmp;
		}
		else return BIList[blockNum];
	}
	FileInfo get_file_info(string db_name, string fileName, int file_type) { return nullptr; }
	void add_block_to_file(BlockInfo block, FileInfo file) {
		if (block->blockNum > listNum ) {
			if (block->blockNum - 1 != listNum) {
				cout << "Error in bm" << endl;
				return;
			}
			listNum++;
			BIList.push_back(block);
		}
		else {
			BIList[block->blockNum] = block;
		}
	}
	vector<BlockInfo> BIList;
	int listNum;
};

