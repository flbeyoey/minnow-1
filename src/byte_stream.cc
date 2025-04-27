#include "byte_stream.hh"
#include <iostream>
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  //(void)data; // Your code here.
  uint64_t sum_cap = data.size() + used_capacity_;
  if ( sum_cap <= capacity_ ) {
    stream_pool_.append( data );
    used_capacity_ = sum_cap;
    total_pushed_ += data.size();
  } else {
    stream_pool_.append( data, 0, capacity_ - used_capacity_ );
    total_pushed_ += capacity_ - used_capacity_;
    used_capacity_ = capacity_;
  }
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

bool Writer::is_closed() const
{
  return closed_; // Your code here.
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - used_capacity_; // Your code here.
}

uint64_t Writer::bytes_pushed() const
{
  return total_pushed_; // Your code here.
}

string_view Reader::peek() const
{
  string_view ret( stream_pool_ );
  return ret; // Your code here.
}

void Reader::pop( uint64_t len )
{
  // (void)len; // Your code here.
  stream_pool_ = stream_pool_.substr( len );
  used_capacity_ -= len;
  total_popped_ += len;
}

bool Reader::is_finished() const
{
  return used_capacity_ == 0 && closed_; // Your code here.
}

uint64_t Reader::bytes_buffered() const
{
  return used_capacity_; // Your code here.
}

uint64_t Reader::bytes_popped() const
{
  return total_popped_; // Your code here.
}
