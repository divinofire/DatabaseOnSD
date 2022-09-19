/*
Copyright (c) 2022, Divino Fire <github.com/divinofire>
All rights reserved.

September 2022

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name Divino Fire or the names of contributors 
  to the repository <github.com/divinofire> may be used to endorse or promote
  products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/

#ifndef DATABASE_ON_SD_H
#define DATABASE_ON_SD_H

#include <Arduino.h>

#ifdef ESP32

#include "FS.h"
#define FILE_READ       "r"
#define FILE_WRITE      "w"
#define FILE_APPEND     "a"
#define CHIP_SELECT_PIN  5

#else

#define FILE_READ       O_READ
#define FILE_WRITE      O_WRITE
#define FILE_APPEND     O_APPEND
#define CHIP_SELECT_PIN  10

#endif 

#include <SPI.h>
#include <SD.h>


#define TEMPFILENAME 			"/tempf.txt"
#define ENDL 		 			'\n'
#define DIL 					','
#define EOF 					'\0'
#define CUSTOM_MAX_BUFFER_SIZE	100
#define STRING_PAD 				" "
#define CELL_LENGTH		 		20

class MyTable{

public:
	MyTable(String name){

	  setName(name);
	  if (SD.exists(getName()) == false){
      File table;
      table = SD.open(getName(), FILE_WRITE);
      table.close();
    }

	}

	~MyTable(){}

	void begin();
	void begin(int numRows, int numCols);
	String getName();
	void setName(String name);
	void printTable();
	String selectRow(int rowN);
	int nthCharIndexTable(char charx, uint16_t positionx);
	bool writeCell(int rowN, int colN, String xdata);
	bool writeCell(int rowN, int colN, long xdata);
	bool writeCell_(int rowN, int colN, String xdata);
	bool writeCellFast_(int rowN, int colN, String xdata);
	bool writeCellFast(int rowN, int colN, String xdata);
	bool writeCellFast(int rowN, int colN, long xdata);
	String readCell(int rowN, int colN);
	int countRows();
	int countCols();
	void appendRow(String row);
	void appendEmptyRow(int colN);
	void appendEmptyRow();
	void overwriteTable(String data);
	void emptyTable();
	bool deleteRow(unsigned int rowN);
	bool insertRow(unsigned int rowN, String row);
	bool overwriteRow(int rowN, String row );
/*
void printTable(); //print all of table content to serial monitor
*/
private:

	String tableName = "/noname.txt";
	static bool SD_begun;

};

//first class helper functions
int nthCharIndex(char charx, uint8_t positionx, String stream);
String generateEmptyRow(int numCols);

//second class helper functions
void DEBUG_Table(MyTable table);
int countColsX(String rowx);
String readAttribute(int colN, String row);
String fitStringD(String ss, int _size);
String fitStringN(String ss, int _size);
String fitString(String ss, int size);
String fd(String s, int size);
String fn(String s, int size);
String cd(String s);
String cn(String s);
String repeatString(String string, uint8_t ntimes);
String updateString(int index, String data, String string_);
long parseNumber(String string);
void emptyBuffer(char buffer[CUSTOM_MAX_BUFFER_SIZE], uint16_t len);

//end of declarations





#endif // DATABASE_ON_SD_H



