#pragma once
#include <fstream>

#define KEY_LENGTH 128
#define KEY_FILE_NAME "key.ini"
#define SUBKEY_SET_SIZE 52        

namespace Keys
{
	typedef unsigned long int TKey;
	typedef unsigned short int TSubkey;
	TKey firstKeyPart, secondKeyPart;
	TSubkey keySet[SUBKEY_SET_SIZE];

	void readKey();
	void rotateKey(int firstKeyPart, int secondKeyPart, int shift);
	void initializeKeySet(TKey *keySet, int setSize);
	bool keyFileSizeIs128Bits(FILE *f);
}