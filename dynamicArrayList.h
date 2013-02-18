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

template <class T>
class dynamicArrayList : public list<T> {
public:
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
	
	dynamicArrayList<T> (int *dims) : list<T> (dims) {
		int nDims = this->getNumOfDimensions ();
		
		if (nDims == 1) iList = new T [dims [0]];
		else if (nDims == 2) {
			T **list = new T* [dims [0]];
			for (T **i = list; i < list + dims [0]; i++)
				*i = new T [dims [1]];
			iList = list;
		} else if (nDims == 3) {
			T ***list = new T** [dims [0]];
			for (T ***i = list; i < list + dims [0]; i++) {
				*i = new T* [dims [1]];
				for (T **j = *i; j < *i + dims [1]; j++)
					*j = new T [dims [2]];
			}
			iList = list;
		}
	}
	
	void createList (int curDim) {
		int nDims = this->getNumOfDimensions ();
		if (curDim >= nDims) {
			   
			;
		} else {
			createList (curDim + 1);
		}
	}
	
	~dynamicArrayList<T> () {
		int nDims = this->getNumOfDimensions ();
		const int *dims = this->getDimensions ();
		
		if (nDims == 1) {
			delete [] (T *)iList;
		} else if (nDims == 2) {
			for (int i = 0; i < dims [0]; i++)
				delete [] ((T **)iList) [i];
			delete [] (T **)iList;
		} else if (nDims == 3) {
			for (int i = 0; i < dims [0]; i++) {
				for (int j = 0; j < dims [1]; j++)
					delete [] ((T ***)iList) [i][j];
				delete [] ((T ***)iList) [i];
			}
			delete [] (T ***)iList;
		}
	}
	
	friend ostream& operator<< (ostream& os, const dynamicArrayList<T> &obj) {
		const int *dims = obj.getDimensions ();
		int nDims = obj.getNumOfDimensions ();
		
		os << dims [0] << "x" << dims [1] << "x" << dims [2] << "-Matrix" << endl << endl;
		
		if (nDims == 1) {
			T *list = (T *)obj.iList;
			for (T *i = list; i < list + dims [0]; i++)
				os << *i << " ";
		} else if (nDims == 2) {
			T **list = (T **)obj.iList;
			for (T **i = list; i < list + dims [0]; i++) {
				for (T *j = *i; j < *i + dims [1]; j++)
					os << *j << " ";
				os << endl;
			}
		} else {
			T ***list = (T ***)obj.iList;
			for (T ***i = list; i < list + dims [0]; i++) {
				for (T **j = *i; j < *i + dims [1]; j++) {
					for (T *k = *j; k < *j + dims [2]; k++)
						os << *k << " ";
					os << "| ";
				}
				os << endl << endl;
			}
		}
		
		return os;
	}
	
	const T getItem (int *index) const { return getItem (index, 1, iList); }
	void setItem (int *index, T value) { setItem (index, 1, value, iList); }

	dalIterator begin () {
		dalIterator dal = dalIterator (*this);
		return dal;
	}
	
	dalIterator end () {
		return dalIterator (*this, true);
	}
	
private:
	list<T> *sortQuickly () {}
	list<T> *sortBubbly () {}
	
	const T getItem (int *index, int curDim, void *list) const {
		int nDims = this->getNumOfDimensions ();
		if (curDim >= nDims) return ((T *)list) [index [curDim - 1]];
		else return getItem (index, curDim + 1, ((T **)list) [index [curDim - 1]]);
	}
	
	void setItem (int *index, int curDim, T value, void *list) {
		int nDims = this->getNumOfDimensions ();
		if (curDim >= nDims) ((T *)list) [index [curDim - 1]] = value;
		else setItem (index, curDim + 1, value, ((T **)list) [index [curDim - 1]]);
	}
	
	void *iList;
};

#endif
