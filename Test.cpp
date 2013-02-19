//
// List.cpp
//
// Author:
//       Antonius Riha <antoniusriha@gmail.com>
//
// Copyright (c) 2013 Antonius Riha
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <algorithm>
#include <iostream>
#include "DynamicArrayList.h"
#include "DoublyLinkedList.h"

using namespace std;

void DoSmthng (int *array, int length) {
	for (int i = 0; i < length; i++)
		cout << array [i] << endl;
	cout << endl; 
}

const int nDims = 3;
int getItem (int *index, int curDim, void *list) {
	if (curDim >= nDims) return ((int *)list) [index [curDim - 1]];
	else return getItem (index, curDim + 1,
	                     ((int **)list) [index [curDim - 1]]);
}

template <class T>
void initVar (T *p) {
	*p = T ();
}

void newMethod () {
	int l2Dim = 5;
	int l1Dim = 2;
	int tDim = 4;
	int ***list = new int** [l2Dim];
	for (int i = 0; i < l2Dim; i++) {
		list [i] = new int* [l1Dim];
		for (int j = 0; j < l1Dim; j++) {
			list [i][j] = new int [tDim];
			for (int k = 0; k < tDim; k++)
				list [i][j][k] = i + j + k;
		}
	}
	
	for (int i = 0; i < l2Dim; i++) {
		for (int j = 0; j < l1Dim; j++) {
			for (int k = 0; k < tDim; k++)
				cout << list [i][j][k] << " ";
			cout << "  |  ";
		}
		cout << endl;
	}
	
	void *ilist = list;
	
	int ***l2Ptr = (int ***)ilist;
	int *lastNum = &l2Ptr [l2Dim - 1][l1Dim - 1][tDim - 1];
	cout << "LastNum: " << *lastNum << endl;
	
	for (int i = 0; i < l2Dim; i++) {
		for (int j = 0; j < l1Dim; j++) {
			delete l2Ptr [i][j];
		}
		delete l2Ptr [i];
	}
	delete l2Ptr;
	cout << endl;
}

template <class T>
void newMethod2 () {
	int *dims = new int [3];
	dims [0] = 7;
	dims [1] = 3;
	dims [2] = 3;
	DynamicArrayList<T> &list = *new DynamicArrayList<T> (dims);
	int nDims = list.getNDims ();
	
	int *idx = new int [3];
	for (int i = 0; i < dims [0]; i++) {
		idx [0] = i;
		for (int j = 0; j < dims [1]; j++) {
			idx [1] = j;
			for (int k = 0; k < dims [2]; k++) {
				idx [2] = k;
				if (nDims == 3) list.setItem (idx, i + j + k);
			}
			if (nDims == 2) list.setItem (idx, i + j);
		}
		if (nDims == 1) list.setItem (idx, i);
	}
	
	cout << list;
	
	cout << endl;
	
	for (int i = 0; i < list.getCount (); i++)
		cout << list.getItemByFlatIndex (i) << " ";
	
	cout << endl;
	
	//DynamicArrayList<int>::dalIterator it = list.begin ();
	//cout << *it << endl;
	//it++;
	//cout << *it << endl;
	
	//DynamicArrayList<int>::dalIterator it2 = list.end ();
	//cout << *it2 << endl;
	//--it2;
	//cout << *it2 << endl;
	
	//for (DynamicArrayList<int>::dalIterator it3 = list.begin (); it3 != list.end (); ++it3)
	//	cout << *it3 << " ";
	cout << endl;
	delete &list;
	delete [] dims;
	delete [] idx;
}

class A {
public:
	A () {
		num = new int [3];
		for (int i = 0; i < 3; i++)
			num [i] = i;
	}
	~A () { delete num; }
	const int *getSomeNums () const {
		return num;
	}
private:
	int *num;
};

void newMethod3 () {
	A *a = new A;
	const int *num = a->getSomeNums ();
	num += 1;
	for (int i = 0; i < 3; i++)
		cout << num [i] << endl;
	delete a;
}

template<class T>
T *createArray (int l) {
	T *t = new T [l];
}

void newMethod4 () {
	double *t = createArray<double> (5);
	for (int i = 0; i < 5; i++)
		cout << t [i] << endl;
	delete t;
}

void newMethod5 () {
	int *dims = new int [3];
	dims [0] = 23;
	dims [1] = 0;
	dims [2] = 9;
	DoublyLinkedList<int> &list = *new DoublyLinkedList<int> (dims);
	
	
	
	delete &list;
	delete [] dims;
}

void newMethod6 () {
	int c = 0;
	int ***l0 = new int** [5];
	for (int ***i = l0; i < l0 + 5; i++) {
		*i = new int* [5];
		for (int **j = *i; j < *i + 5; j++) {
			*j = new int [5];
			for (int *k = *j; k < *j + 5; k++)
				*k = c++;
		}
	}
	
	for (int ***i = l0; i < l0 + 5; i++) {
		for (int **j = *i; j < *i + 5; j++)
			delete [] *j;
		delete [] *i;
	}
	delete [] l0;
}

void newMethod7 () {
	int c = 0;
	int **k = new int* [5];
	for (int **l = k; l < k + 5; l++) {
		*l = new int [5];
		for (int *m = *l; m < *l + 5; m++)
			*m = c++;
	}
	
	for (int **l = k; l < k + 5; l++) {
		for (int *m = *l; m < *l + 5; m++)
			cout << *m << " ";
		cout << endl;
	}
	
	for (int **l = k; l < k + 5; l++)
		delete [] *l;
	delete [] k;
}

void newMethod8 () {
	int *index1 = new int [2];
	index1 [0] = 5;
	index1 [1] = 4;
	
	int *idx = new int [2];
	for (int *i = index1; i < index1 + 2; i++) {
		cout << *i << endl;
		*idx++ = *i;
	}
	
	idx -= 2;
	
	for (int *i = idx; i < idx + 2; i++)
		cout << *i << endl;
		
	delete [] index1;
	delete [] idx;
}

void newMethod9 () {
	int *index1 = new int [2];
	index1 [0] = 5;
	index1 [1] = 4;
	
	int *idx = new int [2];
	copy (index1, index1 + 2, idx);
	
	for (int *i = idx; i < idx + 2; i++)
		cout << *i << endl;
		
	delete [] index1;
	delete [] idx;
}

void newMethod10 () {
	bool fail = true;
	for (int i = 0; fail = i < 5; i++)
		if (i == 4) break;
	cout << "success: " << !fail << endl;
}

void newMethod11 () {
	int *index1 = new int [2];
	index1 [0] = 5;
	index1 [1] = 4;
	
	int *idx = new int [2];
	idx [0] = 500;
	idx [1] = -23;
	cout << "before:" << endl;
	for (int *i = idx; i < idx + 2; i++)
		cout << *i << endl;
	
	cout << endl;
	copy (index1, index1 + 2, idx);
	
	cout << "after:" << endl;
	for (int *i = idx; i < idx + 2; i++)
		cout << *i << endl;
		
	delete [] index1;
	delete [] idx;
}

int main () {

	newMethod11 ();
	//newMethod10 ();
	return 0;

	//newMethod9 ();
	//return 0;

	//newMethod8 ();
	//return 0;

	//newMethod7 ();
	//return 0;

	//newMethod6 ();
	//return 0;

	//newMethod ();
	newMethod2<double> ();
	//newMethod3 ();
	//newMethod4 ();

	return 0;

	int *ilist = new int [5];
	for (int i = 0; i < 5; i++)
		ilist [i] = i;
	void *anything = ilist;
	int *tPtr = (int *)anything;
	
	int *anotherPtr = &tPtr [4];
	cout << "another: " << *anotherPtr << endl;
	
	for (int i = 0; i < 5; i++)
		cout << tPtr [i] << endl;
	
	delete tPtr;
	return 0;

	int *p = new int;
	initVar<int> (p);
	cout << *p << endl;
	delete p;
	
	cout << endl;
	
	int *l = new int;
	cout << *l << endl;
	delete l;
	return 0;
	//int dims [] = { 1, 2, 0 };
	//DynamicArrayList<int> *list = new DynamicArrayList<int> (dims, false);
	
	//delete list;
	
	// ar
	cout << "ar (2d)" << endl;
	int maxI = 3, maxJ = 5;
	int **ar = new int* [maxI];
	for (int i = 0; i < maxI; i++) {
		ar [i] = new int [maxJ];
		for (int j = 0; j < maxJ; j++)
			ar [i][j] = i + j;
	}
	
	for (int i = 0; i < maxI; i++) {
		for (int j = 0; j < maxJ; j++)
			cout << ar [i][j] << " ";
		cout << endl;
	}
	
	for (int i = 0; i < maxI; i++)
		delete ar [i];
	delete ar;
	cout << endl;
	
	// arr
	cout << "arr (3d)" << endl;
	int maxK = 3;
	int ***arr = new int** [maxI];
	for (int i = 0; i < maxI; i++) {
		arr [i] = new int* [maxJ];
		for (int j = 0; j < maxJ; j++) {
			arr [i][j] = new int [maxK];
			for (int k = 0; k < maxK; k++)
				arr [i][j][k] = i + j + k;
		}
	}
	
	for (int i = 0; i < maxI; i++) {
		for (int j = 0; j < maxJ; j++) {
			for (int k = 0; k < maxK; k++)
				cout << arr [i][j][k] << " ";
			cout << "  |  ";
		}
		cout << endl;
	}
	
	for (int i = 0; i < maxI; i++) {
		for (int j = 0; j < maxJ; j++)
			delete arr [i][j];
		delete arr [i];
	}
	delete arr;
	cout << endl;
	
	
	cout << "Test method getItem" << endl;
		
	int ***myList = new int** [maxI];
	for (int i = 0; i < maxI; i++) {
		myList [i] = new int* [maxJ];
		for (int j = 0; j < maxJ; j++) {
			myList [i][j] = new int [maxK];
			for (int k = 0; k < maxK; k++)
				myList [i][j][k] = i + j + k;
		}
	}
	
	int *index = new int [3];
	index [0] = 0;
	index [1] = 1;
	index [2] = 2;
	int item = getItem (index, 1, myList);
	cout << "Item value: " << item;
	
	for (int i = 0; i < maxI; i++) {
		for (int j = 0; j < maxJ; j++)
			delete myList [i][j];
		delete myList [i];
	}
	delete myList;
	delete index;
	cout << endl;
	
	return 0;
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	int a [5] = { 1, 2, 3, 4, 5 };
	cout << a << endl;
	
	int *b = new int [5];
	for (int z = 0; z < 5; z++)
		b [z] = z * 2;
	
	for (int z = 0; z < 5; z++)
		cout << b [z] << endl;
	
	cout << b [2] << endl;
	delete b;
	
	cout << endl;
	int *y = new int (5);
	cout << "Pointer y (cout << y):                  " << y << endl;
	cout << "Value of var of pointer y (cout << *y): " << *y << endl;
	
	cout << endl;
	int x = 5;
	cout << "Value of x:                             " << x << endl;
	cout << "Address of x:                           " << &x << endl;
	
	cout << endl;
	int *w = &x;
	cout << "Pointer w (cout << w) (int *w = &x):    " << w << endl;
	
	cout << endl;
	int &v = *w;
	cout << "Ref to x (cout << v) (int &v = *w):     " << v << endl; 
	cout << "Addr of v (cout << &v):                 " << &v << endl;
	
	delete y;
	
	cout << endl;
	DoSmthng (a, 5);
	
	return 0;
}
