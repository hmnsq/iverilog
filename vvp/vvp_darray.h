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

class vvp_darray : عام vvp_object {

    عام:
      مضمن vvp_darray() { }
      virtual ~vvp_darray() override;

      virtual size_t get_size(فراغ) ثابت =0;

      virtual فراغ set_word(unsigned adr, ثابت vvp_vector4_t&value);
      virtual فراغ get_word(unsigned adr, vvp_vector4_t&value);

      virtual فراغ set_word(unsigned adr, مزدوج value);
      virtual فراغ get_word(unsigned adr, مزدوج&value);

      virtual فراغ set_word(unsigned adr, ثابت std::string&value);
      virtual فراغ get_word(unsigned adr, std::string&value);

      virtual فراغ set_word(unsigned adr, ثابت vvp_object_t&value);
      virtual فراغ get_word(unsigned adr, vvp_object_t&value);

      virtual vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      virtual فراغ reverse_elems(فراغ);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      virtual فراغ sort_elems(bool ascending);
      virtual فراغ shuffle_elems(فراغ);
};

template <class TYPE> class vvp_darray_atom : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_size(فراغ) ثابت override;
      فراغ reverse_elems(فراغ) override;
      فراغ sort_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ set_word(unsigned adr, ثابت vvp_vector4_t&value) override;
      فراغ get_word(unsigned adr, vvp_vector4_t&value) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec4(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_size(فراغ) ثابت override;
      فراغ set_word(unsigned adr, ثابت vvp_vector4_t&value) override;
      فراغ get_word(unsigned adr, vvp_vector4_t&value) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ reverse_elems(فراغ) override;
      فراغ sort_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::vector<vvp_vector4_t> array_;
      unsigned word_wid_;
};

class vvp_darray_vec2 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec2(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_size(فراغ) ثابت override;
      فراغ reverse_elems(فراغ) override;
      فراغ sort_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ set_word(unsigned adr, ثابت vvp_vector4_t&value) override;
      فراغ get_word(unsigned adr, vvp_vector4_t&value) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::vector<vvp_vector2_t> array_;
      unsigned word_wid_;
};

class vvp_darray_real : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_size(فراغ) ثابت override;
      فراغ set_word(unsigned adr, مزدوج value) override;
      فراغ get_word(unsigned adr, مزدوج&value) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ reverse_elems(فراغ) override;
      فراغ sort_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::vector<مزدوج> array_;
};

class vvp_darray_string : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_size(فراغ) ثابت override;
      فراغ set_word(unsigned adr, ثابت std::string&value) override;
      فراغ get_word(unsigned adr, std::string&value) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      فراغ reverse_elems(فراغ) override;
      فراغ sort_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::vector<std::string> array_;
};

class vvp_darray_object : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_size(فراغ) ثابت override;
      فراغ reverse_elems(فراغ) override;
      فراغ sort_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ set_word(unsigned adr, ثابت vvp_object_t&value) override;
      فراغ get_word(unsigned adr, vvp_object_t&value) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    خاص:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : عام vvp_darray {

    عام:
      مضمن vvp_queue(فراغ) { }
      ~vvp_queue() override;

      virtual size_t get_size(فراغ) ثابت override =0;
      virtual فراغ copy_elems(vvp_object_t src, unsigned max_size);

      virtual فراغ set_word_max(unsigned adr, ثابت vvp_vector4_t&value, unsigned max_size);
      virtual فراغ insert(unsigned idx, ثابت vvp_vector4_t&value, unsigned max_size);
      virtual فراغ push_back(ثابت vvp_vector4_t&value, unsigned max_size);
      virtual فراغ push_front(ثابت vvp_vector4_t&value, unsigned max_size);

      virtual فراغ set_word_max(unsigned adr, مزدوج value, unsigned max_size);
      virtual فراغ insert(unsigned idx, مزدوج value, unsigned max_size);
      virtual فراغ push_back(مزدوج value, unsigned max_size);
      virtual فراغ push_front(مزدوج value, unsigned max_size);

      virtual فراغ set_word_max(unsigned adr, ثابت std::string&value, unsigned max_size);
      virtual فراغ insert(unsigned idx, ثابت std::string&value, unsigned max_size);
      virtual فراغ push_back(ثابت std::string&value, unsigned max_size);
      virtual فراغ push_front(ثابت std::string&value, unsigned max_size);

      virtual فراغ pop_back(فراغ) =0;
      virtual فراغ pop_front(فراغ)=0;
      virtual فراغ erase(unsigned idx)=0;
      virtual فراغ erase_tail(unsigned idx)=0;
};

class vvp_queue_real : عام vvp_queue {

    عام:
      size_t get_size(فراغ) ثابت override { החזר queue.size(); };
      فراغ copy_elems(vvp_object_t src, unsigned max_size) override;
      فراغ set_word_max(unsigned adr, مزدوج value, unsigned max_size) override;
      فراغ set_word(unsigned adr, مزدوج value) override;
      فراغ get_word(unsigned adr, مزدوج&value) override;
      فراغ insert(unsigned idx, مزدوج value, unsigned max_size) override;
      فراغ push_back(مزدوج value, unsigned max_size) override;
      فراغ push_front(مزدوج value, unsigned max_size) override;
      فراغ pop_back(فراغ) override { queue.pop_back(); };
      فراغ pop_front(فراغ) override { queue.pop_front(); };
      فراغ erase(unsigned idx) override;
      فراغ erase_tail(unsigned idx) override;
      فراغ reverse_elems(فراغ) override;
      فراغ sort_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<مزدوج> queue;
};

class vvp_queue_string : عام vvp_queue {

    عام:
      size_t get_size(فراغ) ثابت override { החזר queue.size(); };
      فراغ copy_elems(vvp_object_t src, unsigned max_size) override;
      فراغ set_word_max(unsigned adr, ثابت std::string&value, unsigned max_size) override;
      فراغ set_word(unsigned adr, ثابت std::string&value) override;
      فراغ get_word(unsigned adr, std::string&value) override;
      فراغ insert(unsigned idx, ثابت std::string&value, unsigned max_size) override;
      فراغ push_back(ثابت std::string&value, unsigned max_size) override;
      فراغ push_front(ثابت std::string&value, unsigned max_size) override;
      فراغ pop_back(فراغ) override { queue.pop_back(); };
      فراغ pop_front(فراغ) override { queue.pop_front(); };
      فراغ erase(unsigned idx) override;
      فراغ erase_tail(unsigned idx) override;
      فراغ reverse_elems(فراغ) override;
      فراغ sort_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : عام vvp_queue {

    عام:
      size_t get_size(فراغ) ثابت override { החזר queue.size(); };
      فراغ copy_elems(vvp_object_t src, unsigned max_size) override;
      فراغ set_word_max(unsigned adr, ثابت vvp_vector4_t&value, unsigned max_size) override;
      فراغ set_word(unsigned adr, ثابت vvp_vector4_t&value) override;
      فراغ get_word(unsigned adr, vvp_vector4_t&value) override;
      فراغ insert(unsigned idx, ثابت vvp_vector4_t&value, unsigned max_size) override;
      فراغ push_back(ثابت vvp_vector4_t&value, unsigned max_size) override;
      فراغ push_front(ثابت vvp_vector4_t&value, unsigned max_size) override;
      فراغ pop_back(فراغ) override { queue.pop_back(); };
      فراغ pop_front(فراغ) override { queue.pop_front(); };
      فراغ erase(unsigned idx) override;
      فراغ erase_tail(unsigned idx) override;
      فراغ reverse_elems(فراغ) override;
      فراغ sort_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<vvp_vector4_t> queue;
};

خارجي std::string get_fileline();

#endif /* IVL_vvp_darray_H */
