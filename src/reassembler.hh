#pragma once
#include "byte_stream.hh"
#include <set>
class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ) {}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  // This function is for testing only; don't add extra state to support it.
  uint64_t count_bytes_pending() const;
  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }
  void output_block_stack();

private:
  class BlockStreamInfo
  {
  public:
    uint64_t L, R;
    std::string info_;
    auto operator<=>( const BlockStreamInfo& rhs ) const = default;
    BlockStreamInfo( const uint64_t l_, const uint64_t r_, const std::string& i_ ) : L( l_ ), R( r_ ), info_( i_ )
    {}
    void merge_back( const BlockStreamInfo& rhs )
    {
      if ( rhs.R <= R )
        return;
      info_.append( rhs.info_, R - rhs.L + 1, rhs.R - R );
      R = rhs.R;
    }
    void merge_front( const BlockStreamInfo& lhs )
    {
      std::string str = lhs.info_;
      if ( lhs.R < R )
        str.append( info_, lhs.R - L + 1, R - lhs.R );
      L = lhs.L;
      R = std::max( R, lhs.R );
      info_ = str;
    }
  };

  ByteStream output_;
  uint64_t next_expected_seq_ = 0, first_unpoped_index = 0;
  uint64_t available_capacity() const { return output_.writer().available_capacity(); }
  std::set<BlockStreamInfo> block_storage_ {};
  uint64_t last_pos = -1;
  uint64_t insert_cnt = 0;
};
