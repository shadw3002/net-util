#pragma once

#include <functional>
#include <sys/time.h>

class Timer
{
public:
  using Callback = std::function<void(Timer*)>;

  Timer(const struct timeval& next_active, const Callback& cb, std::pair<bool, struct timeval> repeat = {false, timeval()});

  const struct timeval& next_active() const;

  void do_callback();

  bool is_repeat() const;

private:
  struct timeval m_interval;

  struct timeval m_next_active;

  Callback m_callback;

  bool m_repeat;
};

Timer::Timer(const struct timeval& next_active, const Callback& cb, std::pair<bool, struct timeval> repeat = {false, timeval()})
  : m_interval(repeat.second)
  , m_next_active(next_active)
  , m_callback(cb)
  , m_repeat(repeat.first)
{

}

const struct timeval& Timer::next_active() const
{
  return m_next_active;
}

void Timer::do_callback()
{
  m_callback(this);

  if (m_repeat) { // long int
    m_next_active.tv_sec += m_interval.tv_sec;
    m_next_active.tv_usec += m_interval.tv_usec;
    m_next_active.tv_sec += m_next_active.tv_usec / 1000000;
    m_next_active.tv_usec = m_next_active.tv_usec % 1000000;
  }
}

bool Timer::is_repeat() const
{
  return m_repeat;
}