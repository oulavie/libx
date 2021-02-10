
#include "pbx_levenshtein.h"
#include <array>
#include <assert.h>
#include <fstream>  // std::ifstream std::ofstream
#include <iostream> // std::cout
#include <map>
#include <set>
#include <sstream> // std::sringstream
#include <string.h>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------
std::vector<std::string> split(const char *str, char c = ' ')
{
  std::vector<std::string> result;
  do
  {
    const char *begin = str;

    while (*str != c && *str)
      str++;

    result.push_back(std::string(begin, str));
  } while (0 != *str++);
  return result;
}

//--------------------------------------------------------------------------------------------------
template <typename TagCB> void pair_tokenizer(const char *str_, size_t len_, char separator_, TagCB cb_)
{
  size_t tagPos = 0, valPos = 0;
  for (size_t i(0); i < len_; ++i)
  {
    if (str_[i] == '=')
    {
      valPos = ++i;
      for (; i < len_; ++i)
      {
        if (str_[i] == separator_) // il faut terminer par un separateur si on veut recuperer la derneire paire
        {
          cb_(tagPos, valPos, i - valPos);
          tagPos = i + 1;
          break;
        }
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------
bool get_fix_separator(char &separator_, size_t &pos35_, const std::string &str_)
{
  pos35_ = str_.find("35=", 0);
  if (pos35_ != std::string::npos)
  {
    size_t pos = str_.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "1234567890",
                                        pos35_ + 3);
    if (pos != std::string::npos)
    {
      separator_ = str_[pos];
      if (pos35_ > 1 && str_[pos35_ - 1] != separator_)
        return false;
      return true;
    }
  }
  pos35_ = 0;
  return false;
}

//--------------------------------------------------------------------------------------------------
// au minimum, un message FIX commence par "35=" et se termine par "10="
// attention: cette routine suppose un seul message fix dans str_
bool get_fix_message(size_t &start_, size_t &stop_, const std::string &str_, const char separator_, size_t pos35_)
{
  std::string tag10{separator_};
  tag10 += "10=";
  stop_ = str_.rfind(tag10);
  if (stop_ != std::string::npos)
  {
    stop_ += 4;
    size_t stop = str_.find_first_not_of("1234567890", stop_);
    if (stop != std::string::npos)
      stop_ = stop;

    start_ = str_.find("8=");
    if (not(start_ != std::string::npos and start_ < pos35_))
    {
      start_ = str_.find("9=");
      if (not(start_ != std::string::npos and start_ < pos35_))
        start_ = pos35_;
    }
    return true;
  }
  return false;
}

bool get_fix_pairs(std::vector<std::pair<std::string, std::string>> &fixpairs_, const std::string &str_)
{
  char separator;
  size_t pos35;
  if (get_fix_separator(separator, pos35, str_))
  {
    size_t start(0), stop(0);
    if (get_fix_message(start, stop, str_, separator, pos35))
    {
      const char *pstr = str_.data();
      pstr += start;
      pair_tokenizer(pstr, stop - start + 1, separator, [&](size_t tagPos_, size_t valPos_, size_t valLen_) -> void {
        auto a =
            std::make_pair(std::string(pstr + tagPos_, valPos_ - tagPos_ - 1), std::string(pstr + valPos_, valLen_));
        fixpairs_.push_back(a);
      });
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------------------------------
template <size_t SIZE = 3> class ruv : private std::vector<ruv<SIZE> *>
{
  std::string _tag = {};
  std::array<std::string, SIZE> _val = {};

public:
  ruv() : _tag{}
  {
  }
  // https://stackoverflow.com/questions/36320636/c11-making-variadic-constructor-understand-an-initialization-list-of-initializ
  template <typename... T> ruv(const std::string &tag_, T... ts_) : _tag(tag_), _val({ts_...})
  {
    // ruv ruv1(a,b);
    // ruv ruv2(a,b,c);
  }
  ruv(const std::string &tag_, int idx_, const std::string &val_) : _tag(tag_)
  {
    _val[idx_] = val_;
  }
  ruv(const ruv &) noexcept = delete;
  ruv &operator=(const ruv &) noexcept = delete;
  ruv(ruv &&) noexcept = delete;
  ruv &operator=(ruv &&) noexcept = delete;
  ~ruv()
  {
    for (size_t i(0); i < this->size(); ++i)
    {
      delete (*this)[i];
      this->operator[](i) = nullptr;
    }
  }
  // private:
  size_t size_ruv() const
  {
    return this->size();
  }
  ruv *ruv_at(size_t i) const
  {
    return (*this)[i];
  }
  void push_back_ruv(ruv *p_)
  {
    this->push_back(p_);
  }
  void insert_ruv(typename std::vector<ruv<SIZE> *>::const_iterator pos_, ruv *p_)
  {
    this->insert(pos_, p_);
  }
  typename std::vector<ruv<SIZE> *>::const_iterator begin_ruv() const
  {
    return this->begin();
  }
  // getter:
  const std::string &tag() const
  {
    return _tag;
  }
  const std::string &val(size_t idx_ = 0) const
  {
    assert(idx_ < SIZE);
    return _val[idx_];
  }
  // setter:
  void tag(const std::string &tag_)
  {
    _tag = tag_;
  }
  void val(size_t idx_, const std::string &val_)
  {
    assert(idx_ < SIZE);
    _val[idx_] = val_;
  }
  void init(const std::string &tag_, std::initializer_list<std::string> il_)
  {
    _tag = tag_;
    // std::copy( il_.begin(), il_.end(), _val );
  }
};

//--------------------------------------------------------------------------------------------------
typedef std::map<std::string, std::set<std::string>> tRPGs;
typedef std::pair<std::string, std::string> tTagVal;

// clang-format off
const static tRPGs sgroups = 
{
  {"33",{"58","354","355",}}, // NoLinesOfText
  {"73",{"11","198","159","118","67","15","21","38","40","44","423","54","60","63","64","121","120","29749","5214","22031","58","59","7541","6312","20300","20301","20302","20303","20304","20305","29700","29731","5528","5961","124","526","583","160","229","75","1","660","581","589","590","591","70","544","635","18","110","1089","1090","111","100","1133","81","140","2102","1688","401","114","854","1092","99","15","376","2404","2351","2352","377","23","117","1080","1081","59","168","432","126","427","1629","528","529","1091","582","121","120","775","58","354","355","192","193","640","77","203","210","847","848","849","494","1028","453","78","386","457","887","1058","","","",}}, // NoOrders
  {"78",{"79","80","467","539","154","742","737","5673","5674","661","366","1047","1593","1729","1832","2727","776","209","161","360","361","1732","1733","1734","989","993","992","2515","1752","1753","1754","1755","2770","2769","1908","539","736",}}, // NoAllocs
  {"124",{"32","17","1003","31","527","669","29","1041","2749","2524","2668","1838",}}, // NoExecs
  {"136",{"137","138","139","891","2712","2216","2217","2713","2633","22457",}}, // NoMiscFees
  {"146",{"1003","6731","55","66","6847","60","75","464","20086","20280","20287","22659","1091","20393","20073","20387","20074","20075","20076","20156","20077","20078","20079","20080","20081","20082","20084","20088","20083","20085","20183","20090","20421","20072","20098","20265","23070","23074","23076","23060","2668","828","40","20058","21057","20281","20283","20285","453","5745","1","6668","48","22","460","167","762","15","20422","20423","20424","64","541","225","223","5729","5759","5730","106","38","54","132","133","423","44","5767","5023","662","663","22570","20333","699","761","561","562","6110","221","222","6633","470","22005","20130","20132","20134","20136","20138","20140","20145","20147","20149","20150","20153","20157","20158","20161","20162","20165","20167","20171","20172","20175","224","118","381","20179","235","236","120","22630","5114","555",}}, // NoRelatedSym
  {"232",{"233","234",}}, // NoStipulations
  {"457",{"458","459",}}, // NoUnderlyingSecurityAltID
  {"386",{"336","625",}}, // NoTradingSessions
  {"453",{"448","447","452","802","2376","5098","22086",}}, // NoPartyIDs
  {"454",{"455","456",}}, // NoSecurityAltID
  {"539",{"524","525","538","804","2384",}}, // NoNestedPartyIDs
  {"552",{"54","453","66","528","1","15","381","575","159","118","1072","660","581","58","44","576","157","238","119","155","156","232","37","11","114","29","1427","1154","1155","12","13","22193","920","921","922","136","38","22464","22466","1724","2593","1029","735","5419","114","1917","9503","22092","22093","22104","22105","22427","22428","22078","22115",}}, // NoSides
  {"555",{"600","601","602","603","604","607","609","764","556","20425","611","588","249","739","824","685","690","624","662","663","20388","20045","20046","20047","20053","20055","20282","20284","20286","681","684","686","566","20048","615","617","620","675","676","22644","679","680","677","678","596","20050","20118","20710","20711","20712","20713","22006","20131","20133","20135","20137","20139","20141","20142","20143","20144","20146","20148","20151","20152","20154","20159","20160","20163","20164","20166","20168","20173","20174","248","20176","6726","6723","20180","20177","20178","23071","23075","23077","23082","23081","22567","22568","22569","5118","5117","20170","539","20341","20057","20059","20061","20063","20210","26360","29704","5236","5237","22124","27953","29715","29716","22125","1788","2149","2150","2067","2068","2163","2164","2165","2172","2173","2174","2175","253","614","1871","1872","1873","740","955","40241","22142","587","683","42198","42203","22168","22117","637","1075","1418","1893","1591","1907","6731","20185","20187","20191","20234","20249","21017","21018","21019","21020","21022","21024","21026","22571","20267","20268","20269","20270","20277", }},
  {"604",{"605","606",}}, // NoLegSecurityAltID
  {"683",{"688","689",}}, // NoLegStipulations
  {"735",{"695",}}, // NoQuoteQualifiers
  {"768",{"769","770", "771",}}, // NoTrdRegTimestamps
  {"802",{"523","803",}}, // NoPartySubIDs
  {"804",{"545","805",}}, // NoNestedPartySubIDs
  {"887",{"888","889",}}, // NoUnderlyingStips
  {"948",{"949","950","951",}}, // NoNested3PartyIDs
  {"1058",{"1059","1060","1061","2391",}}, // NoUndlyInstrumentParties
  {"1116",{"1117","1118","1119","1120",}}, // NoRootPartyIDs
  {"1120",{"58","1121","1122",}}, // NoRootPartySubIDs
  {"1838",{"1839",}}, // NoTradePriceConditions
  {"1907",{"1903","1904","1905","1906","2411","2397",}}, // NoRegulatoryTradeIDs
  {"1908",{"1909","1910","1911","1912","2406","2939",}}, // NoAllocRegulatoryTradeIDs
  {"2593",{"2594","2595",}}, // NoOrderAttributes
  {"2633",{"2634","2635","2636","2637","2638",}}, // NoMiscFeeSubTypes
  {"2668",{"2669","2670",}}, // NoTrdregPublications
  {"5114",{"5113","255","20169","20227",}}, // NoCreditRating
  {"5118",{"5117","20170",}}, // NoLegCreditRating
  {"9471",{"9472","9473","9474","9475","9476","9477",}}, // NoTransactionCosts
  {"9610",{"9611","9712","9613",}},
  {"22078",{"22079","22080","22081","22082","22083","22085","22197","22128",}},
  {"20200",{"20201","20202","20203","20204","20205","20206",}},
  {"20210",{"20211","20212","20213","20214","20215","20216",}},
  {"22078",{"22079","22080","22081","22082","22083","22085","22150","22156","22197","22198",}}, // NoRefPrices
  {"22086",{"22087","22088","22089",}}, // NoPartyAltIDs
  {"22089",{"22090","22091",}}, // NoPartyAltSubIDs
  {"22115",{"22116","22117",}}, // NoReferenceIds
  //{"22117",{"","","",}}, // MA
  {"29771",{"29772","29773","29774","29775",}},
  //{"40241",{"","","",}}, // MA
  //{"42198",{"","","",}}, // MA
  //{"42203",{"","","",}}, // MA
};
// clang-format on

template <typename T>
int rmake(T &rT_, const std::vector<tTagVal> &rTagVals_, const tRPGs &rRPGs_, int idx_ = 0, size_t inc_ = 0,
          std::set<std::string> *pRPG_ = nullptr)
{
  std::map<std::string, int> countingTheRPGs;

  while (inc_ < rTagVals_.size())
  {
    const tTagVal &pair = rTagVals_[inc_];
    if (pRPG_ != nullptr)
    {
      auto found = pRPG_->find(pair.first);
      if (found == pRPG_->end())
      {
        return inc_;
      }
      else
      {
        pRPG_->erase(found);
      }
    }
    tRPGs::const_iterator foundRpgCounter = rRPGs_.find(pair.first);
    if (foundRpgCounter != rRPGs_.end())
    {
      auto it = countingTheRPGs.find(pair.first);
      if (it == countingTheRPGs.end())
        countingTheRPGs.insert({pair.first, 0});

      const std::set<std::string> &rRptTags = foundRpgCounter->second;
      size_t pos = inc_ + 1;

      tTagVal npair = rTagVals_[pos];
      if (pos < rTagVals_.size() && rRptTags.find(npair.first) != rRptTags.end())
      {
        while (pos < rTagVals_.size() && rRptTags.find(npair.first) != rRptTags.end())
        {
          auto it = countingTheRPGs.find(pair.first);
          assert(it != countingTheRPGs.end());
          it->second++;

          T *pRpg = nullptr;
          if (idx_ == 0)
          {
            pRpg = new T(pair.first, pair.second); // new RPG
            rT_.push_back_ruv(pRpg);
          }
          else
          {
            int ref{0}, count{0};
            for (size_t i(0); i < rT_.size_ruv(); ++i)
            {
              ++ref;
              T *p = rT_.ruv_at(i);
              if (p->tag() == pair.first)
              {
                ++count;
                if (count == it->second)
                {
                  pRpg = p;
                  break;
                }
              }
              else if (count != 0)
                --ref;
            }
            if (pRpg == nullptr)
            {
              pRpg = new T(pair.first, idx_, pair.second); // new RPG with _val at [idx_]
              rT_.insert_ruv(rT_.begin_ruv() + ref, pRpg);
            }
          }

          std::set<std::string> copy_of_rpg = std::set<std::string>(rRptTags);
          inc_ = rmake(*pRpg, rTagVals_, rRPGs_, idx_, pos, &copy_of_rpg);
          pos = inc_;

          if (pos < rTagVals_.size())
          {
            npair = rTagVals_[pos];
          }
        }
      }
      else
      {
        ++inc_;
      }
    }
    else
    {
      if (idx_ == 0)
      {
        T *p = new T(pair.first, pair.second);
        rT_.push_back_ruv(p);
      }
      else
      {
        bool bDone = {};
        for (size_t i(0); i < rT_.size_ruv(); ++i)
        {
          if (rT_.ruv_at(i)->tag() == pair.first)
          {
            rT_.ruv_at(i)->val(idx_, pair.second);
            bDone = true;
            break;
          }
        }
        if (!bDone)
          rT_.push_back_ruv(new ruv(pair.first, idx_, pair.second));
      }
      ++inc_;
    }
  }
  return inc_;
}

//--------------------------------------------------------------------------------------------------
typedef std::map<std::string, std::string> tRPGss; // start/stop

const static tRPGss sStartStopGroups = {
    {"SOH", "EOH"},       {"SCONN", "ECONN"}, {"SQTE", "EQTE"},   {"SFIXED", "EFIXED"},
    {"SFLOAT", "EFLOAT"}, {"STRD", "ETRD"},   {"SPART", "EPART"}, {"SSMKT", "ESMKT"},
    {"SBMKT", "EBMKT"},   {"SSCNF", "ESCNF"}, {"SBCNF", "EBCNF"},
};

template <typename T>
int ssmake(T &rT_, const std::vector<tTagVal> &rTagVals_, const tRPGss &rRPGs_, int idx_ = 0, size_t inc_ = 0,
           const std::string *pRPG_STOP_ = nullptr)
{
  std::map<std::string, int> countingThisRPG;

  while (inc_ < rTagVals_.size())
  {
    const tTagVal &pair = rTagVals_[inc_];
    if (pRPG_STOP_ != nullptr && *pRPG_STOP_ == pair.first)
    {
      return inc_;
    }
    tRPGss::const_iterator foundRpgCounter = rRPGs_.find(pair.first);
    if (foundRpgCounter != rRPGs_.end())
    {
      auto it = countingThisRPG.find(pair.first);
      if (it == countingThisRPG.end())
        countingThisRPG.insert({pair.first, 0});

      size_t pos = inc_ + 1;
      tTagVal npair = rTagVals_[pos];
      if (pos < rTagVals_.size() && npair.first != foundRpgCounter->second)
      {
        while (pos < rTagVals_.size() && npair.first != foundRpgCounter->second)
        {
          auto it = countingThisRPG.find(pair.first);
          assert(it != countingThisRPG.end());
          it->second++;

          T *pRpg = nullptr;
          if (idx_ == 0)
          {
            pRpg = new T(pair.first, pair.second); // new RPG
            rT_.push_back_ruv(pRpg);
          }
          else
          {
            int ref{0}, count{0};
            for (size_t i(0); i < rT_.size_ruv(); ++i)
            {
              ++ref;
              T *p = rT_.ruv_at(i);
              if (p->tag() == pair.first)
              {
                ++count;
                if (count == it->second)
                {
                  pRpg = p;
                  break;
                }
              }
              else if (count != 0)
                --ref;
            }
            if (pRpg == nullptr)
            {
              pRpg = new T(pair.first, idx_, pair.second); // new RPG with _val at [idx_]
              rT_.insert_ruv(rT_.begin_ruv() + ref, pRpg);
            }
          }

          inc_ = ssmake(*pRpg, rTagVals_, rRPGs_, idx_, pos, &(foundRpgCounter->second));
          pos = inc_;

          if (pos < rTagVals_.size())
          {
            npair = rTagVals_[pos];
          }
        }
      }
      else
      {
        ++inc_;
      }
    }
    else
    {
      if (idx_ == 0)
      {
        T *p = new T(pair.first, pair.second);
        rT_.push_back_ruv(p);
      }
      else
      {
        bool bDone = {};
        for (size_t i(0); i < rT_.size_ruv(); ++i)
        {
          if (rT_.ruv_at(i)->tag() == pair.first)
          {
            rT_.ruv_at(i)->val(idx_, pair.second);
            bDone = true;
            break;
          }
        }
        if (!bDone)
          rT_.push_back_ruv(new ruv(pair.first, idx_, pair.second));
      }
      ++inc_;
    }
  }
  return inc_;
}

//------------------------------------------------------------------------------
template <typename T, typename CB> void tree_parsing(T &rT_, int idx_, CB CB_, int level_ = 0)
{
  for (size_t i(0); i < rT_.size_ruv(); ++i)
  {
    T *p = rT_.at(i);
    CB_(p->tag(), p->val(idx_), level_);
    if (p->size_ruv() != 0)
      tree_parsing(*p, idx_, CB_, level_ + 1);
  }
}

//------------------------------------------------------------------------------
static const std::string OFFSET_INCREMENT = " ";
//------------------------------------------------------------------------------
template <typename T> void display_imp(std::ostream &ostr_, T &rT_, const std::string &roffset_)
{
  for (size_t i(0); i < rT_.size_ruv(); ++i)
  {
    T *p = rT_.ruv_at(i);
    if (p->size_ruv() == 0)
    {
      ostr_ << roffset_ << p->tag() << " = ";

      ostr_ << (!p->val(0).empty() ? p->val(0) : "[]");

      if (!p->val(1).empty())
      {
        ostr_ << ((p->val(0) == p->val(1)) ? " |=| " : " |!| ");
        ostr_ << p->val(1);

        if (!p->val(2).empty())
        {
          ostr_ << ((p->val(1) == p->val(2)) ? " |=| " : " |!| ");
          ostr_ << p->val(2);
        }
      }
      else if (!p->val(2).empty())
      {
        ostr_ << " || [] || " << p->val(2);
      }

      ostr_ << std::endl;
    }
    else
    {
      ostr_ << roffset_ << p->tag() << " = " << p->val() << std::endl;
      display_imp(ostr_, *p, roffset_ + OFFSET_INCREMENT);
    }
  }
}

std::ostream &display_raw(std::ostream &ostr_, ruv<> &rT_)
{
  std::string offset("");
  display_imp(ostr_, rT_, offset);
  return ostr_;
}

//------------------------------------------------------------------------------
typedef size_t width4_t[4];
template <typename T> void ruv_widths_imp(width4_t &widths_, T &rT_, const std::string &roffset_)
{
  for (size_t i(0); i < rT_.size_ruv(); ++i)
  {
    T *p = rT_.ruv_at(i);
    if (p->size_ruv() == 0)
    {
      widths_[0] = std::max(widths_[0], roffset_.length() + p->tag().length());
      widths_[1] = std::max(widths_[1], p->val(0).length());
      widths_[2] = std::max(widths_[2], p->val(1).length());
      widths_[3] = std::max(widths_[3], p->val(2).length());
    }
    else
    {
      ruv_widths_imp(widths_, *p, roffset_ + OFFSET_INCREMENT);
    }
  }
}
template <typename T> void ruv_widths(width4_t &widths_, T &rT_, const std::string &offset_)
{
  ruv_widths_imp(widths_, rT_, offset_);
}

std::string left_pad(const std::string &str_, size_t width_)
{
  std::string str;
  while (str.length() + str_.length() < width_)
    str += " ";
  str += str_;
  return str;
}

std::string right_pad(const std::string &str_, size_t width_)
{
  std::string str(str_);
  while (str.length() < width_)
    str += " ";
  return str;
}

template <typename T>
void padded_display_imp(std::ostream &ostr_, T &rT_, const std::string &roffset_, const width4_t &widths_)
{
  for (size_t i(0); i < rT_.size_ruv(); ++i)
  {
    T *p = rT_.ruv_at(i);
    if (p->size_ruv() == 0)
    {
      ostr_ << right_pad(roffset_ + p->tag(), widths_[0]) << " = ";

      //ostr_ << (!p->val(0).empty() ? left_pad(p->val(0), widths_[1]) : left_pad("[]", widths_[1]));
      ostr_ << (!p->val(0).empty() ? left_pad(p->val(0), widths_[1]) : left_pad("", widths_[1]));

      if (!p->val(1).empty())
      {
        ostr_ << ((p->val(0) == p->val(1)) ? " |=| " : " |!| ");
        ostr_ << right_pad(p->val(1), widths_[2]);

        if (!p->val(2).empty())
        {
          ostr_ << ((p->val(1) == p->val(2)) ? " |=| " : " |!| ");
          ostr_ << right_pad(p->val(2), widths_[3]);
        }
      }
      else if (!p->val(2).empty())
      {
        ostr_ << " || [] || " << p->val(2);
      }

      ostr_ << std::endl;
    }
    else
    {
      ostr_ << roffset_ << p->tag() << " = " << p->val() << std::endl;
      padded_display_imp(ostr_, *p, roffset_ + OFFSET_INCREMENT, widths_);
    }
  }
}

std::ostream &display_padded(std::ostream &ostr_, ruv<> &rT_)
{
  width4_t widths;
  widths[0] = 0;
  widths[1] = 0;
  widths[2] = 0;
  widths[3] = 0;

  std::string offset("");
  ruv_widths(widths, rT_, offset);
  padded_display_imp(ostr_, rT_, offset, widths);
  return ostr_;
}

std::ostream &operator<<(std::ostream &ostr_, ruv<> &rT_)
{
  return display_raw(ostr_, rT_);
  // return display_padded(ostr_, rT_);
}

//--------------------------------------------------------------------------------------------------
void display_args(int argc, char *argv[], char **argenv)
{
  std::cout << "---------------------------------------------------------------------" << std::endl;
  char **req = argenv;
  while (*req)
  {
    std::cout << *req << std::endl;
    ++req;
  }
  std::cout << "---------------------------------------------------------------------" << std::endl;
  std::cout << "argc=" << argc << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl;
  for (int i(0); i < argc; ++i)
    std::cout << argv[i] << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl;
}

//--------------------------------------------------------------------------------------------------
void display_fix(const char *str1_)
{
  std::cout << str1_ << std::endl;
  std::string sstr1(str1_);
  std::vector<std::pair<std::string, std::string>> vstr1;
  if (get_fix_pairs(vstr1, sstr1))
  {
    ruv a;
    rmake(a, vstr1, sgroups);

    // std::cout << a << std::endl;
    display_raw(std::cout, a);
  }
}
void display_fix(const char *str1_, const char *str2_)
{
  std::cout << "---------------------------------------------------------------------" << std::endl;
  display_fix(str1_);
  std::cout << "---------------------------------------------------------------------" << std::endl;
  display_fix(str2_);
  std::cout << "---------------------------------------------------------------------" << std::endl;

  std::string sstr1(str1_), sstr2(str2_);
  std::vector<std::pair<std::string, std::string>> vstr1, vstr2;
  if (get_fix_pairs(vstr1, sstr1) && get_fix_pairs(vstr2, sstr2))
  {
    ruv a;
    rmake(a, vstr1, sgroups);
    rmake(a, vstr2, sgroups, 1);

    // std::cout << a << std::endl;
    display_padded(std::cout, a);
  }
}

//--------------------------------------------------------------------------------------------------
int main(int argc, char *argv[], char **argenv)
{
  if (argc == 2)
  {
    display_fix(argv[1]);
    return 0;
  }
  else if (argc == 3)
  {
    display_fix(argv[1], argv[2]);
    return 0;
  }

  // clang-format off
  std::vector<std::pair<std::string, std::string>> v1 = {
      {"8", "val8"}, {"9", "val9"}, {"35", "val35"}, {"49", "val49"}, {"56", "val56"}, {"60", "val60"},
      {"555", "val555"},
        {"600", "val600.A"}, {"601", "val601.A"}, 
        // {"600","val600.B"}, {"601","val601.B"}, {"602","val602.B"},
      {"10", "val10"},
  };

  std::vector<std::pair<std::string, std::string>> v2 = {
      {"8", "val8.2"}, {"9", "val9"}, {"35", "val35.2"}, {"56", "val56"}, {"52", "val52.2"},
      {"60", "val60.2"}, {"55", "val55.2"},     
      {"555", "val555.2"},   
        {"600", "val600.A.2"}, {"601", "val601.A.2"}, {"602", "val602.A.2"}, 
        {"600", "val600.B.2"}, {"601", "val601.B.2"}, 
        {"600", "val600.C.2"}, {"601", "val601.C.2"}, {"602", "val602.C.2"}, 
      {"10", "val10.2"},
  };

  std::vector<std::pair<std::string, std::string>> v3 = {
      {"8", "val8.3"}, {"9", "val9.3"}, {"35", "val35.3"}, {"49", "val49.3"}, {"56", "val56.3"},
      {"52", "val52.3"}, {"55", "val55.3"},     
      {"555", "val555.3"},   
        {"600", "val600.A.3"}, {"601", "val601.A.3"}, {"603", "val603.A.3"}, 
        {"600", "val600.C.3"}, {"601", "val601.C.3"}, {"602", "val602.C.3"}, 
      {"10", "val10.3"},
  };
  // clang-format on

  ruv root;

  rmake(root, v1, sgroups);
  // std::cout << root << std::endl;
  display_raw(std::cout, root);

  std::cout << "----------------------------------------------------------------" << std::endl;

  rmake(root, v2, sgroups, 1);
  // std::cout << root << std::endl;
  display_padded(std::cout, root);

  // rmake(root, v3, sgroups, 2);
  // std::cout << root << std::endl;

  return 0;

  // std::cout << levenshtein_distance("toto", 4, "toto", 4) << std::endl;
  // std::cout << levenshtein_distance("tito", 4, "toto", 4) << std::endl;
  // std::cout << levenshtein_distance("titi", 4, "toto", 4) << std::endl;
  // std::cout << levenshtein_distance("tili", 4, "toto", 4) << std::endl;
  /*
      tree_parsing(root, 0, [](const std::string &tag_, const std::string &val_, int level_) {
        std::cout << level_ << " " << tag_ << " = " << val_ << std::endl;
      });
  */

  /*
      std::string a{},b{};
      ruv t0();
      ruv t1(a);
      ruv t2("toto","titi");
      ruv t3(a,a,a);
      ruv t4(a,a,a,a);
      //ruv t5(a,a,a,a,a);


      tree_parsing(t2, 0, [](const std::string &tag_, const std::string &val_, int level_) {
        std::cout << level_ << " " << tag_ << " = " << val_ << std::endl;
      });
  */

  // clang-format off
  /*
  const char* dplmsg = 
  "|SOH="
    "|VERS=1|DATE=678|TIME=432|PGRP=DSWP"
  "|EOH=|"
  "|CANRESP=Y|CANQUOTE=Y|CANREQUOTE=Y"
  "|STRD="
    "|TNUM=109|TRANS=MKT|TRDPRCTYP=RFM"
    "|SFIXED="
      "|DYCTBAS=30/360|CPNFQ=2"
    "|EFIXED="
    "|SFLOAT="
      "|DYCTBAS=30/360|CPNFQ=4"
    "|EFLOAT="
    "|ADIDT=MODFOLLOW|ISMN=24"
    "|SBMKT="
      "|RATE=0"
    "|EBMKT="
    "|SSMKT="
      "|RATE=0"
    "|ESMKT="
    "|COMPQT=1.1|CMPB=1.5"
    "|SPART="
      "|QTYP=RATE"
    "|EPART="
  "|ETRD="
  "|EOM="
  "|";
  */
  
  std::vector<std::pair<std::string, std::string>> dpl1 = {
    {"SOH",""},
      {"VERS1","1"},{"DATE","678"},{"TIME","432"},{"PGRP","DSWP"},
    {"EOH",""},
    {"CANRESP","Y"},{"CANQUOTE","N"},{"CANREQUOTE","Y"},
    {"STRD",""},
      {"TNUM","109"},{"TRANS","MKT"},{"TRDPRCTYP","RFM"},
      {"SFIXED",""},
        {"DYCTBAS","30/360"}, {"CPNFQ","2"},
      {"EFIXED",""},
      {"SFLOAT",""},
        {"DYCTBAS","40/360"},{"CPNFQ","3"},
      {"EFLOAT",""},
      {"ADIDT","MODFOLLOW"},{"ISMN","24"},
      {"SBMKT",""},
        {"RATE","0"},
      {"EBMKT",""},
      {"SSMKT",""},
        {"RATE","1"},
      {"ESMKT",""},
      {"COMPQT","1,1"},{"CMPB","1.1"},
      {"SPART",""},
        {"QTYP","RATE 1"},
      {"EPART",""},
      {"SPART",""},
        {"QTYP","RATE 2"},
      {"EPART",""},
    {"ETRD",""},
    {"EOM",""},
  };
  // clang-format on

  ruv ssroot;
  ssmake(ssroot, dpl1, sStartStopGroups);
  std::cout << ssroot << std::endl;

  return 0;
}

// clang-format-9 -i tst_ruv.cpp
// g++ -Wall -fno-default-inline -Weffc++ -std=c++17 -I../libpbx tst_ruv.cpp
// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
