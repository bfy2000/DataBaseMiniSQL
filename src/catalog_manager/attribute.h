#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_

	#include<iostream>
	#include<string>
	#include"FieldType.h"

	using namespace std;
	class Attribute {
	public:
		string attributeName;
		bool isUnique;
		FieldType type;
		Attribute() { ; }
		Attribute(string attributeName, NumType type, int length, bool isUnique) {
			this->attributeName = attributeName;
			FieldType* temp = new FieldType(type, length);
			this->type = *temp;
			this->isUnique = isUnique;
		}
		Attribute(string attributeName, NumType type, bool isUnique) {
			this->attributeName = attributeName;
			FieldType* temp = new FieldType(type);
			this->type = *temp;
			this->isUnique = isUnique;
		}
	};

#endif // !_ATTRIBUTE_H_

