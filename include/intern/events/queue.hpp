/**
 * @file
 * @date 13 juil. 2010
 * @todo comment
 */

#ifndef INTERN_EVENTS_QUEUE_HPP_
#define INTERN_EVENTS_QUEUE_HPP_

#include <deque>
#include <vector>
#include <sstream>

#include <boost/thread.hpp>

#include "meta/singleton.hpp"

#include "event.hpp"

/**
 * @todo Create two different queue models. One dispatching queue that will run asynchronously and
 *        call event dispatch callback for each event engine tick.
 *        One checking queue model that will permit a single thread to check for new events in the queue,
 *        deleting them (or not ? ... yeah deleting them), and get the events synchronously.
 */

template< typename event_format_t >
struct event_queue_base {
    typedef event_queue_base< event_format_t > queue_t;
    typedef event< event_format_t > event_type_t;

    typedef ::std::deque< event_type_t > queue_type_t;
    typedef typename queue_type_t::iterator iterator_t;

    static const int max_treated_events;

  protected:
    ::boost::mutex mutex;

    queue_type_t events;

    bool initialized;

    struct is_past {
        typedef bool result_type;

        bool operator()(const ::boost::system_time& now, const event_type_t& event) {
          return event.get_datetime() <= now;
        }
    };

    struct is_cancelled {
        typedef bool result_type;

        bool operator()(const event_type_t& event) {
          return event.is_cancelled();
        }
    };

    void cleanup_queue() {
      // We have to sort the event queue so we can separate cancelled events, and past from future events.
      ::std::sort(events.begin(), events.end());

      // Cleanup by removing cancelled events.
      iterator_t from = ::std::find_if(events.begin(), events.end(), ::boost::bind(is_cancelled(), _1));
      events.erase(from, events.end());
    }

    ::std::vector< event_type_t > get_current_events() {
      ::boost::unique_lock< queue_t > lock(*this);
      ::boost::system_time now = ::boost::get_system_time();

      cleanup_queue();

      // Find first past event.
      // No need to start searching earlier than the maximum events we are allowed to handle at once.
      iterator_t from = events.begin();
      if (events.end() - from > max_treated_events) {
        from = events.end() - max_treated_events;
      }
      from = ::std::find_if(from, events.end(), ::boost::bind(is_past(), now, _1));
      iterator_t to = events.end();

      // We have now our current events to dispatch, we can remove them from the queue.
      ::std::vector< event_type_t > current_events(from, to);
      events.erase(from, to);
      lock.unlock();

      ::std::reverse(current_events.begin(), current_events.end());

      return current_events;
    }

  public:
    event_queue_base() {
      initialized = false;
    }

    bool is_initialized() const {
      return initialized;
    }

    void set_initialized() {
      initialized = true;
    }

    bool is_empty() {
      return events.empty();
    }

    void push_event(const event_type_t& event) {
      ::boost::lock_guard< queue_t > lock(*this);

      events.push_back(event);
    }

    template< typename UnaryOperator >
    void transform_future(UnaryOperator unary_op, const ::boost::system_time& from) {
      ::boost::lock_guard< queue_t > lock(*this);

      cleanup_queue();

      // Find first past event.
      // No need to start searching earlier than the maximum events we are allowed to handle at once.
      iterator_t to = ::std::find_if(events.begin(), events.end(), ::boost::bind(is_past(), from, _1));

      ::std::for_each(events.begin(), to, unary_op);
    }

    struct event_printer {
        typedef void result_type;

        void operator()(::std::ostringstream& out, const event_type_t& event) {
          ::boost::system_time now = ::boost::get_system_time();
          ::boost::posix_time::time_duration duration(event.get_datetime() - now);

          if (event.is_cancelled()) {
            out << "X ";
          } else {
            for (int i = 0; i <= ::std::abs(duration.total_milliseconds()) / 100; ++i) {
              if (duration.total_milliseconds() >= 0) {
                out << ">";
              } else {
                out << "<";
              }
            }
            out << " ";
          }
        }
    };

    void debug() {
      ::std::ostringstream out;
      out << event_format_t::event_name << " [" << events.size() << "] ";
      ::std::for_each(events.begin(), events.end(), ::boost::bind(event_printer(), ::boost::ref(out), _1));

      __infoM(event_queue) << out.str();
    }

    void lock() {
      mutex.lock();
    }

    void try_lock() {
      mutex.try_lock();
    }

    void unlock() {
      mutex.unlock();
    }
};

template< typename event_format_t >
const int event_queue_base< event_format_t >::max_treated_events = 100;

template< typename event_format_t >
struct dispatch_event_queue: public event_queue_base< event_format_t > , public singleton< dispatch_event_queue<
    event_format_t > > {
    using event_queue_base< event_format_t >::get_current_events;

    typedef typename event_queue_base< event_format_t >::event_type_t event_type_t;
    typedef typename event_queue_base< event_format_t >::queue_t queue_t;

    typedef ::boost::signals2::signal< void(const event_type_t&) > dispatch_slot_t;
    typedef typename dispatch_slot_t::slot_function_type dispatch_function_t;

    void dispatch() {
      typedef void(dispatch_slot_t::* dispatch_ptr_t)(const event_type_t&);

      ::std::vector< event_type_t > current_events = get_current_events();

      dispatch_ptr_t dispatch_ptr = static_cast< dispatch_ptr_t > (&dispatch_slot_t::operator());
      dispatch_function_t dispatch_fun = ::boost::bind(dispatch_ptr, &dispatch_event, _1);

      if (current_events.size() > 0) {
        ::std::for_each(current_events.begin(), current_events.end(), dispatch_fun);
      }
    }

    ::boost::signals2::connection connect(dispatch_function_t function) {
      ::boost::lock_guard< queue_t > lock(*this);

      return dispatch_event.connect(function);
    }

  protected:
    dispatch_slot_t dispatch_event;
};

template< typename event_format_t >
struct checking_event_queue: public event_queue_base< event_format_t > , public singleton< checking_event_queue<
    event_format_t > > {

};

#endif /* INTERN_EVENTS_QUEUE_HPP_ */
