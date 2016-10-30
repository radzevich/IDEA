#pragma once
#include <fstream>

#define KEY_LENGTH 128
#define KEY_FILE_NAME "key.ini"
#define SUBKEY_SET_SIZE 52

namespace KEYS
{
	unsigned __int64 firstKeyPart, secondKeyPart;

	void readKey();
	void rotateKey(int firstKeyPart, int secondKeyPart, int shift);
	bool keyFileSizeIs128Bits(FILE *f);
}