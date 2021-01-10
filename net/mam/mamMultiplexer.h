
#pragma once
#ifndef mam_multiplexer_h
#define mam_multiplexer_h

#include <iostream>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h> // shutdown
#include <unistd.h>
#include <vector>

namespace mam
{

using handler_t = void (*)(int fd_, void *context_, const struct ::epoll_event &events_);

//--------------------------------------------------------------------------------------------------
class multiplexer
{
  struct fd_t
  {
    void *_context = nullptr;
    handler_t _handler = nullptr;
  };
  std::vector<fd_t> _fd = {};
  std::size_t _no_active_fd = 0;

  int _epoll_fd = -1;
  static constexpr const int epoll_events_size = 32;
  struct epoll_event _epoll_events[epoll_events_size];

public:
  multiplexer()
  {
    std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
    // https://man7.org/linux/man-pages/man2/epoll_create.2.html
    _epoll_fd = ::epoll_create1(0);

    memset(_epoll_events, 0, sizeof(_epoll_events));
    _fd.resize(32);
  }
  ~multiplexer()
  {
    std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
    for (size_t fd = 0; fd < _fd.size(); ++fd)
    {
      unregister_close_fd(fd);
    }
    close(_epoll_fd);
  }
  void register_fd(int fd_, void *context_, handler_t handler_)
  {
    if (fd_ < 0 || context_ == nullptr || handler_ == nullptr)
    {
      std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << " FAILED" << AT << std::endl;
      return;
    }
    std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << AT << std::endl;
    if ((size_t)fd_ >= _fd.size())
    {
      auto sz = std::max((size_t)fd_ + 1, _fd.size() * 2);
      _fd.resize(sz);
    }
    struct epoll_event evt;
    memset(&evt, 0, sizeof(evt));
    // evt.data.ptr   quelque chose qui existe
    evt.data.fd = fd_;
    evt.events = EPOLLERR | EPOLLHUP;
    // evt.events |= EPOLLET; // if edge trigger
    evt.events |= EPOLLIN; // wait for read
    // evt.events |= EPOLLOUT; // wait for write
    if (_fd[fd_]._handler != nullptr)
    {
      ::epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd_, &evt);
    }
    else
    {
      ::epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd_, &evt);
    }
    _fd[fd_]._context = context_;
    _fd[fd_]._handler = handler_;
    ++_no_active_fd;
    std::cout << __PRETTY_FUNCTION__ << " _no_active_fd=" << _no_active_fd << AT << std::endl;
  }
  void unregister_close_fd(int fd_)
  {
    if (fd_ < 0 || (size_t)fd_ >= _fd.size() || _fd[fd_]._handler == nullptr)
    {
      std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << " FAILED" << AT << std::endl;
      return;
    }
    ::epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd_, nullptr);
    _fd[fd_] = fd_t{};
    shutdown(fd_, SHUT_RDWR); // ?????????????
    close(fd_);
    --_no_active_fd;
    std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << " _no_active_fd=" << _no_active_fd << AT << std::endl;
  }
  std::size_t poll(int timeout_ = 0)
  {
    int toprocess = 0;
    if (_epoll_fd > 0)
    {
      toprocess = ::epoll_wait(_epoll_fd, _epoll_events, epoll_events_size, timeout_);
      if (toprocess > 0)
      {
        for (int i(0); i < toprocess; ++i)
        {
          int fd = _epoll_events[i].data.fd;
          if (fd >= 0 && fd < (int)_fd.size())
          {
            auto &sfd = _fd[fd];
            sfd._handler(fd, sfd._context, _epoll_events[i]);
          }
        }
      }
      else if (toprocess == -1)
      {
        // int err = errno;
      }
    }
    return toprocess;
  }
  void run()
  {
    while (_no_active_fd > 0)
    {
      poll();
    }
  }

  multiplexer(const multiplexer &) = delete;
  multiplexer &operator=(const multiplexer &) = delete;
  multiplexer(multiplexer &&) = delete;
  multiplexer &operator=(multiplexer &&) = delete;
};

} // namespace mam

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
