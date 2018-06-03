#pragma once
#include<map>
#include<vector>
#include<algorithm>
#include"BitStream.h"
#include"ImageFileBuffer.h"
#include"Image.h"

// [ISO/IEC 10918-1 : 1993(E)]



class Jpeg : public Image
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
	// Table B.1 – Marker code assignments
	enum markers
	{
		// Start Of Frame markers, non-differential, Huffman coding
		SOF0 = 0xC0, // Baseline DCT
		SOF1 = 0xC1, // Extended sequential DCT
		SOF2 = 0xC2, // Progressive DCT
		SOF3 = 0xC3, // Lossless (sequential)
		
		// Start Of Frame markers, differential, Huffman coding
		SOF5 = 0xC5, // Differential sequential DCT
		SOF6 = 0xC6, // Differential progressive DCT
		SOF7 = 0xC7, // Differential lossless (sequential)

		// Start Of Frame markers, non - differential, arithmetic coding
		JPG = 0xC8, // Reserved for JPEG extensions
		SOF9 = 0xC9, // Extended sequential DCT
		SOF10 = 0xCA, // Progressive DCT
		SOF11 = 0xCB, // Lossless (sequential)

		// Start Of Frame markers, differential, arithmetic coding
		SOF13 = 0xCD, // Differential sequential DCT
		SOF14 = 0xCE, // Differential progressive DCT
		SOF15 = 0xCF, // Differential lossless (sequential)

		// Huffman table specification
		DHT = 0xC4, // Define Huffman Table(s)

		// Arithmetic coding conditioning specification
		DAC = 0xCC, // Define arithmetic coding conditioning(s)

		// Restart interval termination
		RST0 = 0xD0, // Restart with modulo 8 count <0>
		RST1 = 0xD1, // Restart with modulo 8 count <1>
		RST2 = 0xD2, // Restart with modulo 8 count <2>
		RST3 = 0xD3, // Restart with modulo 8 count <3>
		RST4 = 0xD4, // Restart with modulo 8 count <4>
		RST5 = 0xD5, // Restart with modulo 8 count <5>
		RST6 = 0xD6, // Restart with modulo 8 count <6>
		RST7 = 0xD7, // Restart with modulo 8 count <7>

		// Other markers
		SOI = 0xD8, // Start of image
		EOI = 0xD9, // End of image
		SOS = 0xDA, // Start of scan
		DQT = 0xDB, // Define quantization table(s)
		DNL = 0xDC, // Define number of lines
		DRI = 0xDD, // Define restart interval
		DHP = 0xDE, // Define hierarchical progression
		EXP = 0xDF, // Expand reference component(s)
		APP0 = 0xE0, // Reserved for application segments
		APP1 = 0xE1, // Reserved for application segments
		APP2 = 0xE2, // Reserved for application segments
		APP3 = 0xE3, // Reserved for application segments
		APP4 = 0xE4, // Reserved for application segments
		APP5 = 0xE5, // Reserved for application segments
		APP6 = 0xE6, // Reserved for application segments
		APP7 = 0xE7, // Reserved for application segments
		JPG0 = 0xF0, // Reserved for JPEG extensions
		JPG1 = 0xF1, // Reserved for JPEG extensions
		JPG2 = 0xF2, // Reserved for JPEG extensions
		JPG3 = 0xF3, // Reserved for JPEG extensions
		JPG4 = 0xF4, // Reserved for JPEG extensions
		JPG5 = 0xF5, // Reserved for JPEG extensions
		JPG6 = 0xF6, // Reserved for JPEG extensions
		JPG7 = 0xF7, // Reserved for JPEG extensions
		COM = 0xFE, // Comment

		// Reserved markers
		TEM = 0x01, // For temporary private use in arithmetic coding
	};
	
	enum coef_type 
	{ 
		// [3.1.35]
		DC = 0,
		// [3.1.2]
		AC = 1,
	};

	bool check_for_image_correctness(InputBitStream& image_content_);
	// void process_start_of_image(InputBitStream& image_content_);

	/**
	 * \verbatim
	 * [B.2.2]
	 * Figure 1: frame header
	 * _________________________________________________________
	 * |    |   |   |   |   |   |   |   |   |    | Comp.-spec. |
	 * |  SOFn  |  Lf   | P |   Y   |   X   | Nf | parameters  |
	 * |____|___|___|___|___|___|___|___|___|____|_____________|
	 *
	 *
	 * Figure 2: frame component-specification parameters
	 * ____________________________________________________
	 * |   |     |    |   |     |    |     |   |     |    |
	 * | C | H|V | Tq | C | H|V | Tq | ... | C | H|V | Tq |
	 * |___|_____|____|___|_____|____|_____|___|_____|____|
	 * |______________|______________|     |______________|
	 *         |              |                    |
	 *         v              v                    v
	 *         1              2                   Nf
	 * \endverbatim
	 *
	 *
	 *
	 * * * * Description for Figure 1 * * * *
	 *
	 * SOFn: Start of frame marker:
	 * > Marks the beginning of the frame parameters.
	 * * The subscript n identifies whether the encoding process is baseline
	 * * sequential, extended sequential, progressive, or lossless, as well
	 * * as which entropy encoding procedure is used.
	 * * SOF0: Baseline DCT
	 * * SOF1: Extended sequential DCT, Huffman coding
	 * * SOF2: Progressive DCT, Huffman coding
	 * * SOF3: Lossless (sequential), Huffman coding
	 * * SOF9: Extended sequential DCT, arithmetic coding
	 * * SOF10: Progressive DCT, arithmetic coding
	 * * SOF11: Lossless (sequential), arithmetic coding
	 *
	 * Lf: Frame header length:
	 * > Specifies the length of the frame header shown in Figure 1.
	 *
	 * P: Sample precision:
	 * > Specifies the precision in bits for the samples of the components in the frame.
	 *
	 * Y: Number of lines:
	 * > Specifies the maximum number of lines in the source image.
	 * * This shall be equal to the number of lines in the component with the maximum
	 * * number of vertical samples. Value 0 indicates that the number of lines shall
	 * * be defined by the DNL marker and parameters at the end of the first scan.
	 *
	 * X: Number of samples per line:
	 * > Specifies the maximum number of samples per line in the source image.
	 * * This shall be equal to the number of samples per line in the component with
	 * * the maximum number of horizontal samples
	 *
	 * Nf: Number of image components in frame:
	 * > Specifies the number of source image components in the frame.
	 * * The value of Nf shall be equal to the number of sets of frame component
	 * * specification parameters (Ci, Hi, Vi, and Tqi) present in the frame header.
	 *
	 *
	 * * * * Description for Figure 2 * * * *
	 *
	 * Ci: Component identifier:
	 * > Assigns a unique label to the ith component in the sequence of frame
	 * > component specification parameters.
	 * * These values shall be used in the scan headers to identify the components in
	 * * the scan. The value of Ci shall be different from the values of C1 through Ci - 1.
	 *
	 * Hi: Horizontal sampling factor:
	 * > Specifies the relationship between the component horizontal dimension and
	 * > maximum image dimension X. Also specifies the number of horizontal data units
	 * > of component Ci in each MCU, when more than one component is encoded in a scan.
	 *
	 * Vi: Vertical sampling factor:
	 * > Specifies the relationship between the component vertical dimension and maximum
	 * > image dimension Y. Also specifies the number of vertical data units of component
	 * > Ci in each MCU, when more than one component is encoded in a scan.
	 *
	 * Tqi: Quantization table destination selector:
	 * > Specifies one of four possible quantization table destinations from which the
	 * > quantization table to use for dequantization of DCT coefficients of component Ci
	 * > is retrieved. If the decoding process uses the dequantization procedure, this table
	 * > shall have been installed in this destination by the time the decoder is ready
	 * > to decode the scans containing component Ci. The destination shall not be respecified,
	 * > or its contents changed, until all scans containing Ci have been completed.
	 *
	 * \verbatim
	 * Figure 1: frame header parameter sizes and values
	 * _____________________________________________________________________________
	 * |           |            |                                                  |
	 * |           |            |                      Values                      |
	 * |           |            |__________________________________________________|
	 * |           |            |                     |                 |          |
	 * | Parameter | Size(bits) |    Sequential DCT   |                 |          |
	 * |           |            |_____________________|                 |          |
	 * |           |            |          |          | Progressive DCT | Lossless |
	 * |           |            | Baseline | Extended |                 |          |
	 * |___________|____________|__________|__________|_________________|__________|
	 * |           |            |                                                  |
	 * |    Lf     |     16     |                   8 + 3 * Nf                     |
	 * |___________|____________|__________________________________________________|
	 * |           |            |          |          |                 |          |
	 * |     P     |      8     |     8    |   8, 12  |      8, 12      |   2-16   |
	 * |___________|____________|__________|__________|_________________|__________|
	 * |           |            |                                                  |
	 * |     Y     |     16     |                     0-65535                      |
	 * |___________|____________|__________________________________________________|
	 * |           |            |                                                  |
	 * |     X     |     16     |                     1-65535                      |
	 * |___________|____________|__________________________________________________|
	 * |           |            |          |          |                 |          |
	 * |    Nf     |      8     |  1-255   |  1-255   |       1-4       |  1-255   |
	 * |___________|____________|__________|__________|_________________|__________|
	 * |           |            |                                                  |
	 * |    Ci     |      8     |                      0-255                       |
	 * |___________|____________|__________________________________________________|
	 * |           |            |                                                  |
	 * |    Hi     |      4     |                       1-4                        |
	 * |___________|____________|__________________________________________________|
	 * |           |            |                                                  |
	 * |    Vi     |      4     |                       1-4                        |
	 * |___________|____________|__________________________________________________|
	 * |           |            |          |          |                 |          |
	 * |    Tqi    |      8     |   0-3    |   0-3    |       0-3       |    0     |
	 * |___________|____________|__________|__________|_________________|__________|
	 * \endverbatim
	 *
	 */

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
	Jpeg(const std::string& file_path_)
		: _image_content(ImageFileBuffer(file_path_).Get())
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
