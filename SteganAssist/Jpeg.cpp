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

Jpeg::HuffmanTree::Node * Jpeg::HuffmanTree::GetRoot()
{
	return _root;
}


bool Jpeg::check_for_image_correctness(InputBitStream& _image_content)
{
	if (image_content_.size() < 4) // TODO: understand, how much can be minimal size of picture
	{
		throw std::exception("Too small size of picture");
	}
	if (image_content_[0] * 0x100 + image_content_[1] != 0xFFD8)
	{
		throw std::exception("This is not JPEG file, start bytes are note matching");
	}
	if (image_content_[image_content_.size() - 2] * 0x100 + image_content_[image_content_.size() - 1] != 0xFFD9)
	{
		throw std::exception("This is not JPEG file, end bytes are note matching");
	}
	// TODO: add another checks
}


void Jpeg::process_start_of_frame_simple(InputBitStream& _image_content)
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
		frame._id = image_content_[index_];
		frame._horizontal_thinning = image_content_[index_ + 1] & 0xF0;
		frame._vertical_thinning = image_content_[index_ + 1] & 0x0F;
		frame._id_of_quantization_table = image_content_[index_ + 2];
		_frames.push_back(frame);

		_max_horizontal_thinning = std::max(frame._horizontal_thinning, _max_horizontal_thinning);
		_max_vertical_thinning = std::max(frame._vertical_thinning, _max_vertical_thinning);

		index_ += 3;
	}
	return size_of_frame;
}

void Jpeg::process_start_of_frame_extended(InputBitStream& _image_content)
{
	throw std::exception("Not implemented yet");
}

void Jpeg::process_start_of_frame_progressive(InputBitStream& _image_content)
{
	throw std::exception("Not implemented yet");
}

void Jpeg::process_huffman_table(InputBitStream& _image_content)
{
	int size_of_table = image_content_[index_] * 0x100 + image_content_[index_ + 1];

	int coef_type = image_content_[index_ + 2] & 0xF0;
	int table_id = image_content_[index_ + 2] & 0x0F;

	index_ += 3;

	std::vector<int> huffman_codes_lenght(0x100);
	int number_of_lengths = 0;
	for (int i = 0; i < huffman_codes_lenght.size(); i++)
	{
		huffman_codes_lenght[i] = image_content_[index_ + i];
		number_of_lengths += huffman_codes_lenght[i];
	}

	index_ += huffman_codes_lenght.size();

	std::vector<int> huffman_codes_values(number_of_lengths);
	for (int i = 0; i < huffman_codes_values.size(); i++)
	{
		huffman_codes_values[i] = image_content_[index_ + i];
	}

	index_ += huffman_codes_values.size();

	HuffmanTree* tree = new HuffmanTree(); 
	_huffman_trees[table_id] = tree;

	for (int i = 0, count = 0; i < huffman_codes_lenght.size(); i++)
	{
		while (huffman_codes_lenght[i])
		{
			tree->AddElement(i, huffman_codes_values[count++]);
			huffman_codes_lenght[i]--;
		}
	}
	return size_of_table;
}

void Jpeg::process_quantization_table(InputBitStream& _image_content)
{
	int size_of_table = image_content_[index_] * 0x100 + image_content_[index_ + 1];

	int value_in_2_bytes = image_content_[index_ + 2] & 0xF0;
	int table_id = image_content_[index_ + 2] & 0x0F;
	int size_of_matrix = sqrt(size_of_table - 3);

	_quantization_tables[table_id].resize(size_of_matrix, std::vector<int>(size_of_matrix));

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
		_quantization_tables[table_id][next_index_.first][next_index_.second] = image_content_[index_ + i];
		if (value_in_2_bytes)
		{
			_quantization_tables[table_id][next_index_.first][next_index_.second] *= 0x100;
			_quantization_tables[table_id][next_index_.first][next_index_.second] += image_content_[index_ + i + 1];
		}
	}

	return size_of_table;
}

void Jpeg::process_restart_interval(InputBitStream& _image_content)
{
	throw std::exception("Not implemented yet");
}

void Jpeg::process_start_of_scan(InputBitStream& _image_content)
{
	int header_size = image_content_[index_] * 0x100 + image_content_[index_ + 1];
	int number_components_to_read = image_content_[index_ + 2];

	index_ += 3;

	struct component_t
	{
		int id;
		int id_for_DC_and_AC_coefs;
	};

	std::vector<component_t> components(number_components_to_read);

	for (int i = 0; i < components.size(); i++)
	{
		components[i].id = image_content_[index_ + 2 * i];
		components[i].id_for_DC_and_AC_coefs = image_content_[index_ + 2 * i + 1];
	}

	index_ += components.size() * 2;
	index_ += 3; // TODO: I don't know what exactly means these 3 bytes

	for (int component_number = 0; component_number < components.size(); component_number++)
	{
		HuffmanTree::HuffmanTreeIterator* huffman_tree_iterator = 
			new HuffmanTree::HuffmanTreeIterator(_huffman_trees[components[component_number].id_for_DC_and_AC_coefs & 0xF0]);

		// if ( i % 2 == 0 ) -> DC coef
		// if ( i % 2 == 1 ) -> AC coef

		int size_of_code = 0;
		int first_bit = 0;
		while (index_ < image_content_.size() - 2)
		{
			for (int bit_number = 3; bit_number >= 0; bit_number--)
			{
				int bit = image_content_[index_] & (1 << bit_number);
				huffman_tree_iterator->Step(bit);
				size_of_code++;
				if (size_of_code == 1)
				{
					first_bit = bit;
				}
				if (huffman_tree_iterator->IsCodeEnd())
				{
					int huffman_tree_value = huffman_tree_iterator->GetValue();
					int number_of_0_to_add = huffman_tree_value & 0xF0;
					int bits_to_read = huffman_tree_value & 0x0F;

					int real_value = 0;

					while (bits_to_read--)
					{
						real_value *= 2;
						real_value += 0; // TODO
					}

					if (first_bit == 1)
					{
						
					}
				}
			}
			index_++;
		}
	}
}

void Jpeg::process_restart(InputBitStream& _image_content)
{
	throw std::exception("Not implemented yet");
}

void Jpeg::process_application_specific(InputBitStream& _image_content)
{
}

void Jpeg::process_comment(InputBitStream& _image_content)
{
	byte size_1, size_2;
	_image_content >> size_1 >> size_2;
	int size_of_comment = size_1 * 0x100 + size_2;

	for ( int i = 2 ; i < size_of_comment; i++ )
	{
		byte temp;
		_image_content >> temp;
		_comment += temp;
	}
}


Jpeg::HuffmanTree::HuffmanTreeIterator::HuffmanTreeIterator(HuffmanTree * tree_)
	: _state(tree_->GetRoot())
	, _root(tree_->GetRoot())
{
}

Jpeg::HuffmanTree::HuffmanTreeIterator * Jpeg::HuffmanTree::HuffmanTreeIterator::Step(int way_)
{
	_state = (way_ == 0 ? _state->_left : _state->_right);
	if (_state == nullptr)
	{
		throw std::exception("Something goes wrong");
	}
	return this;
}

bool Jpeg::HuffmanTree::HuffmanTreeIterator::IsCodeEnd()
{
	return _state->_code_end;
}

void Jpeg::HuffmanTree::HuffmanTreeIterator::GetValue()
{
	return _state->_value;
}

void Jpeg::HuffmanTree::HuffmanTreeIterator::Reset()
{
	_state = _root;
}


