#include "RecordManager.h"


/**
         从逻辑上——
         1、recordManger的操作应该是建立在"表"这个概念上的。也就是对表进行操作。
         2、根据word版本的《Record Manager模块设计说明》里面，
            是由catalog
   manger“提取表名及记录，根据信息计算插入记录的块号，并调用Buffer Manager
   的功能”。 3、而catalog
   manager里面已经把从表名得到表的函数写好了，直接调用就行判断不为空即可。不需要额外判断表的存在与否。
         4、所以我认为应该把buffer里面的FileInfo作为Table类里面的一个成员，要用到数据的时候再去从buffer里面得到。
         5、感谢buffer同学，让我能直接从tableName和index中就得到我所需要的数据。
 */

// TODO:查询时可能需要考虑索引，但尚未知道接口

//是否应当与buffer中的filelist联系起来
// Table* RecordManager::isExistTable(string tableName)
//{
//	int n = tables.size();
//	for (int i = 0; i < n; i++) {
//		if (tables[i].tableName == tableName) {
//			return &(tables[i]);
//		}
//	}
//	return NULL;
//}

////TODO:createTable 与
/// dropTable在catalogManager中也存在，二者可能需要做一个统一或直接采用CatalogManager中的函数
// bool RecordManager::createTable(string tableName, string primaryKey,
// vector<Attribute> attributeVector)
//{
//	if (isExistTable(tableName) != NULL) {
//		cerr << "同名的表格已经存在" << endl;
//		return false;
//	}
//	Table table(tableName, primaryKey, attributeVector);
//	this->tables.push_back(table);
//	return true;
//}


// insertValue返回值为false表示出错，否则为正确执行

int RecordManager::insertValue(Table* table, Tuple tuple, int hasIndex, IndexManager& index_manager) {
  // Table* table = catalog_manager.get_table(tableName);  //建议把这个函数的返回值定义成返回指针
  // if (table == NULL) {// 这里无法判断是否有这个表
  // 	cerr << "没有这个表" << endl;
  // 	return NO_SUCH_TABLE;
  // }
  if (isMatchTheAttribute(table, &tuple) == false) {
    cerr << "Tuple attributes do not match the table" << endl;
    return NO_SUCH_ATTR;
  }

  if (isConflictTheUnique(table, &tuple, hasIndex, index_manager) == true) {
    cerr << "the primary key conflict" << endl;
    return UNIQUE_CONFLICT;//Table* table, Tuple* tuple, int hasIndex, IndexManager& index_manager
  }

  char* tmpChar = (char*)malloc((table->rowLength + 2) * sizeof(char));
  tupleToChar(tuple, tmpChar);
  bool writeResult = writeToIndex(table->tableName, table->rowNum, tmpChar,
                                  table->rowLength, DB_NAME, 0);
  free(tmpChar);
  if (writeResult == false) {
    cerr << "写入buffer失败" << endl;
    return false;
  }
  // else {
  //   int attrNum = catalog_manager.get_attribute_num(tableName);
  //   for(int i=0; i<attrNum; i++){
  //     if(!catalogManager.is_unique(tableName, i)) continue;//不是唯一的不可能存在索引
  //     if(indexManager.is_index_exist(DB_NAME,
  //           tableName, catalogManager.get_attribute_name(tableName, i),
  //           catalogManager.get_type_for_match_IndexManager(tableName, i))){//判断是否建立了索引，若建立则插入索引
  //       if(!indexManager.insert_index(DB_NAME, tableName, catalogManager.get_attribute_name(tableName, i),
  //             tuple.getData()[i].toString(), table->rowNum, catalogManager.get_type_for_match_IndexManager(tableName, i))){
  //         cerr << "insert index error" << endl;
  //         return false;
  //       }
  //     }
  //   }
  //   cout << "写入buffer成功" << endl;
  //   table->rowNum += 1;
  // }
  return 1;//表示成功插入一条数据
}

int RecordManager::insertValue(Table* table, vector<pair<NumType, string>> tupleString, int hasIndex, IndexManager& index_manager) {
  // Table* table = catalogManager.get_table(tableName);  // 建议把这个函数的返回值定义成返回指针
  // if(table == NULL){
  //   cerr << "没有这个表" << endl;
  // 	return false;
  // }
  Tuple tuple;
  tuple.setIndex(table->rowNum);  // 表示写在表的最后

  for (int i = 0; i < table->attributeVector.size(); i++) {
    if(table->attributeVector[i].type.get_type() != tupleString[i].first){
      cerr << "Record: 插入数据，元组类型不符合" << endl;
      return ERROR;
    }
    Element e;                                   // 对于每个 Element
    e.setType(table->attributeVector[i].type.get_type());  // 需要知道类型
    e.setLength(table->attributeVector[i].type.get_length());  // 需要知道存储的长度，这里的长度，int，float是4，char[n]是n，具体定义参考
                             // FieldType 类
    if (e.type == INT) {
      e.setData(stoi(tupleString[i].second.c_str()));
    } else if (e.type == FLOAT) {
      e.setData(stof(tupleString[i].second.c_str()));
    } else {
      e.setData(tupleString[i].second);
    }
    tuple.push_back_Data(e);
  }

  return insertValue(table, tuple, hasIndex, index_manager);  // 调用之前写的接口
}

bool RecordManager::isMatchTheAttribute(Table* table, Tuple* tuple) {
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

//修改，与indexManager相关
bool RecordManager::isConflictTheUnique(Table* table, Tuple* tuple, int hasIndex, IndexManager& index_manager) {
  int attrNum = table->attributeNum;
  for(int i=0; i<attrNum; i++){
    if(!table->attributeVector[i].isUnique) continue;//不是唯一的不管
    // if(indexManager.is_index_exist(DB_NAME,
    //       tableName, catalogManager.get_attribute_name(tableName, i),
    //       catalogManager.get_type_for_match_IndexManager(tableName, i))){//判断是否建立了索引，若建立
    if(hasIndex > 0){//判断是否建立了索引，若建立
      vector<int> block_id;
      block_id.clear();
      if(index_manager.find_element(DB_NAME, table->tableName, table->attributeVector[i].attributeName, 
            table->attributeVector[i].type, tuple->getData()[i].toString(), block_id)){
        //表示在unique的index上找到了
        return true;//表示发生了冲突
      }
    } else{//表示一个unique的属性上没有索引，所以需要遍历
      for(int j=0; j<table->rowNum; j++){
        Tuple tmpTuple = getTupleByRowNumber(table, j);
        if (isConflict(&tmpTuple, tuple, i) == true) {
          cerr << "unique key conflict" << endl;
          return true;  //认为发生了冲突
        }
      }
    }
  }
  return false;
}

bool RecordManager::isConflict(Tuple* tuple1, Tuple* tuple2, int index) {
  if (tuple1->getIsDeleted() ||
      tuple2->getIsDeleted()) {  //若其中一个元组被删除，则不可能发生冲突
    return false;
  }
  return tuple1->getData()[index] == tuple2->getData()[index];
}

Tuple RecordManager::getTupleByRowNumber(Table* table, int RowNumber) {
  Tuple tuple;
  char* tmpData = (char*)malloc((table->rowLength + 2) * sizeof(char));
  if (tmpData == NULL) {
    cerr << "can't get memory in isConflictTheUnique()" << endl;
    return tuple;
  }
  int tmpLength = table->rowLength;
  // 不知道db_name是什么东西
  readData(table->tableName, DB_NAME, RowNumber * table->rowLength, tmpData,
           tmpLength, 0);
  Tuple tmpTuple = charToTuple(table, tmpData);
  free(tmpData);
  return tuple;
}

//仅允许单个条件，或者多个条件的and类型的查询操作（or是不行的，需要预处理成and）
Result RecordManager::searchQuery(string tableName, vector<SelectCondition> selectConditions, vector<Tuple>& tuples) {
  Table* table = catalogManager.get_table(
      tableName);  //建议把这个函数的返回值定义成返回指针
  if (table == NULL) {// 这里无法判断是否有这个表
  	cerr << "没有这个表" << endl;
  	return NO_SUCH_TABLE;
  }

  int scn = selectConditions.size();
  vector<bool> vis(scn, false);
  int cntIndex = 0;
  vector<int> block_id;
  int cntNormal = 0;
  vector<Tuple> tuples;
  while(true){//select with index
    int tmp = -1;
    for(int i=0; i<scn; i++){
      if(vis[i] == true) continue;
      if(!(table->attributeVector[selectConditions[i].attributeIndex].isUnique)) continue;
      if(!indexManager.is_index_exist(DB_NAME,
          tableName, catalogManager.get_attribute_name(tableName, i),
          catalogManager.get_type_for_match_IndexManager(tableName, i))) continue;
      tmp = i;//找到一个建立了索引的属性
      break;
    }
    if(tmp == -1) break;
    vis[tmp] = true;
    if(cntIndex == 0){
      block_id = selectWithIndex(tableName, selectConditions[tmp]);
      sort(block_id.begin(), block_id.end());//做排序，为了下面更好的操作
    } else {
      vector<int> tmpBlockId = selectWithIndex(tableName, selectConditions[tmp]);
      sort(tmpBlockId.begin(), tmpBlockId.end());//做排序，为了下面更好的操作
      for(int i=0; i<block_id.size(); i++){
        if(find(tmpBlockId.begin(), tmpBlockId.end(), block_id[i]) == tmpBlockId.end()){//没找到，说明并不是两个集合里面都有的，所以删去
          block_id.erase(block_id.begin()+i);
          i--;
        }
      }
    }
    cntIndex++;
  }
  if(cntIndex == 0){//说明这个表没有可用索引
    return searchQueryWithoutIndex(tableName, selectConditions);
  } else {
    for (int i = 0; i < block_id.size(); i++) {
      Tuple tmp = getTupleByRowNumber(table, block_id[i]);
      if (tmp.getIsDeleted()) continue;
      tuples.push_back(tmp);
    }
  }
  while(true){
    int tmp = -1;
    for(int i=0; i<scn; i++){
      if(vis[i]) continue;
      tmp = i;
      break;
    }
    if(tmp == -1) break;//表示所有条件均考虑了
    vis[tmp] = true;
    selectWithTempTuples(tableName, selectConditions[tmp], tuples);
  }

  return SUCCESS;
}

vector<int> RecordManager::selectWithIndex(string& tableName, SelectCondition& condition){
  vector<int> block_id;
  block_id.clear();
  if(condition.opt == EQUAL){
    indexManager.find_element(DB_NAME, tableName, catalogManager.get_attribute_name(tableName, condition.attributeIndex),
        condition.value.toString(), block_id, catalogManager.get_type_for_match_IndexManager(tableName, condition.attributeIndex));
  } else if(condition.opt == NOT_EQUAL){
    int flag = -1;
    if(indexManager.find_element(DB_NAME, tableName, catalogManager.get_attribute_name(tableName, condition.attributeIndex),
        condition.value.toString(), block_id, catalogManager.get_type_for_match_IndexManager(tableName, condition.attributeIndex))){
      flag = block_id[0];
    }
    block_id.clear();
    for(int i=0; i<catalogManager.get_table(tableName)->rowNum; i++){
      block_id.push_back(i);
    }
    if(flag != -1){//如果找到了EQU的，那么就删去，因为是unique的，所以最多只需要删去1个元素
      block_id.erase(find(block_id.begin(), block_id.end(), flag));
    }
  } else if(condition.opt == GREATER){
    indexManager.greater_than(DB_NAME, tableName, catalogManager.get_attribute_name(tableName, condition.attributeIndex),
        condition.value.toString(), block_id, catalogManager.get_type_for_match_IndexManager(tableName, condition.attributeIndex), 0);
  } else if(condition.opt == GREATER_EQUAL){
    indexManager.greater_than(DB_NAME, tableName, catalogManager.get_attribute_name(tableName, condition.attributeIndex),
        condition.value.toString(), block_id, catalogManager.get_type_for_match_IndexManager(tableName, condition.attributeIndex), 1);
  } else if(condition.opt == LESS){
    indexManager.less_than(DB_NAME, tableName, catalogManager.get_attribute_name(tableName, condition.attributeIndex),
        condition.value.toString(), block_id, catalogManager.get_type_for_match_IndexManager(tableName, condition.attributeIndex), 0);
  } else if(condition.opt == LESS_EQUAL){
    indexManager.less_than(DB_NAME, tableName, catalogManager.get_attribute_name(tableName, condition.attributeIndex),
        condition.value.toString(), block_id, catalogManager.get_type_for_match_IndexManager(tableName, condition.attributeIndex), 1);
  }
  return block_id;
}
/*
vector<Tuple> RecordManager::selectWithBlockID(string& tableName, SelectCondition& condition, vector<int>& block_id){
  vector<Tuple> ans;
  Table* table = catalogManager.get_table(tableName);  //建议把这个函数的返回值定义成返回指针
  for (int i = 0; i < block_id.size(); i++) {
    Tuple tmp = getTupleByRowNumber(table, block_id[i]);
    if (tmp.getIsDeleted()) continue;
    bool valid = true;
    if (judgeCondition(tmp, condition) == false) {
      valid = false;
      break;
    }
    if (valid) {
      ans.push_back(tmp);
    }
  }
  return ans;
}


vector<Tuple> RecordManager::selectWithoutBlockID(string& tableName, SelectCondition& condition){
  vector<Tuple> ans;
  Table* table = catalogManager.get_table(tableName);  //建议把这个函数的返回值定义成返回指针
  for (int i = 0; i < table->rowNum; i++) {
    Tuple tmp = getTupleByRowNumber(table, i);
    if (tmp.getIsDeleted()) continue;
    bool valid = true;
    if (judgeCondition(tmp, condition) == false) {
      valid = false;
      break;
    }
    if (valid) {
      ans.push_back(tmp);
    }
  }
  return ans;
}
*/
void RecordManager::selectWithTempTuples(string& tableName, SelectCondition& condition, vector<Tuple>& tuples){
  for(int i=0; i<tuples.size(); i++){
    if(judgeCondition(tuples[i], condition) == false){
      tuples.erase(tuples.begin()+i);
    }
  }
}

vector<Tuple> RecordManager::searchQuery(string tableName) {
  vector<Tuple> ans;
  Table* table = catalogManager.get_table(
      tableName);  //建议把这个函数的返回值定义成返回指针
  // if (table == NULL) {// 这里无法判断是否有这个表
  //	cerr << "没有这个表" << endl;
  //	return ans;
  //}
  for (int i = 0; i < table->rowNum; i++) {
    Tuple tmp = getTupleByRowNumber(table, i);
    if (tmp.getIsDeleted()) continue;
    ans.push_back(tmp);
  }
  return ans;
}

vector<Tuple> RecordManager::searchQueryWithoutIndex(string tableName, vector<SelectCondition> selectConditions) {
  vector<Tuple> ans;
  Table* table = catalogManager.get_table(tableName);  //建议把这个函数的返回值定义成返回指针
  if (table == NULL) {// 这里无法判断是否有这个表
  	cerr << "没有这个表" << endl;
  	return ans;
  }
  for (int i = 0; i < table->rowNum; i++) {
    Tuple tmp = getTupleByRowNumber(table, i);
    if (tmp.getIsDeleted()) continue;
    bool valid = true;
    for (auto it = selectConditions.begin(); it != selectConditions.end();
         ++it) {
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

bool RecordManager::judgeCondition(Tuple tuple, SelectCondition condition) {
  if (tuple.getIsDeleted() == true) {
    return false;  // A<10       B<10
  } else if (condition.opt == LESS) {
    if (tuple.getData()[condition.attributeIndex] < condition.value)
      return true;
  } else if (condition.opt == LESS_EQUAL) {
    if (tuple.getData()[condition.attributeIndex] <= condition.value)
      return true;
  } else if (condition.opt == EQUAL) {
    if (tuple.getData()[condition.attributeIndex] == condition.value)
      return true;
  } else if (condition.opt == GREATER_EQUAL) {
    if (tuple.getData()[condition.attributeIndex] >= condition.value)
      return true;
  } else if (condition.opt == GREATER) {
    if (tuple.getData()[condition.attributeIndex] > condition.value)
      return true;
  } else if (condition.opt == NOT_EQUAL) {
    if (tuple.getData()[condition.attributeIndex] != condition.value)
      return true;
  }

  //删除所有的区间比较，需要其他模块，把类似于区间的操作解析为两个比较操作，
  //比如and 的话，变成两个符号操作。
  // else if (condition.opt == INTERVAL) {
  //	if (condition.value.first <= tuple.data[condition.attributeIndex] &&
  // tuple.data[condition.attributeIndex] < condition.value.second) 		return
  // true;
  //}
  return false;
}

int RecordManager::deleteTuple(string tableName, vector<SelectCondition> selectConditions = vector<SelectCondition>()) {
  Table* table = catalogManager.get_table(
      tableName);  //建议把这个函数的返回值定义成返回指针
  if (table == NULL) {// 这里无法判断是否有这个表
  	cerr << "没有这个表" << endl;
  	return -1;
  }

  vector<Tuple> searchResult = searchQuery(tableName, selectConditions);
  if (searchResult.size() == 0) {
    cerr << "no tuple need to be deleted, please check the conditions" << endl;
    return 0;
  }

  vector<int> attrWithIndex_attrIndex;
  int attrNum = catalogManager.get_attribute_num(tableName);
  for(int i=0; i<attrNum; i++){
    if(!catalogManager.is_unique(tableName, i)) continue;//不是唯一的不可能存在索引
    if(indexManager.is_index_exist(DB_NAME,
          tableName, catalogManager.get_attribute_name(tableName, i),
          catalogManager.get_type_for_match_IndexManager(tableName, i))){//判断是否建立了索引，若建立
      attrWithIndex_attrIndex.push_back(i);
    }
  }

  int deleteNum = searchResult.size();
  for (int i = 0; i < deleteNum; i++) {
    Tuple tuple = searchResult[i];
    char* tmpChar = (char*)malloc(sizeof(char));
    *tmpChar = '0';
    bool writeResult = writeToIndex(tableName, tuple.getIndex(), tmpChar, 1,
                                    DB_NAME, 0);  //修改最前面的删除标记
    free(tmpChar);
    if (writeResult == false) {
      cerr << "write to buffer error" << endl;
      return -1;
    }
    // table.rowNum -= 1;//但是不修改数据行数标记
    for(int j=0; j<attrWithIndex_attrIndex.size(); j++){//删除这条数据的所有索引
      indexManager.delete_index(DB_NAME, tableName,
          catalogManager.get_attribute_name(tableName, attrWithIndex_attrIndex[j]),
          tuple.getData()[attrWithIndex_attrIndex[j]].toString(),
          catalogManager.get_type_for_match_IndexManager(tableName, attrWithIndex_attrIndex[j]));
    }
  }
  return deleteNum;
}

/*
int RecordManager::deleteTuple(string tableName) {
  Table* table = catalogManager.get_table(
      tableName);  //建议把这个函数的返回值定义成返回指针
  if (table == NULL) {// 这里无法判断是否有这个表
  	cerr << "没有这个表" << endl;
  	return -1;
  }
  vector<Tuple> searchResult = searchQuery(tableName);
  if (searchResult.size() == 0) {
    cerr << "no tuple need to be deleted, please check the conditions" << endl;
    return 0;
  }

  vector<int> attrWithIndex_attrIndex;
  int attrNum = catalogManager.get_attribute_num(tableName);
  for(int i=0; i<attrNum; i++){
    if(!catalogManager.is_unique(tableName, i)) continue;//不是唯一的不可能存在索引
    if(indexManager.is_index_exist(DB_NAME,
          tableName, catalogManager.get_attribute_name(tableName, i),
          catalogManager.get_type_for_match_IndexManager(tableName, i))){//判断是否建立了索引，若建立
      attrWithIndex_attrIndex.push_back(i);
    }
  }

  int deleteNum = searchResult.size();
  for (int i = 0; i < deleteNum; i++) {
    Tuple tuple = searchResult[i];
    char* tmpChar = (char*)malloc(sizeof(char));
    *tmpChar = '0';
    bool writeResult = writeToIndex(tableName, tuple.getIndex(), tmpChar, 1,
                                    DB_NAME, 0);  //修改最前面的删除标记
    free(tmpChar);
    if (writeResult == false) {
      cerr << "write to buffer error" << endl;
      return -1;
    }
    // table.rowNum -= 1;//但是不修改数据行数标记
    for(int j=0; j<attrWithIndex_attrIndex.size(); j++){//删除这条数据的所有索引
      indexManager.delete_index(DB_NAME, tableName,
          catalogManager.get_attribute_name(tableName, attrWithIndex_attrIndex[j]),
          tuple.getData()[attrWithIndex_attrIndex[j]].toString(),
          catalogManager.get_type_for_match_IndexManager(tableName, attrWithIndex_attrIndex[j]));
    }
  }
  return deleteNum;
}
*/
