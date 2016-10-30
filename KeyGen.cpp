#include "KeyGen.h"
#include <io.h>
#include <stdio.h>
#include <string.h>
#include "Error.h"

namespace KEYS
{
	void readKey()
	{
		FILE *f = fopen( KEY_FILE_NAME, "r+b" );
		if (f != NULL)
		{
			char *firstKeyParts, *secondKeyParts;
			if (keyFileSizeIs128Bits(f))
				fscanf_s(f, "%s%s", firstKeyParts, secondKeyParts);
			else
				ERROR_THROW(104);
		}
		else
			ERROR_THROW(110);
		fclose(f);
	}

	TKey stringToInt64(char* str)
	{
		TKey key = 0;
		int digit = 0;
		for (int i = strlen(str); i >= 0; i--)
		{
			
		}
	}

	bool keyFileSizeIs128Bits(FILE *f)
	{
		fseek(f, 0L, SEEK_END);
		return (ftell(f) == round(KEY_LENGTH / 8));
	}

	void rotateKey(int firstKeyPart, int secondKeyPart, int shift = 25)
	{
		TKey temp1 = 0xFFFFFF8000000000;
		TKey temp2 = 0xFFFFFF8000000000;
		temp1 &= firstKeyPart;
		temp2 &= secondKeyPart;
		firstKeyPart <<= shift;
		secondKeyPart <<= shift;
		temp2 >>= sizeof(temp2) - shift;
		temp1 >>= sizeof(temp1) - shift;
		firstKeyPart += temp2;
		secondKeyPart += temp1;
	}
}