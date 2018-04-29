#include "Jpeg.h"

Jpeg::HuffmanTree::Node::Node()
	: _left(nullptr)
	, _right(nullptr)
	, _value(-1)
	, _code_end(false)
{

}
bool Jpeg::HuffmanTree::add_element(Node*& current_node, int lenght, int value)
{
	if (current_node != nullptr && current_node->_code_end == true)
		return false;
	if (current_node == nullptr)
	{
		current_node = new Node();
	}
	if (lenght == 0)
	{
		current_node->_value = value;
		current_node->_code_end = true;
		return true;
	}
	if (add_element(current_node->_left, lenght - 1, value))
	{
		return true;
	}
	if (add_element(current_node->_right, lenght - 1, value))
	{
		return true;
	}
	return false;
}

Jpeg::HuffmanTree::HuffmanTree()
	: _root(new Node())
	, _state(_root)
{
}


void Jpeg::HuffmanTree::AddElement(int lenght_, int value_)
{
	add_element(_root, lenght_, value_);
}

Jpeg::HuffmanTree::Node* Jpeg::HuffmanTree::NextState(int way_)
{
	_state = (way_ == 0 ? _state->_left : _state->_right);
	if (_state == nullptr)
	{
		throw std::exception("Something goes wrong");
	}
	return _state;
}

void Jpeg::HuffmanTree::Reset()
{
	_state = _root;
}

int Jpeg::process_start_of_image(const std::vector<char>& image_content__, int index_)
{
	return 0;
}

int Jpeg::process_start_of_frame_simple(const std::vector<char>& image_content_, int index_)
{
	int size_of_frame = image_content_[index_] * 0x100 + image_content_[index_ + 1];

	int precision = image_content_[index_ + 2];
	int picture_height = image_content_[index_ + 3] * 0x100 + image_content_[index_ + 4];
	int picture_width = image_content_[index_ + 5] * 0x100 + image_content_[index_ + 6];

	int components_count = image_content_[index_ + 7];
	index_ += 8;

	_max_horizontal_thinning = SHRT_MIN;
	_max_vertical_thinning = SHRT_MIN;

	for (int i = 0; i < components_count; i++)
	{
		Frame frame;
		frame._identifier = image_content_[index_];
		frame._horizontal_thinning = image_content_[index_ + 1] & 0xF0;
		frame._vertical_thinning = image_content_[index_ + 1] & 0x0F;
		frame._identifier_of_quantization_table = image_content_[index_ + 2];
		_frames.push_back(frame);

		_max_horizontal_thinning = std::max(frame._horizontal_thinning, _max_horizontal_thinning);
		_max_vertical_thinning = std::max(frame._vertical_thinning, _max_vertical_thinning);

		index_ += 3;
	}
	return size_of_frame;
}

int Jpeg::process_start_of_frame_extended(const std::vector<char>& image_content_, int index_)
{
	return 0;
}

int Jpeg::process_start_of_frame_progressive(const std::vector<char>& image_content_, int index_)
{
	return 0;
}

int Jpeg::process_huffman_table(const std::vector<char>& image_content_, int index_)
{
	int size_of_table = image_content_[index_] * 0x100 + image_content_[index_ + 1];

}

int Jpeg::process_quantization_table(const std::vector<char>& image_content_, int index_)
{
	int size_of_table = image_content_[index_] * 0x100 + image_content_[index_ + 1];

	int value_in_2_bytes = image_content_[index_ + 2] & 0xF0;
	int table_identifier = image_content_[index_ + 2] & 0x0F;
	int size_of_matrix = sqrt(size_of_table - 3);

	_quantization_tables[table_identifier].resize(size_of_matrix, std::vector<short>(size_of_matrix));

	std::vector<std::pair<int, int>> indices(size_of_table - 3);
	
	// Calculating indices for zigzag order traversal
	for (int i = 0, t = 1; i < 2 * size_of_matrix - 1; i++)
	{
		if (i % 2 == 1) 
		{
			int x = i < size_of_matrix ? 0 : i - size_of_matrix + 1;
			int y = i < size_of_matrix ? i : size_of_matrix - 1;
			while (x < size_of_matrix && y >= 0) {
				indices[t++] = {x++, y--};
			}
		}
		else 
		{
			int x = i < size_of_matrix ? i : size_of_matrix - 1;
			int y = i < size_of_matrix ? 0 : i - size_of_matrix + 1;
			while (x >= 0 && y < size_of_matrix) {
				indices[t++] = {x--, y++};
			}
		}
	}

	for (int i = 3, t = 0; i < size_of_table; i += 1 + value_in_2_bytes, t++)
	{
		std::pair<int, int> next_index_ = indices[t];
		_quantization_tables[table_identifier][next_index_.first][next_index_.second] = image_content_[index_ + i];
		if (value_in_2_bytes)
		{
			_quantization_tables[table_identifier][next_index_.first][next_index_.second] *= 0x100;
			_quantization_tables[table_identifier][next_index_.first][next_index_.second] += image_content_[index_ + i + 1];
		}
	}

	return size_of_table;
}

int Jpeg::process_restart_interval(const std::vector<char>& image_content_, int index_)
{
}

int Jpeg::process_start_of_scan(const std::vector<char>& image_content_, int index_)
{
}

int Jpeg::process_restart(const std::vector<char>& image_content_, int index_)
{
}

int Jpeg::process_application_specific(const std::vector<char>& image_content_, int index_)
{
	return 0;
}

int Jpeg::process_comment(const std::vector<char>& image_content_, int index_)
{
	int size_of_comment = image_content_[index_] * 0x100 + image_content_[index_ + 1];

	for ( int i = 2 ; i < size_of_comment; i++ )
	{
		_comment += image_content_[index_ + i];
	}

	return size_of_comment;
}

int Jpeg::process_end_of_image(const std::vector<char>& image_content_, int index_)
{
}
