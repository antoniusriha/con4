//
// Board.cpp
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

#include "Board.h"

bool Board::connected (int *index, Player cur) const {
	bool fail = true;
	int *idx = new int [3];
	
	for (int i = -1; fail && i <= 1; i++) {
		for (int j = -1; fail && j <= 1; j++) {
			if (i == 0 && j == 0) continue;
			copy (index, index + 3, idx);
			for (int m = 1; fail = m < nConnect; m++) {
				idx [0] += i;
				idx [1] += j;
				if (!isIndexValid (idx)) break;
				Player val = iList.getItem (idx);
				if (val != cur) break;
			}
		}
	}
	
	delete [] idx;
	return !fail;
};
