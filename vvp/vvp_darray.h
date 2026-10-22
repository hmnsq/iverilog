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

class vvp_darray : সার্ৱজনিক vvp_object {

    সার্ৱজনিক:
      অন্তঃস্থ vvp_darray() { }
      আভাস ~vvp_darray() override;

      আভাস size_t get_size(শূন্য) ধ্রুৱ =0;

      আভাস শূন্য set_word(unsigned adr, ধ্রুৱ vvp_vector4_t&মান);
      আভাস শূন্য get_word(unsigned adr, vvp_vector4_t&মান);

      আভাস শূন্য set_word(unsigned adr, দ্ৱিগুণ মান);
      আভাস শূন্য get_word(unsigned adr, দ্ৱিগুণ&মান);

      আভাস শূন্য set_word(unsigned adr, ধ্রুৱ std::string&মান);
      আভাস শূন্য get_word(unsigned adr, std::string&মান);

      আভাস শূন্য set_word(unsigned adr, ধ্রুৱ vvp_object_t&মান);
      আভাস শূন্য get_word(unsigned adr, vvp_object_t&মান);

      আভাস vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      আভাস শূন্য reverse_elems(শূন্য);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      আভাস শূন্য sort_elems(bool ascending);
      আভাস শূন্য shuffle_elems(শূন্য);
};

template <class TYPE> class vvp_darray_atom : সার্ৱজনিক vvp_darray {

    সার্ৱজনিক:
      explicit অন্তঃস্থ vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_size(শূন্য) ধ্রুৱ override;
      শূন্য reverse_elems(শূন্য) override;
      শূন্য sort_elems(bool ascending) override;
      শূন্য shuffle_elems(শূন্য) override;
      শূন্য set_word(unsigned adr, ধ্রুৱ vvp_vector4_t&মান) override;
      শূন্য get_word(unsigned adr, vvp_vector4_t&মান) override;
      শূন্য shallow_copy(ধ্রুৱ vvp_object*obj) override;
      vvp_object* duplicate(শূন্য) ধ্রুৱ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    গুপ্ত:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : সার্ৱজনিক vvp_darray {

    সার্ৱজনিক:
      অন্তঃস্থ vvp_darray_vec4(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_size(শূন্য) ধ্রুৱ override;
      শূন্য set_word(unsigned adr, ধ্রুৱ vvp_vector4_t&মান) override;
      শূন্য get_word(unsigned adr, vvp_vector4_t&মান) override;
      শূন্য shallow_copy(ধ্রুৱ vvp_object*obj) override;
      vvp_object* duplicate(শূন্য) ধ্রুৱ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      শূন্য reverse_elems(শূন্য) override;
      শূন্য sort_elems(bool ascending) override;
      শূন্য shuffle_elems(শূন্য) override;

    গুপ্ত:
      std::vector<vvp_vector4_t> array_;
      unsigned word_wid_;
};

class vvp_darray_vec2 : সার্ৱজনিক vvp_darray {

    সার্ৱজনিক:
      অন্তঃস্থ vvp_darray_vec2(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_size(শূন্য) ধ্রুৱ override;
      শূন্য reverse_elems(শূন্য) override;
      শূন্য sort_elems(bool ascending) override;
      শূন্য shuffle_elems(শূন্য) override;
      শূন্য set_word(unsigned adr, ধ্রুৱ vvp_vector4_t&মান) override;
      শূন্য get_word(unsigned adr, vvp_vector4_t&মান) override;
      শূন্য shallow_copy(ধ্রুৱ vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    গুপ্ত:
      std::vector<vvp_vector2_t> array_;
      unsigned word_wid_;
};

class vvp_darray_real : সার্ৱজনিক vvp_darray {

    সার্ৱজনিক:
      explicit অন্তঃস্থ vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_size(শূন্য) ধ্রুৱ override;
      শূন্য set_word(unsigned adr, দ্ৱিগুণ মান) override;
      শূন্য get_word(unsigned adr, দ্ৱিগুণ&মান) override;
      শূন্য shallow_copy(ধ্রুৱ vvp_object*obj) override;
      vvp_object* duplicate(শূন্য) ধ্রুৱ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      শূন্য reverse_elems(শূন্য) override;
      শূন্য sort_elems(bool ascending) override;
      শূন্য shuffle_elems(শূন্য) override;

    গুপ্ত:
      std::vector<দ্ৱিগুণ> array_;
};

class vvp_darray_string : সার্ৱজনিক vvp_darray {

    সার্ৱজনিক:
      explicit অন্তঃস্থ vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_size(শূন্য) ধ্রুৱ override;
      শূন্য set_word(unsigned adr, ধ্রুৱ std::string&মান) override;
      শূন্য get_word(unsigned adr, std::string&মান) override;
      শূন্য shallow_copy(ধ্রুৱ vvp_object*obj) override;
      vvp_object* duplicate(শূন্য) ধ্রুৱ override;
      শূন্য reverse_elems(শূন্য) override;
      শূন্য sort_elems(bool ascending) override;
      শূন্য shuffle_elems(শূন্য) override;

    গুপ্ত:
      std::vector<std::string> array_;
};

class vvp_darray_object : সার্ৱজনিক vvp_darray {

    সার্ৱজনিক:
      explicit অন্তঃস্থ vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_size(শূন্য) ধ্রুৱ override;
      শূন্য reverse_elems(শূন্য) override;
      শূন্য sort_elems(bool ascending) override;
      শূন্য shuffle_elems(শূন্য) override;
      শূন্য set_word(unsigned adr, ধ্রুৱ vvp_object_t&মান) override;
      শূন্য get_word(unsigned adr, vvp_object_t&মান) override;
      শূন্য shallow_copy(ধ্রুৱ vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    গুপ্ত:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : সার্ৱজনিক vvp_darray {

    সার্ৱজনিক:
      অন্তঃস্থ vvp_queue(শূন্য) { }
      ~vvp_queue() override;

      আভাস size_t get_size(শূন্য) ধ্রুৱ override =0;
      আভাস শূন্য copy_elems(vvp_object_t src, unsigned max_size);

      আভাস শূন্য set_word_max(unsigned adr, ধ্রুৱ vvp_vector4_t&মান, unsigned max_size);
      আভাস শূন্য ঢোকাও(unsigned idx, ধ্রুৱ vvp_vector4_t&মান, unsigned max_size);
      আভাস শূন্য ঠেলো_back(ধ্রুৱ vvp_vector4_t&মান, unsigned max_size);
      আভাস শূন্য ঠেলো_front(ধ্রুৱ vvp_vector4_t&মান, unsigned max_size);

      আভাস শূন্য set_word_max(unsigned adr, দ্ৱিগুণ মান, unsigned max_size);
      আভাস শূন্য ঢোকাও(unsigned idx, দ্ৱিগুণ মান, unsigned max_size);
      আভাস শূন্য ঠেলো_back(দ্ৱিগুণ মান, unsigned max_size);
      আভাস শূন্য ঠেলো_front(দ্ৱিগুণ মান, unsigned max_size);

      আভাস শূন্য set_word_max(unsigned adr, ধ্রুৱ std::string&মান, unsigned max_size);
      আভাস শূন্য ঢোকাও(unsigned idx, ধ্রুৱ std::string&মান, unsigned max_size);
      আভাস শূন্য ঠেলো_back(ধ্রুৱ std::string&মান, unsigned max_size);
      আভাস শূন্য ঠেলো_front(ধ্রুৱ std::string&মান, unsigned max_size);

      আভাস শূন্য বেৰ_কৰো_back(শূন্য) =0;
      আভাস শূন্য বেৰ_কৰো_front(শূন্য)=0;
      আভাস শূন্য erase(unsigned idx)=0;
      আভাস শূন্য erase_tail(unsigned idx)=0;
};

class vvp_queue_real : সার্ৱজনিক vvp_queue {

    সার্ৱজনিক:
      size_t get_size(শূন্য) ধ্রুৱ override { ঘূৰাই queue.size(); };
      শূন্য copy_elems(vvp_object_t src, unsigned max_size) override;
      শূন্য set_word_max(unsigned adr, দ্ৱিগুণ মান, unsigned max_size) override;
      শূন্য set_word(unsigned adr, দ্ৱিগুণ মান) override;
      শূন্য get_word(unsigned adr, দ্ৱিগুণ&মান) override;
      শূন্য ঢোকাও(unsigned idx, দ্ৱিগুণ মান, unsigned max_size) override;
      শূন্য ঠেলো_back(দ্ৱিগুণ মান, unsigned max_size) override;
      শূন্য ঠেলো_front(দ্ৱিগুণ মান, unsigned max_size) override;
      শূন্য বেৰ_কৰো_back(শূন্য) override { queue.বেৰ_কৰো_back(); };
      শূন্য বেৰ_কৰো_front(শূন্য) override { queue.বেৰ_কৰো_front(); };
      শূন্য erase(unsigned idx) override;
      শূন্য erase_tail(unsigned idx) override;
      শূন্য reverse_elems(শূন্য) override;
      শূন্য sort_elems(bool ascending) override;
      শূন্য shuffle_elems(শূন্য) override;

    গুপ্ত:
      std::deque<দ্ৱিগুণ> queue;
};

class vvp_queue_string : সার্ৱজনিক vvp_queue {

    সার্ৱজনিক:
      size_t get_size(শূন্য) ধ্রুৱ override { ঘূৰাই queue.size(); };
      শূন্য copy_elems(vvp_object_t src, unsigned max_size) override;
      শূন্য set_word_max(unsigned adr, ধ্রুৱ std::string&মান, unsigned max_size) override;
      শূন্য set_word(unsigned adr, ধ্রুৱ std::string&মান) override;
      শূন্য get_word(unsigned adr, std::string&মান) override;
      শূন্য ঢোকাও(unsigned idx, ধ্রুৱ std::string&মান, unsigned max_size) override;
      শূন্য ঠেলো_back(ধ্রুৱ std::string&মান, unsigned max_size) override;
      শূন্য ঠেলো_front(ধ্রুৱ std::string&মান, unsigned max_size) override;
      শূন্য বেৰ_কৰো_back(শূন্য) override { queue.বেৰ_কৰো_back(); };
      শূন্য বেৰ_কৰো_front(শূন্য) override { queue.বেৰ_কৰো_front(); };
      শূন্য erase(unsigned idx) override;
      শূন্য erase_tail(unsigned idx) override;
      শূন্য reverse_elems(শূন্য) override;
      শূন্য sort_elems(bool ascending) override;
      শূন্য shuffle_elems(শূন্য) override;

    গুপ্ত:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : সার্ৱজনিক vvp_queue {

    সার্ৱজনিক:
      size_t get_size(শূন্য) ধ্রুৱ override { ঘূৰাই queue.size(); };
      শূন্য copy_elems(vvp_object_t src, unsigned max_size) override;
      শূন্য set_word_max(unsigned adr, ধ্রুৱ vvp_vector4_t&মান, unsigned max_size) override;
      শূন্য set_word(unsigned adr, ধ্রুৱ vvp_vector4_t&মান) override;
      শূন্য get_word(unsigned adr, vvp_vector4_t&মান) override;
      শূন্য ঢোকাও(unsigned idx, ধ্রুৱ vvp_vector4_t&মান, unsigned max_size) override;
      শূন্য ঠেলো_back(ধ্রুৱ vvp_vector4_t&মান, unsigned max_size) override;
      শূন্য ঠেলো_front(ধ্রুৱ vvp_vector4_t&মান, unsigned max_size) override;
      শূন্য বেৰ_কৰো_back(শূন্য) override { queue.বেৰ_কৰো_back(); };
      শূন্য বেৰ_কৰো_front(শূন্য) override { queue.বেৰ_কৰো_front(); };
      শূন্য erase(unsigned idx) override;
      শূন্য erase_tail(unsigned idx) override;
      শূন্য reverse_elems(শূন্য) override;
      শূন্য sort_elems(bool ascending) override;
      শূন্য shuffle_elems(শূন্য) override;

    গুপ্ত:
      std::deque<vvp_vector4_t> queue;
};

বাহ্য std::string get_fileline();

#endif /* IVL_vvp_darray_H */
