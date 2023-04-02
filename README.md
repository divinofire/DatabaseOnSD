# DatabaseOnSD

## Description
- A library to easily create and manage a database on an SD card. it makes reading and writing to an SD card a breeze.
- Easily read and write csv style files on an SD card
- Made by Divino Fire, https://github.com/divinofire/

## Practical Usage
- offline spreadsheet for IOT systems
- credential storage
- alternative long String storage
- weather logging and retrieving
- temperature logging and retrieving
NB: For logging purposes only with no intention to read back the logged data using the microcontroller, consider using the bare SD library.


## Installation
Download this project folder into arduino libaries folder or search 'DatabaseOnSD' in arduino library manager.

## reference pin connections
Lookup the right SPI pin connections for your microcontroller

SPI pin mapping for some microcontrollers

| SD CARD PIN       | ARDUINO UNO  | ESP32 devKIT-WROOM |
|-------------------|--------------|--------------------|
| MOSI              |  11          |  23                |
| MISO              |  12          |  19                |
| CLK               |  13          |  18                |
| CS                |  4           |  5                 |
| VCC               |  VCC         |  VCC               |
| GND               |  GND         |  GND               |


## Usage
```c++

#include <DatabaseOnSD.h>
//make sure you have connected the SD card appropriately 
//(lookup the SPI pin connection for your microcontroller)

//the table name should be short (advisably less than 11 characters including extension) - This is not a must

//note: the csv files created by this library will be compartible with ms excel program
// but csv files generated from ms excel are not compartible with this library
// from version 1.1.0, all will be compartible.




MyTable testTable("test.csv");  //this will create or open a table named test.csv in the root of the SD card

// MyTable testTable("testfolder/test.csv"); //if table is in a folder.

//it could be any extension instead of .csv


void setup(){

// Serial.begin(115200); //start serial monitor with a baud rate of 115200 (good for ESP32)
Serial.begin(9600); // start serial monitor with a baud rate of 9600 (good for Arduino)

testTable.printSDstatus(); //[optional] print the initialization status of SD card
testTable.emptyTable(); //[optional] empty table content (make sure to call begin(rowN, colN) after emptying a table) // you could always add more rows.
testTable.begin(3, 2); //[optional] initialize an empty table with 3 rows and 2 columns (has no effect if table is not empty)
//NOTE: there is no need to call begin() if table in NOT empty



/*  
    0       1
0     NAME    AGE
1     Divino      22
2     Fire      21

The table above has 3 rows and 2 columns
NAME is in cell (0, 0) and 21 is in cell (2, 1)


*/


//write table content
Serial.println("writing to table...");
testTable.writeCell(0, 0, "NAME");   testTable.writeCell(0, 1, "AGE");
testTable.writeCell(1, 0, "Divino"); testTable.writeCell(1, 1, "23");
testTable.writeCell(2, 0, "Fire");   testTable.writeCell(2, 1, "22");
Serial.println("finished writing!");


//the max size of each cell is 20 characters for the sake of memory,
//the max cell size can be changed in header file [but I do not recommend it]


}


void loop(){

  //read all table content cell by cell

  int numRows = testTable.countRows();
  int numCols = testTable.countCols();


  Serial.println("-------------read all table content cell by cell ----------------------");

  for (int i = 0; i < numRows; ++i)
  {
    for (int j = 0; j < numCols; ++j)
    {
      Serial.println(testTable.readCell(i, j));
      delay(1000); // one second delay after each read
    }
  }

  Serial.println(); // space separate the next serial print by an empty line

  delay(3000);

  //read all table content at once
  Serial.println("-------------read all table content at once ----------------------");
  testTable.printTable(); //print entire table content to serial monitor

  Serial.println("\n\n\n\n\n"); // space separate the next serial print by 5 empty lines


  //to erase the data in a cell, parse in an empty string as the data content
  //when you read a cell, it returns a String object, if it is a number, you can convert using toint or toFloat method on the String

  delay(10000);
  
}

```

### Output
The above code will create a table on the SD card in a file named table.csv and read its content cell by cell. The table created is as shown below:

| NAME     | AGE    |
|----------|--------|
| Divino   | 22     |
| Fire     | 21     |


## Quirks:
 1. All cells have the same width (default is 20 characters)- you can increase or decrease this size in the header file.
 2. Writing a comma (',') or newline character ('\n') to a cell will affect cell writing and reading of the entire table.
 3. Row and column indices begin from zero.
 4. Increasing the number of rows is easy [ appendEmptyRow() ] but there is no option to increase the number of columns after filling the table.
 
## Reference - some useful methods (member functions)
```c++
void begin(); //used to initialize existing tables (not necessary if you started called SD.begin() in setup())
void begin(int numRows, int numCols); //used to initialize a new table
bool writeCell(int rowN, int colN, String xdata); //write to a cell 
String readCell(int rowN, int colN); //read a cell 
int countRows(); //return the number of rows in the table
int countCols(); //return the number of columns in the table
void appendEmptyRow(); //append an empty row to the table
bool deleteRow(unsigned int rowN); //delete the specified row
```

## Reference2 - other useful methods (member functions)
```c++
//the functions below are dangerous if not used well,
void emptyTable(); // * completely empty a table
void appendEmptyRow(int colN); //append an empty row to the table with colN number of columns, use this after calling emptyTable()
bool insertRow(unsigned int rowN, String row); //insert the row into the row index
bool overwriteRow(int rowN, String row ); //* overwrite the specified row, 
bool deleteRow(int rowN); //deletes the specified row from the table
String selectRow(int rowN); //return an entire row as a string

//For further methods and functions, see header file.
```


## DEBUG mode
To debug an existing table or to try out what is possible with this library, use the DEBUG_Table() function

## DEBUG example
```c++
//make sure you have connected the SD card appropriately (lookup the SPI pin connection for your microcontroller)

#define DEBUG_TABLE_ENABLED //enable the DEBUG_Table() function
#include <DatabaseOnSD.h>


MyTable debugTable("debug.csv"); //change the table name here to your desired name

void setup(){
Serial.begin(9600); //start serial monitor with a baud rate of 115200
debugTable.printSDstatus(); //[optional] print the initialization status of SD card
debugTable.emptyTable(); // [optional] clear table contents
debugTable.begin(1, 1); //begin with 1 row and 4 columns with no cell data [only if table is empty]
// debugTable.begin(); //debug a non empty table 
//number of columns is rigid but not so with the number of rows

}


void loop(){
  DEBUG_Table(debugTable);
}

/*

upload the above code and open the serial monitor
adjust the baud rate to match with the one set in the code
turn on 'no line ending' in the arduino serial monitor

send any of the following commands to start debugging your table
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

//all paramters after '=' can be modified. e.g. read=1,2
//send 'print' to see changes on the table

*/
```


*Happy coding!!!*
