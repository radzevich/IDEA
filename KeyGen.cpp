#include "KeyGen.h"
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include "Error.h"

namespace Keys
{
	//функция открывает файл с 16-байтным ключом для чтения в бинарном формате.
	//Возвращает 0, если работа завершена успешно, иначе - код ошибки
	int readKey()
	{
		FILE *f = fopen( KEY_FILE_NAME, "rb" );
		//проверка на наличие файла
		if (f != NULL)
		{
			//проверка размера файла-ключа (16 байт)
			if (keyFileSizeIsCorrect(f, KEY_LENGTH))
				increaseKey(f, &firstKeyPart, &secondKeyPart);
			else
			{
				//генерация ошибки, в случае несоответствия размера файла
				ERROR_THROW(104);
				return 104;
			}
		}
		else
		{
			//генерация ошибки в случае, когда файл не открыт
			ERROR_THROW(110);
			return 110;
		}
		fclose(f);
		return 0;
	}

	//Процедура посимвольно читает символьный ключ из файла и преобразует в числовой формат
	void increaseKey(FILE *f, TKey *firstKeyPart, TKey *secondKeyPart)
	{
		char num;
		int i = 0;
		TKey *target;
		//посимвольное чтение из файла, пока не достигнут конец или функция fscan_f выдала ошибку
		while ((!feof(f)) & (1 == fscanf_s(f, "%c", &num)))
		{
			if (i < 8)
				target = firstKeyPart;
			else
				target = secondKeyPart;
			//сдвигаем число влево на количество бит, равное размеру char
			*target <<= 8;
			//прибавляем прочитанный символ к числу
			*target += num;
			i++;
		}
	}

	//функций, проверяющая размер файла. Возвращаемые значения:
	//true - размер соответсвует эталону
	//false - размер отличается от эталонного
	bool keyFileSizeIsCorrect(FILE *f, unsigned int fileLength)
	{
		fseek(f, 0L, SEEK_END);
		bool result = (ftell(f) == fileLength >> 3);
		fseek(f, 0, SEEK_SET);
		return result;
	}

	//Процедура побитового сдвига 128-битного ключа (по умолчанию на 25 бит)
	void rotateKey(TKey *firstKeyPart, TKey *secondKeyPart, int shift = 25)
	{
		//*поскольку размер ключа - 16 байт (128) бит, а максимальный размер целочисленной переменной в данном проекте 8 байт (64 бит),
		//*то весь ключ разделён на две 8-байтные части: firstKeyPart, secondKeyPart - над которыми и выполняются преобразования 
		TKey temp1, temp2;
		//сохранение первых shift бит каждой из двух частей ключа
		temp1 = *firstKeyPart & 0xFFFFFF8000000000;
		temp2 = *secondKeyPart & 0xFFFFFF8000000000;
		//сдвиг на shift бит ключа влево
		*firstKeyPart <<= shift;
		*secondKeyPart <<= shift;
		int rest = KEY_LENGTH / 2 - shift;
		//сдвиг сохранённых shift бит ключа в конец
		temp2 >>= rest;
		temp1 >>= rest;
		//дополнение младших разрядов сдвинутого на shift бит ключа сохранёнными ранее shift старшими битами
		*firstKeyPart += temp2;
		*secondKeyPart += temp1;
	}

	//функция возвращает 2-байтный подключ, полученный из основного ключа
	TSubkey getSubkey(TKey key, int index)
	{
		int shift = index * SUBKEY_SIZE;
		int restShift = KEY_LENGTH / 2 - shift - SUBKEY_SIZE;
		//шаблон для побитовой конъюнкции (требуемые 2 байта заполнены единицами, остальные - нули)
		TKey subkeyTemplate = (SUBKEY_SHIFT_TEMPLATE >> shift);
		//в key получаем два нужных байта подключа
		key &= subkeyTemplate;
		//сдвиг 2-ух полученных байт в крайнюю праую позицию и приведение типа к TSubkey
		return (TSubkey)(key >>= restShift);
	}

	//Процедура инициализации таблицы подключей
	void initializeKeySet(TKey firstKeyPart, TKey secondKeyPart, TSubkey *keySet, int setSize = 52)
	{
		TKey *target;
		for (int i = 1; i <= SUBKEY_SET_SIZE; i++)
		{
			//выбираем с какой из двух частей ключа работать
			if ((i - 1) % 8 < 4)
				target = &firstKeyPart;
			else
				target = &secondKeyPart;
			//инициализируем подключ
			keySet[i - 1] = getSubkey(*target, (i - 1) % 4);
			//если прошло 8 итерации, то делаем циклический сдвиг ключа (по умолчанию на 25 позиций)
			if (i % 8 == 0)
				rotateKey(&firstKeyPart, &secondKeyPart);
		}
	}	

	//функция инициализации ключей. Возвращает 0, если инициализация прошла успешно, иначе - код ошибки
	int initialize()
	{
		//чтение ключа из файла
		int retCode = readKey();
		//если чтение ключа из файла прошло успешно, то происходит генерация подключей
		if (0 == retCode)
			initializeKeySet(firstKeyPart, secondKeyPart, keySet);
		else
			return 110;
		return 0;
	}
}

