//data on 21.07.2020
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <ctime>
#include <fstream>
using namespace std;
#define sizeArr 100 // Array size for testing

int compare (int* Arr, int* changeArr, int size, int* snapshotA, int* snapshotB, int compression);
int halMemDiffSnapshot(const int* srcAddr, const int* snapshotA, const int* snapshotB, int sizeSnapshot, int compression);
int halMemDiffSnapshot(int* const  srcAddr, const int* snapshotA, const int* snapshotB, int sizeSnapshot, int compression,int ** diffAddr, int& sizeDiffAddr);
int halMemDiffSnapshotNew(const int* srcAddr, const int* snapshotA, const int* snapshotB, int sizeArray, int compression);
int halMemMakeSnapshot(const int* srcAddr, int srcSize, int compression, int* snapshot);
int halMemMakeSnapshotNew(const int* srcAddr, int srcSize, int compression, int* snapshot);
void printArr(int* arr, int size,string nameArr);
void printArr(int** arr, int size,string nameArr);
void printArr(const int* arr, int size, string nameArr);
int sumArr(const int* arr, int const start, int const end);
unsigned int CRC32_function(const int* buf, int start, int end);
void test(int compression, int iteration);

int myRand(int max, int min);
int myRand1 (int max , int min);
void scan (int* arr, int size);
void newTest (void);
int fileScan (int* arr, int nOfString, ifstream& file);

int main() {
    test(10,1000);
    //newTest();
    //string filename = "data.txt";
    //ifstream file;
    //file.open (filename.c_str(), ifstream::in);
    //fileScan(arr, 2, file);
    //fileScan(arr, 2, file);
    // fileScan(arr, 2, file);
}
int  halMemMakeSnapshot(const int* srcAddr, int srcSize, int compression, int* snapshot) 
{

    int Sizesnapshot = srcSize/compression;

    int i = 0;
    int j = 0;
    while ((i+compression) <= srcSize) {
	snapshot[j] = CRC32_function(srcAddr, i, i + compression);
	i = i + compression;
	j++;
    }
    if (srcSize % compression != 0) {
	snapshot[Sizesnapshot] = CRC32_function(srcAddr,srcSize - (srcSize % compression), srcSize);
	++Sizesnapshot;
    }
    snapshot[Sizesnapshot] = srcSize % compression; 

    return Sizesnapshot;
}
void printArr(int* arr, int size, string nameArr)
{
    cout << nameArr << endl;
    for (int i = 0; i < size; i++) {
	cout <<"arr[" << i << "]" <<" = " <<arr[i] << "    " << arr + i;
	cout << endl;
    }
    cout << endl;
}
void printArr(int** arr, int size, string nameArr)
{
    cout << nameArr << endl;
    for (int i = 0; i < size; i++) {
	cout << "arr[" << i << "]" << " = " << arr[i];
	cout << endl;
    }
    cout << endl;
}
void printArr(const int*arr, int size, string nameArr)
{
    cout << nameArr << endl;
    for (int i = 0; i < size; i++) {
	cout << "arr[" << i << "]" << " = " << arr[i];
	cout << endl;
    }
    cout << endl;
}
int sumArr(const int* arr,int const start, int const end) {
    int sum = 0;

    for (int i = start; i < end; i++) {
	sum += arr[i];
    }

    return sum;
}
unsigned int CRC32_function(const int* buf, int start, int end)
{
    unsigned long crc_table[256];
    unsigned long crc;
    for (int i = 0; i < 256; i++)
	{
	    crc = i;
	    for (int j = 0; j < 8; j++)
		crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
	    crc_table[i] = crc;
	};
    crc = 0xFFFFFFFFUL;
    while (start < end) {
	crc = crc_table[(crc ^ buf[start]) & 0xFF] ^ (crc >> 8);
	start++;
    }
    return crc ^ 0xFFFFFFFFUL;
}
void test(int compression, int iteration) {

    //int iteration = 100;
    int srcAddrLast[sizeArr] ;
    int srcAddrNew[sizeArr];
    int snapshotA[sizeArr];
    int snapshotB[sizeArr];
    int* diffAddress[sizeArr];
    int* diffAddressSnap[sizeArr];
    int sizeDiffAddr = 0;
    int realDiff = 0;
    // int compression = 3;
    int notFound = 0;
    int extraSnapAddr = 0;
    int allSnap = 0;
    bool find = 1;

    for (int k = 0; k < iteration; k++) {
			
	int dif = myRand1(sizeArr,0);
	for (int i = 0; i < sizeArr; i++) { // array fillng (max elemant 1000)
	    srcAddrLast[i] = myRand1(1000,0);
	    srcAddrNew[i] = srcAddrLast[i];
	}
	int Sizesnapshot = halMemMakeSnapshotNew(srcAddrLast, sizeArr, compression, snapshotA);
	// change array
	for (int i = 0; i < dif; i++) {  
	    srcAddrNew[myRand1(sizeArr - 1, 0)] = myRand1(1000, 0);
	}
	//find real difference
	for (int i = 0; i < sizeArr; i++) {  
	    if (srcAddrLast[i] != srcAddrNew[i]) {
		diffAddress[realDiff] = srcAddrNew + i;
		realDiff++;
	    }
	}
	halMemMakeSnapshotNew(srcAddrNew, sizeArr, compression, snapshotB);
	int snapDiff = halMemDiffSnapshot(srcAddrNew, snapshotA, snapshotB, sizeArr, compression,diffAddressSnap, sizeDiffAddr);
	int found = 0; 
	for (int i = 0; i < realDiff; ++i) {
	    if (find == 0) {
		printArr (srcAddrLast, sizeArr, "Old Array");
		printArr(srcAddrNew, sizeArr, "New Array");
		printArr(snapshotA, Sizesnapshot, "SnapshotA");
		printArr(snapshotB, Sizesnapshot, "SnapshotB");
		cout << "No difference was found in the array NewArray. In an element equal to "<< *diffAddress[i-1] << endl << endl;
		
	    }
	    find = 0;
	    for (int j = 0; j < sizeDiffAddr; j += 2)
		{
		    if ((diffAddressSnap[j] <= diffAddress[i]) && (diffAddress[i] <= diffAddressSnap[j + 1])) {
			found++;
			find = 1;
			break;
		    }
		}
	}
	if (find == 0) {
	    printArr (srcAddrLast, sizeArr, "Old Array");
	    printArr(srcAddrNew, sizeArr, "New Array");
	    printArr(snapshotA, Sizesnapshot, "SnapshotA");
	    printArr(snapshotB, Sizesnapshot, "SnapshotB");
	    cout << "No difference was found in the array NewArray. In an element equal "<< *diffAddress[realDiff-1] << endl << endl;
	}
	notFound = realDiff - found + notFound ;  
	realDiff = 0;
    }
    double result = 100 - ((double)notFound / (sizeArr * iteration))*100;
    cout << fixed;
    cout.precision(5);
    cout << "We analyzed " << iteration <<" arrays of "<< sizeArr << " elements with a compression ratio of " << compression << endl;
    cout << "Percentage of changes found in memory: " <<  result <<"%" << endl;
    if (result > 85) {
	cout << "This compression method can be applied in practice (With confidence greater than 85%) " <<"\n\n";
    }
    else
	{
	    cout << "This compression method can't be applied in practice (With confidence greater than 85%) " << "\n\n";
	}
}
int myRand (int max, int min) {

    return rand() % (max - min + 1) + min;

}
int halMemDiffSnapshot(int* const srcAddr, const int* snapshotA, const int* snapshotB, int sizeArray, int compression, int** diffAddr, int& sizeDiffAddr)
{
    int sizeSnapshot = (sizeArray - 1) / compression + 1;
    bool b = 0;
    bool out = 0;
    int lastElement = sizeArray % compression;
    int end = 0;
    int Diff = 0;
    int i = 0;
    int k =0;

    for (i = 0; i < sizeSnapshot; i++) {
	
	if (snapshotA[i] != snapshotB[i]) {
	    if (i != sizeSnapshot - 1) {
		Diff += compression;
	    }
	    if (i == sizeSnapshot - 1 && lastElement != 0) {
		Diff += lastElement;
	    }
	    if (i == sizeSnapshot - 1 && lastElement == 0) {
		Diff += compression;
	    }
	    if (b != 0 && end + 1 != i) {
		if (end == sizeSnapshot - 1) {
		    cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + lastElement - 1 << endl;
		    diffAddr[k] = srcAddr + end * compression + lastElement - 1;
		    k++;
		}
		else {
		    cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + compression - 1 << endl;
		    diffAddr[k] = srcAddr + end * compression + compression - 1;
		    k++;
		}
		b = 0;
		out = 1;
	    }
	    if (end + 1 == i) {
		end++;
	    }
	    if (b == 0) {
		b = 1;
		cout << "start area of diff memory: " << "\t" << srcAddr + i * compression << endl;
		diffAddr[k] = srcAddr + i * compression;
		k++;
		end = i;
		out = 0;
	    }
	}
    }
    if (out == 0) {
	if (end == sizeSnapshot - 1) {
	    if (lastElement != 0) {
		cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + lastElement - 1 << endl;
		diffAddr[k] = srcAddr + end * compression + lastElement - 1;
		k++;
	    }
	    else {
		cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + compression - 1 << endl;
		diffAddr [k] = srcAddr + end * compression + compression - 1;
		k++;
	    }
	}
	else {
	    cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + compression - 1 << endl;
	    diffAddr [k] = srcAddr + end * compression + compression - 1;
	    k++;
	}
    }
    sizeDiffAddr = k;

    return Diff;
}
int halMemMakeSnapshotNew(const int* srcAddr, int srcSize, int compression, int* snapshot)
{
    int Sizesnapshot = srcSize / compression;
    int i = 0;
    int j = 0;
    if (compression != 1){
	while (j < Sizesnapshot) {
	    snapshot[j] = CRC32_function(srcAddr, i, i + compression);
	    i = i + compression;
	    j++;
	}
	if (srcSize % compression != 0) {
	    snapshot[Sizesnapshot] = CRC32_function(srcAddr, srcSize - (srcSize % compression), srcSize);
	    Sizesnapshot++;
	}
    } else {
	while (j < Sizesnapshot) {
	    snapshot[j] = srcAddr[j];
	    j++;
	}
    }
    return Sizesnapshot;
}
int halMemDiffSnapshotNew(const int* srcAddr, const int* snapshotA, const int* snapshotB, int sizeArray, int compression)
{
    int sizeSnapshot = (sizeArray  - 1) / compression + 1;
    bool b = 0;
    bool out =0;
    int lastElement = sizeArray % compression;
    int end = 0;
    int Diff = 0;
    int i = 0;
	
    for (i = 0; i < sizeSnapshot; i++) {
	if (snapshotA[i] != snapshotB[i]) {
	    if (i != sizeSnapshot - 1) {
		Diff += compression;
	    }
	    if (i == sizeSnapshot - 1 && lastElement != 0) {
		Diff += lastElement ;
	    }
	    if (i == sizeSnapshot - 1 && lastElement == 0) {
		Diff += compression;
	    }
	    if (b != 0 && end + 1 != i) {
		if (end == sizeSnapshot - 1) {
		    cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + lastElement - 1 << endl;
		}
		else {
		    cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + compression - 1 << endl;
		}
		b = 0;
		out = 1;
	    }
	    if (end + 1 == i) {
		end++;
	    }
	    if (b == 0) {
		b = 1;
		cout <<"start area of diff memory: " << "\t" <<  srcAddr + i * compression << endl ;
		end = i;
		out = 0;
	    }
	}
    }
    if (out == 0) {
	if (end == sizeSnapshot - 1) {
	    if (lastElement != 0) {
		cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + lastElement - 1 << endl;
	    }
	    else {
		cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + compression - 1 << endl;
	    }
	} else {
	    cout << "end area of diff memory: " << "\t" << srcAddr + end * compression + compression - 1 << endl;
	}
    }
    return Diff;
}
int myRand1(int max, int min)
{	
    return rand() % (max - min + 1) + min;
}
int compare (int* Arr, int* changeArr, int size, int* snapshotA, int* snapshotB, int compression)    
{
    int sizeSnapshotA = halMemMakeSnapshotNew (Arr, size, compression, snapshotA);
    int sizeSnapshotB = halMemMakeSnapshotNew (changeArr, size, compression, snapshotB);
    int** diffAddress = new int* [size];
    int** diffAddressSnap = new int* [size];
    int realDiff = 0;
    int sizeDiffAddr = 0;
    bool find = 1;
    int found = 0;
    int diffSnap = 0;
    
    for (int i = 0; i < size; i++) {  
	if (Arr[i] != changeArr[i]) {
	    diffAddress[realDiff] = changeArr + i;
	    realDiff++;
	}
    }
    diffSnap =  halMemDiffSnapshot(changeArr, snapshotA, snapshotB, size, compression, diffAddressSnap, sizeDiffAddr);
    for (int i = 0; i < realDiff; ++i) {
	if (find == 0) {
	    printArr (Arr, size, "Old Array");
	    printArr(changeArr, size, "New Array");
	    cout << "diff wasn't found in New Array element =  "<< *diffAddress[i-1] << endl;
	}
	find = 0;
	for (int j = 0; j < sizeDiffAddr; j += 2){
	    if ((diffAddressSnap[j] <= diffAddress[i]) && (diffAddress[i] <= diffAddressSnap[j + 1])) {
		found++;
		find = 1;
		break;
	    }
	}
    }
    if (find == 0) {
	printArr (Arr, size, "Old Array");
	printArr(changeArr, size, "New Array");
	cout << "diff wasn't found in New Array element =  "<< *diffAddress[realDiff-1] << endl;
    }
    return found;
}
void scan (int* arr, int size) {
    int i = 0;
    cout << "scan start" << endl;
    while (cin >> arr[i] && size < i){
	i++;
	cout << "scan progressing" << endl;
    }
    cout << "end scan" << endl;
}
int fileScan (int* arr, int nOfString, ifstream& file) {

    const char* delimiter = " ";
    int i = 0;
    string data;
    static int numberOfString = 1;
    if( numberOfString <= nOfString){
	if(!file.is_open()) {
	    cout << "error, file don't open";
	    return 0;
	}
	getline (file, data);
	char cdata[data.length() + 1];
	strcpy (cdata,data.c_str());
	char* token = strtok(cdata, " ");
	while (token != NULL){
      	    arr[i] = (int) stod(token);
	    i++;
	    token = strtok (NULL, " "); 
	}
    }
    numberOfString++;
    return 1;
}
    
void newTest (void)
{
    int iteration = 1;
    int srcArrLast[sizeArr];
    int srcArrNew[sizeArr];
    int snapshotA[sizeArr];
    int snapshotB[sizeArr];
    int compression = 1;
    string filename = "data.txt";
    ifstream file;
    cout << "newTest" << endl;
    file.open (filename.c_str(), ifstream::in);
    fileScan (srcArrLast, iteration*2, file);
    printArr(srcArrLast, sizeArr, "Last Array");
    fileScan (srcArrNew, iteration*2, file);
    printArr(srcArrNew, sizeArr, "New Array");
    compare (srcArrLast, srcArrNew, sizeArr, snapshotA, snapshotB, compression);
    printArr(snapshotA, sizeArr, "snapshotA");
    printArr(snapshotB, sizeArr, "snapshotB");
}
