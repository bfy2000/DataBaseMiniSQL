#pragma once
#ifndef __Minisql__BPlusTree__
#define __Minisql__BPlusTree__
#include "BufferManager.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <queue>
//#include <type_traits>
using namespace std;

typedef int BlockId;

template <typename Type>
class Node {
public:
	Node(int Degree, bool isLeaf, BlockId blockid);//for new node
	Node(int Degree, BlockId blockid, char* block, int SizeOfType);//for node load from disk
	~Node() {}
	int Search(Type x);
	void Splite(Type &x, std::shared_ptr<Node<Type>> &NewNode);
	void AddElement(Type x, std::shared_ptr<Node<Type>> NewSon);
	void AddElementLeaf(Type x, int offset);
	void DeleteElement(int index);

public:
	BlockId blockid;
	int Degree;				//degree of B+ tree, n=degree+1
	BlockId Parent;			//parent
	int Num;				//number of element
	vector<Type> Element;	//
	vector<BlockId> Sons;
	vector<int> Offset;		//blockid for record
	BlockId NextLeaf;			//to construct a leaf list
	bool isLeaf;			//is leaf node

};

template <typename Type>
class BPlusTree {
public:
	BPlusTree(BufferManager* bm, string db_name, string table_name, int Degree, int SizeOfType, BlockId Root=-1, BlockId FirstLeaf=-1);
	~BPlusTree();

	void DeleteNode(std::shared_ptr<Node<Type>> x);
	int Search(Type x);
	bool InsertElement(Type element, int offset);
	bool DeleteElement(Type element);


	void OutputTree(void);
	bool CheckParent(BlockId nodeid);
private:
	void RealSearch(std::shared_ptr<Node<Type>> node, Type x, std::shared_ptr<Node<Type>> &res_node, int &index);
	void MaintainForSplite(std::shared_ptr<Node<Type>> node);
	int Union(std::shared_ptr<Node<Type>> node, std::shared_ptr<Node<Type>> &DeletedNode);
	void MaintainForUnion(std::shared_ptr<Node<Type>> node);
	BlockId GetEmptyBlockId();
	void writeback(std::shared_ptr<Node<Type>> node);
	void writebackdeleted(std::shared_ptr<Node<Type>> node);
	char* getBlock(BlockId id);
public:
	BlockId Root;
	BlockId FirstLeaf;	//for traverse
	int Degree;
	int SizeOfType;
	BufferManager* bm;
	string db_name;
	string table_name;
	string file_name;
};

/*
template<>
void BPlusTree<string>::writeback(std::shared_ptr<Node<string>> node) {
	BlockInfo blockinfo = bm->get_block_info(db_name, table_name, 1, node->blockid);
	char* block = blockinfo->cBlock;
	int i = 0;
	block[i] = 2;
	if (node->isLeaf)block[i] = 1;
	//block[i] = (char)node->isLeaf;
	i += sizeof(char);
	*(int*)(block + i) = node->Num;
	i += sizeof(int);
	*(int*)(block + i) = node->Parent;
	i += sizeof(int);
	if (node->isLeaf) {
		*(int*)(block + i) = node->NextLeaf;
		i += sizeof(int);
		for (int j = 0; j < node->Num; j++) {
			char* tmp = new char[SizeOfType];
			memset(tmp, 0, SizeOfType);
			memcpy(tmp, node->Element[j].c_str(), node->Element[j].size());
			memcpy(block + i, tmp, SizeOfType);
			i += SizeOfType;
			*(int*)(block + i) = node->Offset[j];
			i += sizeof(int);
		}
	}
	else {
		*(int*)(block + i) = node->Sons[0];
		i += sizeof(int);
		for (int j = 0; j < node->Num; j++) {
			char* tmp = new char[SizeOfType];
			memset(tmp, 0, SizeOfType);
			memcpy(tmp, node->Element[j].c_str(), node->Element[j].size());
			memcpy(block + i, tmp, SizeOfType);
			i += SizeOfType;
			*(int*)(block + i) = node->Sons[j + 1];
			i += sizeof(int);
		}
	}
	//
	blockinfo->dirtyBit = true;
	blockinfo->charNum = i;
}*/

template<class Type>
void BPlusTree<Type>::writeback(std::shared_ptr<Node<Type>> node) {
	BlockInfo blockinfo = bm->get_block_info(db_name, table_name, 1, node->blockid);
	char* block = blockinfo->cBlock;
	int i = 0;
	block[i] = -1;
	if (node->isLeaf)block[i] = 1;
	//block[i] = (char)node->isLeaf;
	i += sizeof(char);
	*(int*)(block + i) = node->Num;
	i += sizeof(int);
	*(int*)(block + i) = node->Parent;
	i += sizeof(int);
	if (node->isLeaf) {
		*(int*)(block + i) = node->NextLeaf;
		i += sizeof(int);
		for (int j = 0; j < node->Num; j++) {
			if constexpr (std::is_same_v<Type, std::string>) {
				char* tmp = new char[SizeOfType];
				memset(tmp, 0, SizeOfType);
				memcpy(tmp, node->Element[j].c_str(), node->Element[j].size());
				memcpy(block + i, tmp, SizeOfType);
				i += SizeOfType;
				*(int*)(block + i) = node->Offset[j];
				i += sizeof(int);
			}
			else {
				*(Type*)(block + i) = node->Element[j];
				i += sizeof(Type);
				*(int*)(block + i) = node->Offset[j];
				i += sizeof(int);
			}
		}
	}
	else {
		*(int*)(block + i) = node->Sons[0];
		i += sizeof(int);
		for (int j = 0; j < node->Num; j++) {
			if constexpr (std::is_same_v<Type, std::string>) {
				char* tmp = new char[SizeOfType];
				memset(tmp, 0, SizeOfType);
				memcpy(tmp, node->Element[j].c_str(), node->Element[j].size());
				memcpy(block + i, tmp, SizeOfType);
				i += SizeOfType;
				*(int*)(block + i) = node->Sons[j + 1];
				i += sizeof(int);
			}
			else {
				*(Type*)(block + i) = node->Element[j];
				i += sizeof(Type);
				*(int*)(block + i) = node->Sons[j + 1];
				i += sizeof(int);
			}
		}
	}
	//
	blockinfo->dirtyBit = true;
	blockinfo->charNum = i;
}

template <typename Type>
Node<Type>::Node(int Degree, bool isLeaf, BlockId blockid) {
	this->blockid = blockid;
	this->Degree = Degree;
	this->isLeaf = isLeaf;
	Parent = -1;
	Num = 0;
	NextLeaf = -1;
	for (int i = 0; i < Degree + 1; i++) {//total degree+1, one more space for splite
		Element.push_back(Type());
		Offset.push_back(int());
		Sons.push_back(-1);
	}
	Sons.push_back(-1);
}
/*
template <>
Node<string>::Node(int Degree, BlockId blockid, char* block, int SizeOfType) {
	this->blockid = blockid;
	this->Degree = Degree;
	int i = 0;
	if (block[i] == 1) {//is leaf
		this->isLeaf = true;
	}
	else {
		this->isLeaf = false;
	}

	i += sizeof(char);
	Num = *(int*)(block + i);
	i += sizeof(int);
	Parent = *(int*)(block + i);
	i += sizeof(int);

	for (int j = 0; j < Degree + 1; j++) {//total degree+1, one more space for splite
		Element.push_back(string());
		Offset.push_back(int());
		Sons.push_back(-1);
	}
	Sons.push_back(-1);

	if (this->isLeaf) {//leaf
		NextLeaf = *(int*)(block + i);
		i += sizeof(int);
		for (int j = 0; j < Num; j++) {
			char* tmp = new char[SizeOfType + 1];
			memcpy(tmp, block + i, SizeOfType);
			tmp[SizeOfType] = 0;
			Element[j] = string(tmp);
			i += SizeOfType;
			delete[] tmp;
			Offset[j] = *(int*)(block + i);
			i += sizeof(int);
		}
	}
	else {//not leaf
		NextLeaf = -1;
		Sons[0] = *(int*)(block + i);
		i += sizeof(int);
		for (int j = 0; j < Num; j++) {
			char* tmp = new char[SizeOfType + 1];
			memcpy(tmp, block + i, SizeOfType);
			tmp[SizeOfType] = 0;
			Element[j] = string(tmp);
			i += SizeOfType;
			delete[] tmp;
			Sons[j + 1] = *(int*)(block + i);
			i += sizeof(int);
		}

	}
}*/


template <typename Type>
Node<Type>::Node(int Degree, BlockId blockid, char* block, int SizeOfType) {
	this->blockid = blockid;
	this->Degree = Degree;
	int i = 0;
	if (block[i] == 1) {//is leaf
		this->isLeaf = true;
	}
	else {
		this->isLeaf = false;
	}

	i += sizeof(char);
	Num = *(int*)(block + i);
	i += sizeof(int);
	Parent = *(int*)(block + i);
	i += sizeof(int);

	for (int j = 0; j < Degree + 1; j++) {//total degree+1, one more space for splite
		Element.push_back(Type());
		Offset.push_back(int());
		Sons.push_back(-1);
	}
	Sons.push_back(-1);

	if (this->isLeaf) {//leaf
		NextLeaf = *(int*)(block + i);
		i += sizeof(int);
		for (int j = 0; j < Num; j++) {
			if constexpr (std::is_same_v<Type, std::string>) {
				char* tmp = new char[SizeOfType + 1];
				memcpy(tmp, block + i, SizeOfType);
				tmp[SizeOfType] = 0;
				Element[j] = string(tmp);
				i += SizeOfType;
				delete[] tmp;
				Offset[j] = *(int*)(block + i);
				i += sizeof(int);
			}
			else//float or int
			{
				Element[j] = *(Type*)(block + i);
				i += sizeof(Type);
				Offset[j] = *(int*)(block + i);
				i += sizeof(int);
			}

		}
	}
	else {//not leaf
		NextLeaf = -1;
		Sons[0] = *(int*)(block + i);
		i += sizeof(int);
		for (int j = 0; j < Num; j++) {
			if constexpr (std::is_same_v<Type, std::string>) {
				char* tmp = new char[SizeOfType + 1];
				memcpy(tmp, block + i, SizeOfType);
				tmp[SizeOfType] = 0;
				Element[j] = string(tmp);
				i += SizeOfType;
				delete[] tmp;
				Sons[j + 1] = *(int*)(block + i);
				i += sizeof(int);
			}
			else {//float or int
				Element[j] = *(Type*)(block + i);
				i += sizeof(Type);
				Sons[j + 1] = *(int*)(block + i);
				i += sizeof(int);
			}
		}

	}
}
template <typename Type>
int Node<Type>::Search(Type x) {
	if (Num == 0) {
		return -1;
	}
	if (Element[0] > x) {
		return -1;
	}
	else if (Element[Num - 1] < x) {
		return -(Num + 1);
	}
	else {//in this range
		if (Num > 0) {//binary search
			int l = 0, r = Num - 1;
			while (l <= r) {
				int mid = (l + r) >> 1;
				if (Element[mid] == x) {
					return mid;
				}
				else if (Element[mid] < x) {
					l = mid + 1;
				}
				else {
					r = mid - 1;
				}
			}
			return -(l + 1);
		}
		else {//traverse
			for (int i = 0; i < Num; i++) {
				if (Element[i] == x) {
					return i;
				}
				if (Element[i] > x) {
					return -(i + 1);
				}
			}
			return -(Num + 1);
		}
	}
	cout << "error" << endl;
	return -1;
}

template <typename Type>
void Node<Type>::Splite(Type &x, std::shared_ptr<Node<Type>> &NewNode) {
	//BlockId NewId=
	//Node* NewNode = new Node(this->Degree, this->isLeaf);
	/*if (NewNode == -1) {
		//error
	}*/
	NewNode->isLeaf = this->isLeaf;
	if (this->isLeaf) {
		x = this->Element[Num / 2];//the minimum element of the NewNode
		for (int i = Num / 2; i < Num; i++) {//splite
			NewNode->Element[i - Num / 2] = this->Element[i];
			NewNode->Offset[i - Num / 2] = this->Offset[i];
		}
		NewNode->NextLeaf = this->NextLeaf;
		this->NextLeaf = NewNode->blockid;
		NewNode->Parent = this->Parent;
		NewNode->Num = Num - Num / 2;
		this->Num = Num / 2;
	}
	else {
		x = this->Element[(Num + 1) / 2 - 1];//the minimum element of the NewNode
		for (int i = (Num + 1) / 2; i < Num + 1; i++) {
			NewNode->Sons[i - (Num + 1) / 2] = this->Sons[i];
			//will do after this function
			//NewNode->Sons[i - (Num + 1) / 2]->Parent = NewNode;
			this->Sons[i] = -1;
		}
		for (int i = (Num + 1) / 2; i < Num; i++) {
			NewNode->Element[i - (Num + 1) / 2] = this->Element[i];
		}
		NewNode->Parent = this->Parent;
		NewNode->Num = Num - (Num + 1) / 2;
		this->Num = (Num + 1) / 2 - 1;//Element[(Num + 1)/2-1] is removed.
	}
	//return NewNode;
}

template <typename Type>
int BPlusTree<Type>::Union(std::shared_ptr<Node<Type>> node, std::shared_ptr<Node<Type>> &DeletedNode) {//return merge(1) or move one element(0)  
	if (node->Parent == -1) {//root
		return 0;//no need to delete and union
	}
	else {
		if (node->isLeaf) {//leaf
			BlockId parent = node->Parent;
			std::shared_ptr<Node<Type>> parent_node = std::make_shared<Node<Type>>(Degree, parent, getBlock(parent), SizeOfType);
			int index = parent_node->Search(node->Element[0]);//
			if (index < 0) {//after this if, index is the index of Sons
				index = -(index + 1);
			}
			else {
				index++;
			}
			std::shared_ptr<Node<Type>> brother_node;
			BlockId brother;
			if (index == 0) {//the first son, union with right son
				brother = parent_node->Sons[index + 1];
				brother_node = std::make_shared<Node<Type>>(Degree, brother, getBlock(brother), SizeOfType);
				if (node->Num + brother_node->Num <= node->Degree) {//only union
					BlockId DeletedNodeId = parent_node->Sons[1];
					DeletedNode = std::make_shared<Node<Type>>(Degree, DeletedNodeId, getBlock(DeletedNodeId), SizeOfType);
					parent_node->DeleteElement(0);
					for (int i = 0; i < brother_node->Num; i++) {
						node->Element[node->Num + i] = brother_node->Element[i];
						node->Offset[node->Num + i] = brother_node->Offset[i];
					}
					node->Num += brother_node->Num;
					node->NextLeaf = brother_node->NextLeaf;
					writeback(parent_node);
					writeback(node);
					writeback(brother_node);
					return 1;
				}
				else {//move from right son
					node->Element[node->Num] = brother_node->Element[0];
					node->Offset[node->Num] = brother_node->Offset[0];
					(node->Num)++;
					for (int i = 0; i < brother_node->Num - 1; i++) {
						brother_node->Element[i] = brother_node->Element[i + 1];
						brother_node->Offset[i] = brother_node->Offset[i + 1];
					}
					(brother_node->Num)--;
					parent_node->Element[0] = brother_node->Element[0];
					writeback(parent_node);
					writeback(node);
					writeback(brother_node);
					return 0;
				}
			}
			else {//union with left son
				brother = parent_node->Sons[index - 1];
				brother_node = std::make_shared<Node<Type>>(Degree, brother, getBlock(brother), SizeOfType);
				if (node->Num + brother_node->Num <= node->Degree) {//only union
					BlockId DeletedNodeId = parent_node->Sons[index];
					DeletedNode = std::make_shared<Node<Type>>(Degree, DeletedNodeId, getBlock(DeletedNodeId), SizeOfType);
					parent_node->DeleteElement(index - 1);//index of element is index-1
					for (int i = 0; i < node->Num; i++) {
						brother_node->Element[brother_node->Num + i] = node->Element[i];
						brother_node->Offset[brother_node->Num + i] = node->Offset[i];
					}
					brother_node->Num += node->Num;
					brother_node->NextLeaf = node->NextLeaf;
					writeback(parent_node);
					writeback(node);
					writeback(brother_node);
					return 1;
				}
				else {//move one element from left to right
					for (int i = node->Num - 1; i >= 0; i--) {
						node->Element[i + 1] = node->Element[i];
						node->Offset[i + 1] = node->Offset[i];
					}
					node->Element[0] = brother_node->Element[brother_node->Num - 1];
					node->Offset[0] = brother_node->Offset[brother_node->Num - 1];
					(node->Num)++;
					(brother_node->Num)--;
					parent_node->Element[index - 1] = node->Element[0];
					writeback(parent_node);
					writeback(node);
					writeback(brother_node);
					return 0;
				}
			}
		}
		else {//not leaf
			BlockId parent = node->Parent;
			std::shared_ptr<Node<Type>> parent_node = std::make_shared<Node<Type>>(Degree, parent, getBlock(parent), SizeOfType);
			BlockId son = node->Sons[0];
			std::shared_ptr<Node<Type>> son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
			int index = parent_node->Search(son_node->Element[0]);//


			if (index < 0) {
				index = -(index + 1);
			}
			else {
				index++;
			}
			BlockId brother;
			std::shared_ptr<Node<Type>> brother_node;
			if (index == 0) {//do with right son
				brother = parent_node->Sons[index + 1];
				brother_node = std::make_shared<Node<Type>>(Degree, brother, getBlock(brother), SizeOfType);
				if (node->Num + 1 + brother_node->Num + 1 <= node->Degree + 1) {//only union
					node->Element[node->Num] = parent_node->Element[0];
					node->Sons[node->Num + 1] = brother_node->Sons[0];
					BlockId son = node->Sons[node->Num + 1];
					std::shared_ptr<Node<Type>> son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
					son_node->Parent = node->blockid;
					writeback(son_node);
					BlockId DeletedNodeId = parent_node->Sons[1];
					DeletedNode = std::make_shared<Node<Type>>(Degree, DeletedNodeId, getBlock(DeletedNodeId), SizeOfType);
					parent_node->DeleteElement(0);
					for (int i = 0; i < brother_node->Num; i++) {
						node->Element[node->Num + 1 + i] = brother_node->Element[i];
					}
					for (int i = 1; i <= brother_node->Num; i++) {
						node->Sons[node->Num + 1 + i] = brother_node->Sons[i];
						son = node->Sons[node->Num + 1 + i];
						son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
						son_node->Parent = node->blockid;
						writeback(son_node);
					}
					node->Num += brother_node->Num + 1;
					writeback(parent_node);
					writeback(node);
					writeback(brother_node);
					return 1;
				}
				else {
					node->Sons[node->Num + 1] = brother_node->Sons[0];
					BlockId son = node->Sons[node->Num + 1];
					std::shared_ptr<Node<Type>> son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
					son_node->Parent = node->blockid;
					writeback(son_node);

					node->Element[node->Num] = parent_node->Element[0];
					parent_node->Element[0] = brother_node->Element[0];
					for (int i = 0; i < brother_node->Num - 1; i++) {
						brother_node->Element[i] = brother_node->Element[i + 1];
					}
					for (int i = 0; i < brother_node->Num; i++) {
						brother_node->Sons[i] = brother_node->Sons[i + 1];
					}
					(brother_node->Num)--;
					(node->Num)++;
					writeback(parent_node);
					writeback(node);
					writeback(brother_node);
					return 0;
				}
			}
			else {//do with the left son
				brother = parent_node->Sons[index - 1];
				brother_node = std::make_shared<Node<Type>>(Degree, brother, getBlock(brother), SizeOfType);
				if (node->Num + 1 + brother_node->Num + 1 <= node->Degree + 1) {//union only
					brother_node->Element[brother_node->Num] = parent_node->Element[index - 1];
					brother_node->Sons[brother_node->Num + 1] = node->Sons[0];
					BlockId son = brother_node->Sons[brother_node->Num + 1];
					std::shared_ptr<Node<Type>> son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
					son_node->Parent = brother_node->blockid;
					writeback(son_node);

					BlockId DeletedNodeId = parent_node->Sons[index];
					DeletedNode = std::make_shared<Node<Type>>(Degree, DeletedNodeId, getBlock(DeletedNodeId), SizeOfType);
					parent_node->DeleteElement(index - 1);//index of Sons - 1 
					for (int i = 0; i < node->Num; i++) {
						brother_node->Element[brother_node->Num + 1 + i] = node->Element[i];
					}
					for (int i = 1; i <= node->Num; i++) {
						brother_node->Sons[brother_node->Num + 1 + i] = node->Sons[i];
						son = brother_node->Sons[brother_node->Num + 1 + i];
						son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
						son_node->Parent = brother_node->blockid;
						writeback(son_node);
					}
					brother_node->Num += node->Num + 1;
					writeback(parent_node);
					writeback(node);
					writeback(brother_node);
					return 1;
				}
				else {//move one element
					for (int i = node->Num; i > 0; i--) {
						node->Element[i] = node->Element[i - 1];
					}
					for (int i = node->Num + 1; i > 0; i--) {
						node->Sons[i] = node->Sons[i - 1];
					}
					node->Element[0] = parent_node->Element[index - 1];
					parent_node->Element[index - 1] = brother_node->Element[brother_node->Num - 1];
					node->Sons[0] = brother_node->Sons[brother_node->Num];
					BlockId son = node->Sons[0];
					std::shared_ptr<Node<Type>> son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
					son_node->Parent = node->blockid;
					writeback(son_node);
					(node->Num)++;
					(brother_node->Num)--;
					writeback(parent_node);
					writeback(node);
					writeback(brother_node);
					return 0;
				}
			}



		}
	}
}


template <typename Type>
void Node<Type>::AddElement(Type x, std::shared_ptr<Node<Type>> NewSon) {
	int index = Search(x);
	if (index < 0) {//not exist, add to Element[]
		index = -index - 1;
		for (int i = Num - 1; i >= index; i--) {
			this->Element[i + 1] = this->Element[i];
		}
		this->Element[index] = x;
		for (int i = Num; i > index; i--) {
			this->Sons[i + 1] = this->Sons[i];
		}
		this->Sons[index + 1] = NewSon->blockid;//add NewNode of Splite
		NewSon->Parent = this->blockid;
		Num++;
	}
	else {
		for (int i = 0; i < Num; i++) {
			cout << Element[i]<<" ";
		}
		cout << endl;
		cout << "Error: Element is already in this node" << endl;
	}
	return;
}

template <typename Type>
void Node<Type>::AddElementLeaf(Type x, int offset) {
	int index = Search(x);
	if (index < 0) {//not exist, add to Element[]
		index = -index - 1;
		for (int i = Num - 1; i >= index; i--) {
			this->Element[i + 1] = this->Element[i];
			this->Offset[i + 1] = this->Offset[i];
		}
		this->Element[index] = x;
		this->Offset[index] = offset;

		Num++;
	}
	else {
		cout << "Error: Element is already in this node" << endl;
	}
	return;
}
template <typename Type>
void Node<Type>::DeleteElement(int index) {
	if (isLeaf) {
		for (int i = index; i < Num - 1; i++) {
			this->Element[i] = this->Element[i + 1];
			this->Offset[i] = this->Offset[i + 1];
		}
		Num--;
	}
	else {
		for (int i = index; i < Num - 1; i++) {
			this->Element[i] = this->Element[i + 1];
		}
		for (int i = index + 1; i < Num; i++) {
			this->Sons[i] = this->Sons[i + 1];
		}
		this->Sons[Num] = -1;
		Num--;
	}
}

template<typename Type>
BPlusTree<Type>::BPlusTree(BufferManager* bm, string db_name, string table_name, int Degree, int SizeOfType, BlockId Root, BlockId FirstLeaf){
	this->db_name = db_name;
	//this->table_name = table_name;
	this->table_name = table_name;
	this->Degree = Degree;
	this->SizeOfType = SizeOfType;
	this->Root = Root;
	this->FirstLeaf = FirstLeaf;
	this->bm = bm;
	if (this->Root == -1) {
		this->Root = GetEmptyBlockId();
		std::shared_ptr<Node<Type>> Root_node = std::make_shared<Node<Type>>(Degree, true, this->Root);
		this->FirstLeaf = this->Root;
		writeback(Root_node);
	}
}

template<typename Type>
BPlusTree<Type>::~BPlusTree() {
	//DeleteNode(Root);
}

template<typename Type>
void BPlusTree<Type>::DeleteNode(std::shared_ptr<Node<Type>> x) {
	if (x->isLeaf) {
		delete x;
	}
	else {
		for (int i = 0; i <= x->Num; i++) {
			DeleteNode(x->Sons[i]);
		}
		delete x;
	}
}

template<typename Type>
void BPlusTree<Type>::RealSearch(std::shared_ptr<Node<Type>> node, Type x, std::shared_ptr<Node<Type>> &res_node, int &index) {
	if (node == nullptr) {
		res_node = nullptr;
		index = -1;
		return;
	}
	if (node->isLeaf) {
		int search_result = node->Search(x);
		if (search_result < 0) {
			index = search_result;
			res_node = node;
		}
		else {
			index = search_result;
			res_node = node;
		}
		return;
	}
	else {
		int search_result = node->Search(x);
		if (search_result < 0) {
			search_result = -(search_result + 1) - 1;
		}
		char* block = getBlock(node->Sons[search_result + 1]);
		std::shared_ptr<Node<Type>> tmp = std::make_shared<Node<Type>>(Degree, node->Sons[search_result + 1], block, SizeOfType);
		RealSearch(tmp, x, res_node, index);
	}
}

template<typename Type>
int BPlusTree<Type>::Search(Type x) {
	std::shared_ptr<Node<Type>> res_node;
	int index = -1;

	char* block = getBlock(Root);
	std::shared_ptr<Node<Type>> Root_node = std::make_shared<Node<Type>>(Degree, Root, block, SizeOfType);
	RealSearch(Root_node, x, res_node, index);
	if (index < 0)return -1;//not find
	else {
		return res_node->Offset[index];
	}
}


template<typename Type>
bool BPlusTree<Type>::InsertElement(Type element, int offset) {
	std::shared_ptr<Node<Type>> res_node;
	int index = -1;
	
	std::shared_ptr<Node<Type>> Root_node = std::make_shared<Node<Type>>(Degree, Root, getBlock(Root), SizeOfType);
	RealSearch(Root_node, element, res_node, index);
	if (index < 0) {
		index = -(index + 1);
		res_node->AddElementLeaf(element, offset);
		writeback(res_node);
		//res_node = std::make_shared<Node<Type>>(Degree, res_node->blockid, getBlock(res_node->blockid), SizeOfType);
		//cout << res_node->Num << " " << res_node->Element[0] << endl;

		if (res_node->Num == Degree + 1) {//need to splite(for leaf, the maximum number of element is Degree)
			MaintainForSplite(res_node);
		}
		return true;
	}
	else {
		return false;//already in the tree
	}
}

template<typename Type>
void BPlusTree<Type>::MaintainForSplite(std::shared_ptr<Node<Type>> node) {
	Type tmp;
	BlockId NewNodeId = GetEmptyBlockId();
	std::shared_ptr<Node<Type>> NewNode = std::make_shared<Node<Type>>(Degree, node->isLeaf, NewNodeId);
	node->Splite(tmp, NewNode);
	writeback(node);
	writeback(NewNode);

	if (!NewNode->isLeaf) {
		for (int i = 0; i <= NewNode->Num; i++) {
			BlockId son = NewNode->Sons[i];
			std::shared_ptr<Node<Type>> son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
			son_node->Parent = NewNode->blockid;
			writeback(son_node);
		}
	}
	if (node->Parent == -1) {//root
		BlockId NewRoot = GetEmptyBlockId();
		std::shared_ptr<Node<Type>> NewRoot_node = std::make_shared<Node<Type>>(Degree, false, NewRoot);
		if (NewRoot == -1) {
			cout << "Error: no more memory(MaintainForSplite)" << endl;
			return;
		}
		else {
			//change root
			this->Root = NewRoot;
			node->Parent = NewRoot;
			NewNode->Parent = NewRoot;
			NewRoot_node->Element[0] = tmp;
			NewRoot_node->Num = 1;
			NewRoot_node->Sons[0] = node->blockid;
			node->Parent = NewRoot;
			NewRoot_node->Sons[1] = NewNode->blockid;
			NewNode->Parent = NewRoot;
			writeback(NewRoot_node);
			writeback(node);
			writeback(NewNode);
			return;
		}
	}
	else {//not root
		BlockId parent = node->Parent;
		std::shared_ptr<Node<Type>> parent_node = std::make_shared<Node<Type>>(Degree, parent, getBlock(parent), SizeOfType);
		parent_node->AddElement(tmp, NewNode);
		NewNode->Parent = node->Parent;
		writeback(NewNode);
		writeback(node);
		writeback(parent_node);
		if (parent_node->Num == Degree + 1) {//max number of element is Degree
			MaintainForSplite(parent_node);
		}
	}
}

template<typename Type>
bool BPlusTree<Type>::DeleteElement(Type element) {
	std::shared_ptr<Node<Type>> res_node;
	int index = -1;

	std::shared_ptr<Node<Type>> Root_node = std::make_shared<Node<Type>>(Degree, Root, getBlock(Root), SizeOfType);
	RealSearch(Root_node, element, res_node, index);
	if (index < 0) {
		cout << "Error: No such element(DeleteElement)" << endl;
		return false;
	}
	else {
		if (res_node->Parent == -1) {//root is leaf
			res_node->DeleteElement(index);
			writeback(res_node);
			return true;
		}
		else {
			//cout << FirstLeaf << " " << res_node->blockid;
			if (index == 0 && res_node->blockid != FirstLeaf) {//need to modify the ancestor(nobody cares FirstLeaf's minimum element)
				//if(res_node==FirstLeaf)

				BlockId parent = res_node->Parent;
				std::shared_ptr<Node<Type>> parent_node = std::make_shared<Node<Type>>(Degree, parent, getBlock(parent), SizeOfType);
				int find_first_occur_parent_index = parent_node->Search(res_node->Element[0]);
				while (find_first_occur_parent_index < 0) {
					if (parent_node->Parent != -1) {//not root
						parent = parent_node->Parent;
						parent_node = std::make_shared<Node<Type>>(Degree, parent, getBlock(parent), SizeOfType);
						find_first_occur_parent_index = parent_node->Search(res_node->Element[0]);
					}
					else {
						//already in root, but can't find it, must in an error, need to rearrange the tree
						break;
					}
				}
				parent_node->Element[find_first_occur_parent_index] = res_node->Element[1];
				writeback(parent_node);
				res_node->DeleteElement(0);
				writeback(res_node);
				if (res_node->Num < (res_node->Degree + 1) / 2) {
					MaintainForUnion(res_node);
				}

			}
			else {
				res_node->DeleteElement(index);
				writeback(res_node);
				if (res_node->Num < (res_node->Degree + 1) / 2 && res_node->Parent != -1) {
					MaintainForUnion(res_node);
				}
			}
		}
		return true;
	}
}
template<typename Type>
void BPlusTree<Type>::MaintainForUnion(std::shared_ptr<Node<Type>> node) {
	BlockId parent = node->Parent;
	std::shared_ptr<Node<Type>> parent_node = std::make_shared<Node<Type>>(Degree, parent, getBlock(parent), SizeOfType);
	int index;
	if (node->isLeaf) {
		index = parent_node->Search(node->Element[0]);
	}
	else {
		//Num may be 0, so need to get son to find the index
		BlockId son = node->Sons[0];
		std::shared_ptr<Node<Type>> son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
		index = parent_node->Search(son_node->Element[0]);
	}


	if (index < 0) {
		index = -(index + 1);
		index--;
	}
	std::shared_ptr<Node<Type>> DeletedNode;
	if (Union(node, DeletedNode)) {//merge
		//OutputTree();
		writebackdeleted(DeletedNode);
		//delete DeletedNode;
		//regain since parent_node may be changed during union 
		parent_node = std::make_shared<Node<Type>>(Degree, parent, getBlock(parent), SizeOfType);
		if (parent_node->Num < (Degree + 1) / 2 && parent_node->Parent != -1) {
			
			MaintainForUnion(parent_node);
		}
		else if (parent_node->Num < 1 && parent_node->Parent == -1) {
			//delete parent;
			Root = parent_node->Sons[0];//must use parent->Sons[0], since node may be deleted before
			std::shared_ptr<Node<Type>> Root_node = std::make_shared<Node<Type>>(Degree, Root, getBlock(Root), SizeOfType);
			Root_node->Parent = -1;
			writeback(Root_node);

			writebackdeleted(parent_node);
			//delete parent;
		}
	}

}


template<typename Type>
void BPlusTree<Type>::OutputTree(void) {
	queue<BlockId> q;
	queue<int> q1;
	q.push(Root);
	q1.push(0);
	while (!q.empty()) {
		BlockId tmp = q.front();
		q.pop();
		int tmp1 = q1.front();
		q1.pop();
		cout << tmp1 << " : ";
		std::shared_ptr<Node<Type>> tmp_node = std::make_shared<Node<Type>>(Degree, tmp, getBlock(tmp), SizeOfType);
		if (tmp_node->isLeaf) {
			cout << "(leaf) ";
			for (int i = 0; i < tmp_node->Num; i++) {
				cout << tmp_node->Element[i] << ", ";
			}
			cout << endl;
			continue;
		}
		for (int i = 0; i < tmp_node->Num; i++) {
			cout << tmp_node->Element[i] << ", ";
		}
		cout << endl;
		for (int i = 0; i <= tmp_node->Num; i++) {
			q.push(tmp_node->Sons[i]);
			q1.push(tmp1 + 1);
		}
	}
}


template<typename Type>
bool BPlusTree<Type>::CheckParent(BlockId nodeid) {
	std::shared_ptr<Node<Type>> node = std::make_shared<Node<Type>>(Degree, nodeid, getBlock(nodeid), SizeOfType);
	if (node->isLeaf) {
		return true;
	}
	for (int i = 0; i <= node->Num; i++) {
		BlockId son = node->Sons[i];
		std::shared_ptr<Node<Type>> son_node = std::make_shared<Node<Type>>(Degree, son, getBlock(son), SizeOfType);
		if (son_node->Parent != nodeid)return false;
		if (!CheckParent(node->Sons[i]))return false;
	}
	return true;
}

template<typename Type>
BlockId BPlusTree<Type>::GetEmptyBlockId() {
	BlockId i = 0;
	char *block;
	do {
		i++;
		BlockInfo tmp = bm->get_block_info(db_name,table_name, 1, i);
		/*if (tmp == nullptr) {
			BlockInfo newbi = new blockInfo();
			newbi->blockNum = i;
			newbi->dirtyBit = true;
			newbi->next = nullptr;
			newbi->file = bm->get_file_info(db_name, file_name, 1);
			newbi->charNum = 1;
			newbi->cBlock = (char*)malloc(sizeof(char) * 4096);
			newbi->call_times = 1;
			newbi->lock = 0;
			newbi->isfree = false;
			bm->add_block_to_file(newbi, newbi->file);
			break;
		}*/
		block = tmp->cBlock;
	} while (block[0] != 0);
	return i;
}

template<typename Type>
char* BPlusTree<Type>::getBlock(BlockId id) {
	BlockInfo blockinfo = bm->get_block_info(db_name, table_name, 1, id);
	return blockinfo->cBlock;
}

template<typename Type>
void BPlusTree<Type>::writebackdeleted(std::shared_ptr<Node<Type>> node) {
	BlockInfo blockinfo = bm->get_block_info(db_name, table_name, 1, node->blockid);
	char* block = blockinfo->cBlock;
	blockinfo->charNum = 1;
	blockinfo->cBlock[0] = 0;
	blockinfo->dirtyBit = true;

}

#endif