#ifndef LIB_CORE_LISTENER_HPP_
#define LIB_CORE_LISTENER_HPP_

#include <atomic>
// #include <condition_variable>
// #include <shared_mutex>
#include <thread>

NAMESPACE_BEGIN

class Listener {
 public:
  Listener();
  virtual ~Listener();

  virtual void start() = 0;
  virtual void stop() = 0;

 protected:
  typedef std::mutex Mutex;
  using LockGuard = std::lock_guard<Mutex>;
  using Lock = std::unique_lock<Mutex>;
  // using WriteLock = std::unique_lock<Mutex>;
  // using ReadLock = std::shared_lock<Mutex>;

  inline const std::thread& thread() const { return thread_; }
  inline std::thread&       thread() { return thread_; }

  inline const Mutex& mutex() const { return mutex_; }
  inline Mutex&       mutex() { return mutex_; }

  inline const std::atomic<bool>& running() const { return running_; }

  inline const std::condition_variable& signal() const { return signal_; }
  inline std::condition_variable&       signal() { return signal_; }

 protected:
  std::atomic<bool> running_;
  mutable Mutex           mutex_;
  std::thread thread_;
  std::condition_variable signal_;
};

NAMESPACE_END

#endif  // LIB_CORE_LISTENER_HPP_
