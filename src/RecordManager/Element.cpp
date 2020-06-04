#include "Element.h"

Element::Element()
{
}


Element::~Element()
{
}

bool Element::operator<(Element &e)
{
	if (e.type != type) {
		return false;
	} if (e.type == INT) {
		return m_int < e.m_int;
	}
	else if (e.type == FLOAT) {
		return m_float < e.m_float;
	}
	else {
		return m_char < e.m_char;
	}
	return false;
}

bool Element::operator==(const Element &e)
{
	if (e.type != type) {
		return false;
	} if (e.type == INT) {
		return m_int == e.m_int;
	}
	else if (e.type == FLOAT) {
		return m_float == e.m_float;
	}
	else {
		return m_char == e.m_char;
	}
	return false;
}
bool Element::operator>(Element &e)
{
	if (e.type != type) {
		return false;
	} if (e.type == INT) {
		return m_int > e.m_int;
	}
	else if (e.type == FLOAT) {
		return m_float > e.m_float;
	}
	else {
		return m_char > e.m_char;
	}
	return false;
}

bool Element::operator!=(const Element &e)
{
	return !operator==(e);
}

bool Element::operator>=(Element &e)
{
	if (e.type != type) {
		return false;
	} if (e.type == INT) {
		return m_int >= e.m_int;
	}
	else if (e.type == FLOAT) {
		return m_float >= e.m_float;
	}
	else {
		return m_char >= e.m_char;
	}
}

bool Element::operator<=(Element &e)
{
	if (e.type != type) {
		return false;
	} if (e.type == INT) {
		return m_int <= e.m_int;
	}
	else if (e.type == FLOAT) {
		return m_float <= e.m_float;
	}
	else {
		return m_char <= e.m_char;
	}
}

std::ostream& operator<<(std::ostream& out, Element &e)
{
	switch (e.type)
	{
	case CHAR:
		out << e.m_char;
		break;        
	case FLOAT:
		out << e.m_float;
		break;
	case INT:
		out << e.m_int;
		break;
	default:
		break;
	}
	return out;
}