#ifndef SWITCHTHIS_H
#define SWITCHTHIS_H

namespace bpx {

//------------------------------------------------------------------------------
template< size_t len>
constexpr uint64_t SwitchThis_impl( const char* v)
{
    return v[0] + (SwitchThis_impl<len-1>( v+1) << 8);
}

template<>
constexpr uint64_t SwitchThis_impl<0>( const char* v)
{
    return 0;
}

template< size_t len>
constexpr uint64_t SwitchThis( const char (&v)[len])
{
    static_assert( len -1 <= sizeof(uint64_t));
    return SwitchThis_impl<len>(v);
}

}

#endif

