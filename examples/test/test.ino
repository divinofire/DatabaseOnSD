//by Divino Fire <github.com/divinofire>
//date: 18/09/2022
// write and read to a table on SD card 

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
		0   		1
0 		NAME		AGE
1 		Divino      22
2  		Fire   		21

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


