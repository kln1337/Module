#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;
#define sizeArr 100 // Array size for testing


int halMemDiffSnapshot(const int* srcAddr, const int* snapshotA, const int* snapshotB, int sizeSnapshot, int compression);
int halMemDiffSnapshot(int* const  srcAddr, const int* snapshotA, const int* snapshotB, int sizeSnapshot, int compression,int ** diffAddr);
int  halMemMakeSnapshot(const int* srcAddr, int srcSize, int compression, int* snapshot);
void printArr(int* arr, int size,string nameArr);
void printArr(int** arr, int size,string nameArr);
void printArr(const int* arr, int size, string nameArr);
int sumArr(const int* arr, int const start, int const end);
unsigned int CRC32_function(const int* buf, int start, int end);
void test (void);
int myRand(int max, int min);

int myTest() {
  
	int  snapshotA[8];
	int  snapshotB[8];
	int  arr[8];
	for (int i = 0; i < 8; i++) {
		arr[i] = i;
	}
	halMemMakeSnapshot(arr, 8, 1, snapshotA);
	arr[0] = 1;
	halMemMakeSnapshot(arr, 8, 1, snapshotB);
	halMemDiffSnapshot(arr, snapshotA, snapshotB, 8, 1);
	return 1;
}

int main() {
	myTest();
	/*int arr[sizeArr];
	int size = sizeArr;
	int compression = 3;
	int sizeSnapshot = 0;
	int snapshotA[sizeArr];
	int snapshotB[sizeArr];

	for (int i = 0; i < size; i++) arr[i] = i;
	printArr(arr, sizeArr,"Arr");
	sizeSnapshot = halMemMakeSnapshot(arr, size, compression, snapshotA);
	arr[0] = 1;
	arr[9] = 11;
	halMemMakeSnapshot(arr, size, compression, snapshotB);
	printArr(arr,sizeArr,"new Arr");
	printArr(snapshotA, sizeSnapshot+2,"snapA");
	printArr(snapshotB, sizeSnapshot+1,"snapB");
	int dif = halMemDiffSnapshot(arr, snapshotA, snapshotB, sizeSnapshot, compression);*/
	test();
	
	return 0;
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
int halMemDiffSnapshot(const int* srcAddr, const int* snapshotA, const int* snapshotB, int sizeSnapshot, int compression)
{
	int diff = 0;
	int *addr = new int[sizeSnapshot];
	int b = 0;
	int end = 0;
	int realDiff= 0;
	int i = 0;
	for ( i = 0; i < sizeSnapshot; i++) {
		if (snapshotA[i] != snapshotB[i]) {
			addr[diff] = i;
			diff++;
			if (i != sizeSnapshot - 1) {
				realDiff += compression;
			}
			if (i == sizeSnapshot - 1) {
				realDiff += snapshotA[sizeSnapshot];
			}
			/*if (i == sizeSnapshot - 2 && (snapshotA[sizeSnapshot+2] == 1)) {
				realDiff += snapshotA[sizeSnapshot-1];
			}*/
		}
	}
	
	
	if (diff == 0) {
		goto end;
	}
	cout <<"start area of diff memory: "<< srcAddr + addr[0] * compression << endl;
	for (int i = 1; i < diff - 1; i++) {
		If ( (Addr[i] == (addr[i-1] + 1)) && (addr[i] + 1) != addr[i + 1]) {

			cout << "end area of diff memory: " << srcAddr + addr[i] * compression + compression  << endl;
		}
		if ((addr[i] + 1 == addr[i + 1]) && (addr[i] != (addr[i - 1] + 1))) {
			cout << "start area of diff memory: " << srcAddr + addr[i] * compression << endl;
		}
	}
	cout << "end area of diff memory: " << srcAddr + addr[diff-1] * compression + compression-1 << endl;
	cout << "number of diff: " << realDiff << endl;;
end:
	delete[] addr; 
	return realDiff;
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
void test(void) {

		int iteration = 100;
		int srcAddrLast[sizeArr] ;
		int srcAddrNew[sizeArr];
		int snapshotA[sizeArr];
		int snapshotB[sizeArr];
		int* diffAddress[sizeArr];
		int* diffAddressSnap[sizeArr];
		int realDiff = 0;
		int compression = 5;
		int notFound=0;
		int extraSnapAddr=0;
		int allSnap = 0;
		for (int k = 0; k < iteration; k++) {
			
			int dif = myRand(sizeArr,0);

			for (int i = 0; i < sizeArr; i++) { // array fillng (max elemant 1000)
				srcAddrLast[i] = myRand(1000,0);
				srcAddrNew[i] = srcAddrLast[i];
			}

			int SizeSnapshotA = halMemMakeSnapshot(srcAddrLast, sizeArr, compression, snapshotA);

			for (int i = 0; i < dif; i++) {  // change array
				srcAddrNew[myRand(sizeArr - 1, 0)] = myRand(1000, 0);
			}
			
			for (int i = 0; i < sizeArr; i++) {  //find real difference
				if (srcAddrLast[i] != srcAddrNew[i]) {
					diffAddress[realDiff] = srcAddrNew + i;
					realDiff++;
				}
			}
			halMemMakeSnapshot(srcAddrNew, sizeArr, compression, snapshotB);
			int snapDiff = halMemDiffSnapshot(srcAddrNew, snapshotA, snapshotB, SizeSnapshotA, compression,diffAddressSnap);
			int found=0; 

			for (int i = 0; i < realDiff; ++i) {
				for (int j = 0; j < snapDiff; j++)
				{
					if (diffAddress[i] == diffAddressSnap[j]) {
						found++;
						break;
					}
				}
			}
			notFound = realDiff - found + notFound ;  
			extraSnapAddr = snapDiff - realDiff + extraSnapAddr;
			allSnap += snapDiff;
			/*cout << "real Difference: " << realDiff << endl;
			cout << "______________________________________" << endl;
			cout << "\n\n";*/
			realDiff=0;
		}
		double result = 100 - ((double)notFound / (sizeArr * iteration));
		cout << fixed;
		cout.precision(1);
		cout << "We analyzed 100 arrays of 100 elements with a compression ratio of 5" << endl;
		cout << "Percentage of changes found in memory: " << result <<"%" << endl;
		cout << "But percentage of extra addresses: " << (double)extraSnapAddr / allSnap * 100 << "%" << endl;
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
int halMemDiffSnapshot(int* const srcAddr, const int* snapshotA, const int* snapshotB, int sizeSnapshot, int compression, int** diffAddr)
{
	int diff = 0;
	int* addr = new int[sizeSnapshot];
	int b = 0;
	int end = 0;
	int realDiff = 0;
	int k=0;
	int i=0;

	for (int i = 0; i < sizeSnapshot; i++) {
		if (snapshotA[i] != snapshotB[i]) {
			addr[diff] = i;
			diff++;

			if (i != sizeSnapshot) {
				realDiff += compression;
				for (int j = 0; j < compression; j++) {
					diffAddr[k] = srcAddr + j + (i * compression);
					++k;
				}
			}
			if (i == sizeSnapshot - 1) {
				for (int j = 0; j < snapshotA[sizeSnapshot]; j++) {
					diffAddr[k] = srcAddr + j + (i * compression);
					++k;
				}
				realDiff += snapshotA[sizeSnapshot];
			}
		}
	}

	if (diff == 0) {
		goto end;
	}
	
	//diffAddr[0]= srcAddr + addr[0] * compression;
	//cout << "start area of diff memory: " << srcAddr + addr[0] * compression << endl;
	//for (int i = 1; i < diff - 1; i++) {
	//	if ((addr[i] == (addr[i - 1] + 1)) && (addr[i] + 1) != addr[i + 1]) {
	//		cout << "end area of diff memory: " << srcAddr + addr[i] * compression + compression << endl;
	//	}
	//	if ((addr[i] + 1 == addr[i + 1]) && (addr[i] != (addr[i - 1] + 1))) {
	//		cout << "start area of diff memory: " << srcAddr + addr[i] * compression << endl;
	//	}
	//}
	//cout << "end area of diff memory: " << srcAddr + addr[diff - 1] * compression + compression - 1 << endl;

	//cout << "number of snapDiff: " << realDiff << endl;
end:
	delete[] addr;
	return realDiff;
}
int  halMemMakeSnapshot(const int* srcAddr, int srcSize, int compression, int* snapshot) {
  
	int Sizesnapshot = srcSize/compression;
	Sizesnapshot = srcSize 
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
void call (){
  
