#ifndef LIB_MODBUS_MODBUS_LOGGER_HPP_
#define LIB_MODBUS_MODBUS_LOGGER_HPP_

#include <string>
#include <type_traits>
#include <utility>

#include <boost/core/noncopyable.hpp>

#include <fmt/format.h>

namespace modbus {
class logger : private boost::noncopyable {
 public:
  /**
   * Create singleton instance
   *
   * @tparam Logger  logger derivative type
   * @tparam Args    arguments type
   *
   * @param  args    arguments to pass to constructor
   */
  template <typename Logger = logger,
            typename... Args,
            typename = std::enable_if_t<std::is_base_of_v<logger, Logger>>>
  inline static void create(Args&&... args) {
    if (instance_ == nullptr) {
      static Logger instance(std::forward<Args>(args)...);
      instance_ = &instance;
    }
  }

  /**
   * Set singleton instance
   *
   * @tparam Logger  logger derivative type
   *
   * @param  instance__ pointer of logger derivative type
   */
  template <typename Logger = logger,
            typename = std::enable_if_t<std::is_base_of_v<logger, Logger>>>
  inline static void set(Logger* instance) {
    instance_ = instance;
  }

  /**
   * Get singleton pointer
   *
   * @return logger pointer
   */
  static logger* get();

  /**
   * logger constructor
   *
   * @param debug  debug status
   */
  explicit logger(bool debug = false);

  /**
   * logger destructor
   */
  virtual ~logger();

  /**
   * Set debug status
   *
   * @param debug__ debug status
   */
  inline void set_debug(bool debug__) noexcept { debug_ = debug__; }

  /**
   * Log info message to stdout
   *
   * @tparam FormatString string format type
   * @tparam Args         arguments type
   *
   * @param  fmt          string format
   * @param  args         arguments
   */
  template <typename FormatString, typename... Args>
  inline void info(const FormatString& fmt, Args&&... args) const {
    info(fmt::format(fmt, std::forward<Args>(args)...));
  }

  /**
   * Log error message to stdout
   *
   * @tparam FormatString string format type
   * @tparam Args         arguments type
   *
   * @param  fmt          string format
   * @param  args         arguments
   */
  template <typename FormatString, typename... Args>
  inline void error(const FormatString& fmt, Args&&... args) const {
    error(fmt::format(fmt, std::forward<Args>(args)...));
  }

  /**
   * Log debu message to stdout
   *
   * @tparam FormatString string format type
   * @tparam Args         arguments type
   *
   * @param  fmt          string format
   * @param  args         arguments
   */
  template <typename FormatString, typename... Args>
  inline void debug(const FormatString& fmt, Args&&... args) const {
    debug(fmt::format(fmt, std::forward<Args>(args)...));
  }

  /**
   * Log info message to stdout
   *
   * @param message message to log
   */
  virtual void info(const std::string& message) const noexcept;

  /**
   * Log error message to stdout
   *
   * @param message message to log
   */
  virtual void error(const std::string& message) const noexcept;

  /**
   * Log debug message
   *
   * @param message message to log
   */
  virtual void debug(const std::string& message) const noexcept;

 protected:
  /**
   * Debug
   */
  bool debug_;

 private:
  /**
   * Singleton instance
   */
  static logger* instance_;
};
}

#endif // LIB_MODBUS_MODBUS_LOGGER_HPP_



