#ifndef IVL_vvp_darray_H
#define IVL_vvp_darray_H
/*
 * Copyright (c) 2012-2025 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

# include  "vvp_object.h"
# include  "vvp_net.h"
# include  <deque>
# include  <string>
# include  <vector>

class vvp_darray : பொது vvp_object {

    பொது:
      உள்வரி vvp_darray() { }
      மெய்நிகர் ~vvp_darray() override;

      மெய்நிகர் size_t get_size(வெறுமை) மாறிலி =0;

      மெய்நிகர் வெறுமை set_word(unsigned adr, மாறிலி vvp_vector4_t&மதிப்பு);
      மெய்நிகர் வெறுமை get_word(unsigned adr, vvp_vector4_t&மதிப்பு);

      மெய்நிகர் வெறுமை set_word(unsigned adr, இரட்டை மதிப்பு);
      மெய்நிகர் வெறுமை get_word(unsigned adr, இரட்டை&மதிப்பு);

      மெய்நிகர் வெறுமை set_word(unsigned adr, மாறிலி std::string&மதிப்பு);
      மெய்நிகர் வெறுமை get_word(unsigned adr, std::string&மதிப்பு);

      மெய்நிகர் வெறுமை set_word(unsigned adr, மாறிலி vvp_object_t&மதிப்பு);
      மெய்நிகர் வெறுமை get_word(unsigned adr, vvp_object_t&மதிப்பு);

      மெய்நிகர் vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      மெய்நிகர் வெறுமை reverse_elems(வெறுமை);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      மெய்நிகர் வெறுமை sort_elems(bool ascending);
      மெய்நிகர் வெறுமை shuffle_elems(வெறுமை);
};

template <class TYPE> class vvp_darray_atom : பொது vvp_darray {

    பொது:
      explicit உள்வரி vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_size(வெறுமை) மாறிலி override;
      வெறுமை reverse_elems(வெறுமை) override;
      வெறுமை sort_elems(bool ascending) override;
      வெறுமை shuffle_elems(வெறுமை) override;
      வெறுமை set_word(unsigned adr, மாறிலி vvp_vector4_t&மதிப்பு) override;
      வெறுமை get_word(unsigned adr, vvp_vector4_t&மதிப்பு) override;
      வெறுமை shallow_copy(மாறிலி vvp_object*obj) override;
      vvp_object* duplicate(வெறுமை) மாறிலி override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    தனிப்பட்ட:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : பொது vvp_darray {

    பொது:
      உள்வரி vvp_darray_vec4(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_size(வெறுமை) மாறிலி override;
      வெறுமை set_word(unsigned adr, மாறிலி vvp_vector4_t&மதிப்பு) override;
      வெறுமை get_word(unsigned adr, vvp_vector4_t&மதிப்பு) override;
      வெறுமை shallow_copy(மாறிலி vvp_object*obj) override;
      vvp_object* duplicate(வெறுமை) மாறிலி override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      வெறுமை reverse_elems(வெறுமை) override;
      வெறுமை sort_elems(bool ascending) override;
      வெறுமை shuffle_elems(வெறுமை) override;

    தனிப்பட்ட:
      std::vector<vvp_vector4_t> array_;
      unsigned word_wid_;
};

class vvp_darray_vec2 : பொது vvp_darray {

    பொது:
      உள்வரி vvp_darray_vec2(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_size(வெறுமை) மாறிலி override;
      வெறுமை reverse_elems(வெறுமை) override;
      வெறுமை sort_elems(bool ascending) override;
      வெறுமை shuffle_elems(வெறுமை) override;
      வெறுமை set_word(unsigned adr, மாறிலி vvp_vector4_t&மதிப்பு) override;
      வெறுமை get_word(unsigned adr, vvp_vector4_t&மதிப்பு) override;
      வெறுமை shallow_copy(மாறிலி vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    தனிப்பட்ட:
      std::vector<vvp_vector2_t> array_;
      unsigned word_wid_;
};

class vvp_darray_real : பொது vvp_darray {

    பொது:
      explicit உள்வரி vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_size(வெறுமை) மாறிலி override;
      வெறுமை set_word(unsigned adr, இரட்டை மதிப்பு) override;
      வெறுமை get_word(unsigned adr, இரட்டை&மதிப்பு) override;
      வெறுமை shallow_copy(மாறிலி vvp_object*obj) override;
      vvp_object* duplicate(வெறுமை) மாறிலி override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      வெறுமை reverse_elems(வெறுமை) override;
      வெறுமை sort_elems(bool ascending) override;
      வெறுமை shuffle_elems(வெறுமை) override;

    தனிப்பட்ட:
      std::vector<இரட்டை> array_;
};

class vvp_darray_string : பொது vvp_darray {

    பொது:
      explicit உள்வரி vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_size(வெறுமை) மாறிலி override;
      வெறுமை set_word(unsigned adr, மாறிலி std::string&மதிப்பு) override;
      வெறுமை get_word(unsigned adr, std::string&மதிப்பு) override;
      வெறுமை shallow_copy(மாறிலி vvp_object*obj) override;
      vvp_object* duplicate(வெறுமை) மாறிலி override;
      வெறுமை reverse_elems(வெறுமை) override;
      வெறுமை sort_elems(bool ascending) override;
      வெறுமை shuffle_elems(வெறுமை) override;

    தனிப்பட்ட:
      std::vector<std::string> array_;
};

class vvp_darray_object : பொது vvp_darray {

    பொது:
      explicit உள்வரி vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_size(வெறுமை) மாறிலி override;
      வெறுமை reverse_elems(வெறுமை) override;
      வெறுமை sort_elems(bool ascending) override;
      வெறுமை shuffle_elems(வெறுமை) override;
      வெறுமை set_word(unsigned adr, மாறிலி vvp_object_t&மதிப்பு) override;
      வெறுமை get_word(unsigned adr, vvp_object_t&மதிப்பு) override;
      வெறுமை shallow_copy(மாறிலி vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    தனிப்பட்ட:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : பொது vvp_darray {

    பொது:
      உள்வரி vvp_queue(வெறுமை) { }
      ~vvp_queue() override;

      மெய்நிகர் size_t get_size(வெறுமை) மாறிலி override =0;
      மெய்நிகர் வெறுமை copy_elems(vvp_object_t src, unsigned max_size);

      மெய்நிகர் வெறுமை set_word_max(unsigned adr, மாறிலி vvp_vector4_t&மதிப்பு, unsigned max_size);
      மெய்நிகர் வெறுமை செருகு(unsigned idx, மாறிலி vvp_vector4_t&மதிப்பு, unsigned max_size);
      மெய்நிகர் வெறுமை தள்ளு_back(மாறிலி vvp_vector4_t&மதிப்பு, unsigned max_size);
      மெய்நிகர் வெறுமை தள்ளு_front(மாறிலி vvp_vector4_t&மதிப்பு, unsigned max_size);

      மெய்நிகர் வெறுமை set_word_max(unsigned adr, இரட்டை மதிப்பு, unsigned max_size);
      மெய்நிகர் வெறுமை செருகு(unsigned idx, இரட்டை மதிப்பு, unsigned max_size);
      மெய்நிகர் வெறுமை தள்ளு_back(இரட்டை மதிப்பு, unsigned max_size);
      மெய்நிகர் வெறுமை தள்ளு_front(இரட்டை மதிப்பு, unsigned max_size);

      மெய்நிகர் வெறுமை set_word_max(unsigned adr, மாறிலி std::string&மதிப்பு, unsigned max_size);
      மெய்நிகர் வெறுமை செருகு(unsigned idx, மாறிலி std::string&மதிப்பு, unsigned max_size);
      மெய்நிகர் வெறுமை தள்ளு_back(மாறிலி std::string&மதிப்பு, unsigned max_size);
      மெய்நிகர் வெறுமை தள்ளு_front(மாறிலி std::string&மதிப்பு, unsigned max_size);

      மெய்நிகர் வெறுமை எடு_back(வெறுமை) =0;
      மெய்நிகர் வெறுமை எடு_front(வெறுமை)=0;
      மெய்நிகர் வெறுமை erase(unsigned idx)=0;
      மெய்நிகர் வெறுமை erase_tail(unsigned idx)=0;
};

class vvp_queue_real : பொது vvp_queue {

    பொது:
      size_t get_size(வெறுமை) மாறிலி override { திரும்பு queue.size(); };
      வெறுமை copy_elems(vvp_object_t src, unsigned max_size) override;
      வெறுமை set_word_max(unsigned adr, இரட்டை மதிப்பு, unsigned max_size) override;
      வெறுமை set_word(unsigned adr, இரட்டை மதிப்பு) override;
      வெறுமை get_word(unsigned adr, இரட்டை&மதிப்பு) override;
      வெறுமை செருகு(unsigned idx, இரட்டை மதிப்பு, unsigned max_size) override;
      வெறுமை தள்ளு_back(இரட்டை மதிப்பு, unsigned max_size) override;
      வெறுமை தள்ளு_front(இரட்டை மதிப்பு, unsigned max_size) override;
      வெறுமை எடு_back(வெறுமை) override { queue.எடு_back(); };
      வெறுமை எடு_front(வெறுமை) override { queue.எடு_front(); };
      வெறுமை erase(unsigned idx) override;
      வெறுமை erase_tail(unsigned idx) override;
      வெறுமை reverse_elems(வெறுமை) override;
      வெறுமை sort_elems(bool ascending) override;
      வெறுமை shuffle_elems(வெறுமை) override;

    தனிப்பட்ட:
      std::deque<இரட்டை> queue;
};

class vvp_queue_string : பொது vvp_queue {

    பொது:
      size_t get_size(வெறுமை) மாறிலி override { திரும்பு queue.size(); };
      வெறுமை copy_elems(vvp_object_t src, unsigned max_size) override;
      வெறுமை set_word_max(unsigned adr, மாறிலி std::string&மதிப்பு, unsigned max_size) override;
      வெறுமை set_word(unsigned adr, மாறிலி std::string&மதிப்பு) override;
      வெறுமை get_word(unsigned adr, std::string&மதிப்பு) override;
      வெறுமை செருகு(unsigned idx, மாறிலி std::string&மதிப்பு, unsigned max_size) override;
      வெறுமை தள்ளு_back(மாறிலி std::string&மதிப்பு, unsigned max_size) override;
      வெறுமை தள்ளு_front(மாறிலி std::string&மதிப்பு, unsigned max_size) override;
      வெறுமை எடு_back(வெறுமை) override { queue.எடு_back(); };
      வெறுமை எடு_front(வெறுமை) override { queue.எடு_front(); };
      வெறுமை erase(unsigned idx) override;
      வெறுமை erase_tail(unsigned idx) override;
      வெறுமை reverse_elems(வெறுமை) override;
      வெறுமை sort_elems(bool ascending) override;
      வெறுமை shuffle_elems(வெறுமை) override;

    தனிப்பட்ட:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : பொது vvp_queue {

    பொது:
      size_t get_size(வெறுமை) மாறிலி override { திரும்பு queue.size(); };
      வெறுமை copy_elems(vvp_object_t src, unsigned max_size) override;
      வெறுமை set_word_max(unsigned adr, மாறிலி vvp_vector4_t&மதிப்பு, unsigned max_size) override;
      வெறுமை set_word(unsigned adr, மாறிலி vvp_vector4_t&மதிப்பு) override;
      வெறுமை get_word(unsigned adr, vvp_vector4_t&மதிப்பு) override;
      வெறுமை செருகு(unsigned idx, மாறிலி vvp_vector4_t&மதிப்பு, unsigned max_size) override;
      வெறுமை தள்ளு_back(மாறிலி vvp_vector4_t&மதிப்பு, unsigned max_size) override;
      வெறுமை தள்ளு_front(மாறிலி vvp_vector4_t&மதிப்பு, unsigned max_size) override;
      வெறுமை எடு_back(வெறுமை) override { queue.எடு_back(); };
      வெறுமை எடு_front(வெறுமை) override { queue.எடு_front(); };
      வெறுமை erase(unsigned idx) override;
      வெறுமை erase_tail(unsigned idx) override;
      வெறுமை reverse_elems(வெறுமை) override;
      வெறுமை sort_elems(bool ascending) override;
      வெறுமை shuffle_elems(வெறுமை) override;

    தனிப்பட்ட:
      std::deque<vvp_vector4_t> queue;
};

வெளி std::string get_fileline();

#endif /* IVL_vvp_darray_H */
