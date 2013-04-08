/*
 * tst_global.h
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

#ifndef TST_GLOBAL_H
#define TST_GLOBAL_H

#include <QTest>
#include <QList>
#include <QString>
#include <QSharedPointer>

#define QVERIFY_THROW(expression, ExpectedExceptionType) \
do \
{ \
bool caught_ = false; \
try { expression; } \
catch (ExpectedExceptionType const&) { caught_ = true; } \
catch (...) {} \
if (!QTest::qVerify(caught_, #expression ", " #ExpectedExceptionType, "", __FILE__, __LINE__))\
return; \
} while(0)

namespace TstGlobal {
typedef QList<QObject*> TestList;

inline TestList& testList () {
    static TestList list;
    return list;
}

inline bool findObject (QObject* object) {
    TestList& list = testList ();
    if (list.contains (object)) return true;
    foreach (QObject* test, list)
        if (test->objectName () == object->objectName ()) return true;
    return false;
}

inline void addTest (QObject* object) {
    TestList& list = testList();
    if (!findObject (object)) list.append (object);
}

inline int run (int argc, char *argv []) {
    int ret = 0;
    foreach (QObject* test, testList ())
        ret += QTest::qExec (test, argc, argv);
    return ret;
}
}

template <class T>
class Test {
public:
    QSharedPointer<T> child;

    Test (const QString& name) : child (new T) {
        child->setObjectName (name);
        TstGlobal::addTest (child.data ());
    }
};

#define DECLARE_TEST(className) static Test<className> t(#className);

#define TEST_MAIN \
int main(int argc, char *argv[]) \
{ \
return TstGlobal::run(argc, argv); \
}

#endif // TST_GLOBAL_H
