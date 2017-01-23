#include"stdafx.h";
#include <iostream>;
using namespace std;

#include "Filesys.h";

FileDirectory::FileDirectory()
{
	//to initialize all entries in the fileDirectory and FAT16 to be 0; i.e.safe values.
	for (int i = 0; i < 256; i++) FAT16[i] = 0;
	for (int a = 0; a < 4; a++) {
		for (int b = 0;b < 32;b++)
		{
			fileDirectory[a][b] = 0;
		}
	}
	for (int c = 0; c < 1024; c++)
	{
		data[c] = 0;
	}
};

bool FileDirectory::create(char   filename[], int numberBytes)
{
	unsigned short int numClusters = numberBytes / 4;
	unsigned short int freeclusters = 0;
	unsigned int unused_entry_index = 0;
	int i, j;

	bool enough_clusters = false;
	bool unused_entry = false;

	if (numberBytes % 4 != 0)
	{
		numClusters = numClusters + 1;
	}//end if

	 //(1)	to check if there is an unused entry in the File Directory;  (i.e.the first character of the file name in the File Directory is zero).Return false if not true.
	for (i = 0; i < 4; i++)
	{
		if (fileDirectory[i][0] == 0)
		{
			unused_entry = true;
			unused_entry_index = i;
			break;
		}//end if
		 //(2)	to check if there are enough unused clusters to store the file with the numberBytes.Return false if not true. 
		for (j = 2; j < 256; j++)
		{
			if (FAT16[j] == 0 || FAT16[j] == 1)
			{
				freeclusters++;
			}//end if
		}//end for j
	}//end for i

	 //check if there is enough unused clusters
	if (freeclusters >= numClusters) enough_clusters = true;

	//put the file in the directory
	if (unused_entry && enough_clusters)
	{
		for (int j = 0; j < 8; j++) fileDirectory[unused_entry_index][j] = filename[j];
		return true;
	}//end if
};//end create


bool FileDirectory::deleteFile(char   filename[])
{
	//	(0)	to check if the file to be deleted; filename[]; is in the Directory.If not; return false.
	int i, j;
	unsigned short int firstClusterAddress;
	unsigned short int storeClusterLocations[256];
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (fileDirectory[i][j] != filename[j])
				break;
		}//end for j

		 //  (1)	to change the first character of the file name in the File Directory to be zero;
		if (j == 8)
		{
			//found file to be deleted in the directory
			for (int k = 0; k < 8; k++)
			{
				fileDirectory[i][k] = 0;
			}//end for

			 //	(2)	to change all entries of the clusters of this file in the FAT to 1; i.e.; unused.
			firstClusterAddress = (fileDirectory[i][26] + (fileDirectory[i][27] << 8));
			int a = 0;
			storeClusterLocations[a] = firstClusterAddress;

			while (FAT16[storeClusterLocations[a]] != 0xfff8) //0xfff8 is EOF
			{
				storeClusterLocations[a + 1] = FAT16[storeClusterLocations[a]];
				FAT16[storeClusterLocations[a]] = 1;
				a++;
			}//end while
		}//end if

	}
	if (i == 4) return false;
	else return true;


};//end deletefile

bool FileDirectory::read(char   filename[])
{
	//purpose: to read  of data from data[] array from the file with the specified file name.
	//(0)	to check if the file to be read; filename[]; is in the Directory.If not; return false.
	int j;
	unsigned short int firstClusterAddress, ClusterAddress;
	unsigned short int storeClusterLocation[256];
	unsigned char fileData[1024];
	int i, p, k;

	for (i = 0; i < 4; i++)
	{
		//(1)	use the file name to get the file information from the File Directory; including date; time; number of bytes and the first cluster address;

		for (j = 0; j < 8; j++)
		{
			if (fileDirectory[i][j] != filename[j]) break;
		}

		if (j == 8)
		{
			//found file in the directory matching the input file name

			//(2)	use the first cluster address to get all the cluster addresses of this file from the FAT - 16;
			firstClusterAddress = (fileDirectory[i][26] + (fileDirectory[i][27] << 8)); //first cluster address is stored in bytes 26 and 27
			ClusterAddress = firstClusterAddress;

			for (k = 0; k < 256 && ClusterAddress < 0xFFF8; k++)
			{
				storeClusterLocation[k] = ClusterAddress;
				ClusterAddress = FAT16[ClusterAddress];//store successive cluster address
			}

			//(3)	use all the cluster addresses to read the data from the disk / flash memory.
			p = 0;
			for (int n = 0; n < k; i++)
			{
				for (int l = 0; l < 4; l++)
				{
					fileData[p] = data[storeClusterLocation[n] * 4 + l];
					p = p + 1;
				}
			}
		}//end of j loop
	} // end of i loop
	if (i == 4) return false;		//file not found
	else return true;
};

bool FileDirectory::write(char   filename[], int numberBytes,
	char datainput[], int year, int month, int day,
	int hour, int minute, int second)
{
	//purpose: to write numberBytes bytes of data from data[] array into the file with the specified file name
	//(0)	to look for the first unused entry(0 or 1) in the FAT - 16; and use it as the First Cluster Address.
	unsigned short int firstClusterAddress, nextClusterAddress, numClusters, date, time;
	int i, a, b, x;

	for (i = 2; i < 256; i++)
	{
		if (FAT16[i] == 0 || FAT16[i] == 1)
		{
			firstClusterAddress = i;
			break;
		}//end if
	}//end for

	if (i == 256) {
		cout << "can't find empty cluster for " << filename << endl;
		return false;
	}

	//(1)	to look for the next unused entry(0 or 1) in the FAT - 16; and use it as the Next Cluster Address; and write its value into the FAT - 16.
	nextClusterAddress = firstClusterAddress;
	for (a = 1; a < (numberBytes / 4) + ((numberBytes % 4 != 0) ? 1 : 0); a++)
	{
		for (b = 2; b < 256; b++)
		{
			if ((FAT16[b] == 0 || FAT16[b] == 1) && b != nextClusterAddress)
			{
				FAT16[nextClusterAddress] = b;
				nextClusterAddress = b;
				break;
			}//end if
		}//end for
		if (b == 256) return false;
	}//end for
	unsigned int sectors_needed = b;

	FAT16[nextClusterAddress] = 0xfff8; //end of file

	unsigned short int p = 2; //initialize starting sector to 2

	while (FAT16[p] != 0 && FAT16[p] != 1) p++; //where to write to

	unsigned int firstSectorAddress = p;

	//(2)	Repeat Step 2 until all clusters are found and the FAT - 16 is updated.

	unsigned int empty;

	//(3)to write / update the file name; extension; date; time; file length and first cluster address into the first unused entry in the File Directory;
	for (x = 0; x < 4; i++)
	{
		if (fileDirectory[x][0] == 0)
		{
			empty = x;
			break;
		}//end if
	}//end for

	if (x == 4) return false;


	//write file name into the entry of the file directory
	for (int j = 0;j < 8; j++) { fileDirectory[empty][j] = filename[j]; }


	//write time into directory[23:22]
	fileDirectory[empty][23] = (hour << 3) + ((minute >> 3) & 0x7);
	fileDirectory[empty][22] = ((minute & 0x7) << 5) + (second / 2);

	//write date into directory[25:24]
	fileDirectory[empty][25] = ((year - 1980) << 1) + ((month >> 3) & 0x01);
	fileDirectory[empty][24] = ((month & 0x7) << 5) + (day);

	//write first sector address into directory[27:26]
	fileDirectory[empty][27] = firstClusterAddress >> 8;
	fileDirectory[empty][26] = firstClusterAddress;

	//write file length into directory[31:28]
	fileDirectory[empty][31] = numberBytes >> 24;
	fileDirectory[empty][30] = numberBytes >> 16;
	fileDirectory[empty][29] = numberBytes >> 8;
	fileDirectory[empty][28] = numberBytes;


	//writing data 
	for (unsigned int d = 0; d < sectors_needed; d++) data[d] = datainput[d];

};//end write


void FileDirectory::printClusters(char filename[])
{
	///purpose: to print all the clusters of a file.
	int j, k;
	unsigned short int firstClusterAddress;
	unsigned short int ClusterAddress;

	for (int i = 0; i < 4; i++)
	{//(1)	to check if the file to be printed; filename[]; is in the Directory.If not; return false.
		for (j = 0; j < 8; j++)
		{
			if (fileDirectory[i][j] != filename[j])
			{
				break;
			}//end if
		}//end for
		if (j == 8)
		{
			cout << "Clusters of " << filename << " are: " << endl;
			//(2)	use the file name to get the file information from the File Directory; including the first cluster address;
			firstClusterAddress = (fileDirectory[i][27] << 8) + fileDirectory[i][26];
			ClusterAddress = firstClusterAddress;
			//(3)	use the first cluster address to get all cluster addresses from the FAT - 16;
			while (ClusterAddress != 0xfff8)
			{
				cout << ClusterAddress << " -> ";
				ClusterAddress = FAT16[ClusterAddress];
			}
		}
	}
};

void FileDirectory::printDirectory(char filename[])
{
	unsigned short int date, time;
	unsigned int year, month, day, hour, minute, second;
	unsigned char fileext;
	//prints all the  files of the directory.
	for (int i = 0; i < 4; i++)
	{
		//(1)if the file name is valid, print file name, ',' , and file information
		if (fileDirectory[i][0] != 0)
		{
			for (int b = 4; b < 11; b++)
			{
				fileext = fileDirectory[i][b];
			}

			//(2)print date
			date = (fileDirectory[i][25] << 8) + fileDirectory[i][24];
			year = 1980 + ((date & 0xfe00) >> 9); //output year
			month = ((date & 0x01e0) >> 5); //output month //&& is a boolean operator
			day = (date & 0x001f); //output day

								   //(3) print time
			time = (fileDirectory[i][23] << 8) + (fileDirectory[i][22]);
			hour = ((time & 0xf800) >> 11); //hour
			minute = ((time & 0x07e0) >> 5); //minute
			second = ((time & 0x001f) << 1); //second

											 //(4)print file length
			unsigned short int filelength;
			filelength = (fileDirectory[i][31] << 24) + (fileDirectory[i][30] << 16) + (fileDirectory[i][29] << 8) + (fileDirectory[i][28]);

			cout << "\nFile Information: \n" << endl;
			for (int x = 0; x < 8; x++)
			{
				cout << filename[x] << " ";
			}
			cout << " , " << fileext << "  " << filelength << " \n"<<
				year << "/" << month << "/" << day << "  " <<
				hour <<":" << minute << ":" << second << endl;
		}
	}
}; //printDirectory


void FileDirectory::printData(char filename[])
{
	/*prints the data of a file.
			(3)	use cluster address to read the data of the file.Use the file length to print these data in hexadecimal format.*/

	cout << "data for " << filename << endl;
	int i, j, k;
	unsigned short int firstClusterAddress, ClusterAddress;
	unsigned short int ClusterLocations[256];
	unsigned char dataout[1024];
	//(1)	use the file name to get the file information from the File Directory; including the first cluster address;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (fileDirectory[i][j] != filename[j])
				break;
		}//end for
		//(2)use the first cluster address to get all cluster addresses from the FAT - 16;
		if (j == 8)
		{
			firstClusterAddress = (fileDirectory[i][27] << 8) + (fileDirectory[i][26]);
			
			ClusterAddress = firstClusterAddress;
			k = 0;
			while (ClusterAddress != 0xfff8)
			{
				ClusterLocations[k] = ClusterAddress;
				ClusterAddress = FAT16[ClusterAddress];
				k++;
			}//end while

			for (int p = 0; p < k; p++)
			{
				for (int n = 0; n < 4; n++)
				{
					dataout[p*4 + n] = data[ClusterLocations[p] * 4 + n];
					cout << dataout[p * 4 + n];
				}
			}
		
		}

	}

};//printData
