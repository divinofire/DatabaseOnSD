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

## Usage
```c++

#include <DatabaseOnSD.h> // include the library


//the table name should be short (less than 8 characters)

//note: the csv files created by this library will be compartible with ms excel program
// but csv files generated from ms excel are not compartible with this library
// from version 1.0.2, all will be compartible.


MyTable users("table.csv");  //create or open a table named table.csv in the root of the SD card

// MyTable users("folder/table.csv"); //if table is in folder.

void setup(){

Serial.begin(115200); //start serial monitor with a baud rate of 115200


users.begin(3, 2); // if table is empty, initialize table with 3 rows and two columns
//users.begin(); //for completely empty tables or filled tables. 
// users.emptyTable(); //empty table contents


/*---sample table---  

      0         1
0     NAME      AGE
1     Divino    22
2     Fire      21


The table above (implemented below) has 3 rows and 2 columns
cell indices start from zero:  'NAME' is in cell (0, 0) and '21' is in cell (2, 1)


*/


//write table content
users.writeCell(0, 0, "NAME");   users.writeCell(0, 1, "AGE");
users.writeCell(1, 0, "Divino"); users.writeCell(1, 1, "22");
users.writeCell(2, 0, "Fire");   users.writeCell(2, 1, "21")


//the max size of each cell is 20 characters for the sake of memory,
//the max cell size can be changed in header file [but it's not advisable]


}


void loop(){

  //get the number of rows and columns in the table
  int numRows = users.countRows();
  int numCols = users.countCols();

  //read all table content cell by cell
  Serial.println("-------------read all table content cell by cell ----------------------");

  for (int i = 0; i < numRows; ++i)
  {
    for (int j = 0; j < numCols ++j)
    {
      users.readCell(i, j);
      delay(1000); // one second delay after each read
    }
  }

  Serial.println("\n\n\n\n\n") // space separate the next serial print by 5 empty lines

  delay(3000);

  //read all table content at once
  Serial.println("-------------read all table content at once ----------------------");
  users.printTable(); //print entire table content to serial monitor

  Serial.println("\n\n\n\n\n") // space separate the next serial print by 5 empty lines


  //to erase the data in a cell, pass an empty string as the data content
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
 2. Writing a comma or newline character to a cell will affect cell writing and reading of the entire table.
 3. Row and column indices begin from zero.
 4. increasing the number of rows is easy [ appendEmptyRow() ] but there is no option to increase the number of columns after filling the table.
 
## Reference - some useful methods
```c++
void begin(); //used to initialize existing tables (not necessary if you started called SD.begin() in loop)
void begin(int numRows, int numCols); //used to initialize a new table
bool writeCell(int rowN, int colN, String xdata); //write to a cell 
String readCell(int rowN, int colN); //read a cell 
int countRows(); //return the number of rows in the table
int countCols(); //return the number of columns in the table
void appendEmptyRow(); //append an empty row to the table
bool deleteRow(unsigned int rowN); //delete the specified row
```

## Reference2 - other useful methods
```c++
//the functions below are dangerous if not used well,
void emptyTable(); // * complete empty a table
void appendEmptyRow(int colN); //append an empty row to the table with colN number of columns, use this after calling emptyTable()
bool insertRow(unsigned int rowN, String row); //insert the row into the row index
bool overwriteRow(int rowN, String row ); //* overwrite the specified row, 
bool deleteRow(int rowN); //deletes the specified row from the table
bool writeCellFast(int rowN, int colN, String xdata); //same as writeCell but just very fast. Use this only if you are running your code on an arduino board like uno, nano or mega.
String selectRow(int rowN); //return an entire row as a string

//For further methods and functions, see header file.
```


## DEBUG mode
To debug an existing table or to try out what is possible with this library, use the DEBUG_Table() function

## DEBUG example
```c++
#include <DatabaseOnSD.h>


MyTable users("table.csv"); //change the table name here to your desired name

void setup(){
Serial.begin(115200); //start serial monitor with a baud rate of 115200
users.begin(1, 4); //if table is empty, initialize table with 1 row and 4 columns with no cell data
//number of columns is rigid but not so with the number of rows
}


void loop(){
  DEBUG_table(users);
}

/*

upload the above code and open the serial monitor
adjust the baud rate to match with the one set in the code
turn on no line ending

send any of the following commands to start debugging your table
    print // print table
    delete // delete last row
    del=1 // delete row 1
    empty // empy table
    read=1,1 // read cell 1,1
    write=1,1+data // write data to cell 1,1
    insert=1 // insert empty row at row index 1
    append // append empty row to the end of table

//all paramters after '=' can be modified. e.g. read=0,2
//send 'print' to see changes on the table

*/
```


*Happy coding!!!*
