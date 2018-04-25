#include "Jpeg.h"

Jpeg::HuffmanTree::Node::Node()
	: left(nullptr)
	, right(nullptr)
	, value(-1)
	, code_end(false)
{

}
bool Jpeg::HuffmanTree::add_element(Node*& current_node, int lenght, int value)
{
	if (current_node != nullptr && current_node->code_end == true)
		return false;
	if (current_node == nullptr)
	{
		current_node = new Node();
	}
	if (lenght == 0)
	{
		current_node->value = value;
		current_node->code_end = true;
		return true;
	}
	if (add_element(current_node->left, lenght - 1, value))
	{
		return true;
	}
	if (add_element(current_node->right, lenght - 1, value))
	{
		return true;
	}
	return false;
}

Jpeg::HuffmanTree::HuffmanTree()
	: root(new Node())
	, state(root)
{
}


void Jpeg::HuffmanTree::AddElement(int lenght, int value)
{
	add_element(root, lenght, value);
}

Jpeg::HuffmanTree::Node* Jpeg::HuffmanTree::NextState(int way)
{
	state = (way == 0 ? state->left : state->right);
	if (state == nullptr)
	{
		throw std::exception("Something goes wrong");
	}
	return state;
}

void Jpeg::HuffmanTree::Reset()
{
	state = root;
}

int Jpeg::process_start_of_image(const std::vector<char>& image_content, int index)
{
	return 0;
}

int Jpeg::process_start_of_frame_simple(const std::vector<char>& image_content, int index)
{
	return 0;
}

int Jpeg::process_start_of_frame_extended(const std::vector<char>& image_content, int index)
{
	return 0;
}

int Jpeg::process_start_of_frame_progressive(const std::vector<char>& image_content, int index)
{
	return 0;
}

int Jpeg::process_huffman_table(const std::vector<char>& image_content, int index)
{
}

int Jpeg::process_quantization_table(const std::vector<char>& image_content, int index)
{
	int size_of_table = image_content[index] * 0xFF + image_content[index + 1];

	int value_in_2_bytes = image_content[index + 2] & 0xF0;
	int table_identifier = image_content[index + 2] & 0x0F;
	int size_of_matrix = sqrt(size_of_table - 3);

	quantization_tables[table_identifier].resize(size_of_matrix, std::vector<short>(size_of_matrix));

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
		std::pair<int, int> next_index = indices[t];
		quantization_table[next_index.first][next_index.second] = image_content[index + i];
		if (value_in_2_bytes)
		{
			quantization_table[next_index.first][next_index.second] *= 0xFF;
			quantization_table[next_index.first][next_index.second] += image_content[index + i + 1];
		}
	}

	return size_of_table;
}

int Jpeg::process_restart_interval(const std::vector<char>& image_content, int index)
{
}

int Jpeg::process_start_of_scan(const std::vector<char>& image_content, int index)
{
}

int Jpeg::process_restart(const std::vector<char>& image_content, int index)
{
}

int Jpeg::process_application_specific(const std::vector<char>& image_content, int index)
{
	return 0;
}

int Jpeg::process_comment(const std::vector<char>& image_content, int index)
{
	int size_of_comment = image_content[index] * 0xFF + image_content[index + 1];

	for ( int i = 2 ; i < size_of_comment; i++ )
	{
		comment += image_content[index + i];
	}

	return size_of_comment;
}

int Jpeg::process_end_of_image(const std::vector<char>& image_content, int index)
{
}
