#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;
const uint64_t __UP32__ = ( static_cast<uint64_t>( -1 ) >> 32 ) << 32;
const uint64_t __TWO32__ = static_cast<uint64_t>( 1 ) << 32;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return Wrap32 { zero_point + static_cast<uint32_t>( n ) };
}

uint64_t uint64dis( uint64_t lhs, uint64_t rhs )
{
  return lhs > rhs ? lhs - rhs : rhs - lhs;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  uint64_t above_32 = checkpoint & __UP32__, absolute_seqno32 = raw_value_ - zero_point.raw_value_;
  uint64_t absolute_seqno1_dis = uint64dis( above_32 + absolute_seqno32, checkpoint );
  uint64_t absolute_seqno2_dis = uint64dis( above_32 + absolute_seqno32 + __TWO32__, checkpoint );
  uint64_t absolute_seqno3_dis = uint64dis( above_32 + absolute_seqno32 - __TWO32__, checkpoint );

  if ( absolute_seqno1_dis < absolute_seqno2_dis && absolute_seqno1_dis < absolute_seqno3_dis )
    return above_32 + absolute_seqno32;
  else if ( absolute_seqno2_dis < absolute_seqno1_dis && absolute_seqno2_dis < absolute_seqno3_dis )
    return above_32 + absolute_seqno32 + __TWO32__;
  return above_32 + absolute_seqno32 - __TWO32__;
}
