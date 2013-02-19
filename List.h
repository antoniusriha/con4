//
// List.h
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

#ifndef LIST_H
#define LIST_H

#include <stdexcept>

using namespace std;

template <class T>
class List {
public:
	enum SortMode { Bubble, Quick };
	virtual ~List<T> () {}
	virtual const T getItem (int *index) const = 0;
	virtual void setItem (int *index, T value) = 0;
	
	T getItemByFlatIndex (int flatIndex) const {
		if (flatIndex < 0)
			throw new logic_error ("Index cannot be less than 0.");
		
		int count = getCount ();
		int *index = new int [nDims];
		for (int i = 0; i < nDims; i++)
			index [i] = 0;
		trIndex (flatIndex, count, 1, index);
		T item = getItem (index);
		delete [] index;
		return item;
	}
	
	void setItemByFlatIndex (int flatIndex, T value) {
		if (flatIndex < 0)
			throw new logic_error ("Index cannot be less than 0.");
		
		int count = getCount ();
		int *index = new int [nDims];
		for (int i = 0; i < nDims; i++)
			index [i] = 0;
		trIndex (flatIndex, count, 1, index);
		setItem (index, value);
		delete index;
	}
	
	const int *getDims () const { return dims; }
	int getNDims () const { return nDims; }
	
	int getCount () const {
		int count = 1;
		for (int i = 0; i < nDims; i++)
			count *= dims [i];
		return count;
	}
	
	List<T> *sort (SortMode mode) const {
		if (nDims > 1)
			throw new logic_error ("sort can only be called with 1d lists.");
		
		// although not necessary, use fctn ptrs to fulfill requirements
		
	}

protected:
	List<T> (int *dims) : dims (dims), nDims (getNDims (dims)) {
		if (dims [0] == 0)
			throw new logic_error ("The object must at least be 1d.");
	}
	
	virtual List<T> &sortQuickly () const = 0;
	virtual List<T> &sortBubbly () const = 0;

private:
	const int getNDims (int *dims) const {
		int nDims = 1;
		for (; nDims < 4; nDims++)
			if (nDims >= 3 || dims [nDims] == 0) break;
		return nDims;
	}

	void trIndex (int flatIndex, int curCount, int curDim, int *index) const {
		if (flatIndex <= 0)
			return;
		curCount /= dims [curDim - 1];
		int idx = flatIndex / curCount;
		flatIndex -= curCount * idx;
		index [curDim - 1] = idx;
		trIndex (flatIndex, curCount, curDim + 1, index);
	}
	
	const int * const dims;
	const int nDims;
};

#endif
