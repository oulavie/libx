
#include <algorithm>
#include <array>
#include <assert.h>
#include <boost/utility/string_ref.hpp>
#include <chrono>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <set>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
// sudo apt install libboost-all-dev
// limon 06 85 52 05 03

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
namespace
{

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

//----------------------------------------------------------------------------
// static constexpr char SEP = '\1';
//static constexpr char SEP = ';';
struct TVPair
{
  const int tag;
  const char *const content;
  size_t len;
};
static TVPair step(const char *&current_, const char *const end_, const char SEP = ';')
{
  const auto start_tag = current_;
  const char *equal = {};
  const char *sep = {};
  for (; current_ < end_; ++current_)
  {
    if (*current_ == '=')
    {
      equal = current_;
      current_++;
      break;
    }
  }
  if (!equal)
  {
    throw std::runtime_error("");
  }
  int tag = 0;
  tag = std::stoi(std::string(start_tag, static_cast<int>(equal - start_tag)));
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

//----------------------------------------------------------------------------
namespace v1
{

template <class None = void> class msg_t
{
  using Storage = std::unique_ptr<char[]>;

  //----------------------------------------------------------------------------
  static constexpr int AcceptedTags[] = {8, 9, 35, 48, 22, 11, 41};
  template <typename T, int N> static constexpr size_t get_size(T (&)[N])
  {
    return N;
  }
  static constexpr size_t NB_ENTRIES = get_size(AcceptedTags);
  static constexpr int TAG_MIN = *std::min_element(std::begin(AcceptedTags), std::end(AcceptedTags));
  static constexpr int TAG_MAX = *std::max_element(std::begin(AcceptedTags), std::end(AcceptedTags));
  static constexpr size_t MapSize = 2 + TAG_MAX - TAG_MIN;

  static constexpr size_t clipTag(int tag) // pixel shading
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

  //----------------------------------------------------------------------------
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
  using Wheres = std::array<Where, NB_ENTRIES>;
  mutable Wheres _alreadySeenTagsCache = {};
  size_t _originalSize = 0;
  mutable size_t _lastLookupPosition = 0;
  // static constexpr size_t max_lookup = 1024;
public:
  template <int TAG> boost::string_ref getPair() const
  {
    //uint64_t t1 = gettime2();
    constexpr auto position = mapTag2Index(TAG);
    //uint64_t t2 = gettime2();
    //std::cout << t2 - t1 << std::endl;

    try
    {
      const char *const start = _content.get();
      const char *current = start + _lastLookupPosition;
      const char *const end = start + _originalSize;
      Where &loc = _alreadySeenTagsCache[position - 1];
      // for (; (!loc.offset + loc.len) && (current < end) && (_lastLookupPosition < max_lookup);)
      for (; (current < end);)
      {
        TVPair p = step(current, end);
        _lastLookupPosition = static_cast<size_t>(current - start);
        auto position = mapTag2Index(p.tag);
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
  //----------------------------------------------------------------------------
  static size_t roundUpToPowerOfTwo(size_t sz_)
  {
    auto powtwo = sz_;
    for (int i(1); powtwo & (powtwo + 1); i *= 2)
    {
      powtwo |= powtwo << i;
    }
    powtwo = powtwo + 1;
    assert(powtwo >= sz_);
    assert(sz_ >= powtwo / 2);
    assert(!(powtwo & (powtwo - 1)));
    return powtwo;
  }
  size_t _max = 4096;
  static constexpr size_t reserve = 1024;
  Storage _content = Storage{new char[_max]};

public:
  bool set(const char *buf, size_t size)
  {
    // if( UNLIKELY(!isEmpty())
    if (!isEmpty())
    {
      reset();
    }
    if (size > 100000)
      return false;
    assert(_alreadySeenTagsCache == Wheres{});
    if ((size > 0) && memcmp(buf + size - 8,
                             "\1"
                             "10=",
                             4) == 0)
    {
      size -= 7;
    }
    auto newmax = size + reserve;
    // if( UNLIKELY(newmax > _max))
    if (newmax > _max)
    {
      _max = roundUpToPowerOfTwo(newmax);
      std::cout << "received size=" << size << " growing to=" << _max << std::endl;
      _content.reset(new char[_max]);
    }
    memcpy(_content.get(), buf, size);
    _originalSize = size;
    assert(size == 0 || !isEmpty());
    return true;
  }

private:
  size_t _used = 0;

  bool isEmpty() const
  {
    const bool empty = _originalSize == 0;
    if (empty)
    {
      assert(_lastLookupPosition == 0);
      assert(_used == 0);
      assert(_alreadySeenTagsCache == Wheres{});
    }
    return empty;
  }
  void reset()
  {
    // static auto& seen = seenMap();
    //(void)_seen;
    CheckMessageAndLogicAfterSend();
    _originalSize = 0;
    _lastLookupPosition = 0;
    _used = 0;
    _alreadySeenTagsCache = Wheres{};
    assert(isEmpty());
    CheckMessageAndLogicAfterSend();
  }
  static bool &avoidCheckMessageAndLogicAfterSend()
  {
    static bool b;
    return b;
  }
  bool CheckMessageAndLogicAfterSend() const
  {
    if (avoidCheckMessageAndLogicAfterSend())
      return true;
    try
    {
      const char *const start = _content.get();
      // if( UNILKELY(!start))
      if ((!start))
      {
        throw std::logic_error("missing message _content");
      }
      if (isEmpty())
      {
        // if( UNLIKELY( (_originalSize!=0) || (_lastLookupPosition != 0) || (_used != 0) || (!(_alreadySeenTagsCache ==
        // Wheres{})) ))
        if (((_originalSize != 0) || (_lastLookupPosition != 0) || (_used != 0) ||
             (!(_alreadySeenTagsCache == Wheres{}))))
        {
          throw std::logic_error("bad message reset");
        }
        const auto &BodyLength = getPair<9>();
        // if (UNLIKELY(BodyLength.empty()))
        if ((BodyLength.empty()))
        {
          throw std::runtime_error("BodyLength(9) error");
        }
        return true;
      }
    }
    catch (const std::runtime_error &e)
    {
      std::cerr << e.what() << std::endl;
      return false;
    }
    catch (const std::logic_error &e)
    {
      std::cerr << e.what() << std::endl;
      return false;
    }
    return true;
  }

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
        if ((clip - 1 + TAG_MIN) != static_cast<size_t>(tag))
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

} // namespace v1

//--------------------------------------------------------------------------------------------------
namespace v2
{
template <typename T, int N> static constexpr size_t get_size(T (&)[N])
{
  return N;
}
template <int... tags> class clip
{
public:
  static constexpr int AcceptedTags[] = {tags...};
  static constexpr size_t NB_ENTRIES = get_size(AcceptedTags);
  static constexpr int TAG_MIN = *std::min_element(std::begin(AcceptedTags), std::end(AcceptedTags));
  static constexpr int TAG_MAX = *std::max_element(std::begin(AcceptedTags), std::end(AcceptedTags));
  static constexpr size_t MapSize = 2 + TAG_MAX - TAG_MIN;
  static constexpr size_t clipTag(int tag) // pixel shading
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
};
template <typename CLIPPER, char SEP = ';'> class msg_t
{
  static constexpr CLIPPER _clipper = {};

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
  template <int TAG> boost::string_ref getPair() const
  {
   // uint64_t t1 = gettime2();
    constexpr auto position = _clipper.mapTag2Index(TAG);
    //uint64_t t2 = gettime2();
    //std::cout << t2 - t1 << std::endl;

    try
    {
      const char *const start = _content;
      const char *current = start + _lastLookupPosition;
      const char *const end = start + _originalSize;
      Where &loc = _alreadySeenTagsCache[position - 1];
      // for (; (!loc.offset + loc.len) && (current < end) && (_lastLookupPosition < max_lookup);)
      for (; (current < end);)
      {
        TVPair p = step(current, end, SEP);
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
  size_t _max = 4096;
  static constexpr size_t reserve = 1024;
  const char* _content = {};

public:
  bool set(const char *buf, size_t size)
  {
    assert(_alreadySeenTagsCache == Wheres{});
    _content = buf;
    _originalSize = size;
    return true;
  }
};


} // namespace v2

//--------------------------------------------------------------------------------------------------
template <typename... Tags> void printVariadic(Tags... tags)
{
  for (const auto p : {tags...})
  {
    std::cout << p << std::endl;
  }
}
template <typename... Tags> void printVariadic2(Tags... tags)
{
  std::array<int, sizeof...(tags)> list = {tags...};
  for (const auto p : list)
  {
    std::cout << p << std::endl;
  }
}
template <typename... Tags> void printVariadic3(int i, Tags... tags)
{
  static std::vector<int> v;
  v.push_back(i);
  printVariadic3(tags...);
}

//--------------------------------------------------------------------------------------------------
template <typename... Types> struct thing
{
  std::tuple<std::vector<Types>...> _x;
  thing(std::vector<Types>... args) : _x{std::move(args)...}
  {
  }
  void print()
  {
    do_print_vectors(std::index_sequence_for<Types...>());
  }
private:
  template <std::size_t... Is> void do_print_vectors(std::index_sequence<Is...>)
  {
    using swallow = int[];
    (void)swallow{0, (print_one(std::get<Is>(_x)), 0)...};
  }
  template <class Vector> void print_one(const Vector &v)
  {
    copy(begin(v), end(v), std::ostream_iterator<typename Vector::value_type>(std::cout, ","));
    std::cout << std::endl;
  }
};
void test_thing()
{
  thing<int, double, std::string> t { { 1, 2, 3, 4}, { 1.1, 2.2, 3.3 }, { "one", "two", "three" } };
  t.print();
  thing<int> t2 { { 8, 9, 48, 22, 11, 41} };
  t2.print();
}

//--------------------------------------------------------------------------------------------------
// https://stackoverflow.com/questions/56288041/variadic-template-parameters-from-integer
// marche pas
template<int... Is>
struct A {};

template<class>
struct make_A_impl;

template<int... Is>
struct make_A_impl<std::integer_sequence<int, Is...>> {
      using Type = A<Is...>;
};

template<int size>
using make_A = typename make_A_impl<std::make_integer_sequence<int, size>>::Type;

void testA()
{
  //A<0, 4000, 2999> a;
}

//--------------------------------------------------------------------------------------------------
void test_msg_t()
{
  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  v1::msg_t mc;
  // static constexpr int AcceptedTags[] = {8, 9, 35, 48, 22, 11, 41};
  std::string s{"8=toto;9=tata;41=tyty;22=gfgf;35=BN;11=tag11;48=porduit;10=100;"};
  const char *p = s.data();
  size_t l = s.length();

  uint64_t t00 = gettime();
  mc.set(p, l);
  uint64_t t0 = gettime();
  const auto &tag8 = mc.getPair<8>();
  uint64_t t1 = gettime();
  const auto &tag9 = mc.getPair<9>();
  const auto &tag41 = mc.getPair<41>();
  const auto &tag22 = mc.getPair<22>();
  const auto &tag35 = mc.getPair<35>();
  const auto &tag11 = mc.getPair<11>();
  const auto &tag48 = mc.getPair<48>();
  const auto &tag10 = mc.getPair<10>();
  uint64_t t2 = gettime();

  std::cout << "ctor=" << (t0 - t00) << std::endl;
  std::cout << "getpair=" << (t1 - t0) << std::endl;
  std::cout << "getpair=" << (t2 - t1) / 7. << std::endl;
  std::cout << "all=" << (t2 - t00) << std::endl;

  {
    std::string tag{tag8.data(), tag8.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag9.data(), tag9.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag41.data(), tag41.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag22.data(), tag22.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag35.data(), tag35.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag11.data(), tag11.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag48.data(), tag48.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag10.data(), tag10.size()};
    std::cout << tag.data() << std::endl;
  }
}

void test_clip()
{
  std::cout << "-------------------------------------------------------------------------------" << std::endl;
  typedef v2::clip<8, 9, 35, 48, 22, 11, 41, 1> t;
  /*
  t to;
  size_t t8 = t::mapTag2Index(8);
  std::cout << "8=" << t::mapTag2Index(8) << std::endl;
  std::cout << "9=" << t::mapTag2Index(9) << std::endl;
  std::cout << "35=" << t::mapTag2Index(35) << std::endl;
  std::cout << "48=" << t::mapTag2Index(48) << std::endl;
  std::cout << "22=" << t::mapTag2Index(22) << std::endl;
  std::cout << "11=" << t::mapTag2Index(11) << std::endl;
  std::cout << "41=" << t::mapTag2Index(41) << std::endl;
  std::cout << "1=" << t::mapTag2Index(1) << std::endl;
  */

  v2::msg_t<t> mc;
  std::string s{"8=toto;9=tata;41=tyty;22=gfgf;35=BN;11=tag11;48=porduit;10=100;"};
  const char *p = s.data();
  size_t l = s.length();

  uint64_t t00 = gettime();
  mc.set(p, l);
  uint64_t t0 = gettime();
  const auto &tag8 = mc.getPair<8>();
  uint64_t t1 = gettime();
  const auto &tag9 = mc.getPair<9>();
  const auto &tag41 = mc.getPair<41>();
  const auto &tag22 = mc.getPair<22>();
  const auto &tag35 = mc.getPair<35>();
  const auto &tag11 = mc.getPair<11>();
  const auto &tag48 = mc.getPair<48>();
  const auto &tag10 = mc.getPair<10>();
  uint64_t t2 = gettime();
  std::cout << "ctor=" << (t0 - t00) << std::endl;
  std::cout << "getpair=" << (t1 - t0) << std::endl;
  std::cout << "getpair=" << (t2 - t1) / 7. << std::endl;
  std::cout << "all=" << (t2 - t00) << std::endl;

  {
    std::string tag{tag8.data(), tag8.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag9.data(), tag9.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag41.data(), tag41.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag22.data(), tag22.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag35.data(), tag35.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag11.data(), tag11.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag48.data(), tag48.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag10.data(), tag10.size()};
    std::cout << tag.data() << std::endl;
  }

}


//--------------------------------------------------------------------------------------------------
int main()
{
  test_msg_t();
  test_clip();
  // printVariadic2(1, 2, 3);

  return 0;
}

// clang-format off
// clang-format-9 -i mich.cpp
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a mich.cpp
// g++ -std=c++17 -Wall -Wextra -Wpedantic mich.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto
// g++ -std=c++17 -Wall -Wextra -Wpedantic -Wno-deprecated-declarations mich.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
