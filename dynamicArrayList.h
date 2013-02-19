//
// dynamicArrayList.h
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

#ifndef DYNAMIC_ARRAY_LIST_H
#define DYNAMIC_ARRAY_LIST_H

#include <stdexcept>
#include "list.h"

using namespace std;

namespace {

template <class T>
class dal {
public:
	virtual ~dal<T> () {}
	virtual ostream &output (ostream &os) const = 0;
	virtual const T getItem (int *index) const = 0;
	virtual void setItem (int *index, T value) = 0;
	virtual list<T> &sortQuickly () const { throw new logic_error (errMsg); }
	virtual list<T> &sortBubbly () const { throw new logic_error (errMsg); }
	
private:
	static const char *errMsg;
};

template <class T>
const char *dal<T>::errMsg = "Sorting is not supported by this instance.";

template <class T>
class dal1dImpl : public dal<T> {
public:
	dal1dImpl (int *dims) : dim (dims [0]) { iList = new T [dim]; }
	~dal1dImpl () { delete [] iList; }

	ostream &output (ostream &os) const {
		os << "List with length " << dim << endl << endl;
		for (T *i = iList; i < iList + dim; i++)
			os << *i << " ";
		return os;
	}
	
	const T getItem (int *index) const { return iList [index [0]]; }
	void setItem (int *index, T value) { iList [index [0]] = value; }
	list<T> &sortQuickly () const { throw; }
	list<T> &sortBubbly () const { throw; }
	
private:
	T *iList;
	const int dim;
};

template <class T>
class dal2dImpl : public dal<T> {
public:
	dal2dImpl (int *dims) : dims (dims) {
		iList = new T* [dims [0]];
		for (T **i = iList; i < iList + dims [0]; i++)
			*i = new T [dims [1]];
	}
	
	~dal2dImpl () {
		for (T **i = iList; i < iList + dims [0]; i++)
			delete [] *i;
		delete [] iList;
	}

	ostream &output (ostream &os) const {
		os << dims [0] << "x" << dims [1] << "-Matrix" << endl << endl;
		for (T **i = iList; i < iList + dims [0]; i++) {
			for (T *j = *i; j < *i + dims [1]; j++)
				os << *j << " ";
			os << endl;
		}
		return os;
	}
	
	const T getItem (int *index) const { return iList [index [0]][index [1]]; }
	void setItem (int *index, T value) { iList [index [0]][index[1]] = value; }
	
private:
	T **iList;
	const int * const dims;
};

template <class T>
class dal3dImpl : public dal<T> {
public:
	dal3dImpl (int *dims) : dims (dims) {
		iList = new T** [dims [0]];
		for (T ***i = iList; i < iList + dims [0]; i++) {
			*i = new T* [dims [1]];
			for (T **j = *i; j < *i + dims [1]; j++)
				*j = new T [dims [2]];
		}
	}
	
	~dal3dImpl () {
		for (T ***i = iList; i < iList + dims [0]; i++) {
			for (T **j = *i; j < *i + dims [1]; j++)
				delete [] *j;
			delete [] *i;
		}
		delete [] iList;
	}

	ostream &output (ostream &os) const {
		os << dims [0] << "x" << dims [1] << "x" << dims [2] << "-Matrix" << endl << endl;
		for (T ***i = iList; i < iList + dims [0]; i++) {
			for (T **j = *i; j < *i + dims [1]; j++) {
				for (T *k = *j; k < *j + dims [2]; k++)
					os << *k << " ";
				os << "| ";
			}
			os << endl << endl;
		}
		return os;
	}
	
	const T getItem (int *index) const {
		return iList [index [0]][index [1]][index[2]];
	}
	
	void setItem (int *index, T value) {
		iList [index [0]][index[1]][index[2]] = value;
	}
	
private:
	T ***iList;
	const int * const dims;
};

}

template <class T>
class dynamicArrayList : public list<T> {
public:
	dynamicArrayList<T> (int *dims) : list<T> (dims), iList (createDal (dims)) {}
	~dynamicArrayList<T> () { delete &iList; }
	
	friend ostream &operator<< (ostream &os, const dynamicArrayList<T> &obj) {
		return obj.iList.output (os);
	}
	
	const T getItem (int *index) const { return iList.getItem (index); }
	void setItem (int *index, T value) { iList.setItem (index, value); }
	
private:
	list<T> &sortQuickly () const { return iList.sortQuickly (); }
	list<T> &sortBubbly () const { return iList.sortBubbly (); }
	
	dal<T> &createDal (int *dims) {
		dal<T> *list;
		int nDims = this->getNDims ();
		if (nDims == 1) list = new dal1dImpl<T> (dims);
		else if (nDims == 2) list = new dal2dImpl<T> (dims);
		else list = new dal3dImpl<T> (dims);
		return *list;
	}
	
	dal<T> &iList;
};

/*

	class dalIterator {
	public:
		dalIterator () : curtPos (0), curl1Pos (0), curl2Pos (0),
			tDim (0), l1Dim (0), l2Dim (0), list (0),
			tPtr (0), l1Ptr (0), l2Ptr (0), numOfDims (0) {}
		
		dalIterator (dynamicArrayList<T> &list, bool end = false)
			: curtPos (0), curl1Pos (0), curl2Pos (0),
			numOfDims (list.getNumOfDimensions ()), list (list) {
			const int *dims = list.getDimensions ();
			
			if (numOfDims == 3) l2Ptr = (T ***)list.iList;
			else if (numOfDims == 2) l1Ptr = (T **)list.iList;
			else tPtr = (T *)list.iList;
			
			l2Dim = dims [2];
			l1Dim = dims [1];
			tDim = dims [0];
			
			if (end) {
				curl2Pos = l2Dim - 1;
				curl1Pos = l1Dim - 1;
				curtPos = tDim - 1;
			}
		}
		
		dalIterator (const dalIterator &it) : list (it.list), numOfDims (it.numOfDims),
			curtPos (it.curtPos), curl1Pos (it.curl1Pos), curl2Pos (it.curl2Pos),
			tDim (it.tDim), l1Dim (it.l1Dim), l2Dim (it.l2Dim),
			tPtr (it.tPtr), l1Ptr (it.l1Ptr), l2Ptr (it.l2Ptr) {}
		
		dalIterator& operator= (const dalIterator &other) {
        	return dalIterator (other);
        }
		
		bool operator== (const dalIterator &other) const {
			return list.iList == other.list.iList &&
			       curl2Pos == other.curl2Pos &&
			       curl1Pos == other.curl1Pos &&
			       curtPos == other.curtPos;
		}
		
		bool operator!= (const dalIterator &other) const {
			return !operator== (other);
		}
		
		dalIterator& operator++ () {
			++curtPos;
			if (numOfDims >= 2 && curtPos == tDim) {
				curtPos = 0;
				++curl1Pos;
			}
			if (numOfDims >= 3 && curl1Pos == l1Dim) {
				curl1Pos = 0;
				++curl2Pos;
			}
			return *this;
		}
		
		dalIterator operator++ (int) {
			dalIterator tmp (*this);
			operator++ ();
			return tmp;
		}
		
		dalIterator& operator-- () {
			--curtPos;
			if (numOfDims >= 2 && curtPos == -1) {
				curtPos = tDim - 1;
				--curl1Pos;
			}
			if (numOfDims >= 3 && curl1Pos == -1) {
				curl1Pos = l1Dim - 1;
				--curl2Pos;
			}
			return *this;
		}
		
		dalIterator operator-- (int) {
			dalIterator tmp (*this);
			operator-- ();
			return tmp;
		}
		
		T& operator*() const {
			if (numOfDims == 3) return l2Ptr [curl2Pos][curl1Pos][curtPos];
			else if (numOfDims == 2) return l1Ptr [curl1Pos][curtPos];
			else return tPtr [curtPos];
		}
		
		T* operator->() const { return &operator* (); }
		
	private:
		dynamicArrayList &list;
		int numOfDims;
		int curtPos;
		int curl1Pos;
		int curl2Pos;
		int tDim;
		int l1Dim;
		int l2Dim;
		T *tPtr;
		T **l1Ptr;
		T ***l2Ptr;
	};

*/

#endif
