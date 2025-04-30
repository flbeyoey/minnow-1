#include "tcp_receiver.hh"
#include "debug.hh"
#include <iostream>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if ( message.SYN ) {
    ISN = message.seqno;
  }

  if ( message.RST ) {
    reassembler_.reader().set_error();
    return;
  }

  if ( message.SYN )
    ackno_ = message.seqno + message.sequence_length();
  if ( ackno_.has_value() ) {
    if ( ISN == message.seqno && !message.SYN )
      return;
    uint64_t first_index = message.seqno.unwrap( ISN, reassembler_.writer().bytes_pushed() ) - ( !message.SYN );
    reassembler_.insert( first_index, message.payload, message.FIN );

    auto next_seg_no = Wrap32::wrap( reassembler_.writer().bytes_pushed() + 1, ISN );
    auto FIN_seg_no = Wrap32::wrap( reassembler_.writer().bytes_pushed() + 2, ISN );
    if ( message.FIN )
      finno_ = message.seqno + message.sequence_length();
    if ( finno_.has_value() && FIN_seg_no == finno_ ) {
      ackno_ = FIN_seg_no;
    } else
      ackno_ = next_seg_no;
  }

  return;
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.

  return TCPReceiverMessage {
    ackno_,
    static_cast<uint16_t>( min( reassembler_.writer().available_capacity(), static_cast<uint64_t> UINT16_MAX ) ),
    reassembler_.reader().has_error() };
}
