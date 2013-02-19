//
// doublyLinkedList.h
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

#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <stdexcept>
#include "list.h"

using namespace std;

template <class T>
class node {
public:
	node<T> (int size = 0) : value (0), prev (0), next (0) {
		node<T> *cur = this;
		// need to create toor (tail node) too, hence (size + 1)
		for (int i = 0; i < size + 1; i++)
			cur = new node<T> (cur);
	}
	
	~node<T> () {}
	
	const T &getValue () const { return *value; }
	void setValue (T &value) { this.value = &value; }
	
	const node<T> * const getRoot () const {
		if (prev == 0) return this;
		return prev->getRoot ();
	}
	
	const node<T> * const getToor () const {
		if (next == 0) return this;
		return next->getToor ();
	}
	
	const node<T> * const first () const {
		return getRoot ().next;
	}
	
	const node<T> * const last () const {
		return getToor ().prev;
	}
	
	const T getItem (int index) const {
		
	}
	
	void setItem (int index, T &value) {}
	
	void add (T &value) {
		
	}
	
	void insert (int index, T &value) {}
	void removeAt (int index) {}
	bool remove (T &value) {}
	
private:
	node<T> (node<T> *prev) : value (0), prev (prev), next (0) {
		prev->next = this;
	}

	node<T> *prev;
	node<T> *next;
	T *value;
};

template <class T>
class doublyLinkedList : public list<T> {
public:
	doublyLinkedList<T> (int *dims) : list<T> (dims), root () {
		int nDims = this->getNDims ();
		
		if (nDims == 1) {
			node<T> *l0 = new node<T> (dims [0]);
			root = l0;
			toor = l0->getToor ();
		} else if (nDims == 2) {
			node<node<T> > *l0 = new node<node<T> > ();
			for (int i = 0; i < dims [0]; i++)
				l0->add (*new node<T> (dims [1]));
			root = l0;
			toor = l0->getToor ();
		} else {
			node<node<node<T> > > *l0 = new node<node<node<T> > > ();
			for (int i = 0; i < dims [0]; i++) {
				node<node<T> > l1 = *new node<node<T> > ();
				l0->add (l1);
				for (int j = 0; j < dims [1]; j++)
					l1.add (*new node<T> (dims [2]));
			}
			root = l0;
			toor = l0->getToor ();
		}
	}
	
	~doublyLinkedList () {
		int nDims = this->getNDims ();
		const int *dims = this->getDims ();
		
		if (nDims == 1) {
			delete (node<T> *)root;
		} else if (nDims == 2) {
			node<node<T> > *l0 = (node<node<T> > *)root;
			for (int i = 0; i < dims [0]; i++);
				
		} else {
		}
	}
	
	const T getItem (int *index) const {}
	void setItem (int *index, T value) {}

private:
	list<T> &sortQuickly () const {}
	list<T> &sortBubbly () const {}
	
	void *root;
	const void *toor;
};

#endif
