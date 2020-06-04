#pragma once
#include "BPlusTree.h"
#include "BufferManager.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <fstream>
//using namespace std;

class IndexManager
{
public:
	IndexManager(BufferManager* bm);
	~IndexManager();
	//int type
	//0 integer
	//-1 float
	//n string
	void create_index(const std::string &db_name, const std::string &table_name, const std::string &index_name, int type);
	bool find_element(const std::string &db_name, const std::string &table_name,
		const std::string &index_name, const std::string &data, std::vector<int> &block_id, int type);
	bool insert_index(const std::string &db_name, const std::string &table_name,
		const std::string &index_name, const std::string &data, int block_id, int type);
	bool delete_index(const std::string &db_name, const std::string &table_name,
		const std::string &index_name, const std::string &data, int type);
	void drop_index(const std::string &table_name, const std::string &index_name, int type);

private:
	BufferManager *bm;
	std::map<std::string, std::shared_ptr<BPlusTree<int>>> int_index;
	std::map<std::string, std::shared_ptr<BPlusTree<float>>> float_index;
	std::map<std::string, std::shared_ptr<BPlusTree<std::string>>> string_index;
	std::map<std::string, int> string_index_length;
	void newFile(std::string filename);
	bool is_tree_already_exist(std::string filename, int type);
	int get_degree(int type);
	void readintree(std::string db_name, std::string table_name, int type);
};