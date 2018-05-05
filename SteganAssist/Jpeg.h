#pragma once
#include<map>
#include<vector>
#include<algorithm>
#include"BitStream.h"
#include"ImageFileBuffer.h"

class Jpeg
{
	class HuffmanTree
	{
	public:

		struct Node
		{
			Node *_left, *_right;
			byte _value;
			bool _code_end;
			Node();
		};

		class HuffmanTreeIterator
		{
			Node* _state;
			Node* _root;
		public:
			HuffmanTreeIterator(HuffmanTree* tree_);
			HuffmanTreeIterator* Step(int way_);
			bool IsCodeEnd();
			byte GetValue();
			void Reset();
		};
	private:
		
		Node* _root;
		Node* _state;

		bool add_element(Node*& current_node_, int lenght_, int value_);
	public:

		HuffmanTree();
		void AddElement(int lenght_, int value_);
		Node* GetRoot();
	};

	
	struct Frame
	{
		byte _id;
		byte _horizontal_thinning;
		byte _vertical_thinning;
		byte _id_of_quantization_table;
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
	enum coef_type 
	{ 
		DC = 0, 
		AC = 1 
	};
	bool check_for_image_correctness(InputBitStream& image_content_);
	// void process_start_of_image(InputBitStream& image_content_);
	void process_start_of_frame_simple(InputBitStream& image_content_);
	void process_start_of_frame_extended(InputBitStream& image_content_);
	void process_start_of_frame_progressive(InputBitStream& image_content_);
	void process_huffman_table(InputBitStream& image_content_);
	void process_quantization_table(InputBitStream& image_content_);
	void process_restart_interval(InputBitStream& image_content_);
	void process_start_of_scan(InputBitStream& image_content_);
	void process_restart(InputBitStream& image_content_);
	void process_application_specific(InputBitStream& image_content_);
	void process_comment(InputBitStream& image_content_);
	void calculating_zigzag_order_traversal(int size_of_table_, int size_of_matrix_);
	// void process_end_of_image(InputBitStream& image_content_);

	InputBitStream _image_content;

	std::map<int,std::vector<HuffmanTree*>> _huffman_trees; // trees for DC and AC coefs
	std::string _comment;
	std::vector<std::vector<std::vector<int>>> _quantization_tables;
	std::vector<std::pair<int, int>> _zigzag_order_traversal_indices;
	std::vector<Frame> _frames;
	byte _max_horizontal_thinning;
	byte _max_vertical_thinning;


public:
	// Jpeg(const std::vector<unsigned char>& image_content_)
	Jpeg(const std::string& file_path)
		: _image_content(ImageFileBuffer(file_path).Get())
	{
		// check_for_image_correctness(_image_content);

		_quantization_tables.resize(0x10);

		byte temp;
		while ( _image_content >> temp )
		{
			if (temp != 0xFF)
			{
				throw std::exception("There must be 0xFF byte");
			}
			byte marker;
			_image_content >> marker;
				
			switch (marker)
			{
			case SOI:
				/*i += process_start_of_image(_image_content);*/
				break;
			case SOF0:
				process_start_of_frame_simple(_image_content);
				break;
			case SOF1:
				process_start_of_frame_extended(_image_content);
				break;
			case SOF2:
				process_start_of_frame_progressive(_image_content);
				break;
			case DHT:
				process_huffman_table(_image_content);
				break;
			case DQT:
				process_quantization_table(_image_content);
				break;
			case DRI:
				process_restart_interval(_image_content);
				break;
			case SOS:
				process_start_of_scan(_image_content);
				break;
			case RST0:
			case RST1:
			case RST2:
			case RST3:
			case RST4:
			case RST5:
			case RST6:
			case RST7:
				_image_content.BytesBack(1); // 1 is for understanding restart type
				process_restart(_image_content); 
				break;
			case APP0:
			case APP1:
			case APP2:
			case APP3:
			case APP4:
			case APP5:
			case APP6:
			case APP7:
				_image_content.BytesBack(1); // 1 is for understanding application type
				process_application_specific(_image_content);
				break;
			case COM:
				process_comment(_image_content);
				break;
			case EOI:
				break;
			//	process_end_of_image(_image_content);
			default:
				throw std::runtime_error("Strange type of marker");
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
