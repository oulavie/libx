
#include "html.h"
#include "http.h"
#include "mamMisc.h"
#include "mamMultiplexer.h"
#include "mamNet.h"
#include "url.h"
//#include "mamNetSSL.h"
#include <functional>
#include <iostream>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <map>
#include <set>
#include <string>

// https://prdeving.wordpress.com/2016/08/26/keylogger-linux-c/

namespace
{

//#define likely(x) __builtin_expect((x), 1)
//#define unlikely(x) __builtin_expect((x), 0)

//------------------------------------------------------------------------------
template <char SEPARATOR = '\1', typename Token> bool tokenize(const char *data_, size_t len_, Token token) noexcept
{
  size_t tagPos = 0;
  for (uint16_t i(0); i < len_; ++i)
  {
    if (data_[i] == SEPARATOR)
    {
      token(tagPos, i);
      tagPos = i + 1;
    }
  }
  if (tagPos != len_)
    token(tagPos, len_);
  return true;
}

//------------------------------------------------------------------------------
template <char SEPARATOR = '\1', typename Token>
bool tokenize_tag_val(const char *data_, uint16_t len_, Token token) noexcept
{
  int tagPos = 0;
  int valPos = 0;
  for (uint16_t i(0); i < len_; ++i)
  {
    if (data_[i] == '=')
    {
      valPos = i;
      for (; i < len_; ++i)
      {
        if (data_[i] == SEPARATOR)
        {
          token(tagPos, valPos, i);
          tagPos = i + 1;
          break;
        }
      }
    }
  }
  token(tagPos, valPos, len_);
  return true;
}

} // namespace

//--------------------------------------------------------------------------------------------------
class htmlclient
{
public:
  void send(int fd_)
  {
    std::string date = mam::date();
    // clang-format off
    mam::html a("ESOPE",
      new mam::h<1>(date),
      new mam::div("",
        new mam::fieldset( "login 1/2",
          new mam::form("login_1_1",
            //new mam::input("hidden", "ip", "", "id='00NF000000DAKs6'", false),
            new mam::input("text", "ip", "", "id='00NF000000DAKs6'", false),
            new mam::label(" email: "),
            new mam::input("email","login_mail","","placeholder='email'",true),
            new mam::label(" password: "),
            new mam::input("password","login_password","","placeholder='password'",true),
//                  new mam::textasis("<p id='demo'></p>"),
            new mam::input("submit", "request_id", "request id ..."),
            new mam::br()
          )
        )
      ) /*,
      new mam::div("",
        new mam::fieldset( "login 2/2",
          new mam::form("login_1_2",
            new mam::label("received id:"),
            new mam::input("text","login_id","","placeholder='received id here'",true),
            new mam::input("submit", "request_login", "request login"),
            new mam::br()
            //new mam::input("button", "request", "send"),
            //new mam::input("reset", "reset1", "reset2")
          )
        )
      ),
      new mam::form("form_action_2",
        new mam::div("",
          new mam::fieldset( "a",
            new mam::input("checkbox","subscribe","email"),
            new mam::label("Subscribe to email ?"),
            new mam::input("checkbox","subscribe","newsletter"),
            new mam::label("Subscribe to newsletter ?"),
            new mam::input("submit", "request", "send")
          )
        )
      ),
      new mam::form("form_action_3",
        new mam::div("",
          new mam::fieldset( "b",
            new mam::label("Subscribe to email ?", new mam::input("radio","subscribe2","email2")),
            new mam::label("Subscribe to newsletter ?", new mam::input("radio","subscribe2","newsletter2")),
            new mam::input("submit", "request", "send")
          )
        )
      ),
      new mam::form("form_action_4",
        new mam::div("",
          new mam::fieldset( "c",
            new mam::input("text","mytext",""),
            new mam::input("submit", "request", "send")
          )
        )
      ),
      new mam::form("form_action_5",
        new mam::div("",
          new mam::fieldset( "date/time",
            new mam::input("date","mydate",""), new mam::br(),
            new mam::input("time","mytime",""), new mam::br(),
            new mam::input("week","myweek",""), new mam::br(),
            new mam::input("month","mymonth",""),
            new mam::input("submit", "request", "send")
          )
        )
      ),
      new mam::form("form_action_6",
        new mam::div("",
          new mam::fieldset( "d",
            new mam::label("Select a pet: "),
            new mam::select("pets",{ {"dog","Dog"},{"cat","Cat"},{"hamster","Hamster"}, }),
            new mam::input("submit", "request", "send")
          )
        )
      )
      */
    );
    // clang-format on
    std::stringstream oss;
    a.gett(oss);
    std::string s = oss.str();
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << s << AT << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    auto http = http::get_http(s.data());
    mam::tcp_write(fd_, (uint8_t *)http.data(), http.length());
  }
};

//--------------------------------------------------------------------------------------------------
class htmlclients
{
  std::set<int> _fds = {};
  std::map<int, std::unique_ptr<htmlclient>> _clients = {};

public:
  void insert_new_client(int new_fd_)
  {
    _fds.insert(new_fd_);
  }
  bool is_client(int fd_)
  {
    auto found = _fds.find(fd_);
    return found != _fds.end();
  }
  void handle_read(int fd_, const uint8_t *buf_, size_t len_)
  {
    if (fd_ == -1)
    {
      _fds.erase(fd_);
    }
    else
    {
      // checker si fd_ est deja occupe dans _clients
      //
      // http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
      // GET / HTTP/1.1
      // GET /favicon.ico HTTP/1.1
      // GET /login_1_1?ip=86.233.161.245&login_mail=a%40q&login_password=a&request_id=request+i
      // GET /login_1_1?ip=86.233.161.245&login_mail=a@q&login_password=a&request_id=request id ... HTTP/1.1

      //
      auto current = buf_;
      auto end = buf_ + len_;
      for (; current < end; ++current)
      {
        if (*current == '\r' || *current == '\n')
          break;
      }

      std::vector<std::string> args;
      tokenize<' '>((char *)buf_, current - buf_, [&](size_t begin_, size_t end_) {
        std::string s((char *)buf_ + begin_, end_ - begin_);
        args.push_back(s);
      });

      // http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
      // /login_1_1?ip=142.116.239.216&login_mail=a@q&login_password=b&request_id=request id ...
      args[1] = pbx::url_decode(args[1]);
      std::size_t pos = args[1].find("?");

      std::string host { args[1].data(), pos };
      char *ptr = args[1].data() + pos + 1;
      size_t len = args[1].length() - pos - 1;

      std::vector<std::pair<std::string, std::string>> queries;
      tokenize_tag_val<'&'>(ptr, len, [&](size_t begin_, size_t equal_, size_t end_) {
        queries.push_back(
            {std::string(ptr + begin_, equal_ - begin_), std::string(ptr + equal_ + 1, end_ - equal_ - 1)});
      });

      std::cout << "=1===============================================================================" << std::endl;
      for (size_t i(0); i < args.size(); ++i)
        std::cout << args[i] << AT << std::endl;
      std::cout << "HOST  : [" << host  << "]" << AT << std::endl;
      for (size_t i(0); i < queries.size(); ++i)
        std::cout << "QUERY : [" << queries[i].first << "] == [" << queries[i].second << "] " << AT << std::endl;
      std::cout << "=2===============================================================================" << std::endl;
      send(fd_);
    }
  }
  void send(int fd_)
  {
    std::string date = mam::date();
    // clang-format off
    mam::html a("ESOPE",
      new mam::h<1>(date),
      new mam::div("",
        new mam::fieldset( "login 1/2",
          new mam::form("login_1_1",
            //new mam::input("hidden", "ip", "", "id='00NF000000DAKs6'", false),
            new mam::input("text", "ip", "", "id='00NF000000DAKs6'", false),
            new mam::label(" email: "),
            new mam::input("email","login_mail","","placeholder='email'",true),
            new mam::label(" password: "),
            new mam::input("password","login_password","","placeholder='password'",true),
//                  new mam::textasis("<p id='demo'></p>"),
            new mam::input("submit", "request_id", "request id ..."),
            new mam::br()
          )
        )
      ) /*,
      new mam::div("",
        new mam::fieldset( "login 2/2",
          new mam::form("login_1_2",
            new mam::label("received id:"),
            new mam::input("text","login_id","","placeholder='received id here'",true),
            new mam::input("submit", "request_login", "request login"),
            new mam::br()
            //new mam::input("button", "request", "send"),
            //new mam::input("reset", "reset1", "reset2")
          )
        )
      )*/
    );
    // clang-format on
    std::stringstream oss;
    a.gett(oss);
    std::string s = oss.str();
    std::cout << ".1..............................................................................." << std::endl;
    std::cout << s << AT << std::endl;
    std::cout << ".2..............................................................................." << std::endl;
    auto http = http::get_http(s.data());
    mam::tcp_write(fd_, (uint8_t *)http.data(), http.length());
  }
};

//--------------------------------------------------------------------------------------------------
class engine_t
{
  mam::multiplexer _multiplex = {};
  int _stdin_fd = -1, _timer_fd = -1, _signal_fd = -1, _tcp_fd = -1;
  std::string _stdin = {};

  htmlclients _htmlclients = {};

  int _server_fd = -1;

  static void static_handler(int fd_, void *context_, const struct ::epoll_event &events_)
  {
    try
    {
      using this_t = engine_t;
      this_t *eng = reinterpret_cast<this_t *>(context_);
      /*
      if (events_.events & (EPOLLERR || EPOLLHUP))
      {
        epoll_ctl( efd, EPOLL_CTL_DEL, fd_, NULL)
        close(fd_);
      }
      else if (events_.events & (EPOLLIN))
      {
        eng->handle_read(fd_);
      }
      */
      if (events_.events & (EPOLLIN || EPOLLERR || EPOLLHUP))
      {
        eng->handle_read(fd_);
      }
      else if (events_.events & (EPOLLOUT))
      {
        eng->handle_write(fd_);
      }
    }
    catch (const std::system_error &e)
    {
    }
    catch (const std::runtime_error &e)
    {
      // throw std::runtime_error("sadasd");
    }
    catch (const std::exception &e)
    {
      std::cerr << "Exception: " << e.what() << "\n" << std::flush;
    }
    catch (...)
    {
      std::cout << "--- the end --- the end --- the end --- the end --- the end --- the end ---" << AT << std::endl;
      exit(0);
    }
  }

public:
  engine_t()
  {
    std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
    //_stdin_fd = mam::create_stdin_fd();
    //_multiplex.register_fd(_stdin_fd, this, static_handler);

    //_timer_fd = mam::create_timer_fd(1000);
    //_multiplex.register_fd(_timer_fd, this, static_handler);

    //_signal_fd = mam::create_signal_fd( SIGINT ); // SIGALRM alarm(3);
    //_signal_fd = mam::create_signal_fd(SIGINT, SIGALRM);
    //_signal_fd = mam::create_signal_fd({SIGINT, SIGALRM});
    //_multiplex.register_fd(_signal_fd, this, static_handler);
    /*
        struct sockaddr_in ipv4; // created from url_/port_
        if (mam::host_port_to_sockaddr_in(ipv4, "api.ipify.org", 80) == 0)
        {
          _tcp_fd = mam::tcp_client_init(&ipv4, IPPROTO_TCP);
          if (_tcp_fd > 0)
          {
            _multiplex.register_fd(_tcp_fd, this, static_handler);
          }
        }
    */

    // --- server http -----------------------------------------------------------------------------
    _server_fd = mam::server_tcp_fd("12345");
    if (_server_fd > 0)
    {
      std::cout << __PRETTY_FUNCTION__ << " _server_fd=" << _server_fd << AT << std::endl;
      _multiplex.register_fd(_server_fd, this, static_handler);
      if (!mam::server_listen(_server_fd))
        std::cerr << __PRETTY_FUNCTION__ << "mam::server_listen FAILED _server_fd=" << _server_fd << AT << std::endl;
    }
    else
      std::cerr << __PRETTY_FUNCTION__ << "mam::server_tcp_fd FAILED _server_fd=" << _server_fd << AT << std::endl;
  }
  ~engine_t()
  {
    std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
    _multiplex.unregister_close_fd(_stdin_fd);
  }
  engine_t(const engine_t &) = delete;
  engine_t &operator=(const engine_t &) = delete;
  engine_t(engine_t &&) = delete;
  engine_t &operator=(engine_t &&) = delete;

  void run()
  {
    std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
    _multiplex.run();
  }
  void handle_write(int)
  {
    std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
  }
  void handle_read(int fd_)
  {
    std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << " [[" << mam::date() << "]]" << AT << std::endl;

    uint32_t nanosec;
    uint32_t sec = mam::now_as_sec(&nanosec);
    if (fd_ == _timer_fd)
    {
      mam::read_timer_fd(fd_, [&](int timer_elapsed_) {
        std::cout << sec << " " << nanosec << " " << timer_elapsed_ << AT << std::endl;
        if (sec % 10 == 0)
        {
          alarm(3);
          if (_tcp_fd != -1)
            write_tcp(_tcp_fd);
        }
      });
    }
    else if (fd_ == _stdin_fd)
    {
      mam::read_stdin_fd(fd_, [&](const std::vector<char> &stdin_) {
        for (auto c : stdin_)
        {
          _stdin += c;
        }
      });
      std::cout << _stdin.data() << AT << std::endl;
      _stdin.clear();
    }
    else if (fd_ == _signal_fd)
    {
      mam::read_signal_fd(fd_, [&](uint32_t ssi_signo) {
        std::cout << "SIGNAL : " << ssi_signo << AT << std::endl;
        if (ssi_signo == SIGINT)
          // exit(0);
          throw(SIGINT);
      });
    }
    else if (fd_ == _tcp_fd)
    {
      const size_t bufsz = 1024 * 1024;
      uint8_t buffer[bufsz];
      size_t sz = bufsz;
      _tcp_fd = read_tcp_fd(fd_, buffer, sz);
    }
    else if (fd_ == _server_fd)
    {
      int new_fd = mam::server_accept_fd(fd_);
      _multiplex.register_fd(new_fd, this, static_handler);
      _htmlclients.insert_new_client(new_fd);
    }
    else if (_htmlclients.is_client(fd_))
    {
      const size_t bufsz = 1024 * 1024;
      uint8_t buffer[bufsz];
      size_t sz = bufsz;
      int fd = read_tcp_fd(fd_, (uint8_t *)buffer, sz);
      _htmlclients.handle_read(fd, buffer, sz);
    }
  }
  int read_tcp_fd(int fd_, uint8_t *buffer_, size_t &buffersize_)
  {
    std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << AT << std::endl;
    ssize_t error;
    buffersize_ = mam::tcp_read(fd_, buffer_, buffersize_, error);
    if (buffersize_ == 0)
    {
      std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << " read()=0 ==> CLOSING" << AT << std::endl;
      _multiplex.unregister_close_fd(fd_);
      fd_ = -1;
    }
    else
    {
      /*
      std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << " read()=" << buffersize_ << " :" << AT << std::endl;
      std::string toto((char *)buffer_, buffersize_);
      std::cout << toto << std::endl;
      std::cout << __PRETTY_FUNCTION__ << " fd=" << fd_ << " ... the end!" << AT << std::endl;
      */
    }
    return fd_;
  }
  void write_tcp(int fd_)
  {
    std::cout << __PRETTY_FUNCTION__ << AT << std::endl;
    std::string url("api.ipify.org");
    std::string get_http = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
    mam::tcp_write(fd_, (uint8_t *)get_http.data(), get_http.length());
    std::cout << "************************************ write_tcp " << get_http << AT << std::endl;
  }
};

//--------------------------------------------------------------------------------------------------
int main()
{
  engine_t toto;
  toto.run();
  return 0;
}

// clang-format off
// clang-format-9 -i main.cpp
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a  main.cpp
// g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto
// g++ -std=c++17 -Wall -Wextra -Wpedantic -Wno-deprecated-declarations  main.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
