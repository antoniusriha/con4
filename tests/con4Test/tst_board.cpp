/*
 * tst_board.cpp
 *
 * Author:
 *       Antonius Riha <antoniusriha@gmail.com>
 *
 * Copyright (c) 2013 Antonius Riha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdexcept>
#include <QtCore/QString>
#include <QtTest/QtTest>

#include "tst_global.h"
#include "../../src/con4/board.h"

using namespace std;

class BoardTest : public QObject {
    Q_OBJECT
    
public:
    BoardTest ();
    
private Q_SLOTS:
    void ctor_except_data();
    void ctor_except();

    void nDims_data ();
    void nDims ();
};

BoardTest::BoardTest()
{
}

void BoardTest::ctor_except_data()
{
    QTest::addColumn<int>("nConnect");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("dim2");
    QTest::addColumn<int>("dim3");

    QTest::newRow("nConnect lower than 4") << 3 << 20 << 20 << 0;
    QTest::newRow("nConnect greater than MAX_DIMS / 2") << MAX_DIMS / 2 + 1 << 20 << 20 << 0;
    QTest::newRow("dim1 lower than nConnect") << 5<< 0 << 20 << 0;
    QTest::newRow("dim1 greater than MAX_DIMS") << 5 << MAX_DIMS + 1 << 20 << 0;
    QTest::newRow("dim2 lower than nConnect") << 5 << 20 << 0 << 0;
    QTest::newRow("dim2 greater than MAX_DIMS") << 5 << 20 << MAX_DIMS + 1 << 0;
    QTest::newRow("dim3 greater than 1 but lower than nConnect") << 5 << 20 << 20 << 4;
    QTest::newRow("dim3 greater than MAX_DIMS") << 5 << 20 << 20 << MAX_DIMS + 1;
    QTest::newRow("total n of fields is an odd number (2D)") << 4 << 7 << 7 << 0;
    QTest::newRow("total n of fields is an odd number (3D)") << 4 << 7 << 7 << 7;
}

void BoardTest::ctor_except()
{
    QFETCH(int, nConnect);
    QFETCH(int, height);
    QFETCH(int, dim2);
    QFETCH(int, dim3);

    QVERIFY_THROW(Board (nConnect, height, dim2, dim3), invalid_argument);
}

void BoardTest::nDims_data () {
    QTest::addColumn<int> ("dim3");
    QTest::addColumn<int> ("result");
    QTest::newRow ("2D") << 1 << 2;
    QTest::newRow ("3D") << 6 << 3;
}

void BoardTest::nDims () {
    QFETCH (int, dim3);
    QFETCH (int, result);

    Board b (4, 5, 6, dim3);
    QCOMPARE (b.nDims (), result);
}


DECLARE_TEST (BoardTest)
#include "tst_board.moc"
