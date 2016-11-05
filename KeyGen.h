#pragma once
#include <fstream>

#define KEY_LENGTH 128										//����� �����
#define KEY_FILE_NAME "key.txt"								//��� �����-�����
#define SUBKEY_SET_SIZE 52									//���������� ���������		
#define SUBKEY_SIZE 16										//������ ���������
#define SUBKEY_SHIFT_TEMPLATE 0xFFFF000000000000			//������ ��� ��������� 2-������� ����� �� 8-�������� �����

namespace Keys
{
	typedef unsigned _int64 TKey;							
	typedef unsigned short int TSubkey;
	//��������� ������ ����� - 16 ���� (128) ���, � ������������ ������ ������������� ���������� � ������ ������� 8 ���� (64 ���),
	//�� ���� ���� ������� �� ��� 8-������� �����, ��� �������� � ����������� ��������������
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