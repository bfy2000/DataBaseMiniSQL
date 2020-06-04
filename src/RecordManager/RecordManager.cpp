#include "recordManager.h"

extern CatalogManager catalogManager;

/**
	 从逻辑上——
	 1、recordManger的操作应该是建立在"表"这个概念上的。也就是对表进行操作。
	 2、根据word版本的《Record Manager模块设计说明》里面，
	    是由catalog manger“提取表名及记录，根据信息计算插入记录的块号，并调用Buffer Manager 的功能”。
	 3、而catalog manager里面已经把从表名得到表的函数写好了，直接调用就行判断不为空即可。不需要额外判断表的存在与否。
	 4、所以我认为应该把buffer里面的FileInfo作为Table类里面的一个成员，要用到数据的时候再去从buffer里面得到。
	 5、感谢buffer同学，让我能直接从tableName和index中就得到我所需要的数据。
 */


//TODO:查询时可能需要考虑索引，但尚未知道接口

//是否应当与buffer中的filelist联系起来
//Table* RecordManager::isExistTable(string tableName)
//{
//	int n = tables.size();
//	for (int i = 0; i < n; i++) {
//		if (tables[i].tableName == tableName) {
//			return &(tables[i]);
//		}
//	}
//	return NULL;
//}

////TODO:createTable 与 dropTable在catalogManager中也存在，二者可能需要做一个统一或直接采用CatalogManager中的函数
//bool RecordManager::createTable(string tableName, string primaryKey, vector<Attribute> attributeVector)
//{
//	if (isExistTable(tableName) != NULL) {
//		cerr << "同名的表格已经存在" << endl;
//		return false;
//	}
//	Table table(tableName, primaryKey, attributeVector);
//	this->tables.push_back(table);
//	return true;
//}

//bool RecordManager::dropTable(string tableName)
//{
//	Table* table = isExistTable(tableName);
//	if (table == NULL) {
//		cerr << "不存在表格：" << tableName << endl;
//		return false;
//	}
//	for (auto it = this->tables.begin(); it != this->tables.end(); ++it) {
//		if (it->tableName == tableName) {
//			this->tables.erase(it);
//			return true;
//		}
//	}
//	return false;
//}

//insertValue返回值为false表示出错，否则为正确执行
bool RecordManager::insertValue(string tableName, Tuple tuple)
{
	Table table = catalogManager.get_table(tableName);//建议把这个函数的返回值定义成返回指针
	//if (table == NULL) {// 这里无法判断是否有这个表
	//	cerr << "没有这个表" << endl;
	//	return false;
	//}
	if (isMatchTheAttribute(&table, &tuple) == false) {
		cerr << "Tuple attributes do not match the table" << endl;
		return false;
	}

	if (isConflictTheUnique(&table, &tuple) == true) {
		cerr << "the primary key conflict" << endl;
		return false;
	}

	char* tmpChar = (char*)malloc((table.rowLength + 2) * sizeof(char));
	tupleToChar(tuple, tmpChar);
	bool writeResult = writeToIndex(table.tableName, table.rowNum, tmpChar, table.rowLength, "db_name?", 0);
	free(tmpChar);
	if (writeResult == false) {
		cerr << "写入buffer失败" << endl;
		return false;
	}
	else {
		cout << "写入buffer成功" << endl;
		table.rowNum += 1;
	}
	return true;
}

bool RecordManager::isMatchTheAttribute(Table* table, Tuple* tuple)
{
	std::vector<Element> data = tuple->getData();
	if (data.size() != table->attributeNum) {
		cerr << "Attribute number not match!" << endl;
		return false;
	}
	for (int i = 0; i < data.size(); ++i) {
		if (data[i].type != table->attributeVector[i].type.get_type()) {
			cerr << "Attribute type not match!" << endl;
			return false;
		}
	}
	return true;
}

bool RecordManager::isConflictTheUnique(Table* table, Tuple* tuple)
{
	for (int i = 0; i < table->rowNum; ++i) {
		Tuple tmpTuple = getTupleByIndex(table, i);
		if (isConflict(&tmpTuple, tuple, table->primaryKeyIndex) == true) {
			cerr << "primary key conflict" << endl;
			return true;//认为发生了冲突
		}
	}
	return false;
}

bool RecordManager::isConflict(Tuple* tuple1, Tuple* tuple2, int index)
{
	if (tuple1->getIsDeleted() || tuple2->getIsDeleted()) {//若其中一个元组被删除，则不可能发生冲突
		return false;
	}
	return tuple1->getData()[index] == tuple2->getData[index];
}

Tuple RecordManager::getTupleByIndex(Table* table, int index)
{
	Tuple tuple;
	char* tmpData = (char*)malloc((table->rowLength + 2) * sizeof(char));
	if (tmpData == NULL) {
		cerr << "can't get memory in isConflictTheUnique()" << endl;
		return tuple;
	}
	int tmpLength = table->rowLength;
	// 不知道db_name是什么东西
	readData(table->tableName, "db_name?", index * table->rowLength, tmpData, tmpLength, 0);
	Tuple tmpTuple = charToTuple(table, tmpData);
	free(tmpData);
	return tuple;
}

//仅允许单个条件，或者多个条件的and类型的查询操作（or是不行的）
vector<Tuple> RecordManager::searchQuery(string tableName, vector<SelectCondition> selectConditions)
{
	vector<Tuple> ans;
	Table table = catalogManager.get_table(tableName);//建议把这个函数的返回值定义成返回指针
	//if (table == NULL) {// 这里无法判断是否有这个表
	//	cerr << "没有这个表" << endl;
	//	return ans;
	//}
	for (int i = 0; i < table.rowNum; i++) {
		Tuple tmp = getTupleByIndex(&table, i);
		if (tmp.getIsDeleted()) continue;
		bool valid = true;
		for (auto it = selectConditions.begin(); it != selectConditions.end(); ++it) {
			if (judgeCondition(tmp, *it) == false) {
				valid = false;
				break;
			}
		}
		if (valid) {
			ans.push_back(tmp);
		}
	}
	return ans;
}
//A<10 or B<10
vector<Tuple> RecordManager::searchQuery(string tableName)
{
	vector<Tuple> ans;
	Table table = catalogManager.get_table(tableName);//建议把这个函数的返回值定义成返回指针
	//if (table == NULL) {// 这里无法判断是否有这个表
	//	cerr << "没有这个表" << endl;
	//	return ans;
	//}
	for (int i = 0; i < table.rowNum; i++) {
		Tuple tmp = getTupleByIndex(&table, i);
		if (tmp.getIsDeleted()) continue;
		ans.push_back(tmp);
	}
	return ans;
}

bool RecordManager::judgeCondition(Tuple tuple, SelectCondition condition)
{
	if (tuple.getIsDeleted() == true) {
		return false;//A<10       B<10
	}
	else if (condition.opt == LESS) {
		if (tuple.getData()[condition.attributeIndex] < condition.value)
			return true;
	}
	else if (condition.opt == LESS_OR_EQUAL) {
		if (tuple.getData()[condition.attributeIndex] <= condition.value)
			return true;
	}
	else if (condition.opt == EQUAL) {
		if (tuple.getData()[condition.attributeIndex] == condition.value)
			return true;
	}
	else if (condition.opt == GREATER_OR_EQUAL) {
		if (tuple.getData()[condition.attributeIndex] >= condition.value)
			return true;
	}
	else if (condition.opt == GREATER) {
		if (tuple.getData()[condition.attributeIndex] > condition.value)
			return true;
	}
	else if (condition.opt == NOT_EQUAL) {
		if (tuple.getData()[condition.attributeIndex] != condition.value)
			return true;
	}

	//删除所有的区间比较，需要其他模块，把类似于区间的操作解析为两个比较操作，
	//比如and 的话，变成两个符号操作。
	//else if (condition.opt == INTERVAL) {
	//	if (condition.value.first <= tuple.data[condition.attributeIndex] && tuple.data[condition.attributeIndex] < condition.value.second)
	//		return true;
	//}
	return false;
}



int RecordManager::deleteTuple(string tableName, vector<SelectCondition> selectConditions)
{
	Table table = catalogManager.get_table(tableName);//建议把这个函数的返回值定义成返回指针
	//if (table == NULL) {// 这里无法判断是否有这个表
	//	cerr << "没有这个表" << endl;
	//	return ans;
	//}
	vector<Tuple> searchResult = searchQuery(tableName, selectConditions);
	if (searchResult.size() == 0) {
		cerr << "no tuple need to be deleted, please check the conditions" << endl;
		return 0;
	}
	int deleteNum = searchResult.size();
	for (int i = 0; i < deleteNum; i++) {
		Tuple tuple = searchResult[i];
		char* tmpChar = (char*)malloc(sizeof(char));
		*tmpChar = '0';
		bool writeResult = writeToIndex(tableName, tuple.getIndex(), tmpChar, 1, "db_name?", 0);//修改最前面的删除标记
		free(tmpChar);
		if (writeResult == false) {
			cerr << "write to buffer error" << endl;
			return -1;
		}
		//table.rowNum -= 1;//但是不修改数据行数标记
	}
	return deleteNum;
}

int RecordManager::deleteTuple(string tableName)
{
	Table table = catalogManager.get_table(tableName);//建议把这个函数的返回值定义成返回指针
	//if (table == NULL) {// 这里无法判断是否有这个表
	//	cerr << "没有这个表" << endl;
	//	return ans;
	//}
	vector<Tuple> searchResult = searchQuery(tableName);
	if (searchResult.size() == 0) {
		cerr << "no tuple need to be deleted, please check the conditions" << endl;
		return 0;
	}
	int deleteNum = searchResult.size();
	for (int i = 0; i < deleteNum; i++) {
		Tuple tuple = searchResult[i];
		char* tmpChar = (char*)malloc(sizeof(char));
		*tmpChar = '0';
		bool writeResult = writeToIndex(tableName, tuple.getIndex(), tmpChar, 1, "db_name?", 0);//修改最前面的删除标记
		free(tmpChar);
		if (writeResult == false) {
			cerr << "write to buffer error" << endl;
			return -1;
		}
		//table.rowNum -= 1;//但是不修改数据行数标记
	}
	return deleteNum;
}
