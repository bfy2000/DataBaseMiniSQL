#ifndef __RECORDMANAGER_H__
#define __RECORDMANAGER_H__
#include "../Public/Base.h"
#include "../Public/Attribute.h"
#include "../Public/Tuple.h"
#include "../Public/Element.h"
#include "../Public/SelectCondition.h"
#include "../Public/Table.h"
#include "../IndexManager/IndexManager.h"
#include <algorithm>

#include <vector>
#include <iostream>
using namespace std;

#define DB_NAME "db_name?"

class RecordManager {
 public:
  //插入数据，可以有两种方式，并完成索引的建立（如有）
  int insertValue(Table* table, Tuple tuple, int hasIndex, IndexManager& index_manager);
  int insertValue(Table* table, vector<pair<NumType, string>> tupleString, int hasIndex, IndexManager& index_manager);
  
  //删除数据行，若没有选择条件，就是删除所有数据和索引
  int deleteTuple(Table* table, IndexManager& index_manager, vector<SelectCondition> selectConditions = vector<SelectCondition>());
  // int deleteTuple(string tableName);

  //通用select with conditions
  Result searchQuery(Table* table, IndexManager& index_manager, vector<SelectCondition> selectConditions, vector<Tuple>& tuples);

  //用于select .. from table;
  Result searchQuery(Table* table, vector<Tuple>& tuples);

  //调用者可以保证查询过程没有索引
  Result searchQueryWithoutIndex(Table* table, vector<SelectCondition> selectConditions, vector<Tuple>& tuples);

  void PrintTable(Table* table) {
    for (int i = 0; i < table->attributeVector.size(); i++) {
      cout << table->attributeVector[i].attributeName << "\t";
    }
    cout << endl;

    char* tmpChar = (char*)malloc(table->rowLength * sizeof(char));
    if (tmpChar == NULL) {
      cerr << "memory error" << endl;
      return;
    }
    for (int i = 0; i < table->rowNum; i++) {
      readData(table->tableName, "db_name?", i, tmpChar, table->rowLength, 0);
      Tuple tuple = getTupleByRowNumber(table, i);
      for (int j = 0; j < tuple.getData().size(); i++) {
        tuple.getData()[j].printElement();
        cout << "\t";
      }
      cout << endl;
    }
    free(tmpChar);
  }

 private:
  bool judgeCondition(Tuple tuple, SelectCondition condition);
  bool isMatchTheAttribute(Table* table, Tuple* tuple);
  bool isConflictTheUnique(Table* table, Tuple* tuple, int hasIndex, IndexManager& index_manager);
  bool isConflict(Tuple* tuple1, Tuple* tuple2, int index);
  Tuple charToTuple(Table* table, char* c) {
    Tuple tuple;  // 0 1234   jsaoeprnvernvjnvjdfsvkl;df
    if (*c == '0') {
      tuple.setIsDeleted(true);
    } else {
      tuple.setIsDeleted(false);
    }
    c = c + sizeof(char);

    int* tmp = (int*)malloc(sizeof(int));
    memcpy(tmp, c, sizeof(int));
    tuple.setIndex(*tmp);
    free(tmp);
    c = c + sizeof(int);

    int n = table->attributeNum;
    for (int i = 0; i < n; i++) {
      Element tmpElement;
      tmpElement.setType(table->attributeVector[i].type.get_type());
      tmpElement.setLength(table->attributeVector[i].type.get_length());
      tmpElement.charToElement(c);
      tuple.push_back_Data(tmpElement);
      c = c + table->attributeVector[i].type.get_length();
    }
    return tuple;
  }
  void tupleToChar(Tuple tuple, char* c) {
    //功能：将tuple拷贝进c地址
    //使用此函数需要保证tuple是合理的，关键是string的长度合理
    //使用此函数需要保证c的合理性
    if (tuple.getIsDeleted() == true) {
      *c = '0';
    } else {
      *c = '1';
    }
    c = c + 1;

    int t = tuple.getIndex();
    memcpy(c, &(t), sizeof(int));
    c = c + 4;

    int n = tuple.getData().size();
    for (int i = 0; i < n; i++) {
      tuple.getData()[i].elementToChar(c);
      c = c + tuple.getData()[i].length;
    }
  }
  Tuple getTupleByRowNumber(Table* table, int RowNumber);

  vector<int> selectWithIndex(Table* table, SelectCondition& condition, IndexManager& index_manager);
  void selectWithTempTuples(SelectCondition& condition, vector<Tuple>& tuples);

  
  
};

#endif