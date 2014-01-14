#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;
char findChar(int);

char findChar(int num){
	char a[] = {'A', 'B', 'C', 'D','E','F','G','H','I','J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q','R','S','T','U','V','W','X','Y','Z'};
	return a[num];
}
int main( int argc, char* argv[]){
	srand(time(0));


	ofstream file;
	file.open(argv[2]);
	int num = atoi( argv[1]);

	for(int i = 1; i <= num; i++){
		string s = "";
		for(int a = 0; a < 8; a++){
			int rand_num;
			rand_num = rand() % 26;
			s = s + findChar(rand_num);
		}
		file << s << " " << i << endl;
	}
	//file << "hello world";
	return 0;
	
}
