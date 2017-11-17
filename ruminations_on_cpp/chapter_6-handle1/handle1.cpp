/*
  将继承和容器共用，迫使我们要处理两个问题：控制内存分配和把不同类型的对象放入同一个容器中。
  使用一个Handle类可以同时解决这两个问题
  这是一个简单的Handle类的实现，通过将对象和引用计数进行封装，然后形成Handle类
*/

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

class Handle;
class UPoint {
  // All the members are private
 private:
  friend class Handle;
  Point p_;
  int u_;

  UPoint() : u_(1) {}
  UPoint(int x, int y) : p_(x, y), u_(1) {}
  UPoint(const Point& p) : p_(p), u_(1) {}
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
  UPoint* up_;
};

Handle::Handle() : up_(new UPoint) {}
Handle::Handle(int x, int y) : up_(new UPoint(x, y)) {}
Handle::Handle(const Point& p) : up_(new UPoint(p)) {}
// copy between handles do not really copy object, only copy pointer and
// increase reference count
Handle::Handle(const Handle& h) : up_(h.up_) { ++up_->u_; }
Handle::~Handle() {
  if (--up_->u_ == 0) {
    delete up_;
  }
}
Handle& Handle::operator=(const Handle& h) {
  ++h.up_->u_;
  // cutoff current pointer and retatch to new pointer
  if (--up_->u_ == 0) {
    delete up_;
  }
  up_ = h.up_;
  return *this;
}

int Handle::x() const { return up_->p_.x(); }
int Handle::y() const { return up_->p_.y(); }

Handle& Handle::x(int x) {
#ifdef copy_on_write
  if (up_->u_ != 1) {
    --up_->u_;
    up_ = new UPoint(up_->p_);
  }
#endif
  up_->p_.x(x);
  return *this;
}
Handle& Handle::y(int y) {
#ifdef copy_on_write
  if (up_->u_ != 1) {
    --up_->u_;
    up_ = new UPoint(up_->p_);
  }
#endif
  up_->p_.y(y);
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