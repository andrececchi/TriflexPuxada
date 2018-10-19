// DWIN_Comm.cpp - Library for communication between Arduinos and DWIN-DGUS LCDs.
// Created by Thiago Lugli Gon�ales, January 27, 2015.

#include "DWIN_Comm1.h"
#include <Arduino.h>

#define TIMEOUT 500

DWIN_Comm::DWIN_Comm(long baud_rate)
{
	Serial3.begin(baud_rate);
	while(Serial3.available()){
		Serial3.read();
	}
}

void DWIN_Comm::enable_write(){
  digitalWrite(ENTX_pin, HIGH);
}

void DWIN_Comm::enable_read(){
  digitalWrite(ENTX_pin, LOW);
}

int DWIN_Comm::page()
{
	byte frame[] = { 0x5A, 0xA5, 0x03, 0x81, 0x03, 0x02 };
	byte frame_read[10];
	int i = 0;
	unsigned long oldMillis = millis();
	int values[1];
	while (Serial3.available())
	{
		Serial3.read();
	}
	while (!Serial3.availableForWrite())
	{
		if ((millis() - oldMillis) > TIMEOUT)
		{
      enable_write();
			Serial.println(F("Comm Error - Read Timeout"));
			Serial.flush();
      enable_read();
			return 0;
		}
	}
  enable_write();
	Serial3.write(frame, 6);
	Serial3.flush();
  enable_read();
	while (i < 8)
	{
		while (!Serial3.available())
		{
			if ((millis() - oldMillis) > TIMEOUT)
			{
        enable_write();
				Serial.println(F("Comm Error - Receive Timeout"));
				Serial.flush();
        enable_read();
				return 0;
			}
		}
		frame_read[i++] = Serial3.read();
	}
	return frame_read[7] | frame_read[6] << 8;
}

void DWIN_Comm::setPage(int page_id)
{
	byte frame[] = { 0x5A, 0xA5, 0x04, 0x80, 0x03, page_id >> 8, page_id & 0xFF };
	unsigned long oldMillis = millis();
	while (Serial3.available())
	{
		Serial3.read();
	}
	while (!Serial3.availableForWrite())
	{
		if ((millis() - oldMillis) > TIMEOUT)
		{
      enable_write();
			Serial.println(F("Comm Error - Send Timeout"));
			Serial.flush();
      enable_read();
			return;
		}
	}
  enable_write();
	Serial3.write(frame, 7);
	Serial3.flush();
  enable_read();
	return;
}

int DWIN_Comm::read(int VP)
{
	int values[1];
	byte frame[7];
	while (Serial3.available())
	{
		Serial3.read();
	}
	Frame_Read(frame, VP, 1);
	if (Receive_Values(1, values))
	{
		return values[0];
	}
	else
	{
		return 0;
	}
}

void DWIN_Comm::read_multiple(int VP, int nvalues, int *values)
{
	int nval_rest = nvalues % 50;
	int i = 0;
	for (i = 0; nvalues / 50 != 0; i++)
	{
		byte frame[7];
		while (Serial3.available())
		{
			Serial3.read();
		}
		if (Frame_Read(frame, VP + (50 * i), 50))
		{
			Receive_Values(50, &values[i * 50]);
			nvalues = nvalues - 50;
		}
		else
		{
      enable_write();
			Serial.println(F("Read multiple - Error"));
			Serial.flush();
      enable_read();
			return;
		}
	}
	if (nval_rest != 0)
	{
		byte frame[7];
		while (Serial3.available())
		{
			Serial3.read();
		}
		if (Frame_Read(frame, VP + (50 * i), nval_rest))
		{
			Receive_Values(nval_rest, &values[i * 50]);
		}
		else
		{
      enable_write();
			Serial.println(F("Read multiple - Error"));
			Serial.flush();
      enable_read();
			return;
		}

	}
  enable_write();
	Serial3.flush();
  enable_read();
	return;
}

long DWIN_Comm::read_long(int VP)
{
	unsigned int ints[2];
	long val = 0;
	ints[0] = read(VP);
	ints[1] = read(VP+1);

	val = (long) ints[1] + ((long) ints[0] << 16);

	return val;
}

void DWIN_Comm::read_string(int VP, int text_size, char *text)
{
	if(text_size % 2 != 0){
		text_size--;
	}
	byte frame[7];
	while (Serial3.available())
	{
		Serial3.read();
	}
	if (Frame_Read(frame, VP, text_size / 2))
	{
		Receive_String(text_size, text);
	}
	else
	{
    enable_write();
		Serial.println(F("Read string - Error"));
		Serial.flush();
    enable_read();
		return;
	}
  enable_write();
  Serial3.flush();
  enable_read();
	return;
}

void DWIN_Comm::write(int VP, int value)
{
	int values[1];
	byte frame[8];
	values[0] = value;
	while (Serial3.available())
	{
		Serial3.read();
	}
	if (Frame_Write(frame, VP, 1, values))
	{
		return;
	}
	else
	{
    enable_write();
		Serial.println(F("Write single - Error"));
		Serial.flush();
    enable_read();
		return;
	}
	return;
}

void DWIN_Comm::write_multiple(int VP, int nvalues, int *values)
{
	int nval_rest = nvalues % 50;
	int i = 0;
	byte *frame;
	for (i = 0; nvalues / 50 != 0; i++)
	{
		frame = new byte[6 + 2 * 50];
		while (Serial3.available())
		{
			Serial3.read();
		}
		if (Frame_Write(frame, VP + (i * 50), 50, &values[i * 50]))
		{
			nvalues = nvalues - 50;
			delete[] frame;
		}
		else
		{
      enable_write();
			Serial.println(F("Write multiple - Error"));
			Serial.flush();
      enable_read();
			delete[] frame;
			return;
		}
	}
	if (nval_rest != 0)
	{
		frame = new byte[6 + 2 * nval_rest];
		while (Serial3.available())
		{
			Serial3.read();
		}
		if (Frame_Write(frame, VP + (i * 50), nval_rest, &values[i * 50]))
		{
			delete[] frame;
		}
		else
		{
      enable_write();
			Serial.println(F("Write multiple - Error"));
			Serial.flush();
      enable_read();
			delete[] frame;
			return;
		}

	}
	return;
}

void DWIN_Comm::write_long(int VP, long value)
{

	int longs[2];

	longs[0] = (int) (value >> 16);
	longs[1] = (int) (value);

	write(VP, longs[0]);
	write(VP+1, longs[1]);
}


void DWIN_Comm::write_string(int VP, int area_size, int text_size, char *text)
{
	byte *frame = new byte[6 + sizeof(text)];
	while (Serial3.available())
	{
		Serial3.read();
	}
	if (Frame_String(frame, VP, area_size, text_size, text))
	{
		delete[] frame;
	}
	else
	{
    enable_write();
		Serial.println(F("Write string - Error"));
		Serial.flush();
    enable_read();
		delete[] frame;
		return;
	}
	return;
}

// Frame_Read:
//	Cria o frame com a mensagem a ser enviada para a leitura de uma ou mais vari�veis.
//		frame: ponteiro para frame ser armazenado;
//		VP: primeiro endere�o da DWIN a ser lido;
//		LEN: n�mero de VPs a serem lidos;
int DWIN_Comm::Frame_Read(byte *frame, int VP, int LEN)
{
	unsigned long oldMillis = millis();
	if (LEN > 50)
	{
		LEN = 50;
	}
	frame[0] = 0x5A;
	frame[1] = 0xA5;
	frame[2] = 0x04;
	frame[3] = 0x83;
	frame[4] = VP >> 8;
	frame[5] = VP & 0xFF;
	frame[6] = LEN & 0xFF;
	while (!Serial3.availableForWrite())
	{
		if ((millis() - oldMillis) > TIMEOUT)
		{
      enable_write();
			Serial.println(F("Comm Error - Send Timeout"));
			Serial.flush();
      enable_read();
			return 0;
		}
	}
  enable_write();
	Serial3.write(frame, 7);
	Serial3.flush();
  enable_read();
	return 1;
}

// Frame_Write:
//	Cria o frame com a mensagem a ser enviada para a escrita de uma ou mais vari�veis.
//		frame: ponteiro para frame ser armazenado;
//		VP: primeiro endere�o da DWIN a ser lido;
//		LEN: n�mero de VPs a serem escritos;
//		values: ponteiro para vetor que armazena os valores a serem escritos;
int DWIN_Comm::Frame_Write(byte *frame, int VP, int LEN, int *values)
{
	unsigned long oldMillis = millis();
	int i;
	int frame_size = 3 + 2 * LEN;
	frame[0] = 0x5A;
	frame[1] = 0xA5;
	frame[2] = frame_size & 0xFF;
	frame[3] = 0x82;
	frame[4] = VP >> 8;
	frame[5] = VP & 0xFF;
	for (i = 0; i < LEN; i++)
	{
		frame[6 + 2 * i] = values[i] >> 8;
		frame[7 + 2 * i] = values[i] & 0xFF;
	}
	while (!Serial3.availableForWrite())
	{
		if ((millis() - oldMillis) > TIMEOUT)
		{
      enable_write();
			Serial.println(F("Comm Error - Send Timeout"));
			Serial.flush();
      enable_read();
			return 0;
		}
	}
  enable_write();
	Serial3.write(frame, frame_size + 3);
	Serial3.flush();
  enable_read();
	return 1;
}

// Frame_String:
//	Cria o frame com a mensagem a ser enviada para a escrita de uma string.
//		frame: ponteiro para frame ser armazenado;
//		VP: primeiro endere�o da DWIN a ser escrito;
//		LEN: n�mero de caracteres a serem escritos;
//		text: ponteiro para vetor que armazena os valores a serem escritos;
int DWIN_Comm::Frame_String(byte *frame, int VP, int area_size, int text_size, char *text)
{
	unsigned long oldMillis = millis();
	int i;
	int frame_size = 3 + area_size;
	frame[0] = 0x5A;
	frame[1] = 0xA5;
	frame[2] = frame_size & 0xFF;
	frame[3] = 0x82;
	frame[4] = VP >> 8;
	frame[5] = VP & 0xFF;
	for (i = 0; i < text_size; i++)
	{
		frame[6 + i] = text[i];
	}
	for (i = text_size; i < area_size; i++)
	{
		frame[6 + i] = '\0';
	}
	while (!Serial3.availableForWrite())
	{
		if ((millis() - oldMillis) > TIMEOUT)
		{
      enable_write();
			Serial.println(F("Comm Error - Send Timeout"));
			Serial.flush();
      enable_read();
			return 0;
		}
	}
  enable_write();
	Serial3.write(frame, frame_size + 3);
	Serial3.flush();
  enable_read();
	return 1;
}


// Receive_Values:
//	Recebe nvalues do frame lido e armazena em um vetor, que � apontado pelo par�metro values.
int DWIN_Comm::Receive_Values(int nvalues, int *values)
{
	unsigned long oldMillis = millis();
	int i = 0;
	byte frame[200];
	while (i < (7 + 2 * nvalues))
	{
		while (!Serial3.available())
		{
			if ((millis() - oldMillis) > TIMEOUT)
			{
        enable_write();
				Serial.println(F("Comm Error - Receive Timeout"));
				Serial.flush();
        enable_read();
				return 0;
			}
		}
		frame[i++] = Serial3.read();
	}
	for (i = 0; i < nvalues; i++)
	{
		values[i] = frame[8 + 2 * i] | frame[7 + 2 * i] << 8;
	}
	return 1;
}

// Receive_String:
//	Recebe uma string do frame lido e armazena em um vetor de caracteres, que � apontado pelo par�metro text.
int DWIN_Comm::Receive_String(int text_size, char *text)
{
	unsigned long oldMillis = millis();
	int i = 0;
	byte frame[200];
	while (i < (7 + text_size))
	{
		while (!Serial3.available())
		{
			if ((millis() - oldMillis) > TIMEOUT)
			{
        enable_write();
				Serial.println(F("Comm Error - Receive Timeout"));
				Serial.flush();
        enable_read();
				return 0;
			}
		}
		frame[i++] = Serial3.read();
	}
	i = 0;
	for (i = 0; i < text_size; i++)
	{
		if (frame[7 + i] == '�' || frame[7 + i] > 126)
		{
			for (i = i; i < text_size; i++)
			{
				text[i] = '\0';
			}
			break;
		}
		text[i] = frame[7 + i];
	}
	if (i == (text_size))
	{
		text[--i] = '\0';
	} else {
		text[i] = '\0';
	}
	return 1;
}
