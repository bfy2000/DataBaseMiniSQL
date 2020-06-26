#include "IndexManager.h"

IndexManager::IndexManager() {
	//this->bm = bm;
}

IndexManager::~IndexManager() {

}
void IndexManager::newFile(std::string db_name, std::string filename) {
	string filepath = rootdir + db_name;
	filepath += "\\index\\";
	filepath += filename + ".txt";
	std::fstream file;
	file.open(filepath, std::ios::out | std::ios::binary);
}
bool IndexManager::is_tree_already_exist(std::string filename, FieldType type) {
	if (type.get_type() == INT) {
		if (int_index.count(filename) == 0) {
			return false;
		}
		else {
			return true;
		}
	}
	else if (type.get_type() == FLOAT) {
		if (float_index.count(filename) == 0) {
			return false;
		}
		else {
			return true;
		}
	}
	else if (type.get_type() == CHAR) {
		if (string_index.count(filename) == 0) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		return false;
	}
}
int IndexManager::get_degree(FieldType type) {
	if (type.get_type() == INT)
	{
		return (BLOCK_SIZE - (sizeof(char) + 3 * sizeof(int))) / (2 * sizeof(int)) - 2;
	}
	else if (type.get_type() == FLOAT)
	{
		return (BLOCK_SIZE - (sizeof(char) + 3 * sizeof(int))) / (sizeof(float) + sizeof(int)) - 2;
	}
	else if (type.get_type() == CHAR)
	{
		return (BLOCK_SIZE - (sizeof(char) + 3 * sizeof(int))) / (type.get_length() + sizeof(int)) - 2;
	}
	else
	{
		return 0;
	}
}
Result IndexManager::create_index(const std::string &db_name, const std::string &table_name, const std::string &attribute_name, FieldType type) {
	std::string filename = table_name;
	newFile(db_name, filename);
	FileInfo tmp = get_file_info(filename, 1);
	if (tmp == nullptr) {
		add_file_to_list(table_name, 1, 4092, 4092);
	}

	BlockId bpt = GetEmptyBlockId(db_name,table_name);//get blockid for tree
	char* block = getBlock(db_name, table_name,bpt);
	*((int*)block) = -1;
	*((int*)(block + sizeof(int))) = -1;
	if (is_tree_already_exist(filename+"_"+attribute_name, type)) {
		return BPT_ALREADY_EXSIST;
	}
	else {
		if (type.get_type() == INT) {//int
			int_index[filename + "_" + attribute_name] = std::make_shared<BPlusTree<int>>(db_name, table_name, get_degree(type), sizeof(int));
			int_index_start_block[filename + "_" + attribute_name] = bpt;
		}
		else if (type.get_type() == FLOAT) {//float
			float_index[filename + "_" + attribute_name] = std::make_shared<BPlusTree<float>>(db_name, table_name, get_degree(type), sizeof(float));
			float_index_start_block[filename + "_" + attribute_name] = bpt;
		}
		else if (type.get_type() == CHAR) {
			string_index[filename + "_" + attribute_name] = std::make_shared<BPlusTree<std::string>>(db_name, table_name, get_degree(type), type.get_length());
			string_index_start_block[filename + "_" + attribute_name] = bpt;
			if (string_index_length.count(filename + "_" + attribute_name) == 0)
			{
				string_index_length[filename + "_" + attribute_name] = type.get_length();
			}
		}else{
			return WRONG_TYPE;
		}
	}
	return SUCCESS;
}

/*void IndexManager::readintree(std::string db_name, std::string table_name, const std::string &attribute_name, int type) {
	std::string filename = table_name;
	FileInfo tmp = get_file_info(filename, 1);
	if (tmp == nullptr) {
		add_file_to_list(table_name, 1, 4092, 4092);
	}
	char* block;
	if (type == 0) {
		block = get_block_info(db_name, table_name, 1, int_index_start_block[filename + "_" + attribute_name])->cBlock;
	}
	else if (type == -1) {
		block = get_block_info(db_name, table_name, 1, float_index_start_block[filename + "_" + attribute_name])->cBlock;
	}
	else if (type > 0) {
		block = get_block_info(db_name, table_name, 1, string_index_start_block[filename + "_" + attribute_name])->cBlock;
	}
	else {
		std::cout << "illegal type in readintree\n";
		return;
	}

	BlockId Root = *((int*)(block));
	BlockId LeftLeaf = *((int*)(block + sizeof(int)));
	if (type == 0) {

		int_index[filename+"_"+ attribute_name] = std::make_shared<BPlusTree<int>>(db_name, table_name, get_degree(type), sizeof(int), Root, LeftLeaf);
	}
	else if (type == -1) {//float
		float_index[filename + "_" + attribute_name] = std::make_shared<BPlusTree<float>>(db_name, table_name, get_degree(type), sizeof(float), Root, LeftLeaf);
	}
	else if (type > 0) {
		string_index[filename + "_" + attribute_name] = std::make_shared<BPlusTree<std::string>>(db_name, table_name, get_degree(type), type, Root, LeftLeaf);
		if (string_index_length.count(filename + "_" + attribute_name) == 0)
		{
			string_index_length[filename + "_" + attribute_name] = type;
		}
	}
}*/

Result IndexManager::find_element(const std::string &db_name, const std::string &table_name,
	const std::string &attribute_name, FieldType type, const std::string &data, std::vector<int> &block_id) {
	std::string filename = table_name;
	FileInfo tmp = get_file_info(filename, 1);
	if (tmp == nullptr) {
		add_file_to_list(table_name, 1, 4092, 4092);
	}
	if (!is_tree_already_exist(filename+"_"+attribute_name, type))
	{
		return NO_INDEX;
		//从硬盘中加载
		//readintree(db_name, table_name, attribute_name, type);
	}
	if (type.get_type() == INT) {
		block_id.push_back(int_index[filename+"_"+ attribute_name]->Search(atoi(data.c_str())));
		if (block_id[0] == -1)return NO_SUCH_ELEMENT;
		return SUCCESS;
	}
	else if (type.get_type() == FLOAT) {
		block_id.push_back(float_index[filename + "_" + attribute_name]->Search(atof(data.c_str())));
		if (block_id[0] == -1)return NO_SUCH_ELEMENT;
		return SUCCESS;
	}
	else if (type.get_type() == CHAR) {
		block_id.push_back(string_index[filename + "_" + attribute_name]->Search(data));
		if (block_id[0] == -1)return NO_SUCH_ELEMENT;
		return SUCCESS;
	}

	return WRONG_TYPE;
}
Result IndexManager::insert_index(const std::string &db_name, const std::string &table_name,
	const std::string &attribute_name, FieldType type, const std::string &data, int block_id) {
	std::string filename = table_name;
	FileInfo tmp = get_file_info(filename, 1);
	if (tmp == nullptr) {
		add_file_to_list(table_name, 1, 4092, 4092);
	}
	if (!is_tree_already_exist(filename+"_"+attribute_name, type))
	{
		return NO_INDEX;
		//从硬盘中加载
		//readintree(db_name, table_name, attribute_name, type);
	}
	if (type.get_type() == INT) {
		if(int_index[filename + "_" + attribute_name]->InsertElement(atoi(data.c_str()), block_id)){
			return SUCCESS;
		}else{
			return INSERT_FAIL;
		}
	}
	else if (type.get_type() == FLOAT) {
		if(float_index[filename + "_" + attribute_name]->InsertElement(atof(data.c_str()), block_id)){
			return SUCCESS;
		}else{
			return INSERT_FAIL;
		}
	}
	else if (type.get_type() == CHAR) {
		if(string_index[filename + "_" + attribute_name]->InsertElement(data, block_id)){
			return SUCCESS;
		}else{
			return INSERT_FAIL;
		}
	}
	return WRONG_TYPE;
}
Result IndexManager::delete_index(const std::string &db_name, const std::string &table_name,
	const std::string &attribute_name, FieldType type, const std::string &data) {
	std::string filename = table_name;
	FileInfo tmp = get_file_info(filename, 1);
	if (tmp == nullptr) {
		add_file_to_list(table_name, 1, 4092, 4092);
	}
	if (!is_tree_already_exist(filename+"_"+attribute_name, type))
	{
		return NO_INDEX;
		//从硬盘中加载
		//readintree(db_name, table_name, attribute_name, type);
	}
	if (type.get_type() == INT) {
		if(int_index[filename + "_" + attribute_name]->DeleteElement(atoi(data.c_str()))){
			return SUCCESS;
		}else{
			return DELETE_FAIL;
		}
	}
	else if (type.get_type() == FLOAT) {
		if(float_index[filename + "_" + attribute_name]->DeleteElement(atof(data.c_str()))){
			return SUCCESS;
		}else{
			return DELETE_FAIL;
		}
	}
	else if (type.get_type() == CHAR) {
		if(string_index[filename + "_" + attribute_name]->DeleteElement(data)){
			return SUCCESS;
		}else{
			return DELETE_FAIL;
		}
	}
	return WRONG_TYPE;
}
Result IndexManager::drop_index(const std::string db_name, const std::string &table_name, const std::string &attribute_name, FieldType type) {
	std::string filename = table_name;
	FileInfo tmp = get_file_info(filename, 1);
	if (tmp == nullptr) {
		add_file_to_list(table_name, 1, 4092, 4092);
	}
	if (is_tree_already_exist(filename+"_"+attribute_name, type))
	{
		if (type.get_type() == INT)
		{
			int_index.erase(filename + "_" + attribute_name);
			BlockInfo blockinfo = get_block_info(db_name, table_name, 1, int_index_start_block[filename + "_" + attribute_name]);
			char* block = blockinfo->cBlock;
			blockinfo->charNum = 1;
			blockinfo->cBlock[0] = 0;
			blockinfo->dirtyBit = true;
		}
		else if (type.get_type() == FLOAT)
		{
			float_index.erase(filename + "_" + attribute_name);
			BlockInfo blockinfo = get_block_info(db_name, table_name, 1, float_index_start_block[filename + "_" + attribute_name]);
			char* block = blockinfo->cBlock;
			blockinfo->charNum = 1;
			blockinfo->cBlock[0] = 0;
			blockinfo->dirtyBit = true;
		}
		// 字符串
		else if (type.get_type() == CHAR)
		{
			string_index.erase(filename + "_" + attribute_name);
			BlockInfo blockinfo = get_block_info(db_name, table_name, 1, string_index_start_block[filename + "_" + attribute_name]);
			char* block = blockinfo->cBlock;
			blockinfo->charNum = 1;
			blockinfo->cBlock[0] = 0;
			blockinfo->dirtyBit = true;
			string_index_length.erase(filename + "_" + attribute_name);
		}else{
			return WRONG_TYPE;
		}
		return SUCCESS;
	}else{
		return NO_INDEX;
	}
	/*fstream _file;
	_file.open(filename, ios::in);
	if (_file) {
		remove(filename.c_str());
	}*/
}
Result IndexManager::greater_than(const std::string &db_name, const std::string &table_name,
	const std::string &attribute_name, FieldType type, const std::string &data, std::vector<int> &block_id, int isEqual) {
	std::string filename = table_name;
	FileInfo tmp = get_file_info(filename, 1);
	if (tmp == nullptr) {
		add_file_to_list(table_name, 1, 4092, 4092);
	}
	if (!is_tree_already_exist(filename+"_"+attribute_name, type))
	{
		return NO_INDEX;
		//从硬盘中加载
		//readintree(db_name, table_name, attribute_name, type);
	}

	filename = filename + "_" + attribute_name;
	if (!isEqual) {

		if (type.get_type() == INT) {
			int_index[filename]->greater_than(atoi(data.c_str()), block_id);
		}
		else if (type.get_type() == FLOAT) {
			float_index[filename]->greater_than(atof(data.c_str()), block_id);
		}
		else if (type.get_type() == CHAR) {
			string_index[filename]->greater_than(data, block_id);
		}else{
			return WRONG_TYPE;
		}
		return SUCCESS;
	}
	else {
		if (type.get_type() == INT) {
			block_id.push_back(int_index[filename]->Search(atoi(data.c_str())));
			if (block_id[0] == -1) {
				block_id.clear();
			}
			int_index[filename]->greater_than(atoi(data.c_str()), block_id);
		}
		else if (type.get_type() == FLOAT) {
			block_id.push_back(float_index[filename]->Search(atof(data.c_str())));
			if (block_id[0] == -1) {
				block_id.clear();
			}
			float_index[filename]->greater_than(atof(data.c_str()), block_id);
		}
		else if (type.get_type() == CHAR) {
			block_id.push_back(string_index[filename]->Search(data));
			if (block_id[0] == -1) {
				block_id.clear();
			}
			string_index[filename]->greater_than(data, block_id);
		}else{
			return WRONG_TYPE;
		}
		return SUCCESS;
	}

}

Result IndexManager::less_than(const std::string &db_name, const std::string &table_name,
	const std::string &attribute_name, FieldType type, const std::string &data, std::vector<int> &block_id, int isEqual) {
	std::string filename = table_name;
	FileInfo tmp = get_file_info(filename, 1);
	if (tmp == nullptr) {
		add_file_to_list(table_name, 1, 4092, 4092);
	}
	if (!is_tree_already_exist(filename+"_"+attribute_name, type))
	{
		return NO_INDEX;
		//从硬盘中加载
		//readintree(db_name, table_name, attribute_name, type);
	}

	filename = filename + "_" + attribute_name;
	if (!isEqual) {

		if (type.get_type() == INT) {
			int_index[filename]->less_than(atoi(data.c_str()), block_id);
		}
		else if (type.get_type() == FLOAT) {
			float_index[filename]->less_than(atof(data.c_str()), block_id);
		}
		else if (type.get_type() == CHAR) {
			string_index[filename]->less_than(data, block_id);
		}else{
			return WRONG_TYPE;
		}
		return SUCCESS;
	}
	else {
		if (type.get_type() == INT) {
			int_index[filename]->less_than(atoi(data.c_str()), block_id);
			BlockId tmp=int_index[filename]->Search(atoi(data.c_str()));
			if (tmp != -1) {
				block_id.push_back(tmp);
			}
		}
		else if (type.get_type() == FLOAT) {
			float_index[filename]->less_than(atof(data.c_str()), block_id);
			BlockId tmp = float_index[filename]->Search(atof(data.c_str()));
			if (tmp != -1) {
				block_id.push_back(tmp);
			}
		}
		else if (type.get_type() == CHAR) {
			string_index[filename]->less_than(data, block_id);
			BlockId tmp = string_index[filename]->Search(data);
			if (tmp != -1) {
				block_id.push_back(tmp);
			}
		}else{
			return WRONG_TYPE;
		}
		return SUCCESS;
	}

}

BlockId IndexManager::GetEmptyBlockId(const std::string &db_name, const std::string &table_name){
	BlockId i = 1;
	char *block;
	do {
		i++;//start from 0
		BlockInfo tmp = get_block_info(db_name, table_name, 1, i);
		block = tmp->cBlock;
	} while (block[0] != 0);
	return i;
}


bool IndexManager::is_index_exist(const std::string &db_name, const std::string &table_name,
	const std::string &attribute_name, FieldType type){
		return is_tree_already_exist(table_name+"_"+attribute_name, type);
	}