#include "KeyGen.h"
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "Error.h"

namespace Keys
{
	void readKey()
	{
		FILE *f = fopen( KEY_FILE_NAME, "r+b" );
		if (f != NULL)
		{
			if (keyFileSizeIs128Bits(f))
				fscanf_s(f, "%lu%lu", &firstKeyPart, &secondKeyPart);
			else
				ERROR_THROW(104);
		}
		else
			ERROR_THROW(110);
		fclose(f);
	}

	bool keyFileSizeIs128Bits(FILE *f)
	{
		fseek(f, 0L, SEEK_END);
		return (ftell(f) == round(KEY_LENGTH / 8));
	}

	void rotateKey(int firstKeyPart, int secondKeyPart, int shift = 25)
	{
		TKey temp1, temp2;
		temp1 = firstKeyPart & 0xFFFFFF8000000000;
		temp2 = secondKeyPart & 0xFFFFFF8000000000;
		firstKeyPart <<= shift;
		secondKeyPart <<= shift;
		temp2 >>= sizeof(temp2) - shift;
		temp1 >>= sizeof(temp1) - shift;
		firstKeyPart += temp2;
		secondKeyPart += temp1;
	}

	TSubkey *getSubkey(TKey firstKeyPart, TKey secondKeyPart)
	{
		TSubkey argv[8];
		TKey subkey = 0xFFFF;
		for (int i = 0; i < 4; i++)
			argv[i] = (subkey << (i * sizeof(argv[i])) & firstKeyPart);
		for (int i = 4; i < 8; i++)
			argv[i] = (subkey << (i * sizeof(argv[i]) % 4) & secondKeyPart);
		return argv;
	}

	void initializeKeySet(TSubkey *keySet, int setSize)
	{
		int i = 0;
		int loopCount = (sizeof(TKey) / sizeof(TSubkey)) >> 1;
		TKey subkey = 0xFFFF;
		while (true)
		{
			for (int j = 0; j < loopCount; j++)
			{
				keySet[i] = (subkey << (i / sizeof(TSubkey)) & firstKeyPart);
				i++;
			}
			if (i < setSize)
				for (int j = 0; j < loopCount; j++)
				{
					keySet[i] = (subkey << (i / sizeof(TSubkey)) & secondKeyPart);
					i++;
				}
			else
				break;
		}
	}	
}

int main(){}

