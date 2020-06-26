#ifndef _FIELDTYPE_H_
#define _FIELDTYPE_H_

#include<iostream>
#include "base.h"
using namespace std;
	

class FieldType {
private:
	NumType type; //type of number
	int length; //length of char type

public:
	static const int CHARSIZE = 1;  //1 byte for a char
	static const int INTSIZE = 4;   //4 bytes for an integer
	static const int FLOATSIZE = 4; //4 bytes for a float number
	static const int DEFAULTSIZE = 0;
	FieldType() { 
		enum NumType t = DEFAULT;
		this->type = t;
		this->length = DEFAULTSIZE;
	}
	FieldType(NumType type) {
		this->type = type; //set type ( for integer and float number )
		this->length = 1;
	}
	FieldType(NumType type, int length) {
		this->type = type; //set type and length ( for char )
		this->length = length;
	}
	NumType get_type() {
		return this->type;
	}
	int get_length() {
		switch (this->type) {
		case CHAR:
			return this->length * CHARSIZE;
		case INT:
			return INTSIZE;
		case FLOAT:
			return FLOATSIZE;
		case DEFAULT: 
			return DEFAULTSIZE;
		}
	}
	void set_type(NumType type) {
		this->type = type;
	}
	void set_length(int length) {
		this->length = length;
	}
	FieldType operator=(const FieldType& x)
	{
		FieldType y(x.type, x.length);
		return y;
	}
};
#endif // !_FIELDTYPE_H_


