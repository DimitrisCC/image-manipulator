#include "Color.h"
#include "Block.h"
#include <stdlib.h>
#include <cstring>
#include <iostream>

//
// Bonus!
//
// Βlock iterator below used to exhaustively read the sequence of
// data in a block.

namespace imaging{

		Block::Block(const size_t & block_size)   // Create an empty block. There is no default constructor. It doesn't make sense.
			: data(NULL), size(block_size), error_margin(0){}

		Block::Block(const Block & src)	// Copy constructor: remember, you need a deep copy of the buffer
			: data(new Component[src.size]), size(src.size), error_margin(src.error_margin){

			setData(src.data);
		}

		// Destructor
		Block::~Block(){
			if (data) delete[] data;
		}

		Block * Block::copyFromImage(Image & src, Image::channel_t channel, const ivec2 & pos, const size_t & size){

			if (((int) size == 0) || ((pos.y > (int) src.getHeight() - 1)) || ((pos.x > (int) src.getWidth() - 1)) || ((int) size > (int) src.getWidth()))
				return NULL;

			int copy_size = (size < (src.getWidth() - pos.x)) ? size : src.getWidth() - pos.x;
			Block * copied_block = new Block(copy_size);

			Component * temp_copy = new Component[copy_size];
 
			Color current_pixel;
			for (int i = 0; i < copy_size; i++){
				current_pixel = src.getPixel(pos.x + i, pos.y);
				switch (channel){
				case 0:
					temp_copy[i] = current_pixel.x;
					break;
				case 1:
					temp_copy[i] = current_pixel.y;
					break;
				case 2:
					temp_copy[i] = current_pixel.z;
					break;
				}
				
			}

			copied_block->setData(temp_copy);

			delete[] temp_copy;

			return copied_block;

		}

		// Size accessors
		size_t Block::getSize() const { return size; }

		// Obtain the pointer to the internal data. Useful for fast data copies from the block
		Component * Block::getDataPtr() const { return data; }

		// Copies the size Component tokens from the provided array to the internal buffer
		void Block::setData(const Component * src){
			if (!data) data = new Component[size];
			memcpy(data, src, size*sizeof(Component));
		}

		// Creates a COPY of the current block and reverses the order of its elements.
		Block Block::reverse() const{

			Block returned(*this);

			Component * temp = new Component[size]; //MA

			for (unsigned int i = 0; i < size; i++){
				temp[i] = data[size - 1 - i];
			}

			returned.setData(temp);

			delete[] temp;

			return returned;

		}

		// Specifies the maximum allowed difference between the i-th cells of two blocks, i=0...size-1, so that
		// the two blocks can be considered equal.
		void Block::setErrorMargin(Component err) { error_margin = err; }

		// Return the index-th element of the block. No bounds are checked, for speed.
		// Caution: Since no bounds are checked, a segmentation fault may occur for invalid index values.
		Component & Block::operator[] (const size_t index) const{
			return data[index];
		}

		// Same as [], but with bounds checking. If out of bounds return the last element
		Component & Block::operator() (const size_t & index) const{

			if ((index < 0) || (index >= size)) return data[size - 1];

			return data[index];

		}

		// "equals" operator. Should return true if ALL components of a block are equal to those of the
		// rhs block. Remember equality is checked using the error_margin: |data[i]-rhs.data[i]|<=error_margin.
		// Caution: In the above equation the quantities are "unsigned". Use appropriate casting to make the formula work!
		// Hint: You can implement it with a "fast compute path", when the error_margin is 0 (no abs() function needs
		// to be called), just check for equality of elements. Also, you can have an early exit mechanism if at least one
		// element pair doesn't satisfy the above equality criteria.
		bool Block::operator==(const Block& rhs) const{

			if (size != rhs.size) return false;

			if (error_margin != rhs.error_margin) return false;

			if (error_margin == 0){

				for (unsigned int i = 0; i < size; i++){
					if (data[i] != rhs.data[i]) return false;
				}

			}
			else{

				for (unsigned int i = 0; i < size; i++){
					if (abs((int)data[i] - (int)rhs.data[i]) > (int)error_margin) return false;
				}

			}

			return true;

		}

		// The "not equal" operator. Should return true if ANY of the elements in the two blocks differ.
		// You can implement this as "!equal"
		bool Block::operator!=(const Block& rhs) const{
			return !(*this == rhs);
		}

		// Assignment operator. Be careful: You need to to perform a deep copy of the buffer.
		Block & Block::operator=(const Block & src){

			if (src == *this) return *this;

			if (data) delete[] data;

			size = src.size;
			error_margin = src.error_margin;
			setData(src.data);

			return *this;

		}


		// Optional
		// Returns an iterator to the first element of the block (leftmost)
		Block::iterator Block::begin() const {
			Block* this_block = const_cast<Block*>(this);
			Block::iterator beg_iter;
			beg_iter = Block::iterator(this_block, 0);
			return beg_iter;
		}

		// Returns an iterator "beyond" the last element of the block (i.e. invalid, out of range)
		Block::iterator Block::end() const {
			Block* this_block = const_cast<Block*>(this);
			Block::iterator end_iter;
			end_iter = Block::iterator(this_block, size);
			return end_iter;
		} // size IS out of bounds




		//OPTIONAL BLOCK ITERATOR

		Block::iterator::iterator(Block *p_data)
			: block_ptr(p_data),  block_size(p_data->size), iter(0) {}	   // Ah..  a practical one. Should start at element 0.

		Block::iterator::iterator(Block::iterator& src)		   // copy constructor
			: block_ptr(src.block_ptr), block_size(src.block_size), iter(src.iter){}


		Block::iterator& Block::iterator::operator++(){
			if (iter < block_size){
				iter++; 
			}
			return *this;
		}// iterator forward operation

		Block::iterator Block::iterator::operator++(int) {
			Block::iterator temp(*this);
			operator++();
			return temp;
		}

		bool Block::iterator::operator==(const Block::iterator& rhs){
			return (iter == rhs.iter);
		} // equality check

		bool Block::iterator::operator!=(const Block::iterator& rhs){
			return !this->operator==(rhs);
		} // inequality check

		Component & Block::iterator::operator*() const{
			return (*block_ptr)(iter);
		}// content retrieval operator. Remember,
		// should return a reference to the original data
		// so that you can actually read/write from/to the
		// block the iterator runs on.


		// make this constructor protected and allow only the using class (Block)
		// to initialize the iterator with a position argument (to implement Block's
		// begin() and end() member functions )
		Block::iterator::iterator(Block *p_data, const size_t position)
			: block_ptr(p_data), block_size(p_data->size), iter(position){ }

}
