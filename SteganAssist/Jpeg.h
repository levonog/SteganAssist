#pragma once
#include<vector>
#include<algorithm>

class Jpeg
{
	class HuffmanTree
	{
	public:
		struct Node
		{
			Node *_left, *_right;
			int _value;
			bool _code_end;
			Node();
		};
	private:
		
		Node* _root;
		Node* _state;

		bool add_element(Node*& current_node_, int lenght_, int value_);
	public:

		HuffmanTree();
		void AddElement(int lenght_, int value_);
		Node* NextState(int way_);
		void Reset();
	};

	struct Frame
	{
		short _identifier;
		short _horizontal_thinning;
		short _vertical_thinning;
		short _identifier_of_quantization_table;
	};

private:
	enum markers
	{
		SOI = 0xD8, // Start of Image
		SOF0 = 0xC0, // Start Of Frame
		SOF1 = 0xC1, //Start Of Frame(extended DCT)
		SOF2 = 0xC2, //Start Of Frame(progressive DCT)
		DHT = 0xC4, // Define Huffman Table(s)
		DQT = 0xDB, // Define Quantization Table(s)
		DRI = 0xDD, // Define Restart Interval
		SOS = 0xDA, // Start Of Scan
		RST0 = 0xD0, // Restart. Inserted every
		RST1 = 0xD1, // 			r macroblocks, where
		RST2 = 0xD2, // 			r is the restart interval
		RST3 = 0xD3, // 			set by a DRI marker. 
		RST4 = 0xD4, // 			Not used if there was 
		RST5 = 0xD5, // 			no DRI marker. The low
		RST6 = 0xD6, // 			three bits of the marker
		RST7 = 0xD7, // 			code cycle in value from 0 to 7.
		APP0 = 0xE0, // Application-specific
		APP1 = 0xE1, // Application-specific
		APP2 = 0xE2, // Application-specific
		APP3 = 0xE3, // Application-specific
		APP4 = 0xE4, // Application-specific
		APP5 = 0xE5, // Application-specific
		APP6 = 0xE6, // Application-specific
		APP7 = 0xE7, // Application-specific
		COM = 0xFE, // Comment
		EOI = 0xD9, // End Of Image
	};
	int process_start_of_image(const std::vector<char>& image_content_, int index_);
	int process_start_of_frame_simple(const std::vector<char>& image_content_, int index_);
	int process_start_of_frame_extended(const std::vector<char>& image_content_, int index_);
	int process_start_of_frame_progressive(const std::vector<char>& image_content_, int index_);
	int process_huffman_table(const std::vector<char>& image_content_, int index_);
	int process_quantization_table(const std::vector<char>& image_content_, int index_);
	int process_restart_interval(const std::vector<char>& image_content_, int index_);
	int process_start_of_scan(const std::vector<char>& image_content_, int index_);
	int process_restart(const std::vector<char>& image_content_, int index_);
	int process_application_specific(const std::vector<char>& image_content_, int index_);
	int process_comment(const std::vector<char>& image_content_, int index_);
	int process_end_of_image(const std::vector<char>& image_content_, int index_);
	HuffmanTree* _tree;
	std::string _comment;
	std::vector<std::vector<std::vector<short>>> _quantization_tables;
	std::vector<Frame> _frames;
	short _max_horizontal_thinning;
	short _max_vertical_thinning;


public:
	Jpeg(const std::vector<char>& image_content_)
	{
		_quantization_tables.resize(0x10);

		for (int i = 0; i < image_content_.size() - 1; i++)
		{
			if (image_content_[i] == 0xFF)
			{
				char marker = image_content_[i + 1];
				switch (marker)
				{
				case SOI:
					i += process_start_of_image(image_content_, i + 2);
				case SOF0:
					i += process_start_of_frame_simple(image_content_, i + 2);
				case SOF1:
					i += process_start_of_frame_extended(image_content_, i + 2);
				case SOF2:
					i += process_start_of_frame_progressive(image_content_, i + 2);
				case DHT:
					i += process_huffman_table(image_content_, i + 2);
				case DQT:
					i += process_quantization_table(image_content_, i + 2);
				case DRI:
					i += process_restart_interval(image_content_, i + 2);
				case SOS:
					i += process_start_of_scan(image_content_, i + 2);
				case RST0:
				case RST1:
				case RST2:
				case RST3:
				case RST4:
				case RST5:
				case RST6:
				case RST7:
					i += process_restart(image_content_, i + 1); // + 1 is for understanding restart type
				case APP0:
				case APP1:
				case APP2:
				case APP3:
				case APP4:
				case APP5:
				case APP6:
				case APP7:
					i += process_application_specific(image_content_, i + 1); // + 1 is for understanding application type
				case COM:
					i += process_comment(image_content_, i + 2);
				case EOI:
					i += process_end_of_image(image_content_, i + 2);
				default:
					throw std::runtime_error("Strange type of marker");
				}
				i++;
			}
		}



		/*tree = new HuffmanTree();
		tree->AddElement(1, 1);
		tree->AddElement(3, 0);
		tree->AddElement(3, 12);
		tree->AddElement(4, 2);
		tree->AddElement(4, 11);
		tree->AddElement(4, 31);
		tree->AddElement(5, 21);
		std::vector<int> vec = { 1, 1, 1, 0 };
		int i;
		for (i = 0; i < vec.size(); i++)
		{
			if (tree->NextState(vec[i])->code_end)
			{
				break;
			}
		}
		i;*/
	}




};
