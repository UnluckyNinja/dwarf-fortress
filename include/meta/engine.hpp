/**
 * @file
 * @date 15 juil. 2010
 * @todo comment
 */

#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include <logging/logging.hpp>

struct engine_base {
  protected:
    ::boost::posix_time::time_duration min_tick_duration;
    ::boost::posix_time::time_duration max_tick_duration;
    ::boost::condition_variable engine_notifier;
    bool stopped;

    ::boost::mutex engine_mutex;

    engine_base();

  public:
    void stop();

    void notify();

    void set_min_tick_duration(::boost::int64_t millis);
    void set_max_tick_duration(::boost::int64_t millis);
};

template< class engine_type_t >
struct engine: public engine_base {
  protected:
    void run() {
      try {
        ::boost::unique_lock< ::boost::mutex > lock(engine_mutex);

        while (!stopped) {
          ::boost::system_time start = ::boost::get_system_time();
          ::boost::system_time end;
          ::boost::posix_time::time_duration elapsed;

          engine_type_t::instance().do_tick();

          end = ::boost::get_system_time();
          elapsed = end - start;

          if (min_tick_duration > elapsed) {
            engine_notifier.timed_wait(lock, min_tick_duration - elapsed);
          } else if (max_tick_duration < elapsed) {
            __warnM(engine)
              << engine_type_t::engine_name << " tick took a total of " << elapsed.total_milliseconds()
                  << "ms over a maximum of " << max_tick_duration.total_milliseconds() << "ms.";
          }
        }
      } catch (::boost::thread_interrupted& e) {
        __warnM(engine)
          << engine_type_t::engine_name << " interrupted.";
      }
    }

    struct engine_callable {
        void operator()() {
          engine_type_t::instance().run();
        }
    };

  public:
    static ::boost::thread start() {
      engine_callable callable;

      return ::boost::thread(callable);
    }
};

#endif /* ENGINE_HPP_ */
