#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <malloc.h>
#include <time.h> //srand((int)time(0));
#define LL long long
#define eps 10e-8
#define ArraySize(a) (sizeof( (a) ) / sizeof( (a[0]) ))
#define SwapInt(a, b) {int t = a; a = b; b = t;}
#define Random(x) (rand()%x)
using namespace std;

ofstream fout ("sudoku.txt");

int randomArray[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
int sudoku[9][9] = {};
int removeArray[9] = {};
int fillingSelector = 0;

void print_array(int* thatArray) {
	for(int i = 0; i < 9; ++i) {
		if (i == 8) fout << thatArray[i];
		else        fout << thatArray[i] << " ";
	}
}

void print_sudoku() {
	for(int i = 0; i < 9; ++i) {
		print_array(sudoku[i]);
		if (i != 8) fout << endl;
	}
}

void delete_row(int y) {
	for(int i = 0; i < 9; ++i) {
		sudoku[y][i] = 0;
	}
}

void generate_random_array() {
	for(int i = 0; i < 9; ++i) {
		randomArray[i] = i+1;
	}
	for(int i = 0; i < 30; ++i) {
		//SwapInt(randomArray[0], randomArray[Random(9)]) is wrong.
		int sub = Random(9);
		int tmp = randomArray[0];
		randomArray[0] = randomArray[sub];
		randomArray[sub] = tmp;
	}
}

void generate_random_array_special() {
	generate_random_array();
	for(unsigned i = 0; i < 9; ++i) {
		if(randomArray[i] == 1) {
			SwapInt(randomArray[i], randomArray[0]);
			break;
		}
	}
}

void remove_impossible_row(int y, int x) {
	for(int i = 0; i < x; ++i) {
		removeArray[ sudoku[y][i]-1 ] = 1;
	}
}

void remove_impossible_column(int y, int x) {
	for(int i = 0; i < y; ++i) {
		removeArray[ sudoku[i][x]-1 ] = 1;
	}
}

void remove_impossible_block(int y, int x) {
	for(int i = y/3*3; i < y/3*3 + 3; ++i) {
		for(int j = x/3*3; j < x/3*3 + 3; ++j) {
			if (sudoku[i][j] != 0) {
				removeArray[ sudoku[i][j]-1 ] = 1;
			}
		}
	}
}

void remove_impossible_unit(int y, int x) {
	remove_impossible_row(y, x);
	remove_impossible_column(y, x);
	remove_impossible_block(y, x);
}

bool filling_unit(int y, int x) {
	memset(removeArray, 0, sizeof(removeArray));
	remove_impossible_unit(y, x);
	for(int i = 0; i < 9; ++i) {
		if(randomArray[i] != 0) {
			if(removeArray[ randomArray[i]-1 ] == 0) {
				sudoku[y][x] = randomArray[i];
				randomArray[i] = 0;
				return true;
			}
		}
	}
	return false;
}

void filling_first_row() {
	for(unsigned i = 0; i < 9; ++i) {
		sudoku[0][i] = randomArray[i];
	}
}

void filling_sudoku() {
	int rowDeathCount = 0;
	for(int i = 0; i < 9; ++i) {
		generate_random_array_special();
		if(i == 0) {
			filling_first_row();
		}
		else {
			for(int j = 0; j < 9; ++j) {
				if( !filling_unit(i, j) ) {
					delete_row(i);
					j = -1;
					generate_random_array();
					rowDeathCount++;
				}
				if(rowDeathCount == 100) {
					memset(sudoku, 0, sizeof(sudoku));
					rowDeathCount = 0;
					i = -1;
					break;
				}
			}
		}
	}
}

void test_of_remove_impossible() {
	sudoku[0][0] = 3;
	sudoku[0][1] = 6;
	sudoku[1][0] = 9;
	sudoku[1][1] = 1;
	remove_impossible_row(0, 2);
	print_array(removeArray);
	cout << endl;
	memset(removeArray, 0, sizeof(removeArray));
	remove_impossible_column(2, 0);
	print_array(removeArray);
	cout << endl;
	memset(removeArray, 0, sizeof(removeArray));
	remove_impossible_block(2, 2);
	print_array(removeArray);
	cout << endl;
}

void test_of_generater_random_array() {
	generate_random_array();
	print_array(randomArray);
	cout << endl;
}

void test_of_no_back_fill() {
	for(int i = 0; i < 9; ++i) {
		generate_random_array();
		print_array(randomArray);cout <<" - ";
		for(int j = 0; j < 9; ++j) {
			filling_unit(i, j);
		}
		print_array(sudoku[i]);cout << endl;
	}
}

void test_of_true_sudoku_output() {	
	for(int i = 0; i < 1000; ++i) {
		cout << i+1 << ":" << endl;
		filling_sudoku();
		print_sudoku();
		cout << endl;
		memset(sudoku, 0, sizeof(sudoku));
	}
}

int trans_argv_into_number(char* str) {
	int ans = 0;
	int strSize = strlen(str);
	for(int i = 0; i < strSize; ++i) {
		if('0' <= str[i] && str[i] <= '9') {
			ans = ans * 10 + str[i] - '0';
		}
		else {
			return -1;
		}
	}
	return ans;
}

int main(int argc, char **argv) {
	srand((int)time(0));
	
	int generateSudokuNum = trans_argv_into_number(argv[2]);
	if(argc != 3 || strcmp(argv[1], "-c") || generateSudokuNum == -1) {
		fout << "parameter is illegal" << endl; 
	}
	else {
		for(int i = 0; i < generateSudokuNum; ++i) {
			filling_sudoku();
			print_sudoku();
			fout << endl;
			if(i != generateSudokuNum - 1) {
				fout << endl;
			}
			memset(sudoku, 0, sizeof(sudoku));
		}	
	}
	
	//cin >> stop;
    return 0;  
} 
