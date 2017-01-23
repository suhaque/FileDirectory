#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Filesys.h"
using namespace std;

int main(int argc, char **argv)
{

	//can handle 1024 bytes
	
	//1.	create and write a file; file1;  of 40 bytes;
	FileDirectory fi;

	/*
	fi.create("file_1", 40);
	fi.write("file_1", 40, "asdnasjakjasd", 1993, 3, 12, 14, 6, 2);
	fi.printData("file_1",40);
	fi.printDirectory("file_1");
	fi.printClusters("file_1");
	*/

	/*
	//2.	create and write a file; file2;  of 200 bytes;
	fi.create("file_2", 200);
	fi.write("file_2", 200, "12345asdlkansfan890asdfg", 1993, 3, 12, 14, 6, 2);
	fi.printData("file_2",200);
	fi.printDirectory("file_2");
	fi.printClusters("file_2");
	*/

	/*
	//3.	create and write a file; file3;  of 300 bytes;
	fi.create("file_3", 300);
	fi.write("file_3", 300, "12asdjbaksjcbpakjvba34567890asdfg", 1993, 3, 12, 14, 6, 2);
	fi.printData("file_3",300);
	fi.printDirectory("file_3");
	fi.printClusters("file_3");
	*/

	/*
	//4.	create and write a file; file4;  of 500 bytes.
	if(fi.write("file_4", 500,"asjdhaskfbvoagdasskueasueha", 2017, 01, 17, 13, 9, 55))		//1040 bytes in the file system, files need to be deleted to make space for new file)
	cout << "can't make file" << endl;
	*/

	//5.	delete file2;
	fi.deleteFile("file_2");
	//6.	create and write a file; file4;  of 500 bytes.
	
	fi.write("file_4", 500, "discarded", 2017, 01, 17, 13, 9, 55);
	fi.printDirectory("file_4");
	fi.printData("file_4", 500);
	
	return 0;

    return 0;
}

