#include "base.h"
#include "Element.h"

class SelectCondition {
 public:
  int attributeIndex;  //当前条件针对第几个字段，约定从0开始
  CMP opt;
  Element value;  //对于><=操作，只需要用到value
};

struct WhereExpr {
	string expr1;
	CMP cmp;
	string expr2;
	WhereExpr(string &in_expr1, CMP &in_cmp, string &in_expr2)
		:expr1{ in_expr1 }, cmp{ in_cmp }, expr2{ in_expr2 } {}
};