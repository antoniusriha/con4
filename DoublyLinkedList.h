//
// DoublyLinkedList.h
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
#include "List.h"

using namespace std;

template <class T>
class Node {
public:
	Node<T> (int size = 0) : value (0), prev (0), next (0) {
		Node<T> *cur = this;
		// need to create toor (tail node) too, hence (size + 1)
		for (int i = 0; i < size + 1; i++)
			cur = new Node<T> (cur);
	}
	
	~Node<T> () {}
	
	const T &getValue () const { return *value; }
	void setValue (T &value) { this.value = &value; }
	
	const Node<T> * const getRoot () const {
		if (prev == 0) return this;
		return prev->getRoot ();
	}
	
	const Node<T> * const getToor () const {
		if (next == 0) return this;
		return next->getToor ();
	}
	
	const Node<T> * const first () const {
		return getRoot ().next;
	}
	
	const Node<T> * const last () const {
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
	Node<T> (Node<T> *prev) : value (0), prev (prev), next (0) {
		prev->next = this;
	}

	Node<T> *prev;
	Node<T> *next;
	T *value;
};

template <class T>
class DoublyLinkedList : public List<T> {
public:
	DoublyLinkedList<T> (int *dims) : List<T> (dims), root () {
		int nDims = this->getNDims ();
		
		if (nDims == 1) {
			Node<T> *l0 = new Node<T> (dims [0]);
			root = l0;
			toor = l0->getToor ();
		} else if (nDims == 2) {
			Node<Node<T> > *l0 = new Node<Node<T> > ();
			for (int i = 0; i < dims [0]; i++)
				l0->add (*new Node<T> (dims [1]));
			root = l0;
			toor = l0->getToor ();
		} else {
			Node<Node<Node<T> > > *l0 = new Node<Node<Node<T> > > ();
			for (int i = 0; i < dims [0]; i++) {
				Node<Node<T> > l1 = *new Node<Node<T> > ();
				l0->add (l1);
				for (int j = 0; j < dims [1]; j++)
					l1.add (*new Node<T> (dims [2]));
			}
			root = l0;
			toor = l0->getToor ();
		}
	}
	
	~DoublyLinkedList () {
		int nDims = this->getNDims ();
		const int *dims = this->getDims ();
		
		if (nDims == 1) {
			delete (Node<T> *)root;
		} else if (nDims == 2) {
			Node<Node<T> > *l0 = (Node<Node<T> > *)root;
			for (int i = 0; i < dims [0]; i++);
				
		} else {
		}
	}
	
	const T getItem (int *index) const {}
	void setItem (int *index, T value) {}

private:
	List<T> &sortQuickly () const {}
	List<T> &sortBubbly () const {}
	
	void *root;
	const void *toor;
};

#endif
