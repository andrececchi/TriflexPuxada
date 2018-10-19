// DWIN_Comm.h - Library for communication between Arduino and DWIN-DGUS LCDs.
// Created by Thiago Lugli Gonçales, January 27, 2015.

#ifndef DWIN_Comm1_h
#define DWIN_Comm1_h

#include <Arduino.h>

class DWIN_Comm
{
public:
  #define ENTX_pin 18
	DWIN_Comm(long baud_rate);
	int page();
	void setPage(int page_id);
	int read(int VP);
	void read_multiple(int VP, int nvalues, int *values);
	long read_long(int VP);
	void read_string(int VP, int text_size, char *text);
	void write(int VP, int value);
	void write_multiple(int VP, int nvalues, int *values);
	void write_long(int VP, long value);
	void write_string(int VP, int area_size, int text_size, char *text);
private:
  void enable_write();
  void enable_read();
	int Frame_Read(byte *frame, int VP, int LEN);
	int Frame_Write(byte *frame, int VP, int LEN, int *values);
	int Frame_String(byte *frame, int VP, int area_size, int text_size, char *text);
	int Receive_Values(int nvalues, int *values);
	int Receive_String(int text_size, char *text);
};
#endif
