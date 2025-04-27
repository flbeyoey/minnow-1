#include "reassembler.hh"
#include "debug.hh"
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{

  if ( is_last_substring )
    last_pos = first_index + data.size();

  uint64_t seg_end_pos = first_index + data.size() - 1, seg_st_pos = first_index;

  if ( seg_end_pos < next_expected_seq_ || data.size() == 0 ) {
    if ( last_pos == next_expected_seq_ )
      output_.writer().close();
    return;
  }

  seg_st_pos = max( seg_st_pos, next_expected_seq_ );

  if ( seg_end_pos > next_expected_seq_ + available_capacity() - 1 )
    data = data.substr( seg_st_pos - first_index, next_expected_seq_ + available_capacity() - seg_st_pos );
  else
    data = data.substr( seg_st_pos - first_index );

  if ( data.size() == 0 ) {
    if ( last_pos == next_expected_seq_ )
      output_.writer().close();
    return;
  }

  BlockStreamInfo incoming_seg( seg_st_pos, seg_st_pos + data.size() - 1, data );

  if ( block_storage_.find( incoming_seg ) != block_storage_.end() )
    return;
  auto righter_seg_it = block_storage_.lower_bound( incoming_seg );
  bool lefter_empty = righter_seg_it == block_storage_.begin();
  auto lefter_seg_it = ( lefter_empty ? block_storage_.begin() : prev( righter_seg_it ) );

  while ( righter_seg_it != block_storage_.end() && righter_seg_it->L - 1 <= incoming_seg.R ) {
    incoming_seg.merge_back( *righter_seg_it );
    ++righter_seg_it;
    block_storage_.erase( prev( righter_seg_it ) );
  }

  while ( !lefter_empty && lefter_seg_it->R + 1 >= incoming_seg.L ) {
    incoming_seg.merge_front( *lefter_seg_it );
    if ( lefter_seg_it != block_storage_.begin() ) {
      --lefter_seg_it;
      block_storage_.erase( next( lefter_seg_it ) );
    } else {
      block_storage_.erase( lefter_seg_it );
      break;
    }
  }
  block_storage_.insert( incoming_seg );

  const Reassembler::BlockStreamInfo& first_seg = *block_storage_.begin();
  if ( first_seg.L == next_expected_seq_ ) {
    output_.writer().push( first_seg.info_ );
    next_expected_seq_ = first_seg.R + 1;
    block_storage_.erase( block_storage_.begin() );
  }

  if ( last_pos == next_expected_seq_ ) {
    output_.writer().close();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  uint64_t bytes_cnt = 0;
  for ( auto it = block_storage_.begin(); it != block_storage_.end(); ++it )
    bytes_cnt += it->info_.size();
  return bytes_cnt;
}
