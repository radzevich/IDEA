#pragma once
#include <fstream>

#define KEY_LENGTH 128										//длина ключа
#define KEY_FILE_NAME "key.txt"								//им€ файла-ключа
#define SUBKEY_SET_SIZE 52									//количество подключей		
#define SUBKEY_SIZE 16										//размер подключей
#define SUBKEY_SHIFT_TEMPLATE 0xFFFF000000000000			//шаблон дл€ получени€ 2-байтной части из 8-байтного числа

namespace Keys
{
	typedef unsigned _int64 TKey;							
	typedef unsigned short int TSubkey;
	//поскольку размер ключа - 16 байт (128) бит, а максимальный размер целочисленной переменной в данном проекте 8 байт (64 бит),
	//то весь ключ разделЄн на две 8-байтные части, над которыми и выполн€ютс€ преобразовани€
	TKey firstKeyPart = 0,
		 secondKeyPart = 0;
	TSubkey keySet[SUBKEY_SET_SIZE];

	bool keyFileSizeIsCorrect(FILE *f, unsigned int fileSize);
	int readKey();
	int initialize();
	void rotateKey(TKey *firstKeyPart, TKey *secondKeyPart, int shift);
	void initializeKeySet(TKey firstKeyPart, TKey secondKeyPart, TSubkey *keySet, int setSize);
	void increaseKey(FILE *f, TKey *firstKeyPart, TKey *secondKeyPart);
	TSubkey getSubkey(TKey key, int index);
}