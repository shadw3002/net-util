#pragma once

#include <functional>

class EventLoop;

class Channel
{
public:
  using Callback = std::function<void()>;

  Channel(EventLoop* owner_loop, int fd);

  int fd() const;

  int events_watch() const;
  void set_events_watch(int events);

  int events_recv() const;
  void set_events_recv(int events);

  void enable_read();
  void enable_write();
  void disable_write();
  void disable_all();

  void set_read_callback(Callback callback);
  void set_write_callback(Callback callback);
  void set_error_callback(Callback callback);

  void handle_event();

private:

  void update();

  EventLoop* m_owner_loop;

  int m_fd;

  int m_events_watch;

  int m_events_recv;

  Callback m_read_callback;
  Callback m_write_callback;
  Callback m_error_callback;
};

