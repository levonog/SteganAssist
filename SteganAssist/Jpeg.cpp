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
}

int Jpeg::process_start_of_frame(const std::vector<char>& image_content, int index)
{
}

int Jpeg::process_huffman_table(const std::vector<char>& image_content, int index)
{
}

int Jpeg::process_quantization_table(const std::vector<char>& image_content, int index)
{
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
}

int Jpeg::process_end_of_image(const std::vector<char>& image_content, int index)
{
}
