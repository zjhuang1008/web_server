#ifndef UNCOPYABLE_H
#define UNCOPYABLE_H

namespace net {

class Uncopyable {
public:
  Uncopyable(const Uncopyable&) = delete;
  Uncopyable& operator=(const Uncopyable&) = delete;
protected:
  Uncopyable() = default;
  ~Uncopyable() = default;
};

}

#endif // UNCOPYABLE_H
