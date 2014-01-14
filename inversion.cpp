/* Count the number of inversions 
 * Author: Ke Er (Brenda) Xiong
 * Using divide and conquer approach to count inversions
 */

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

long count_split(int, int, int, int);

static const int COUNT = 100000;
int array[COUNT];
int helper[COUNT];

void readin()           // reading the file
{
	ifstream fs;
	fs.open("IntegerArray.txt");
	int n = 0;
	for (int i = 0; i < COUNT; i++)
	{ 
		fs >> n;
		array[i] = n;
	}
}

void print_array()       // print array for testing purposes
{
	for (int i = 0; i < COUNT; i++)
	{
		cout << array[i] << endl;
	}
}

long count_inv(int start, int end)
{
	if (end - start < 1) return 0;
	else 
	{
		int mid = (start + end)/2;
		return count_inv(start, mid)                              //divide
        + count_inv(mid + 1, end)                                 //divide
        + count_split(start, mid, mid + 1, end);                  //merge
	}
}

//This fuction count the number of inversions between two sub-arrays
long count_split(int start1, int end1, int start2, int end2)      //helper
{
	int init1 = start1;
	int init2 = start2;
	int current = start1;
	long result = 0;
	while (current <= end2)
	{
		if (init2 > end2 || (init1 <= end1 && array[init1] <= array[init2]))
		{ 
			helper[current] = array[init1];
			init1++;
			current++;
		}
		else 
		{
			helper[current] = array[init2];
			result = result + (end1-init1 + 1);
			init2++;
			current++;
		}
	}
	for (start1; start1 <= end2; start1++)
	{
		array[start1] = helper[start1];
	}
	return result;
}

	
		

int main() 
{
	readin();
	cout << count_inv(0, COUNT-1) << endl;
	return 0;
}
