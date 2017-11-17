#include <iostream>

#define copy_on_write

class Point {
 public:
  Point() : x_(0), y_(0) {}
  Point(int x, int y) : x_(x), y_(y) {}

  int x() const { return x_; }
  int y() const { return y_; }

  Point& x(int x) {
    x_ = x;
    return *this;
  }
  Point& y(int y) {
    y_ = y;
    return *this;
  }

 private:
  int x_;
  int y_;
};

class UseCount {
 public:
  UseCount() : p_(new int(1)) {}
  UseCount(const UseCount& u) : p_(u.p_) { ++*p_; }
  ~UseCount() {
    if (--*p_ == 0) {
      delete p_;
    }
  }

  bool only() { return (*p_ == 1); }
  bool reattach(const UseCount& u) {
    // increase use count of one handle
    ++*u.p_;
    // decrease use count of another
    if (--*p_ == 0) {
      // if last handle holds the data, delete the data
      delete p_;
      p_ = u.p_;
      return true;
    }
    p_ = u.p_;
    return false;
  }

  bool makeonly() {
    if (*p_ == 1) {
      // donot need to create a new object
      return false;
    }
    --*p_;
    p_ = new int(1);
    return true;
  }

 private:
  UseCount& operator=(const UseCount& u) {
    p_ = u.p_;
    ++*p_;
    return *this;
  }

 private:
  int* p_;
};

class Handle {
 public:
  Handle();
  Handle(int, int);
  Handle(const Point&);
  Handle(const Handle&);
  Handle& operator=(const Handle&);
  ~Handle();

  int x() const;
  Handle& x(int);
  int y() const;
  Handle& y(int);

 private:
  Point* p_;
  UseCount u_;
};

Handle::Handle() : p_(new Point) {}
Handle::Handle(int x, int y) : p_(new Point(x, y)) {}
Handle::Handle(const Point& p) : p_(new Point(p)) {}
// copy between handles do not really copy object, only copy pointer and
// increase reference count
Handle::Handle(const Handle& h) : u_(h.u_), p_(h.p_) {}
Handle::~Handle() {
  if (u_.only()) {
    delete p_;
  }
}
Handle& Handle::operator=(const Handle& h) {
  // cutoff current pointer and retatch to new pointer
  if (u_.reattach(h.u_)) {
    delete p_;
  }
  p_ = h.p_;
  return *this;
}

int Handle::x() const { return p_->x(); }
int Handle::y() const { return p_->y(); }

Handle& Handle::x(int x) {
#ifdef copy_on_write
  if (u_.makeonly()) {
    p_ = new Point(*p_);
  }
#endif
  p_->x(x);
  return *this;
}
Handle& Handle::y(int y) {
#ifdef copy_on_write
  if (u_.makeonly()) {
    p_ = new Point(*p_);
  }
#endif
  p_->y(y);
  return *this;
}

int main() {
  // test point
  Point p0;
  std::cout << "empty point: " << p0.x() << ", " << p0.y() << std::endl;

  Point p1(10, 20);
  std::cout << "not empty point: " << p1.x() << ", " << p1.y() << std::endl;
  p1.x(1).y(2);
  std::cout << "update point: " << p1.x() << ", " << p1.y() << std::endl;
  Point p2(p1);
  std::cout << "copy point: " << p2.x() << ", " << p2.y() << std::endl;

  // test handle
  Handle h0;
  std::cout << "empty handle: " << h0.x() << ", " << h0.y() << std::endl;

  Handle h1(10, 20);
  std::cout << "not empty handle: " << h1.x() << ", " << h1.y() << std::endl;

  Handle h2 = h1;
  std::cout << "copy handle: " << h2.x() << ", " << h2.y() << std::endl;

  h2.x(1).y(2);
  std::cout << "update handle: " << h2.x() << ", " << h2.y() << std::endl;
  return 0;
}