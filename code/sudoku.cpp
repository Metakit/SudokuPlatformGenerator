#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h> //srand((int)time(0));
#define LL long long
#define eps 10e-8
#define ArraySize(a) (sizeof( (a) ) / sizeof( (a[0]) ))
#define SwapInt(a, b) {int t = a; a = b; b = t;}
#define Random(x) (rand()%x)
using namespace std;
int stop = 0;

ofstream fout("sudoku.txt");

/*
 * 以下分别是存放一行待选随机数字的数组randomArray，数独盘面sudoku，
 * 标记一单元不可填入数字的数组removeArray，选择一行待选随机数字的
 * 下标fillingSelector。
 */
int randomArray[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
int sudoku[9][9] = {};
int removeArray[9] = {};
int fillingSelector = 0;

/*
 * 输入一个一维数组，打印该数组。
 */
void print_array(int* thatArray) {
	for (int i = 0; i < 9; ++i) {
		if (i == 8) fout << thatArray[i];
		else        fout << thatArray[i] << " ";
	}
}

/*
 * 打印数独盘面。
 */
void print_sudoku() {
	for (int i = 0; i < 9; ++i) {
		print_array(sudoku[i]);
		if (i != 8) fout << endl;
	}
}

/*
 * 输入行数，初始化该行。
 */
void delete_row(int y) {
	for (int i = 0; i < 9; ++i) {
		sudoku[y][i] = 0;
	}
}

/*
 * 使randomArray变为一个随机的、包含不重复1~9的数组。
 */
void generate_random_array() {
	for (int i = 0; i < 9; ++i) {
		randomArray[i] = i + 1;
	}
	for (int i = 0; i < 30; ++i) {
		//SwapInt(randomArray[0], randomArray[Random(9)]) is wrong.
		int sub = Random(9);
		int tmp = randomArray[0];
		randomArray[0] = randomArray[sub];
		randomArray[sub] = tmp;
	}
}

/*
 * 使randomArray变为一个随机的、包含不重复1~9的数组，但
 * randomArray[0]固定为1。
 */
void generate_random_array_special() {
	generate_random_array();
	for (unsigned i = 0; i < 9; ++i) {
		if (randomArray[i] == 1) {
			SwapInt(randomArray[i], randomArray[0]);
			break;
		}
	}
}

/*
 * 输入行、列位置，以行为依据判断不能填入该单元的数字，判断结果
 * 将改变removeArray。
 */
void remove_impossible_row(int y, int x) {
	for (int i = 0; i < x; ++i) {
		removeArray[sudoku[y][i] - 1] = 1;
	}
}

/*
 * 输入行、列位置，以列为依据判断不能填入该单元的数字，判断结果
 * 将改变removeArray。
 */
void remove_impossible_column(int y, int x) {
	for (int i = 0; i < y; ++i) {
		removeArray[sudoku[i][x] - 1] = 1;
	}
}

/*
 * 输入行、列位置，以宫为依据判断不能填入该单元的数字，判断结果
 * 将改变removeArray。
 */
void remove_impossible_block(int y, int x) {
	for (int i = y / 3 * 3; i < y / 3 * 3 + 3; ++i) {
		for (int j = x / 3 * 3; j < x / 3 * 3 + 3; ++j) {
			if (sudoku[i][j] != 0) {
				removeArray[sudoku[i][j] - 1] = 1;
			}
		}
	}
}

/*
 * 输入行、列位置，判断不能填入该单元的数字，判断结果将改变
 * removeArray。
 */
void remove_impossible_unit(int y, int x) {
	remove_impossible_row(y, x);
	remove_impossible_column(y, x);
	remove_impossible_block(y, x);
}

/*
 * 输入行、列位置，尝试向该单元填入数字。填入成功返回true，否则
 * 返回false。
 */
bool filling_unit(int y, int x) {
	/*
	 * 每次填入使用一次removeArray，所以需要初始化。排除完毕后根据
	 * removeArray顺序拿取randomArray中的数字，找到合适的数字，填
	 * 入成功；找不到，填入失败。
	 */
	memset(removeArray, 0, sizeof(removeArray));
	remove_impossible_unit(y, x);
	for (int i = 0; i < 9; ++i) {
		if (randomArray[i] != 0) {
			if (removeArray[randomArray[i] - 1] == 0) {
				sudoku[y][x] = randomArray[i];
				randomArray[i] = 0;
				return true;
			}
		}
	}
	return false;
}

/*
 * 填充数独第一行，需要randomArray存放合适的随机数组。
 */
void filling_first_row() {
	for (unsigned i = 0; i < 9; ++i) {
		sudoku[0][i] = randomArray[i];
	}
}

/*
 * 填充数独盘面。
 */
void filling_sudoku() {
	int rowDeathCount = 0;
	for (int i = 0; i < 9; ++i) {
		if (i == 0) {
			/*
			 * 第一行直接填入行首为1的合法随机数组。
			 */
			generate_random_array_special();
			filling_first_row();
		}
		else {
			/*
			 * 生成合法随机数组，尝试填入行。填入行即为分别填入行内9
			 * 个单元，其中一个单元填入失败，填入行失败；否则成功。如
			 * 果填入行失败，重试。
			 * 在一个数独盘面内，如果填入行失败超过100次，重新执行该函
			 * 数。
			 */
			generate_random_array();
			for (int j = 0; j < 9; ++j) {
				if (!filling_unit(i, j)) {
					delete_row(i);
					j = -1;
					generate_random_array();
					rowDeathCount++;
				}
				if (rowDeathCount == 100) {
					memset(sudoku, 0, sizeof(sudoku));
					rowDeathCount = 0;
					i = -1;
					break;
				}
			}
		}
	}
}

/*
 * 输入一个数字generateSudokuNum，随机生成generateSudokuNum个
 * 数独盘面。
 */
void generate_random_sudoku_multi(int generateSudokuNum) {
	for (int i = 0; i < generateSudokuNum; ++i) {
		filling_sudoku();
		print_sudoku();
		fout << endl;
		if (i != generateSudokuNum - 1) {
			fout << endl;
		}
		memset(sudoku, 0, sizeof(sudoku));
	}
}

/*
 * 测试单元剔除的实例。
 */
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

/*
 * 测试随机数组randomArray生成的实例。
 */
void test_of_generater_random_array() {
	generate_random_array();
	print_array(randomArray);
	cout << endl;
}

/*
 * 测试数独终盘生成的实例。
 */
void test_of_no_back_fill() {
	for (int i = 0; i < 9; ++i) {
		generate_random_array();
		print_array(randomArray); cout << " - ";
		for (int j = 0; j < 9; ++j) {
			filling_unit(i, j);
		}
		print_array(sudoku[i]); cout << endl;
	}
}

/*
 * 测试大量数独终盘生成的实例。
 */
void test_of_true_sudoku_output() {
	for (int i = 0; i < 1000; ++i) {
		cout << i + 1 << ":" << endl;
		filling_sudoku();
		print_sudoku();
		cout << endl;
		memset(sudoku, 0, sizeof(sudoku));
	}
}

/*
 * 输入一个字符串。如果是纯数字串，输出对应的数字；否则输出-1。
 */
int trans_argv_into_number(char* str) {
	int ans = 0;
	int strSize = strlen(str);
	for (int i = 0; i < strSize; ++i) {
		if ('0' <= str[i] && str[i] <= '9') {
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

	/*
	 * 参数处理。默认情况输出1个终盘，使用"-c n"指定生成n个终盘。
	 */
	if (argc == 1) {
		generate_random_sudoku_multi(1);
	}
	else if (argc == 3) {
		int generateSudokuNum = trans_argv_into_number(argv[2]);
		if (!strcmp(argv[1], "-c") && generateSudokuNum != -1) {
			generate_random_sudoku_multi(generateSudokuNum);
		}
		else {
			cout << "parameter is illegal" << endl;
		}
	}
	else {
		cout << "parameter is illegal" << endl;
	}

	//cin >> stop;
	return 0;
}