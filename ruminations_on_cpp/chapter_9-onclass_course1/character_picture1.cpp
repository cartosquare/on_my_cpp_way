/*
编写一系列可以操纵字符图像的类和函数
字符图像就是一个可打印的矩形字符阵列
实现3中操作：加像框、横向连接和纵向连接。
比如对于下面的字符图像 p：
Paris
in the
Sprint
对p加像框之后：
+------+
|Paris |
|in the|
|Sprint|
+------|
把加框之前的图像和加框之后的图像进行横向连接后(两幅图的顶端对齐)：
Paris +------+
in the|Paris |
Sprint|in the|
      |Sprint|
      +------|
将最近产生的两幅图像纵向连接（两幅图的左边对齐）：
+------+
|Paris |
|in the|
|Sprint|
+------|
Paris +------+
in the|Paris |
Sprint|in the|
      |Sprint|
      +------|
再进一步，把上面的图加一个边框：
+--------------+
|+------+      |
||Paris |      |
||in the|      |
||Sprint|      |
|+------|      |
|Paris +------+|
|in the|Paris ||
|Sprint|in the||
|      |Sprint||
|      +------||
+--------------|
*/

#include <algorithm>
#include <iostream>

class Picture {
 public:
  Picture();
  Picture(const char* const*, int);
  Picture(const Picture&);
  ~Picture();

  Picture& operator=(const Picture&);

  int height() const { return height_; }
  int width() const { return width_; }

  // return const value
  char position(int row, int col) const { return data_[row * width_ + col]; }

  friend Picture frame(const Picture& p);
  friend Picture operator&(const Picture& top, const Picture& bottom);
  friend Picture operator|(const Picture& left, const Picture& right);

 private:
  int height_, width_;
  char* data_;

  void init(int h, int w);

  // return reference
  char& position(int row, int col) { return data_[row * width_ + col]; }

  // copy from another picture, specied a start position
  void copyblock(int, int, const Picture&);
};

Picture::Picture() : height_(0), width_(0), data_(nullptr) {}
Picture::Picture(const char* const* array, int n) {
  int w = 0;
  for (int i = 0; i < n; ++i) {
    w = std::max<int>(w, strlen(array[i]));
  }
  init(n, w);
  // copy characters
  for (int i = 0; i < n; ++i) {
    const char* src = array[i];
    int len = strlen(src);
    for (int j = 0; j < len; ++j) {
      position(i, j) = src[j];
    }
    // fill in white spaces
    // for (int j = len; j < width_; ++j) {
    //   position(i, j) = ' ';
    // }
  }
}
Picture::~Picture() {
  if (data_) {
    delete[] data_;
  }
  height_ = 0;
  width_ = 0;
}
void Picture::init(int h, int w) {
  height_ = h;
  width_ = w;
  data_ = new char[height_ * width_];
  memset(data_, ' ', height_ * width_);
}

Picture::Picture(const Picture& p) {
  init(p.height(), p.width());
  copyblock(0, 0, p);
}
Picture& Picture::operator=(const Picture& p) {
  if (this != &p) {
    delete[] data_;
    init(p.height(), p.width());
    copyblock(0, 0, p);
  }
  return *this;
}
void Picture::copyblock(int s_row, int s_col, const Picture& p) {
  for (int i = 0; i < p.height_; ++i) {
    for (int j = 0; j < p.width_; ++j) {
      position(i + s_row, j + s_col) = p.position(i, j);
    }
  }
}

std::ostream& operator<<(std::ostream& o, const Picture& p) {
  for (int i = 0; i < p.height(); ++i) {
    for (int j = 0; j < p.width(); ++j) {
      o << p.position(i, j);
    }
    o << std::endl;
  }
  return o;
}

Picture frame(const Picture& p) {
  Picture r;
  r.init(p.height() + 2, p.width() + 2);

  for (int i = 1; i < r.height() - 1; ++i) {
    r.position(i, 0) = '|';
    r.position(i, r.width() - 1) = '|';
  }
  for (int j = 1; j < r.width() - 1; ++j) {
    r.position(0, j) = '-';
    r.position(r.height() - 1, j) = '-';
  }
  r.position(0, 0) = '+';
  r.position(0, r.width() - 1) = '+';
  r.position(r.height() - 1, 0) = '+';
  r.position(r.height() - 1, r.width() - 1) = '+';

  r.copyblock(1, 1, p);
  return r;
}

Picture operator&(const Picture& top, const Picture& bottom) {
  Picture r;
  r.init(top.height() + bottom.height(),
         std::max<int>(top.width(), bottom.width()));
  r.copyblock(0, 0, top);
  r.copyblock(top.height(), 0, bottom);

  return r;
}

Picture operator|(const Picture& left, const Picture& right) {
  Picture r;
  r.init(std::max<int>(left.height(), right.height()),
         left.width() + right.width());
  r.copyblock(0, 0, left);
  r.copyblock(0, left.width(), right);

  return r;
}

int main() {
  char* init[] = {"Paris", "in the", "Spring"};
  Picture p(init, 3);
  std::cout << p << std::endl;

  Picture q = frame(p);
  std::cout << q << std::endl;

  Picture r = p | q;
  std::cout << r << std::endl;

  Picture s = q & r;
  std::cout << s << std::endl;

  std::cout << frame(s) << std::endl;

  return 0;
}