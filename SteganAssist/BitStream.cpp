#include"BitStream.h"


InputBitStream::InputBitStream(const std::vector<unsigned char>& buffer_)
	: _buffer(buffer_)
	, _index(0)
	, _bit_number(7)
	, _stream_end(false)
{
}

InputBitStream::InputBitStream(const std::string & buffer_)
	: _buffer(buffer_.begin(), buffer_.end())
	, _index(0)
	, _bit_number(7)
	, _stream_end(false)
{
}
InputBitStream::operator bool() const
{
	return !_stream_end;
}

void InputBitStream::BitsBack(int number_of_bits_to_revert_)
{
	int chars_number = number_of_bits_to_revert_ / 8;
	number_of_bits_to_revert_ -= chars_number * 8;
	_bit_number -= number_of_bits_to_revert_;
	if (_bit_number < 0)
	{
		_bit_number += 8;
		_index--;
	}
	this->CharsBack(chars_number);
}

void InputBitStream::CharsBack(int number_of_chars_to_revert_)
{
	_index -= number_of_chars_to_revert_;
	_index = std::max(_index, 0);
}
