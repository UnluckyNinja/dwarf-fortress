/**
 * @file
 * @date 16 juil. 2010
 * @todo comment
 */

#include "meta/engine.hpp"

engine_base::engine_base() {
  min_tick_duration = ::boost::posix_time::milliseconds(5);
  max_tick_duration = ::boost::posix_time::milliseconds(200);
  stopped = false;
}

void engine_base::stop() {
  stopped = true;
}

void engine_base::notify() {
  engine_notifier.notify_one();
}

void engine_base::set_min_tick_duration(::boost::int64_t millis) {
  ::boost::unique_lock< ::boost::mutex > lock(engine_mutex);

  min_tick_duration = ::boost::posix_time::milliseconds(millis);
}

void engine_base::set_max_tick_duration(::boost::int64_t millis) {
  ::boost::unique_lock< ::boost::mutex > lock(engine_mutex);

  max_tick_duration = ::boost::posix_time::milliseconds(millis);
}
