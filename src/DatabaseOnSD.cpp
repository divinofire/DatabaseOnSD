/*
Copyright (c) 2022, Divino Fire <github.com/divinofire>
All rights reserved.

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
#include "DatabaseOnSD.h"

bool MyTable::SD_begun = false; // track if SD was began
//------------------------begin ---------------------verified--------------
void MyTable::begin(){

  if (!SD_begun) {
    SD.begin();
    SD_begun = true;
  }
   
  if (SD.exists(getName()) == false){
      File table;
      table = SD.open(getName(), FILE_WRITE);
      table.close();
  }
}

void MyTable::begin(int numRows, int numCols){

  //begin SD if it has not begun already
  if (!SD_begun) {
    SD.begin();
    SD_begun = true;
  }
   
  //create table with table if it does not exist
  if (SD.exists(getName()) == false){
      File table;
      table = SD.open(getName(), FILE_WRITE);
      table.close();
  }

  //append set rows with set columns
  if (countRows() == 0){
    for (int i = 0; i < numRows; ++i){
      appendEmptyRow(numCols);
    }
  }
}

//----------------getName------------------verified------------------------
String MyTable::getName(){return tableName;}


//------------------------setName--------------verified----------------------
void MyTable::setName(String name){
    if (name[0] == '/') tableName = name;
    else tableName = String('/') + name;
  }

//-----------------printTable-------------verfied----------------------
void MyTable::printTable(){
  if (SD.exists(tableName) == false) {
    Serial.print(tableName); Serial.println(" does not exist");
    return;
  }
  File table;
  table = SD.open(tableName);
  table.seek(0);
  while(table.available()){
    Serial.print(char(table.read()));
  }  
  table.close();
}

// ---------------------------selectRow------ verified--------------------
String MyTable::selectRow(int rowN){
  //Serial.println("start of selectrow");
  File table;
  table = SD.open(tableName);  
  String row = "";
  int start = 0, stop = 0;
  // select index of line start
  if (rowN > 0) start = nthCharIndexTable(ENDL, rowN);
  else {start = -1;}
   //Serial.print("start: "); Serial.println(start);
  // select index of line stop
  stop = nthCharIndexTable(ENDL, rowN+1);
   //Serial.print("stop: "); Serial.println(stop);
  int j=0; char i;
  table.seek(0); // go to begining of table
  while(table.available()){
    i = char(table.read());
    if ((j>start) and (j<=stop)) row += String(i);
    j++;
    if (j>stop) {
      table.close();
      // Serial.print(F("freeMemory in selectRow: ")); Serial.println(freeMemory()); 
      return row;
    }  
  } 
  table.close();

}

//------------------------nthCharIndexTable-----------verfified----------------------
int MyTable::nthCharIndexTable(char charx, uint16_t positionx){
  //Serial.println("start of ncharindex");
  File table;
  table = SD.open(tableName);  
  int counterx = 0;
  int posCount = 0;
  char i;
  table.seek(0);
  while (table.available()){
    i = table.read();
    //Serial.print("i: ");Serial.println(i);
    if (i== charx) posCount++;
    if (posCount == positionx) {table.close(); return counterx; }
    counterx++;   
  }
  table.close();
  return -1;
}



 //--------------------------readCell------------------verfied--------------------------------
String MyTable::readCell(int rowN, int colN){
     File table;
     table = SD.open(tableName);  
     String row = selectRow(rowN);
     int numCols = countCols();
      
     String data = "";
     int start = 0, stop=0;
     int j=0;
     
    start = nthCharIndex(DIL, colN, row);
    stop = nthCharIndex(DIL, colN+1, row);

    //read from start to stop
    //-fix extreme conditions--
    
    if (colN==0) {start = -1;} //if the user wishes to select for example cell [0,0], set start before 0 index
    //if the user selects a column that marks the end of a line, get stop to be the position of '\n' instead of ','
    else if (colN==(numCols-1)) {stop = nthCharIndex(ENDL, 1, row);}
    //Serial.print("start: ");  Serial.print(start); Serial.print("  stop: ");  Serial.println(stop);
    j=0;
    for(auto i : row){
      if ((j>start) and (j<stop)) {data += String(i);}
      j++;
    }
    table.close();
    data.trim();
    // Serial.print(F("freeMemory in readCell: ")); Serial.println(freeMemory()); 
    return data;
 }



//------------------------------------writeCell---------------verified----------------------------------

bool MyTable::writeCell(int rowN, int colN, String xdata){
  return writeCell_(rowN, colN, xdata);
}

bool MyTable::writeCell(int rowN, int colN, long xdata){
  return writeCell_(rowN, colN, String(xdata));
}

bool MyTable::writeCell_(int rowN, int colN, String xdata){
  int numCols = countCols();
  int Nrows = countRows();
  String row = selectRow(rowN);
 
 int start = 0;
 int j=0;
 bool end = false;

//select start of column index
 start = nthCharIndex(DIL, colN, row);

//----------fix extreme conditions------
//if the user wishes to select for example cell [0,0], set start before 0 index
  if (colN==0) {start= -1; }
  else if (colN == (numCols-1)) {end = true;}

  //writing in row from start
  if (!end) row = updateString(++start, fd(xdata, CELL_LENGTH), row);
  else row = updateString(++start, fn(xdata, CELL_LENGTH), row);
 

///---------------------rewritten for esp32 support----------------


// open or create an empty temporary file
  MyTable tempfile(TEMPFILENAME);
    tempfile.emptyTable();
  
// copy all table data to tempfile
    for (int i=0; i<Nrows; i++){
      tempfile.appendRow(selectRow(i)); 
    }

//write row to table
   overwriteRow(rowN, row);
  
    // copy everything in temp file after rown to table after rown
    for (int i=0; i < Nrows; i++){
      if (i>rowN) appendRow(tempfile.selectRow(i)); 
    }

  
  return true;
}



//-------------------------writeCellFast-- use on arduino only --------verfied--------------------------
bool MyTable::writeCellFast(int rowN, int colN, String xdata){
  return writeCellFast_(rowN, colN, xdata);
}

bool MyTable::writeCellFast(int rowN, int colN, long xdata){
  return writeCellFast_(rowN, colN, String(xdata));
}
bool MyTable::writeCellFast_(int rowN, int colN, String xdata){
  int numCols = countCols();
  String row = selectRow(rowN);
 
 int start = 0;
 int j=0;
 bool end = false;

//select start of column index
 start = nthCharIndex(DIL, colN, row);

//----------fix extreme conditions------
//if the user wishes to select for example cell [0,0], set start before 0 index
  if (colN==0) {start= -1; }
  else if (colN == (numCols-1)) {end = true;}

  //writing in row from start
  if (!end) row = updateString(++start, fd(xdata, CELL_LENGTH), row);
  else row = updateString(++start, fn(xdata, CELL_LENGTH), row);
 
  //write new row to table
  int index = CELL_LENGTH*numCols*rowN;
  File table;
  table = SD.open(tableName, FILE_WRITE);  
  table.seek(index);
  for(auto i: row) { table.write(i); }
  table.close();
  return true;
}




//----------------------overwriteRow-----verfied------------------------------------------------
bool MyTable::overwriteRow(int rowN, String row){
  int numCols = countCols();
  int Nrows = countRows();
  if (rowN >= Nrows) return false;
    int index = CELL_LENGTH*numCols*rowN;
    File table;
    table = SD.open(tableName, FILE_WRITE);  
    table.seek(index);
    for(auto i: row) {
      table.write(i); 
    }
    table.close();

  return true;
}




//----------------------------countRows in table  ------------verfied------------------------------------
int MyTable::countRows(){
  File table;
  table = SD.open(tableName);  
  int rows = 0;
  char i;
  table.seek(0);
  while(table.available()){
    i = table.read();
    if (i == ENDL) rows++;
  }
  table.close();
  return rows;
}

//----------------------------countCols in a table ------verified--------------------
int MyTable::countCols(){
  String rowx = selectRow(0);
  int cols = 0;
  for (auto i: rowx){
     if ((i == DIL) or (i == ENDL))cols++;
    }
  return cols;
 }


// ----------------------appendRow ---------------verified-----------------------------
void MyTable::appendRow(String row){
    File table;
    table = SD.open(tableName, FILE_APPEND);  
    // table.seek(table.size());
    for(auto i: row){ table.write(i);}
    table.close();
  }

  void MyTable::appendEmptyRow(int colN){
    appendRow(generateEmptyRow(colN));
  }

  void MyTable::appendEmptyRow(){
    appendRow(generateEmptyRow(countCols()));
  }

//------------------overwriteRow--------------------------verified-----------------------
  void MyTable::overwriteTable(String data){
      emptyTable();
      File table;
      table = SD.open(tableName, FILE_WRITE); 
      table.print(data); 
      table.close();
  }

//---------------------emptyTable ------------------------verified------------------------------
  void MyTable::emptyTable(){
    if (SD.exists(tableName)){
      SD.remove(tableName);
      File table;
      table = SD.open(tableName, FILE_WRITE);
      table.close();
    }
    //create empty tempfile if it doesnt exist
    else {
      File tempfile; 
      tempfile = SD.open(TEMPFILENAME, FILE_WRITE); 
      tempfile.close();
    }
  }

//------------------------deleteRow----------------------------verified-----------------------
  bool MyTable::deleteRow(unsigned int rowN){
    int Nrows = countRows();
     // Serial.print("number of rows: ");Serial.println(Nrows);
     // Serial.print("row to delete: ");Serial.println(rowN);
    if (rowN>=Nrows) {Serial.println(F("early fail to delete")); return false;} // validate row number

    // open or create an empty temporary file
    MyTable tempfile(TEMPFILENAME);
    tempfile.emptyTable();

    // copy rows from table to temp file except desired row
    for (int i=0; i<Nrows; i++){
      if (i==rowN) continue;
      else tempfile.appendRow(selectRow(i)); 
    }

    emptyTable(); // clear table

    for (int i=0; i < (Nrows-1); i++){
      appendRow(tempfile.selectRow(i)); 
    }

    return true;

  }


//--------------insertRow------------------------------------verified------------------
  bool MyTable::insertRow(unsigned int rowN, String row){
    int Nrows = countRows();

     // append row instead if row number is above available rows
    if (rowN>=Nrows) {appendRow(row); return true;}

    // open or create an empty temporary file
    MyTable tempfile(TEMPFILENAME);
    tempfile.emptyTable();

    // copy rows from table to temp file including desired row
    for (int i=0; i<Nrows; i++){
      if (i==rowN) {tempfile.appendRow(row);} 
      tempfile.appendRow(selectRow(i)); 
    }
   
   // clear table
    emptyTable(); 

    // copy everything in temp file to table
    ++Nrows;
    for (int i=0; i < Nrows; i++){
      appendRow(tempfile.selectRow(i)); 
    }

    return true;

  }















//  ------------------- helper functions definitions---------------------------

  //----------------get index of the nth appearance of character in string ---------------------
//warning: positionx starts from 1
int nthCharIndex(char charx, uint8_t positionx, String stream){
  int counterx = 0;
  int posCount = 0;
  for (auto i:stream){
    if (i==charx) posCount++;
    if (posCount == positionx)  return counterx; 
    counterx++;   
  }
  return -1;
}


// -----------------generateEmptyRow ---------------verified------------------
String generateEmptyRow(int numCols){
      String emptyCell = repeatString(" ", CELL_LENGTH-1); // empty cell data
      String emptyRow = emptyCell + DIL; // empty cell
      emptyRow = repeatString(emptyRow, numCols-1); // empty row with last cell
      emptyRow += (emptyCell + ENDL); // complete empty row
      return emptyRow;
  }





  String repeatString(String string, uint8_t ntimes){

      String repeatedString = string;
      for (int i = 0; i < ntimes-1; ++i)
      {
        repeatedString+=string;
      }

      return repeatedString;
  }

  String updateString(int index, String data, String string_){
    String string = string_;
    int j=0;
    for (int i = 0; i<string.length(); i++){
        if (i>=index){
          string[i] = data[j]; 
          // Serial.println(string);
          j++;
          if (data.length()==j) {
            // Serial.print(F("string: "));Serial.println(string);
            return string;
            }
          }
      }
    }

long parseNumber(String string){
  string.trim();
  return string.toInt();
  }

 

 //----------------------------empty a buffer------------------------------------------------
 void emptyBuffer(char buffer[CUSTOM_MAX_BUFFER_SIZE], uint16_t len){
    memset(buffer, 0, len);
  }




  //----------------------------fit String  ------------------------------------------------
//add space or remove chars till size is met

String fitString(String ss, int size){
        String s = ss;
       if (s.length()==size) return s;
       while (s.length()!= size){
          if (s.length( )> size) s.remove(s.length( )-1);
          else if (s.length()>size) s += " ";
          
        }
        return s;
  }
String fn(String s, int size){
  return fitStringN(s, size);
}

String cn(String s){ return fitStringN(s, CELL_LENGTH);}

String fd(String s, int size){
  return fitStringD(s, size);
}
String cd(String s){ return fitStringD(s, CELL_LENGTH);}

//----------------------------fit String N ------------------------------------------------
//format data cell in the last column: add space or remove chars till size is met
// add "\n" after that

String fitStringN(String ss, int _size){
      String s = ss;
       int size = _size-1;
       if (s.length()==size) return s;
       while (s.length()!=size){
          if (s.length()> size) s.remove(s.length()-1);
          else if (s.length()<size) s += STRING_PAD;
     }
      s += ENDL;
      return s;
      
  }
  
//----------------------------fit String D ------------------------------------------------
//format data cell not in the last column: add space or remove chars till size is met
// add ", " after that

String fitStringD(String ss, int _size){
      String s = ss;
       int size = _size-1;
       if (s.length()==size) return s;
       while (s.length()!=size){
          if (s.length( )> size) s.remove(s.length( )-1);
          else if (s.length()<size) s += STRING_PAD;
      }
      s += DIL;
      return s;
      
  }

//-----------------------readAttribute------------------------------------------
  String readAttribute(int colN, String row){

    int rowN = 0;
    int numCols = countColsX(row);
     
 
     String data = "";
     int start = 0, stop=0;
     int j=0;
     
    start = nthCharIndex(DIL, colN, row);
    stop = nthCharIndex(DIL, colN+1, row);

    //read from start to stop
    //------------------fix extreme conditions-----------------------
    
    if (colN==0) start = -1; //if the user wishes to select for example cell [0,0], set start before 0 index
    //if the user selects a column that marks the end of a line, get stop to be the position of '\n' instead of ','
    else if (colN==(numCols-1)) stop = nthCharIndex(ENDL, 1, row);

    j=0;
    for(auto i : row){
      if ((j>start) and (j<stop)) {data += String(i);}
      j++;
    }
    data.trim();
    return data;
 }


//-----------------------------------countColsX------------------------------------

 int countColsX(String rowx){
  int cols = 0;
  for (auto i: rowx){
     if ((i == DIL) or (i == ENDL)) cols++;
    }
  return cols;
 }


//-----------use this function to debug your table from serial monitor (see example)-------------------
void DEBUG_Table( MyTable table){

  /*commands
    print // print table
    delete // delete last row
    del=1 // delete row 1
    empty // empy table
    read = 1,1 // read cell 1,1
    write = 1,1+data // write data to cell 1,1
    insert = 1 // insert empty row at row index 1
    append // append empty row to the end of table
  */

  if(Serial.available()){
      String command = Serial.readString();
      if (command == "print") table.printTable();
      else if (command == "rows") Serial.println(table.countRows());
      else if (command == "cols") Serial.println(table.countCols());
      else if (command == "empty") {table.emptyTable(); Serial.println("table emptied");}
      else if (command == "name") Serial.println(table.getName());
      else if (command == "append") {table.appendRow(generateEmptyRow(table.countCols())); Serial.println("empty row appended!");}
      else if (command == "delete") {table.deleteRow(table.countRows() - 1); Serial.println("last row deleted");}
      else if (command.length() > 4){

        int eq_index = command.indexOf('=');
        int comma_index = command.indexOf(',');
        int plus_index = command.indexOf('+');
        int rowN = parseNumber(command.substring(eq_index+1, comma_index));
        int colN = parseNumber(command.substring(comma_index+1));

        String command2 =  command.substring(0, eq_index);

        if (command2 == "del") {
          rowN = parseNumber(command.substring(eq_index+1));
          table.deleteRow(rowN);
          Serial.println("row deleted");
        }
        else if (command2 == "insert") {
          rowN = parseNumber(command.substring(eq_index+1));
          table.insertRow(rowN, generateEmptyRow(table.countCols()));
          Serial.println("empty row inserted");
        }
        else if (command2 == "row") {
          rowN = parseNumber(command.substring(eq_index+1));
          Serial.println(table.selectRow(rowN));
        }
        else if (command2 == "read") Serial.println(table.readCell(rowN, colN));
        else if (command2 == "write") {
          table.writeCell(rowN, colN, command.substring(plus_index+1));
          Serial.print(command.substring(plus_index+1));
          Serial.println(" written to table");
        }

      }
  }
}
