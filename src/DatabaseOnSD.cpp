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

* Neither the name Divino Fire or the names of contributors if any
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

bool MyTable::SD_begun = false; // track if SD was began, useful if multiple objects are instantiated.
//------------------------begin ---------------------verified--------------

void MyTable::printSDstatus(){
   if (SD_begun) Serial.println("SD card initialized!");
  else Serial.println("SD card failed, or not present - (check pin connections)");
}

//begin() is not meant to be called externally
void MyTable::begin(){
  if (!SD_begun) {
    // see if the card is present and can be initialized:
    if (!SD.begin(CHIP_SELECT_PIN)) {
      
      SD_begun = false;
    }
    else{
      
      SD_begun = true;
    }
  }
}

//initialise your table with a given number of rows and columns
void MyTable::begin(int numRows, int numCols){

  //append set rows with set columns
  default_numCols = numCols;
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
     String row = selectRow(rowN);
     int numCols = countCols();
     int numRows = countRows();
     if ((rowN >= numRows)|| (colN >= numCols)) return "";
      
     String data = "";
     int start = 0, stop=0;
     int j=0;


     
    start = nthCharIndex(DIL, colN, row);
    stop = nthCharIndex(DIL, colN+1, row);

    
    //---fix extreme conditions--
    
    // go the index before zero if first cell
    if (colN==0) {start = -1;} 

    uint8_t FIRST_ENDL_OCCURRENCE = 1;
    if (colN==(numCols-1)) {stop = nthCharIndex(ENDL, FIRST_ENDL_OCCURRENCE, row);} 

    //Serial.print("start: ");  Serial.print(start); Serial.print("  stop: ");  Serial.println(stop);
    
    //read from start to stop [start and stop exclusive]
    j=0;
    for(auto i : row){
      if ((j>start) and (j<stop)) {data += String(i);}
      j++;
    }
  
    data.trim(); // remove end spaces or newlines
    return data;
 }



//------------------------------------writeCell---------------verified----------------------------------

bool MyTable::writeCell(int rowN, int colN, String xdata){
  #ifndef ESP32
  return writeCellFast_(rowN, colN, xdata);
  #else
  return writeCell_(rowN, colN, xdata);
  #endif
  
}

bool MyTable::writeCell(int rowN, int colN, long xdata){
  #ifndef ESP32
  return writeCellFast_(rowN, colN, String(xdata));
  #else
  return writeCell_(rowN, colN, String(xdata));
  #endif
}

bool MyTable::writeCell_(int rowN, int colN, String xdata){
  int numCols = countCols();
  int numRows = countRows();
  if ((rowN >= numRows)|| (colN >= numCols)) return false;
  String row = selectRow(rowN);
 
 int start = 0;
 // int j=0;
 bool end = false;

//select start of column index
 start = nthCharIndex(DIL, colN, row);

//----------fix extreme conditions------
//if the user wishes to select for example cell [0,0], set start before 0 index
  if (colN==0) {start= -1; } //check if the target cell is in the first column
  if (colN == (numCols-1)) {end = true;} //check if the target cell is in the last column

  //writing in row from start
  if (!end) row = updateString(++start, fd(xdata, CELL_LENGTH), row);
  else row = updateString(++start, fn(xdata, CELL_LENGTH), row);
 

///---------------------rewritten for esp32 support----------------


// open or create an empty temporary file
  MyTable tempfile(TEMPFILENAME);
    tempfile.emptyTable();
  
// copy all table data to tempfile
    for (int i=0; i<numRows; i++){
      tempfile.appendRow(selectRow(i)); 
    }

//write row to table
   overwriteRow(rowN, row);
  
    // copy everything in temp file after rown to table after rown
    for (int i=0; i < numRows; i++){
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
  int numRows = countRows();
  if ((rowN >= numRows)|| (colN >= numCols)) return false;
  String row = selectRow(rowN);
 
 int start = 0;
 bool end = false;

//select start of column index
 start = nthCharIndex(DIL, colN, row);

//----------fix extreme conditions------
//if the user wishes to select for example cell [0,0], set start before 0 index
  if (colN==0) {start= -1; }
  if (colN == (numCols-1)) {end = true;}

  //writing in row from start
  if (!end) row = updateString(++start, fd(xdata, CELL_LENGTH), row);
  else row = updateString(++start, fn(xdata, CELL_LENGTH), row);
 
  //write new row to table
  int index = CELL_LENGTH*numCols*rowN;
  File table;
  table = SD.open(tableName, FILE_WRITX);  
  table.seek(index);
  for(auto i: row) { table.write(i); }
  table.close();
  // Serial.print("FILE_WRITX: "); Serial.println(FILE_WRITX);
  return true;
}




//----------------------overwriteRow-----verfied------------------------------------------------
bool MyTable::overwriteRow(int rowN, String row){
  int numCols = countCols();
  int numRows = countRows();
  if (rowN >= numRows) return false;
    int index = CELL_LENGTH*numCols*rowN;
    File table;
    table = SD.open(tableName, FILE_WRITX);  
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
    table = SD.open(tableName, FILE_APPENX);  
    // table.seek(table.size());
    for(auto i: row){ table.write(i);}
    table.close();
  }

  void MyTable::appendEmptyRow(int colN){
    appendRow(generateEmptyRow(colN));
  }

  void MyTable::appendEmptyRow(){
    int numCols = countCols();
    if (numCols < 1){
      numCols = default_numCols;
    }
    appendRow(generateEmptyRow(numCols));
  }

//------------------overwriteRow--------------------------verified-----------------------
  void MyTable::overwriteTable(String data){
      emptyTable();
      File table;
      table = SD.open(tableName, FILE_WRITX); 
      table.print(data); 
      table.close();
  }

//---------------------emptyTable ------------------------verified------------------------------
  void MyTable::emptyTable(){
    if (SD.exists(tableName)){
      SD.remove(tableName);
      File table;
      table = SD.open(tableName, FILE_CREATX);
      table.close();
    }
    //if table does not exists, create empty an empty table
    else {
      File table;
      table = SD.open(tableName, FILE_CREATX);
      table.close();
    }

  }

//----------------deleteTable------------------------------------------
  void MyTable::deleteTable(){
    if (SD.exists(tableName)) SD.remove(tableName);
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
    if (rowN < 0) return false;
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
//warning: positionx starts from 1 (It seemed necessary at the time)
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
      if (numCols <= 0) return "";
      String emptyCell = repeatString(STRING_PAD, CELL_LENGTH-1); // empty cell data
      if (numCols == 1) return emptyCell + ENDL;
      String emptyRow = emptyCell + DIL; // empty cell
      emptyRow = repeatString(emptyRow, numCols-1); // empty row with last cell
      emptyRow += (emptyCell + ENDL); // complete empty row
      return emptyRow;
  }





  String repeatString(String string, uint8_t ntimes){
    //ntimes must be >= 1
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
    append // append empty row to the end of table
    cols // print the number of columns
    del=2 // delete row with index 2
    delete // delete last row
    empty // empy table
    insert=3 // insert empty row at row index 3
    name // print the name of the table
    print // print table
    read=0,1 // read cell at position 0,1
    row=0 // print row 0
    rows // print the number of rows
    write=2,3+data // write data to cell at postion 2,3
  */

  if(Serial.available()){
      String command = Serial.readString();

      if (command == "print") {
        Serial.println("---start of table---");
        table.printTable(); 
        Serial.println("---end of table----");}
      else if (command == "rows") Serial.println(table.countRows());
      else if (command == "cols") Serial.println(table.countCols());
      else if (command == "empty") {
        table.emptyTable();
        Serial.println("table emptied");}
      else if (command == "name") Serial.println(table.getName());
      else if (command == "append") {
          table.appendEmptyRow(); 
          Serial.println("empty row appended!");}
      else if (command == "delete") {
        table.deleteRow(table.countRows() - 1); 
        Serial.println("last row deleted");}
      
      //if it's non of the commands above then move on to advance commands
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
          Serial.println(F("empty row inserted"));
        }
        else if (command2 == "row") {
          rowN = parseNumber(command.substring(eq_index+1));
          Serial.println(table.selectRow(rowN));
        }
        else if (command2 == "read") Serial.println(table.readCell(rowN, colN));
        else if (command2 == "write") {
          table.writeCell(rowN, colN, command.substring(plus_index+1));
          Serial.print(command.substring(plus_index+1));
          Serial.println(F(" written to table"));
        }
        else Serial.println(F("--not a command--"));
      }
      else Serial.println(F("--not a command--"));
  }
}




