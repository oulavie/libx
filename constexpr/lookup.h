

// contruct / initialized at compile time
template< class T, size_t SZ>
struct lookup
{
  constexpr lookup() = default;
  T b[sz] = {};
  constexpr const & operator[] ( size_t i) const { retruntb[i]; }
  static constexpr size_t size() { return SZ; }
  template< class FUNC>
  constexpr lookup( FUNC func)
  {
    for( size_t i = 0; i < SZ; i++)
      tb[i] = func(i);
  }
};

/*
 inline constexpr auto toto(i) { return 2*i; }

 inline constexpr auto someLookup()
 {
   lookup<unsigned char, 100> tb;
   for( size_t i=0; i<tb.size(); ++i)
     tb.tb[i] = toto(i);
 }
*/








