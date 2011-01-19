/**
 * @file
 * @date 13 juil. 2010
 * @todo comment
 */

#ifndef INTERN_EVENTS_EVENT_HPP_
#define INTERN_EVENTS_EVENT_HPP_

#include <boost/date_time.hpp>
#include <boost/thread.hpp>

struct event_base {
  protected:
    static const ::boost::posix_time::time_duration min_delay;

    ::boost::system_time datetime;
    bool cancelled;

  public:
    event_base(const ::boost::posix_time::time_duration& delay = min_delay);
    event_base(const event_base& event);

    void cancel();
    bool is_cancelled() const;

    ::boost::system_time get_datetime() const;

    /**
     * Events are sorted by their status first, alive first, then cancelled.
     * Then by their datetime, in reverse order, most recent or future first.
     * So we can easily remove old and cancelled events from the back.
     */
    bool operator<(const event_base& event) const;
};

/**
 * Event wrapper.
 */
template< typename event_format_t >
struct event: public event_format_t, public event_base {
    typedef event< event_format_t > event_type_t;

  public:
    event(const ::boost::posix_time::time_duration& delay = min_delay) :
      event_format_t(), event_base() {
    }

    event(const event< event_format_t >& event, const ::boost::posix_time::time_duration& delay) :
      event_format_t(dynamic_cast< const event_format_t& > (event)), event_base(delay) {
    }

    event(const event< event_format_t >& event) :
      event_format_t(dynamic_cast< const event_format_t& > (event)), event_base(event) {
    }

    bool operator<(const event_type_t& event) const {
      return event_base::operator <(event);
    }
};

#endif /* INTERN_EVENTS_EVENT_HPP_ */
