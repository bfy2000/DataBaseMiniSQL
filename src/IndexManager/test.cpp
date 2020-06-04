#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <chrono>
#include "test.h"
#include "IndexManager.h"
#include "BufferManager.h"
using namespace std;
int main()
{
	string filename = "test.txt";
	BufferManager* bm = new BufferManager();

	BPlusTree<string> *tree = new BPlusTree<string>(bm," "," ", 3, 2);
	string a[20] = { "aa","bb","cc","dd","ee","ff","gg","hh","ii","jj","kk","ll","mm","nn","oo","pp","qq","rr","ss","tt" };
	//int a[20] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 };
	//int a[20] = { 19,2,10,14,3,18,8,20,15,17,4,11,12,1,6,9,5,16,7,13 };
	//int b[20] = { 7,2,17,1,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	string* a_begin = a;
	string* a_end = a + sizeof(a) / sizeof(a[0]);
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(a_begin, a_end, std::default_random_engine(seed));
	//srand(time(0));
	//random_shuffle(a_begin, a_end);
	for (int i = 0; i < 20; i++) {
		cout << i <<" "<<a[i]<< endl;
		tree->InsertElement(a[i], 1);
		tree->OutputTree();
		//cout << tree->CheckParent(tree->Root) << endl;
	}
	seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(a_begin, a_end, std::default_random_engine(seed));
	for (int i = 0; i < 20; i++) {
		cout << i << " " << a[i] << endl;
		tree->DeleteElement(a[i]);
		tree->OutputTree();
		//cout << tree->CheckParent(tree->Root) << endl;
	}
	//BP *tree=new BP(filename, 4, sizeof(int));
	system("pause");
	return 0;
}