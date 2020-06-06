#ifndef __ELEMENT_H__
#define __ELEMENT_H__
#include <string>

#include "../catalog_manager/FieldType.h"

//定义了一个元素，包含其类型以及数据的存放，通过元算符重载设计实现不同的比较符运算
class Element {
 public:
  NumType type;
  int m_int;
  float m_float;
  std::string m_char;
  int length;  // 注意 type=CHAR 时，这里的 length =
               // 最大能保存字串转长度+1，因为最后一个是'\0'
  // 否则 length = 4（针对 INT 和 FLOAT）

  bool operator<(Element &e);
  bool operator==(const Element &e);
  bool operator>(Element &e);
  bool operator>=(Element &e);
  bool operator<=(Element &e);
  bool operator!=(const Element &e);

  friend std::ostream &operator<<(std::ostream &out, Element &e);
  void setData(int i) { m_int = i; }
  void setData(float f) { m_float = f; }
  // Element(std::string s, int char_num);
  Element(std::string s, int l = 1) {
    type = CHAR;
    m_char = s;
    length = l;
  }
  Element(float f, int l = 4) {
    type = FLOAT;
    m_float = f;
    length = l;
  }
  Element(int i, int l = 4) {
    type = INT;
    m_int = i;
    length = l;
  }
  Element();
  ~Element();

  void setType(NumType t) { type = t; }
  void setLength(int l) { length = l; }

  void elementToChar(char *c) {
    if (type == INT) {  // int长度是4字节
      memcpy(c, &m_int, length);
    } else if (type == FLOAT) {  // float长度是4字节
      memcpy(c, &m_float, length);
    } else {  //最长能够存储length个'a'
      for (int i = 0; i < length; i++) {
        c[i] = m_char[i];
      }
    }
  }
  void charToElement(char *c) {
    if (type == INT) {  // int长度是4字节
      memcpy(&m_int, c, length);
    } else if (type == FLOAT) {  // float长度是4字节
      memcpy(&m_float, c, length);
    } else {                       //最长能够存储length个'a'
      m_char = string(c, length);  //但是存储的空间需要+1
    }
  }
  void printElement() {
    if (type == INT) {  // int长度是4字节
      cout << m_int;
    } else if (type == FLOAT) {  // float长度是4字节
      cout << m_float;
    } else {  //最长能够存储length个'a'
      cout << m_char;
    }
  }
};

#endif