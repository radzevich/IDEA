#include "KeyGen.h"
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include "Error.h"

namespace Keys
{
	//������� ��������� ���� � 16-������� ������ ��� ������ � �������� �������.
	//���������� 0, ���� ������ ��������� �������, ����� - ��� ������
	int readKey()
	{
		FILE *f = fopen( KEY_FILE_NAME, "rb" );
		//�������� �� ������� �����
		if (f != NULL)
		{
			//�������� ������� �����-����� (16 ����)
			if (keyFileSizeIsCorrect(f, KEY_LENGTH))
				increaseKey(f, &firstKeyPart, &secondKeyPart);
			else
			{
				//��������� ������, � ������ �������������� ������� �����
				ERROR_THROW(104);
				return 104;
			}
		}
		else
		{
			//��������� ������ � ������, ����� ���� �� ������
			ERROR_THROW(110);
			return 110;
		}
		fclose(f);
		return 0;
	}

	//��������� ����������� ������ ���������� ���� �� ����� � ����������� � �������� ������
	void increaseKey(FILE *f, TKey *firstKeyPart, TKey *secondKeyPart)
	{
		char num;
		int i = 0;
		TKey *target;
		//������������ ������ �� �����, ���� �� ��������� ����� ��� ������� fscan_f ������ ������
		while ((!feof(f)) & (1 == fscanf_s(f, "%c", &num)))
		{
			if (i < 8)
				target = firstKeyPart;
			else
				target = secondKeyPart;
			//�������� ����� ����� �� ���������� ���, ������ ������� char
			*target <<= 8;
			//���������� ����������� ������ � �����
			*target += num;
			i++;
		}
	}

	//�������, ����������� ������ �����. ������������ ��������:
	//true - ������ ������������ �������
	//false - ������ ���������� �� ����������
	bool keyFileSizeIsCorrect(FILE *f, unsigned int fileLength)
	{
		fseek(f, 0L, SEEK_END);
		bool result = (ftell(f) == fileLength >> 3);
		fseek(f, 0, SEEK_SET);
		return result;
	}

	//��������� ���������� ������ 128-������� ����� (�� ��������� �� 25 ���)
	void rotateKey(TKey *firstKeyPart, TKey *secondKeyPart, int shift = 25)
	{
		//*��������� ������ ����� - 16 ���� (128) ���, � ������������ ������ ������������� ���������� � ������ ������� 8 ���� (64 ���),
		//*�� ���� ���� ������� �� ��� 8-������� �����: firstKeyPart, secondKeyPart - ��� �������� � ����������� �������������� 
		TKey temp1, temp2;
		//���������� ������ shift ��� ������ �� ���� ������ �����
		temp1 = *firstKeyPart & 0xFFFFFF8000000000;
		temp2 = *secondKeyPart & 0xFFFFFF8000000000;
		//����� �� shift ��� ����� �����
		*firstKeyPart <<= shift;
		*secondKeyPart <<= shift;
		int rest = KEY_LENGTH / 2 - shift;
		//����� ���������� shift ��� ����� � �����
		temp2 >>= rest;
		temp1 >>= rest;
		//���������� ������� �������� ���������� �� shift ��� ����� ����������� ����� shift �������� ������
		*firstKeyPart += temp2;
		*secondKeyPart += temp1;
	}

	//������� ���������� 2-������� �������, ���������� �� ��������� �����
	TSubkey getSubkey(TKey key, int index)
	{
		int shift = index * SUBKEY_SIZE;
		int restShift = KEY_LENGTH / 2 - shift - SUBKEY_SIZE;
		//������ ��� ��������� ���������� (��������� 2 ����� ��������� ���������, ��������� - ����)
		TKey subkeyTemplate = (SUBKEY_SHIFT_TEMPLATE >> shift);
		//� key �������� ��� ������ ����� ��������
		key &= subkeyTemplate;
		//����� 2-�� ���������� ���� � ������� ����� ������� � ���������� ���� � TSubkey
		return (TSubkey)(key >>= restShift);
	}

	//��������� ������������� ������� ���������
	void initializeKeySet(TKey firstKeyPart, TKey secondKeyPart, TSubkey *keySet, int setSize = 52)
	{
		TKey *target;
		for (int i = 1; i <= SUBKEY_SET_SIZE; i++)
		{
			//�������� � ����� �� ���� ������ ����� ��������
			if ((i - 1) % 8 < 4)
				target = &firstKeyPart;
			else
				target = &secondKeyPart;
			//�������������� �������
			keySet[i - 1] = getSubkey(*target, (i - 1) % 4);
			//���� ������ 8 ��������, �� ������ ����������� ����� ����� (�� ��������� �� 25 �������)
			if (i % 8 == 0)
				rotateKey(&firstKeyPart, &secondKeyPart);
		}
	}	

	//������� ������������� ������. ���������� 0, ���� ������������� ������ �������, ����� - ��� ������
	int initialize()
	{
		//������ ����� �� �����
		int retCode = readKey();
		//���� ������ ����� �� ����� ������ �������, �� ���������� ��������� ���������
		if (0 == retCode)
			initializeKeySet(firstKeyPart, secondKeyPart, keySet);
		else
			return 110;
		return 0;
	}
}

