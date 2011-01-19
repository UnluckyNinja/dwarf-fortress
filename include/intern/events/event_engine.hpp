/**
 * @file
 * @date 22 juin 2010
 * @todo comment
 */

#ifndef EVENT_ENGINE_HPP_
#define EVENT_ENGINE_HPP_

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <boost/signals2.hpp>

#include "meta/singleton.hpp"
#include "meta/engine.hpp"

#include "event.hpp"
#include "queue.hpp"

struct all_true {
    typedef bool result_type;
    typedef ::boost::signals2::signal< bool(void), all_true >::slot_call_iterator iterator_t;

    bool operator()(iterator_t first, iterator_t last) const {
      bool result = false;
      for (; first != last; ++first) {
        result = result || !(*first);
      }

      return !result;
    }
};

struct event_engine: public singleton< event_engine > , public engine< event_engine > {
    static const char engine_name[];

    ::boost::signals2::signal< void(void) > queues_dispatch;
    ::boost::signals2::signal< bool(void), all_true > queues_are_empty;
    ::boost::signals2::signal< void(void) > queues_debug;

    template< typename event_format_t >
    void register_queue() {
      typedef dispatch_event_queue< event_format_t > queue_t;

      queue_t& queue = queue_t::instance();
      ::boost::unique_lock< queue_t > lock(queue);
      if (!queue.is_initialized()) {
        queues_dispatch.connect(::boost::bind< void >(::boost::mem_fn(&queue_t::dispatch), &queue));
        queues_are_empty.connect(::boost::bind< bool >(::boost::mem_fn(&queue_t::is_empty), &queue));
        queues_debug.connect(::boost::bind< void >(::boost::mem_fn(&queue_t::debug), &queue));
        queue.set_initialized();
      }
      lock.unlock();
    }

    template< typename event_format_t >
    void fire(const event< event_format_t >& event) {
      typedef dispatch_event_queue< event_format_t > queue_t;

      register_queue< event_format_t > ();

      queue_t::instance().push_event(event);

      notify();
    }

    template< typename event_format_t >
    ::boost::signals2::connection on_event(
                                           typename dispatch_event_queue< event_format_t >::dispatch_function_t function) {
      typedef dispatch_event_queue< event_format_t > queue_t;

      return queue_t::instance().connect(function);
    }

    template< typename event_format_t, typename UnaryOperator >
    void transform_future(UnaryOperator unary_op, const ::boost::system_time& from = ::boost::get_system_time()) {
      typedef dispatch_event_queue< event_format_t > dispatch_queue_t;
      typedef checking_event_queue< event_format_t > checking_queue_t;

      dispatch_queue_t::instance().transform_future(unary_op, from);
      checking_queue_t::instance().transform_future(unary_op, from);
    }

    void do_tick();
};

#endif /* EVENT_ENGINE_HPP_ */
