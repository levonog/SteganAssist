#pragma once
#include<vector>
#include<string>
#include<algorithm>

class BitStream
{
	// TODO: realize with inheritance?
};


/// InputBitStream class, that allows read from buffer bit by bit
/// TODO: understand how chars can be read from this stream. Manipulators? 
class InputBitStream
{
private:

	std::vector<unsigned char> _buffer;
	int _index;
	int _bit_number;
	bool _stream_end;

public:

	InputBitStream(const std::vector<unsigned char>& buffer_);
	InputBitStream(const std::string& buffer_);
	operator bool() const;

	void BitsBack(int number_of_bits_to_revert_);
	void CharsBack(int number_of_chars_to_revert_);


	template<class T>
	friend InputBitStream& operator>> (InputBitStream& ibs, T& value);
};

template<class T>
inline InputBitStream & operator>>(InputBitStream & ibs_, T& value_)
{
	value_ = 0;
	if (ibs_._index < ibs_._buffer.size())
	{
		value_ = bool(ibs_._buffer[ibs_._index] & (1 << ibs_._bit_number));
		ibs_._bit_number--;
		if (ibs_._bit_number == -1)
		{
			ibs_._bit_number = 7;
			ibs_._index++;
		}
	}
	else
	{
		ibs_._stream_end = true;
	}
	return ibs_;
}



/// InputBitStream class, that allows write to buffer bit by bit
class OutputBitStream
{
	// TODO
};