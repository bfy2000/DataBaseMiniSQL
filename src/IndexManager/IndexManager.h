#pragma once
#include "BPlusTree.h"
#include "../buffer/buffer.h"
#include "../Public/FieldType.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <direct.h>
using namespace std;
extern string rootdir;

class IndexManager
{
public:
	IndexManager();
	~IndexManager();
	//int type
	//0 integer
	//-1 float
	//n string
	Result create_index(const std::string &db_name, const std::string &table_name,const std::string &attribute_name, FieldType type);
	Result find_element(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type, const std::string &data, std::vector<int> &block_id);
	Result insert_index(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type, const std::string &data, int block_id);
	Result delete_index(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type, const std::string &data);
	Result drop_index(const std::string db_name, const std::string &table_name, const std::string &attribute_name, FieldType type);
	Result greater_than(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type, const std::string &data, std::vector<int> &block_id, int isEqual);
	Result less_than(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type, const std::string &data, std::vector<int> &block_id, int isEqual);

	bool is_index_exist(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type);


private:
	//BufferManager *bm;
	std::map<std::string, std::shared_ptr<BPlusTree<int>>> int_index;
	std::map<std::string, std::shared_ptr<BPlusTree<float>>> float_index;
	std::map<std::string, std::shared_ptr<BPlusTree<std::string>>> string_index;
	std::map<std::string, int> string_index_length;
	std::map<std::string, BlockId> int_index_start_block;
	std::map<std::string, BlockId> float_index_start_block;
	std::map<std::string, BlockId> string_index_start_block;

	void newFile(std::string db_name, std::string filename);
	bool is_tree_already_exist(std::string filename, FieldType type);
	int get_degree(FieldType type);
	//void readintree(std::string db_name, std::string table_name, const std::string &attribute_name, int type);
	BlockId GetEmptyBlockId(const std::string &db_name, const std::string &table_name);
	char* getBlock(const std::string &db_name, const std::string &table_name,BlockId id) {
		BlockInfo blockinfo = get_block_info(db_name, table_name, 1, id);
		return blockinfo->cBlock;
	}
};