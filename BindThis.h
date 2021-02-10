#ifndef BINDTHIS_H
#define BINDTHIS_H

#include <utility>
#include <type_traits>
#include <memory>

//------------------------------------------------------------------------------
// First approach: using a wrapper class
// this captures a raw pointer instead of a smart pointer unlike std::bind
// possible to fix with some duplication for references
//------------------------------------------------------------------------------
template <typename CLASS, typename RET, typename... ARGS> class BindThisT
{
  CLASS *self;
  mptr_t mptr;
public:
  using mptr_t = typename std::conditional<
      std::is_const<CLASS>::value, RET ( CLASS::* )( ARGS... args ) const,
      RET ( CLASS::* )( ARGS... args )>::type;

  constexpr BindThisT( CLASS &self, mptr_t mptr )
      : self( std::addressof( self ) ), mptr( mptr )
  {
  }
  /*
  constexpr RET operator()( ARGS... args )
  {
    return ( ( *self ).*mptr )( std::forward<ARGS>( args )... );
  }
  */
  template <typename... FN_ARGS> constexpr RET operator()( FN_ARGS &&... args )
  {
    return ( ( *self ).*mptr )( std::forward<FN_ARGS>( args )... );
  }
};

template <typename CLASS, typename PTR, typename RET, typename... ARGS>
constexpr auto BindThis( RET ( CLASS::*mptr )( ARGS... args ), PTR self )
    -> BindThisT<CLASS, RET, ARGS...>
{
  return {*self, mptr};
}

template <typename CLASS, typename PTR, typename RET, typename... ARGS>
constexpr auto BindThis( RET ( CLASS::*mptr )( ARGS... args ) const, PTR self )
    -> BindThisT<const CLASS, RET, ARGS...>
{
  return {*self, mptr};
}

template <typename CLASS, typename RET, typename... ARGS>
constexpr auto BindThis( RET ( CLASS::*mptr )( ARGS... args ), CLASS &self )
    -> BindThisT<CLASS, RET, ARGS...>
{
  return {self, mptr};
}

template <typename CLASS, typename RET, typename... ARGS>
constexpr auto BindThis( RET ( CLASS::*mptr )( ARGS... args ) const,
                         CLASS &self ) -> BindThisT<const CLASS, RET, ARGS...>
{
  return {self, mptr};
}

//------------------------------------------------------------------------------
// second approach, using lambda for wrapper class generation
// may be std::function is too generic and not constexpr enough
// note that I know operator->* but still use (*self).* because std::bind does
// the same
//------------------------------------------------------------------------------
/*
template <typename CLASS, typename PTR, typename RET, typename... ARGS>
    constexpr std::function <
    RET<ARGS...> BindThisL( RET ( CLASS::*mptr )( ARGS... args ), PTR self )
{
  return [&self, mptr]( ARGS... args )
  { return ((*self).*mptr)( std::forward<ARGS>((args)...);
  };
}

template <typename CLASS, typename PTR, typename RET, typename... ARGS>
    constexpr std::function <
    RET<ARGS...> BindThisL( RET ( CLASS::*mptr )( ARGS... args ) const,
                            PTR self )
{
  return [&self, mptr]( ARGS... args )
  { return ((*self).*mptr)( std::forward<ARGS>((args)...);
  };
}

template <typename CLASS, typename RET, typename... ARGS>
    constexpr std::function <
    RET<ARGS...> BindThisL( RET ( CLASS::*mptr )( ARGS... args ), CLASS &self )
{
  return [&self, mptr]( ARGS... args )
  { return (self.*mptr)( std::forward<ARGS>((args)...);
  };
}

template <typename CLASS, typename RET, typename... ARGS>
    constexpr std::function <
    RET<ARGS...> BindThisL( RET ( CLASS::*mptr )( ARGS... args ) const,
                            CLASS &self )
{
  return [&self, mptr]( ARGS... args )
  { return (self.*mptr)( std::forward<ARGS>((args)...);
  };
}
*/

#endif

