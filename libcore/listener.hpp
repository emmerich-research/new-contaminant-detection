#ifndef LIB_CORE_LISTENER_HPP_
#define LIB_CORE_LISTENER_HPP_

NAMESPACE_BEGIN

class Listener {
 public:
  Listener();
  virtual ~Listener();

  virtual void start() = 0;
  virtual void stop() = 0;
};

NAMESPACE_END

#endif  // LIB_CORE_LISTENER_HPP_
