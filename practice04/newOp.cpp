#include <iostream>

using namespace std;

class A
{
    int _a;
    int _b;
    int _c;
    short _d;
    // sizeof(A) = 14 à 16 Bytes
public:
    A() {}
    ~A() {}
    static void* operator new(size_t t) {
        cout << "new (inside A): " << endl;
        cout << ">> size = " << t << endl;
        A* p = (A*)malloc(t);
        cout << ">> ptr = " << p << endl;
        return p;
    }
    static void* operator new[](size_t t) {
        cout << "new[] (inside A): " << endl;
        cout << ">> size = " << t << endl;
        A* p = (A*)malloc(t);
        cout << ">> ptr = " << p << endl;
        return p;
    }
    static void operator delete(void* p) {
        cout << "delete (inside A): " << endl;
        cout << ">> ptr = " << p << endl;
        free(p);
    }
    static void operator delete[](void* p) {
        cout << "delete[] (inside A): " << endl;
        cout << ">> ptr = " << p << endl;
        free(p);
    }
};

int main()
{
    A* a = new A;
    cout << endl;
    cout << "new (in main): " << endl;
    cout << ">> ptr = " << a << endl;
    cout << endl;

    delete a;
    cout << endl;

    A* b = new A[10];
    cout << endl;
    cout << "new[] (in main): " << endl;
    cout << ">> ptr = " << b << endl;
    cout << endl;

    delete []b;
    cout << endl;
}
