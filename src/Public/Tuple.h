#ifndef __TUPLE_H__
#define __TUPLE_H__

/*
元组：用于表示表中的一行数据
相关模块：Record模块使用，可能还有其他模块


*/

#include <iostream>
#include <vector>

#include "Element.h"

//元组
class Tuple {
 private:
  std::vector<Element> data;
  bool isDeleted_;
  int index;  //当前这个元组，在表中的第几行，从0开始

 public:
  Tuple() : isDeleted_(false){};
  Tuple(int elementNum) {
    Tuple();
    data.resize(elementNum);
  }

  //拷贝函数
  Tuple(const Tuple& copytuple) {
    data = copytuple.data;
    //	attr = copytuple.attr;
  }

  Tuple(const std::vector<Element>& tuples) {
    for (auto it : tuples) {
      push_back_Data(it);
    }
  }
  bool operator==(const Tuple& t) {
    if (t.data.size() != data.size()) {
      return false;
    }
    for (auto i = 0; i < data.size(); i++) {
      if (data[i] != t.data[i]) return false;
    }
    return true;
  }

  //输入元组
  //初步设想是在insert操作时按顺序一个一个把data导进来
  void push_back_Data(const Element& d) { this->data.push_back(d); }

  //打印一个元祖的数据，按顺序输出，中间的间隔还需调整
  void Printdata(const std::vector<int>& width) {
    int i = 0;
    for (auto it = data.begin(); it != data.end(); it++, i++) {
      std::cout << "|";
      if ((*it).type == INT)
        std::cout << std::left << std::setw(width[i]) << (*it).m_int;
      else if ((*it).type == FLOAT)
        std::cout << std::left << std::setw(width[i]) << (*it).m_float;
      else
        std::cout << std::left << std::setw(width[i]) << (*it).m_char;
    }
    std::cout << "|\n";
  }
  int getWidth(int i) {
    assert(i >= 0 && i < data.size());
    if (data[i].type == INT) {
      auto item = std::to_string(data[i].m_int);
      return item.size();
    } else if (data[i].type == FLOAT) {
      auto item = std::to_string(data[i].m_float);
      return item.size();
    } else {
      assert(false);
    }
  }

  //返回数据
  std::vector<Element> getData() const { return this->data; }
  bool getIsDeleted() { return isDeleted_; }
  void setIsDeleted(bool s) { isDeleted_ = s; }
  int getIndex() { return index; }
  void setIndex(int i) { index = i; }
};

class TupleOfString{

};

#endif  // BASE