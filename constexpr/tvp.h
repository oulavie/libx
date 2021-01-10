#pragma once

#include <assert.h>
#include <boost/utility/string_ref.hpp>
#include <initializer_list>
#include <sstream>
#include <string.h>
#include <string>
//#include <chrono>

// le intclipper est passe en template au reader
// le separateur est passe en parameter aux ctor reader/writer
// reader accepte les tags non mnumerique

namespace
{

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

static inline uint64_t gettime()
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return (t.tv_sec * 1'000'000'000) + t.tv_nsec;
}

// processor time-stamp counter
static inline uint64_t gettime2()
{
  uint32_t hi, lo;
  asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  return (uint64_t)hi << 32 | lo;
}
/*
static inline std::chrono::system_clock::time_point gettime3()
{
  using Clock = std::chrono::system_clock;
  Clock::duration d = std::chrono::nanoseconds(getNanosSinceEpoch());
  return Clock::time_point(d);
}
*/

//----------------------------------------------------------------------------
template <class T, size_t SZ> class array1D
{
public:
  T tb[SZ];
  constexpr size_t size()
  {
    return SZ;
  }
};
template <size_t TAG_MIN, size_t TAG_MAX, size_t SZ, const int(AcceptedTags)[SZ]> constexpr auto MapTable()
{
  array1D<size_t, 2 + TAG_MAX - TAG_MIN> map{};
  for (size_t i(0); i < map.size(); i++)
    map.tb[i] = 0;
  size_t position = 1;
  for (size_t j(0); j < SZ; j++)
  {
    auto tag = AcceptedTags[j];
    auto pos = static_cast<size_t>(tag) - (TAG_MIN - 1);
    if (!map.tb[pos])
      map.tb[pos] = position++;
  }
  return map;
}

} // namespace

//--------------------------------------------------------------------------------------------------
namespace pbx
{
template <typename T, int N> static constexpr size_t get_size(T (&)[N])
{
  return N;
}
template <int... tags> class intclipper
{
public:
  static constexpr int AcceptedTags[] = {tags...};
  static constexpr size_t NB_ENTRIES = get_size(AcceptedTags);
  static constexpr int TAG_MIN = *std::min_element(std::begin(AcceptedTags), std::end(AcceptedTags));
  static constexpr int TAG_MAX = *std::max_element(std::begin(AcceptedTags), std::end(AcceptedTags));
  static constexpr ssize_t MapSize = 2 + TAG_MAX - TAG_MIN;
  static constexpr ssize_t clipTag(int tag) // pixel shading
  {
    tag -= TAG_MIN - 1;
    tag *= tag > 0;
    tag *= tag < 2 + TAG_MAX - TAG_MIN;
    return static_cast<size_t>(tag);
  }
  static constexpr size_t mapTag2Index(int tag)
  {
    constexpr auto _map{MapTable<TAG_MIN, TAG_MAX, NB_ENTRIES, AcceptedTags>()};
    static_assert(_map.tb[1] > 0);
    return _map.tb[clipTag(tag)];
  }

private:
  static constexpr bool compileTimeTestOfClip()
  {
    for (auto tag : AcceptedTags)
    {
      auto clip = clipTag(tag);
      if (!clip)
      {
        throw "accepted tags have clip > 0";
        return false;
      }
      for (auto tag2 : AcceptedTags)
      {
        bool ok = (tag2 == tag) == (mapTag2Index(tag2) == mapTag2Index(tag));
        if (!ok)
        {
          throw "mapTag2Index should be unique for all accepted tags";
          return false;
        }
      }
      auto index = mapTag2Index(tag);
      if (index <= 0)
      {
        throw "index of accepted tags should be >0";
        return false;
      }
      if (index > NB_ENTRIES)
      {
        throw "index of accepted tags should be <=NB_ENTRIES";
        return false;
      }
    }
    for (auto tag = 1; tag < TAG_MAX + 10; tag++)
    {
      auto clip = clipTag(tag);
      if (clip < 0)
      {
        throw "clip < 0 is not allowed outside of maptbl.tb";
        return false;
      }
      if (clip >= MapSize)
      {
        throw "clip >= maptbl.size() is nopt allowed to be outside of maptbl.tb";
        return false;
      }
      if (tag < TAG_MIN || tag > TAG_MAX)
      {
        if (clip)
        {
          throw "tag outside of the accepted tags";
          return false;
        }
      }
      else
      {
        if ((clip - 1 + TAG_MIN) != static_cast<ssize_t>(tag))
        {
          throw "tags between min & max may be clipped";
          return false;
        }
      }
    }
    return true;
  }
  static_assert(compileTimeTestOfClip());
};

// clang-format off
typedef intclipper<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'> numeric_t;
typedef intclipper<'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'> alphabetic_t;
typedef intclipper<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'> alphanum_t;
// clang-format on

} // namespace pbx

namespace
{
//------------------------------------------------------------------------------
// clang-format off
const long ints[16][10] =
{
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90},
  {0, 100, 200, 300, 400, 500, 600, 700, 800, 900},
  {0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000},
  {0, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000},
  {0, 100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000},
  {0, 1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000},
  {0, 10000000, 20000000, 30000000, 40000000, 50000000, 60000000, 70000000, 80000000, 90000000},
  {0, 100000000, 200000000, 300000000, 400000000, 500000000, 600000000, 700000000, 800000000, 900000000},
  {0, 1000000000, 2000000000, 3000000000, 4000000000, 5000000000, 6000000000, 7000000000, 8000000000, 9000000000},
  {0, 10000000000, 20000000000, 30000000000, 40000000000, 50000000000, 60000000000, 70000000000, 80000000000, 90000000000},
  {0, 100000000000, 200000000000, 300000000000, 400000000000, 500000000000, 600000000000, 700000000000, 800000000000, 900000000000},
  {0, 1000000000000, 2000000000000, 3000000000000, 4000000000000, 5000000000000, 6000000000000, 7000000000000, 8000000000000, 9000000000000},
  {0, 10000000000000, 20000000000000, 30000000000000, 40000000000000, 50000000000000, 60000000000000, 70000000000000, 80000000000000, 90000000000000},
  {0, 100000000000000, 200000000000000, 300000000000000, 400000000000000, 500000000000000, 600000000000000, 700000000000000, 800000000000000, 900000000000000},
  {0, 1000000000000000, 2000000000000000, 3000000000000000, 4000000000000000, 5000000000000000, 6000000000000000, 7000000000000000, 8000000000000000, 9000000000000000},
};
// clang-format on

static long fatoi(const char *ptr_, size_t len_)
{
  long rtn(0);
  for (size_t i(0); i < len_; ++i)
  {
    rtn += ints[len_ - i - 1][ptr_[i] - 48];
  }
  return rtn;
}

//----------------------------------------------------------------------------
// static constexpr char SEP = '\1';
// static constexpr char SEP = ';';
struct TVPair
{
  const int tag;
  const char *const content;
  size_t len;
};

static TVPair step(const char *&current_, const char *const end_, const char SEP = '\1')
{
  const auto start_tag = current_;
  const char *equal = {};
  const char *sep = {};
  static pbx::numeric_t numcheck;
  size_t tagnumcheck = 1;
  for (; current_ < end_; ++current_)
  {
    if (*current_ == '=')
    {
      equal = current_;
      current_++;
      break;
    }
    else
    {
      auto pos = numcheck.mapTag2Index(*current_);
      tagnumcheck *= pos;
    }
  }
  if (!equal)
  {
    throw std::runtime_error(" = is missing");
  }
  int tag = 0;
  if (tagnumcheck)
  {
    // tag = std::stoi(std::string(start_tag, static_cast<int>(equal - start_tag)));
    tag = (int)fatoi(start_tag, static_cast<size_t>(equal - start_tag));
  }
  {
    for (; current_ < end_; ++current_)
    {
      if (*current_ == SEP)
      {
        sep = current_;
        current_++;
        break;
      }
    }
  }
  if (!sep)
  {
    throw std::runtime_error("unterminated message");
  }
  if (sep == equal + 1)
  {
    throw std::runtime_error("empty tag");
  }
  return {tag, equal + 1, static_cast<size_t>(sep - (equal + 1))};
}

} // namespace

//--------------------------------------------------------------------------------------------------
namespace pbx
{

//--------------------------------------------------------------------------------------------------
template <typename CLIPPER> class tvp_reader
{
  static constexpr CLIPPER _clipper = {};
  char _sep = '\1';

  using Offset = unsigned short;
  struct Where
  {
    Offset offset = {};
    Offset len = {};
    bool operator==(const Where &w) const
    {
      return offset == w.offset && len == w.len;
    }
  };
  using Wheres = std::array<Where, _clipper.NB_ENTRIES>;
  mutable Wheres _alreadySeenTagsCache = {};
  size_t _originalSize = 0;
  mutable size_t _lastLookupPosition = 0;
  // static constexpr size_t max_lookup = 1024;
public:
  tvp_reader(char sep_ = '\1') : _sep(sep_)
  {
  }
  template <int TAG> boost::string_ref getPair() const
  {
    // uint64_t t1 = gettime2();
    constexpr auto position = _clipper.mapTag2Index(TAG);
    // uint64_t t2 = gettime2();
    // std::cout << t2 - t1 << std::endl;

    try
    {
      const char *const start = _content;
      const char *current = start + _lastLookupPosition;
      const char *const end = start + _originalSize;
      Where &loc = _alreadySeenTagsCache[position - 1];
      // for (; (!loc.offset + loc.len) && (current < end) && (_lastLookupPosition < max_lookup);)
      for (; (current < end);)
      {
        TVPair p = step(current, end, _sep);
        _lastLookupPosition = static_cast<size_t>(current - start);
        auto position = _clipper.mapTag2Index(p.tag);
        if (position != 0)
        {
          _alreadySeenTagsCache[position - 1] = {static_cast<Offset>(p.content - start), static_cast<Offset>(p.len)};
        }
      }
      if (!loc.offset)
        loc.offset = 1;
      return {start + loc.offset, loc.len};
    }
    catch (std::exception &e)
    {
      //_lastLookupPosition = max_lookup;
    }
    return {};
  }

private:
  const char *_content = {};

public:
  bool set(const char *buf, size_t size)
  {
    assert(_alreadySeenTagsCache == Wheres{});
    if ((size > 0) && memcmp(buf + size - 8,
                             "\1"
                             "10=",
                             4) == 0)
    {
      size -= 7;
    }
    _content = buf;
    _originalSize = size;
    return true;
  }
};

//--------------------------------------------------------------------------------------------------
struct tv_t
{
  const char *_tag;
  boost::string_ref _val;

  tv_t(const char *tag_, const boost::string_ref &val_) : _tag(tag_), _val(val_)
  {
  }

  tv_t() = delete;
  tv_t(const tv_t &) = delete;
  tv_t(const tv_t &&) = delete;
  tv_t &operator=(const tv_t &) = delete;
  tv_t &operator=(const tv_t &&) = delete;
};
class tvp_writer
{
public:
  template <class FUNC> void write(const char sep_, const char *fix8_, std::initializer_list<tv_t> &&tvp_, FUNC &&func_)
  {
    std::ostringstream s;
    s << "8=" << fix8_ << sep_;
    // calculer les tag 9 & 10
    for (auto &p : tvp_)
    {
      s << p._tag << "=" << std::string(p._val.data(), p._val.size()) << sep_; // optimal ?
    }
    func_(s);
  }
  static uint8_t checksum(const char *beg_, const char *const end_)
  {
    uint8_t sum = 0;
    while (beg_ < end_)
      sum += static_cast<uint8_t>(*beg_++);
    sum &= 0xFF; // quick %256
    return sum;
  }
};

} // namespace pbx
