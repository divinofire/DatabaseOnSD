//by Divino Fire <github.com/divinofire>
//date: 18/09/2022

//Debug a table on SD card by passing commands via serial monitor


//make sure you have connected the SD card appropriately (lookup the SPI pin connection for your microcontroller)


#include <DatabaseOnSD.h>


MyTable debugTable("debug.csv"); //change the table name here to your desired name

void setup(){
Serial.begin(115200); //start serial monitor with a baud rate of 115200
debugTable.begin(1, 4); //begin with 1 row and 4 columns with no cell data [only if table is empty]
// debugTable.begin(); //debug a non empty table 
//number of columns is rigid but not so with the number of rows

}


void loop(){
  DEBUG_Table(debugTable);
}

/*

upload the above code and open the serial monitor
adjust the baud rate to match with the one set in the code
turn on no line ending

send any of the following commands to start debugging your table
    append // append empty row to the end of table
    print // print table
    delete // delete last row
    del=1 // delete row 1
    empty // empy table
    read=1,1 // read cell 1,1
    write=1,1+data // write data to cell 1,1
    insert=1 // insert empty row at row index 1

//all paramters after '=' can be modified. e.g. read=0,2
//send 'print' to see changes on the table

*/