// This program prints "hello world" 
// (and introduces a lot of new language features).

#include <iostream>  // ostream, cout
#include <cstring>   // strlen()
#include <algorithm> // copy()
#include <new>       // bad_alloc
using namespace std;

// A Vector is a simplified vector with fixed size
template <class T>
class Vector
{
private:
  T *p;  // array of elements
  int n; // number of elements
public:
  explicit Vector(int i = 0);  // constructor without implicit conversion
  Vector(const Vector<T>& v);  // copy constructor
  Vector<T>& operator = (const Vector<T>& v);  // assignment operator
  ~Vector() {delete[] p;}  // destructor
  int size() const {return n;}  // number of elements
  T& operator[](int i) {return p[i];}  // index operator
  const T& operator[](int i) const {return p[i];}  // read-only index op.

  // iterators
  typedef T* iterator;
  iterator begin() {return p;}
  iterator end() {return p+n;}
  typedef const T* const_iterator;
  const_iterator begin() const {return p;}
  const_iterator end() const {return p+n;}

};

// Code for non-inlined Vector members: constructor
template <class T>
Vector<T>::Vector(int i): p(new T[i]), n(i)
{
  for (int i=0; i<n; ++i)
    p[i] = T();
}

// Copy v
template <class T>
Vector<T>::Vector(const Vector<T>& v): p(new T[v.size()]), n(v.size())
{
  copy(v.p, v.p+n, p);
}

// Assign v
template <class T>
Vector<T>& Vector<T>::operator = (const Vector<T>& v)
{
  if (&v != this)  // not assignment to self?
  {
    delete[] p;
    n = v.n;
    p = new T[n];
    copy(v.p, v.p+n, p);
  }
  return *this;  // return reference to self
}

// Print a Vector with elements separated by sep
template <class T>
void print(ostream& out, const Vector<T>& v, const char* sep = "")
{
  for (int i=0; i<v.size(); ++i)
  {
    if (i > 0)
      out << sep;
    out << v[i];
  }
}

// A String is a Vector<char> with a conversion from const char*
class String: public Vector<char>
{
public:
  String(const char* s = "");
};

String::String(const char* cp): Vector<char>(strlen(cp))
{
  copy(cp, cp+size(), begin());
}

// Print a String
ostream& operator << (ostream& out, const String& s)
{
  print(out, s);
  return out;
}

// Print "Hello world"
int main()
{
  try
  {
    const String greeting = "Hello world";
    cout << greeting << endl;
  }
  catch (bad_alloc x)
  {
    cout << "Out of memory\n";  // Vector too big?
  }
  catch (...)
  {
    cout << "Unknown exception\n";
    throw;  // throw exception just caught and abort program
  }
  return 0;
}

/* Notes:

This program introduces templates, inheritance, memory allocation, const,
typedef, and exceptions.  For more details on these features, see the
long version of the C++ reference at
http://cs.fit.edu/~mmahoney/cse2050/how2cpp.html

The program first introduces a templated class Vector, similar to vector.
Then it derives String (similar to string) from Vector<char>, inheriting
all its properties and adding a conversion from char arrays.



Templates.

A function or a class may be templated.  A function template overloads
a function for an unlimited number of types, for example, <algorithm>
defines the function swap() as follows:

  template <class T>
  void swap(T& a, T& b)
  {
    T tmp = a;
    a = b;
    b = tmp;
  }

which allows you to swap any two objects of the same type, for example,

  int a=1, b=2;
  swap(a, b);  // substitute "int" for T
  string s="hi", t="hello";
  swap(s, t);  // substitute "string" for T
  swap(a, s);  // error, can't find a type for T

The compiler tries to figure out T from the types of the arguments.
(T can either be a class or a regular type).

A class may also be templated, but you have to explicitly pass T, e.g.

  Vector<int> v;

means to replace T with "int" throughout class Vector.  <vector> also
uses this technique.

A template may have more than one type parameter.  For example, <algorithm>
defines:

  template<class IN, class OUT>
  OUT copy(IN begin, IN end, OUT dest)
  {
    while (begin != end)
      *dest++ = *begin++;
    return dest;
  }

A template may cause compiler errors if the types are not approriate,
for example, if IN and OUT are not pointers or iterators.



Explicit.

A constructor may be declared explicit to suppress automatic conversion,
for example:

  Vector<int> v = 5;  // error
  v = 10;  // error

would normally be allowed if the constructor were not explicit.  The
keyword "explicit" only appears in the prototype, not the declaration.



Memory allocation.

Normally, C++ requires arrays to have constant size, e.g.

  int a[10];  // OK
  int n;
  cin >> n;
  int b[n];  // Error, compiler doesn't know value of n

The "new" operator allows you to create an array whose size is not known
until the program is running:

  int *p;
  p = new int[n];  // has elements p[0] through p[n-1]

The memory remains allocated until deleted or the program exits.
Unlike ordinary arrays, allocated arrays are NOT destroyed when
the pointer to it goes out of scope.  To release the memory:

  delete[] p;  // destroy the array

The elements of p are initialized the same way as ordinary arrays,
e.g. random values for built in types like int, char, double, and bool.
The expressions int(), char(), double(), and bool() return 0, '\0',
0.0, and false respectively.  Thus, in a template:

  T *p = new T[n];
  for (int i=0; i<n; ++i)
    p[i] = T();

initializes all the elements of p to a default value appropriate for
type T.  If T is a class type such as string, vector, or map, then
the elements are already initialized to default values (empty), and
the assignement of T() has no effect.

There is a second version of new that allows you to pass arguments
to the constructor, but only allocates one element:

  p = new T(arguments...);

which should be deleted without the []

  delete p;

For example:

  int *p = new int(5);
  cout << *p;  // 5
  delete p;



Constructor initialization lists.

The constructor code:

  Vector<T>::Vector(int i): p(new T[i]), n(i) {}

is almost equivalent to:

  Vector<T>::Vector(int i)
  {
    p = new T[i];
    n = i;
  }

The initialization list differs in a subtle way, in the same way that:

  int n = i;  // initialization

differs from

  int n;  // default initialization
  n = i;  // assignment

An initialization list is only required for types that lack default
constructors.  There are no standard library classes that have this
requirement, however we can alwasys use it safely, and it is sometimes
more efficient because it avoids an extra operation.

The order of initialization is the order in which the members are declared,
not the order they appear in the list, so

  Vector<T>::Vector(int i): n(i), p(new T[n])

would not work because n has a random value and would create an
array with the wrong size.  (g++ will warn you if the list order
is different than the declaration order).



const.

A value of a const object cannot be changed once it is initialized:

  const double PI = 3.14159265359;
  PI = 4.0;  // error, PI is const

The most common use is passing function parameter by const reference.
This has the efficiency of pass by reference (because the argument is
not copied), and also removes the restriction that the argument be a
variable (not an expression), for example:

  void print_val(string s)   {cout << s;}  // copies s, slow
  void print_ref(string& s)  {cout << s;}  // faster, but requires a variable
  void print_cref(const string& s) {cout << s;}  // best

  int main()
  {
    print_val("hello");    // OK, but slow
    print_ref("hello");    // Error, argument must be a variable
    print_cref("hello");   // OK, faster
  }

To ensure that a const object cannot be modified through its member
functions, only member functions declared const can be called on
const objects:

  void f(const vector<int>& v)
  {
    cout << v.size();  // OK, size() is const
    v = v;             // Error, can't assign to v
    v.resize(10);      // Error, resize() is not const
  }

This happens because <vector> defines class vector something like:

  template <class T>
  class vector
  {
    unsigned int size() const;
    void resize(int i);  // not const
    ...

A const member function cannot assign or modify any of the data members.

A member may be overloaded on const.  If an object is const, then
the const version will be called, e.g.

  void copyVector(const Vector<int>& in, Vector<int>& out)
  {
    for (int i = 0; i < in.size(); ++i)
      out[i] = in[i];
  }

In this example, out[i] calls the non-const version of operator[],
and in[i] calls the const version.



Return by reference.

If in copyVector() you mistakenly wrote:

  in[i] = out[i];  // Error, in[i] is const

you get an error because the const version of operator[] returns
a const reference to an element.  When a function returns by reference,
you can assign to the result of a function call, and the result is to
assign to whatever it returns, e.g.

  int g;  // global

  int& f()
  {
    return g;
  }

  int main()
  {
    f() = 5;  // g = 5;
    cout << f();  // prints 5
  }

The effect of return by reference is to replace f() with g in main().
No temporary object is created to hold the return value, so

  int& f()
  {
    int a = 5;  // local
    return a;   // Error: a will no longer exist after returning
  }

However, it is generally safe for a member function to return a
reference to a data member, because it continues to exist after
the function returns, so

  out[i] = in[i];

which means

  out.operator[](i) = in.operator[](i);  // non-const = const versions

has the effect

  out.p[i] = in.p[i];

which remains valid until the memory allocated to p is returned
by delete[].

It is also OK for a function to return a reference to a parameter
that was passed by reference, since it must have been created before
the function was called.  Thus,

  ostream& operator << (ostream& out, const String& s)
  {
    print(out, s);
    return out;
  }

means that

  cout << greeting << endl;

is equivalent to

  (operator << (cout, greeting)) << endl;

which returns a reference to out, so

  out << endl;  // after printing greeting

but out is a reference to cout, so this means

  cout << endl;

If a function returns const reference, the effect is the same, except
that the returned object is const, so

  int g;  // global

  const int& cf()
  {
    return g;
  }

  int main()
  {
    cout << cf();  // OK, prints g
    cf() = 5;  // Error, cf() is const
  }

For the same reason, in[i] = out[i] fails because in[i] has type const T
and out[i] has type T.



const pointers and iterators.

The declaration

  const char* p;

declares p to point to const char.  p itself is not const, so:

  p = new char[10];  // OK
  *p = 'x';          // Error, *p is const
  p[5] = 'x';        // Error, *p is const
  p += 2;            // OK
  char *q = p;       // Error, *q is not const
  *q = 'x';          // because otherwise you could change *p this way

To handle iterators properly, all types with iterators (string, vector,
map), also have a type called const_iterator, which works like a const
pointer in that you cannot modify what it points to, so you could
write:

  void copyVector(const Vector<int>& in, Vector<int>& out)
  {
    copy(in.begin(), in.end(), out.begin());
  }

begin() and end() are overloaded on const to return const_iterator
if the object is const.  Thus,

  *in.begin() = 0;  // Error
  *out.begin() = 0; // OK, assigns to first element of out

Likewise if you accidentally wrote:

  copy(out.begin(), out.end(), in.begin());

the templated code above would fail on *dest = ... because type OUT
is type Vector<int>::const_iterator, which is really type const int*.
The statement

  typedef const T* const_iterator;

says to replace const_iterator with const T*.  The declaration

  Vector<int>::const_iterator p;

says to substitute int for T, so is equivalent to:

  const int* p;

In the standard library types, vector and string iterators are usually
just pointers.  In maps, the iterators are actually classes that
overload operators like *, ->, ++, ==, etc. to make them look like
pointers.



Classes allocating memory

To ensure that the memory allocated by Vector is freed when the
Vector is destroyed, we write a destructor, which is automatically
called when the object is destroyed, normally when it goes out of
scope.

  void f()
  {
    Vector<int> v(10);  // constructor allocated 10 ints
    ...
  }  // v destroyed, destructor deletes p here

A destructor has the same name as the class with a ~ in front.
It takes no parameters and has no return type:

  ~Vector()
  {
    delete[] p;
  }

Classes that have destructors generally should have copy constructors
and overload assignment operators, because copy and assignment are
otherwise handled incorrectly.  The default behavior is to copy or
assign each of the data members:

  Vector<int> w = v;  // copy v.p and v.n to w.p and w.n
  w = v;            // assign v.p and v.n to w.p and w.n

This is incorrect because v.p and w.p would point to the same
memory, so

  v[5] = 100;    // v.p[5] = 100, also changes w.p[5]
  cout << w[5];  // 100

A copy constructor is a constuctor that takes the same type as
a parameter (passed by const reference).  It replaces the default
behavior for copying.  The correct behavior is to allocate a new
array for w of the same size as v and copy the elements:

  template <class T>
  Vector<T>::Vector(const Vector<T>& v): p(new T[v.size()]), n(v.size())
  {
    copy(v.p, v.p+n, p);
  }

Assignment (operator = ) must be overloaded separately.  It differs from
copying in that:

- the old value must be destroyed (delete[] p)
- assignment normally returns its new value (return *this)

Thus:

  template <class T>
  Vector<T>& Vector<T>::operator = (const Vector<T>& v)
  {
    if (&v != this)  // not assignment to self?
    {
      delete[] p;
      n = v.n;
      p = new T[n];
      copy(v.p, v.p+n, p);
    }
    return *this;
  }

"this" is a reserved word meaning pointer to self.  Thus, *this
means self.  Returning self allows assignment to be chained as in:

  a = b = c;

which assigns c to b, then assigns the new value of b to a.  This
behavior is not required, but is consistent with other types.  It
is legal for operator = to return any type, including void.
Also, we could return by value, but return by reference is
more efficient because it does not create a temporary copy of itself.

Note that the code inside the "if" statement would fail in the
case of assignment to self, (w = w;) because delete[] p would
also delete w.p before its elements could be copied.  To check
for this, we compare the addresses of the left and right operand,
i.e. this with the address of the parameter v.  & means "address of",
which is the inverse of the * operator, e.g. if p is a pointer,

  &*p == p;

and if x is any object or variable,

  *&x == x;

so

  int x, y;
  int *p = &x; // p points to x
  *p = 5;  // x = 5;
  p = &y;
  *p = 10;  // y = 10;

We cannot check for assignment to self using

  if (*this != v)

because we have not defined operator != for type Vector.  Even if we
had, this only tests if they have the same value, not if they are
the same object.  Comparing addresses is foolproof.


Inheritance

Inheritance is useful for writing classes that are similar to or
special cases of other classes.  If B is a class, then

  class D: public B
  {
    ...
  };

declares D as a derived class of base class B.  D inherits all of B's
members.  D may also add or override members  For example:

  class Bird
  {
  public:
    void fly() {cout << "flap";}
    void sing() {cout << "chirp";}
  };

  class Duck: public Bird
  {
  public:
    void sing() {cout << "quack";}
    void swim() {cout << "splash";}
  };

  int main()
  {
    Bird b;
    b.fly();  // flap
    b.sing(); // chirp
    b.swim(); // Error, no Bird::swim()

    Duck d;
    d.fly();  // flap (inherited from Bird)
    d.sing(); // quack (overridden)
    d.swim(); // splash (added)
  }

Data members are inherited and occupy space in the derived objects,
but if they are private, the derived class can't access them.  This
behavior is normally desirable.  However, a base class can declare
members protected: instead of private: to allow derived classes
(but nobody else) access.

Constructors, destructors, and operator = don't inherit.  Instead their
default behavior is to initialize, destroy, or assign the base class
as if it was just another data member.  If that base class (or data
members) have overloaded the copy constructor or operator =, then those
functions are called to do the copy or assignment.  If they have
destructors, they are automatically called when the derived class
or containing class is destroyed.  Thus,

  void f() {
    String a = "hello", b = a;  // copy a to b
    b = a;  // assign
  };  // destroy b and a

works correctly, even though a String allocates memory, and it does
not have a destructor, copy constructor, or assignment operator.
When a String is copied, assigned, or destroyed, the base part
is copied, assigned, or destroyed using the copy constructor, operator =,
and destructor of Vector<char>.

It is an error for a derived constructor to initialize the data members
inherited from the base class.  Instead, it should call the base constructor
from the initialization list and let the base class initialize the
data members.

  String::String(const char* cp): Vector<char>(strlen(cp))

strlen(), defined in <cstring>, returns the length of a NUL terminated
char array.  The Vector constructor then initializes n to this number
and p to an array of char of length n.  Remember that p and n are
private and not accessible to String.

String inherits everything from vector<char>, e.g.

  String::iterator p;  // inherits Vector<char>::iterator
  p = a.begin();       // inherits begin()
  a[5] = 'x';          // inherits operator[]

It is permitted to use a derived object where a base object is expected.
In this case the object is converted to the base type by discarding
its extra members.

  Duck d;
  Bird b = d;
  b.sing();  // chirp
  b.swim();  // Error

This is used in the print() function, where a String is converted
to Vector<char> before printing.

  template <class T>
  void print(ostream& out, const Vector<T>& v, const char* sep = "");

  ostream& operator << (ostream& out, const String& s)
  {
    print(out, s);
    return out;
  }

Usually it is desired to keep the characteristics of the derived class.
For example, you can print to a file with no extra effort:

  ofstream f("file.txt");
  String greeting = "Hello world";
  f << greeting << endl;

This works because ofstream is a derived class of ostream (and ifstream
from istream).  However, this does not always work for overridden
member functions through pointers or references:

  void song(Bird& b)
  {
    b.sing();
  }

  int main()
  {
    Duck d;
    Bird *bp = &d;
    d.sing();    // quack
    bp->sing();  // chirp
    song(d);     // chirp
  }

This problem can be fixed by declaring functions "virtual" in the
base class to indicate they might be overridden:

  class Bird
  {
  public:
    virtual void sing() {cout << "chirp";}
    virtual ~Bird() {}
  };

  class Duck: public Bird
  {
  public:
    void sing() {cout << "quack";}
  };

  Bird b;
  Duck d;
  Bird *bp = &b;
  bp->sing();  // chirp
  bp = &d;
  bp->sing();  // quack
  song(b);     // chirp
  song(d);     // quack

Whenever a class has any virtual functions, you should add an empty virtual
destructor (or make the existing destructor virtual), so that delete
will call the right one.

  Bird *bp = new Duck;
  delete bp;  // which destructor?

Adding an empty virtual destructor does not require you to write a
copy constructor or assignment operator.

Vector does not need any virtual functions because String does not
override any of them.



Exception handling.

If new is asked to allocate more memory than is available, it throws
an exception of type bad_alloc (defined in <new>).  By default, this
aborts the program.  A "try" block will catch any exceptions of a
matching type, e.g.

  void f()
  {
    throw 3;   // jump to
  }

  int main()
  {
    try
    {
      f();
    }
    catch (int x)  // here
    {
      cout << x;  // print 3
    }
  }

Exceptions are useful when the code that detects the error is far
removed from the code that knows how to handle it.  Throwing an
exception will jump out of (but not back to) deeply nested function
calls.

Any type can be thrown.  A try block is followed by one or more 
catch blocks.  The first catch parameter that matches the type of the
object thrown catches the exception.  ... means to catch any type.
If an exception is not caught, the program aborts.

A catch parameter may be a base type.  It will catch any exception
of a derived type.  For example, bad_alloc is derived from type
exception (defined in <stdexcept>), so

  try
  {
    Vector<double> v(1000000000);
  }
  catch (exception x)
  {
    cout << "buy more memory\n";}
  }

will catch the bad_alloc exception.

*/
