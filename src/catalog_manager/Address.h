#ifndef _ADDRESS_H_
#define _ADDRESS_H_

	#include<iostream>
	#include<string>
	using namespace std;

	class Address {
	private:
		string fileName; //file name
		int blockOffset; //block offset in file
		int byteOffset; //byte offset in block
	public:
		Address() { ; }
		Address(string fileName, int blockOffset, int byteOffset) {
			this->fileName = fileName;
			this->blockOffset = blockOffset;
			this->byteOffset = byteOffset;
		}
		string get_file_name() {
			return this->fileName;
		}
		int get_block_offset() {
			return this->blockOffset;
		}
		int get_byte_offset() {
			return this->byteOffset;
		}

		void set_file_name(string fileName) {
			this->fileName = fileName;
		}
		void set_block_offset(int blockOffset) {
			this->blockOffset = blockOffset;
		}
		void set_byte_offset(int byteOffset) {
			this->byteOffset = byteOffset;
		}
		int compare(const Address &a) {
			if (this->fileName == a.fileName) { //first compare file name
				if (this->blockOffset == a.blockOffset) { //then compare block offset
					return this->byteOffset - a.byteOffset; // finally compare byte offset
				}
				else {
					return this->blockOffset - a.blockOffset;
				}
			}
			else {
				return this->fileName.compare(a.fileName);
			}
		}
	};

#endif // _ADDRESS_H_


