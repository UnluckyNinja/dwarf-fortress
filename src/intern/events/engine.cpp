/**
 * @file
 * @date 15 juil. 2010
 * @todo comment
 */

#include "intern/events/event_engine.hpp"

const char event_engine::engine_name[] = "event engine";

void event_engine::do_tick() {
//  ::boost::system_time start = ::boost::get_system_time();
//  ::boost::system_time end;
//  ::boost::posix_time::time_duration elapsed;
//
//  while (!queues_are_empty() && elapsed < max_tick_duration) {
    //    queues_debug();
    queues_dispatch();

//    end = ::boost::get_system_time();
//    elapsed = end - start;
//
//    ::boost::this_thread::interruption_point();
//  }
}
