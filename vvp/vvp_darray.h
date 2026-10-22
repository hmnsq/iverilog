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

class vvp_darray : عمومی vvp_object {

    عمومی:
      درونخطی vvp_darray() { }
      مجازی ~vvp_darray() override;

      مجازی size_t get_size(تهی) ثابت =0;

      مجازی تهی set_word(unsigned adr, ثابت vvp_vector4_t&مقدار);
      مجازی تهی get_word(unsigned adr, vvp_vector4_t&مقدار);

      مجازی تهی set_word(unsigned adr, دوگانه مقدار);
      مجازی تهی get_word(unsigned adr, دوگانه&مقدار);

      مجازی تهی set_word(unsigned adr, ثابت std::string&مقدار);
      مجازی تهی get_word(unsigned adr, std::string&مقدار);

      مجازی تهی set_word(unsigned adr, ثابت vvp_object_t&مقدار);
      مجازی تهی get_word(unsigned adr, vvp_object_t&مقدار);

      مجازی vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      مجازی تهی reverse_elems(تهی);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      مجازی تهی sort_elems(bool ascending);
      مجازی تهی shuffle_elems(تهی);
};

template <class TYPE> class vvp_darray_atom : عمومی vvp_darray {

    عمومی:
      explicit درونخطی vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_size(تهی) ثابت override;
      تهی reverse_elems(تهی) override;
      تهی sort_elems(bool ascending) override;
      تهی shuffle_elems(تهی) override;
      تهی set_word(unsigned adr, ثابت vvp_vector4_t&مقدار) override;
      تهی get_word(unsigned adr, vvp_vector4_t&مقدار) override;
      تهی shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(تهی) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خصوصی:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : عمومی vvp_darray {

    عمومی:
      درونخطی vvp_darray_vec4(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_size(تهی) ثابت override;
      تهی set_word(unsigned adr, ثابت vvp_vector4_t&مقدار) override;
      تهی get_word(unsigned adr, vvp_vector4_t&مقدار) override;
      تهی shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(تهی) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      تهی reverse_elems(تهی) override;
      تهی sort_elems(bool ascending) override;
      تهی shuffle_elems(تهی) override;

    خصوصی:
      std::vector<vvp_vector4_t> array_;
      unsigned word_wid_;
};

class vvp_darray_vec2 : عمومی vvp_darray {

    عمومی:
      درونخطی vvp_darray_vec2(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_size(تهی) ثابت override;
      تهی reverse_elems(تهی) override;
      تهی sort_elems(bool ascending) override;
      تهی shuffle_elems(تهی) override;
      تهی set_word(unsigned adr, ثابت vvp_vector4_t&مقدار) override;
      تهی get_word(unsigned adr, vvp_vector4_t&مقدار) override;
      تهی shallow_copy(ثابت vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خصوصی:
      std::vector<vvp_vector2_t> array_;
      unsigned word_wid_;
};

class vvp_darray_real : عمومی vvp_darray {

    عمومی:
      explicit درونخطی vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_size(تهی) ثابت override;
      تهی set_word(unsigned adr, دوگانه مقدار) override;
      تهی get_word(unsigned adr, دوگانه&مقدار) override;
      تهی shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(تهی) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      تهی reverse_elems(تهی) override;
      تهی sort_elems(bool ascending) override;
      تهی shuffle_elems(تهی) override;

    خصوصی:
      std::vector<دوگانه> array_;
};

class vvp_darray_string : عمومی vvp_darray {

    عمومی:
      explicit درونخطی vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_size(تهی) ثابت override;
      تهی set_word(unsigned adr, ثابت std::string&مقدار) override;
      تهی get_word(unsigned adr, std::string&مقدار) override;
      تهی shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(تهی) ثابت override;
      تهی reverse_elems(تهی) override;
      تهی sort_elems(bool ascending) override;
      تهی shuffle_elems(تهی) override;

    خصوصی:
      std::vector<std::string> array_;
};

class vvp_darray_object : عمومی vvp_darray {

    عمومی:
      explicit درونخطی vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_size(تهی) ثابت override;
      تهی reverse_elems(تهی) override;
      تهی sort_elems(bool ascending) override;
      تهی shuffle_elems(تهی) override;
      تهی set_word(unsigned adr, ثابت vvp_object_t&مقدار) override;
      تهی get_word(unsigned adr, vvp_object_t&مقدار) override;
      تهی shallow_copy(ثابت vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    خصوصی:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : عمومی vvp_darray {

    عمومی:
      درونخطی vvp_queue(تهی) { }
      ~vvp_queue() override;

      مجازی size_t get_size(تهی) ثابت override =0;
      مجازی تهی copy_elems(vvp_object_t src, unsigned max_size);

      مجازی تهی set_word_max(unsigned adr, ثابت vvp_vector4_t&مقدار, unsigned max_size);
      مجازی تهی درج(unsigned idx, ثابت vvp_vector4_t&مقدار, unsigned max_size);
      مجازی تهی افزودن_back(ثابت vvp_vector4_t&مقدار, unsigned max_size);
      مجازی تهی افزودن_front(ثابت vvp_vector4_t&مقدار, unsigned max_size);

      مجازی تهی set_word_max(unsigned adr, دوگانه مقدار, unsigned max_size);
      مجازی تهی درج(unsigned idx, دوگانه مقدار, unsigned max_size);
      مجازی تهی افزودن_back(دوگانه مقدار, unsigned max_size);
      مجازی تهی افزودن_front(دوگانه مقدار, unsigned max_size);

      مجازی تهی set_word_max(unsigned adr, ثابت std::string&مقدار, unsigned max_size);
      مجازی تهی درج(unsigned idx, ثابت std::string&مقدار, unsigned max_size);
      مجازی تهی افزودن_back(ثابت std::string&مقدار, unsigned max_size);
      مجازی تهی افزودن_front(ثابت std::string&مقدار, unsigned max_size);

      مجازی تهی برداشت_back(تهی) =0;
      مجازی تهی برداشت_front(تهی)=0;
      مجازی تهی erase(unsigned idx)=0;
      مجازی تهی erase_tail(unsigned idx)=0;
};

class vvp_queue_real : عمومی vvp_queue {

    عمومی:
      size_t get_size(تهی) ثابت override { بازگشت queue.size(); };
      تهی copy_elems(vvp_object_t src, unsigned max_size) override;
      تهی set_word_max(unsigned adr, دوگانه مقدار, unsigned max_size) override;
      تهی set_word(unsigned adr, دوگانه مقدار) override;
      تهی get_word(unsigned adr, دوگانه&مقدار) override;
      تهی درج(unsigned idx, دوگانه مقدار, unsigned max_size) override;
      تهی افزودن_back(دوگانه مقدار, unsigned max_size) override;
      تهی افزودن_front(دوگانه مقدار, unsigned max_size) override;
      تهی برداشت_back(تهی) override { queue.برداشت_back(); };
      تهی برداشت_front(تهی) override { queue.برداشت_front(); };
      تهی erase(unsigned idx) override;
      تهی erase_tail(unsigned idx) override;
      تهی reverse_elems(تهی) override;
      تهی sort_elems(bool ascending) override;
      تهی shuffle_elems(تهی) override;

    خصوصی:
      std::deque<دوگانه> queue;
};

class vvp_queue_string : عمومی vvp_queue {

    عمومی:
      size_t get_size(تهی) ثابت override { بازگشت queue.size(); };
      تهی copy_elems(vvp_object_t src, unsigned max_size) override;
      تهی set_word_max(unsigned adr, ثابت std::string&مقدار, unsigned max_size) override;
      تهی set_word(unsigned adr, ثابت std::string&مقدار) override;
      تهی get_word(unsigned adr, std::string&مقدار) override;
      تهی درج(unsigned idx, ثابت std::string&مقدار, unsigned max_size) override;
      تهی افزودن_back(ثابت std::string&مقدار, unsigned max_size) override;
      تهی افزودن_front(ثابت std::string&مقدار, unsigned max_size) override;
      تهی برداشت_back(تهی) override { queue.برداشت_back(); };
      تهی برداشت_front(تهی) override { queue.برداشت_front(); };
      تهی erase(unsigned idx) override;
      تهی erase_tail(unsigned idx) override;
      تهی reverse_elems(تهی) override;
      تهی sort_elems(bool ascending) override;
      تهی shuffle_elems(تهی) override;

    خصوصی:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : عمومی vvp_queue {

    عمومی:
      size_t get_size(تهی) ثابت override { بازگشت queue.size(); };
      تهی copy_elems(vvp_object_t src, unsigned max_size) override;
      تهی set_word_max(unsigned adr, ثابت vvp_vector4_t&مقدار, unsigned max_size) override;
      تهی set_word(unsigned adr, ثابت vvp_vector4_t&مقدار) override;
      تهی get_word(unsigned adr, vvp_vector4_t&مقدار) override;
      تهی درج(unsigned idx, ثابت vvp_vector4_t&مقدار, unsigned max_size) override;
      تهی افزودن_back(ثابت vvp_vector4_t&مقدار, unsigned max_size) override;
      تهی افزودن_front(ثابت vvp_vector4_t&مقدار, unsigned max_size) override;
      تهی برداشت_back(تهی) override { queue.برداشت_back(); };
      تهی برداشت_front(تهی) override { queue.برداشت_front(); };
      تهی erase(unsigned idx) override;
      تهی erase_tail(unsigned idx) override;
      تهی reverse_elems(تهی) override;
      تهی sort_elems(bool ascending) override;
      تهی shuffle_elems(تهی) override;

    خصوصی:
      std::deque<vvp_vector4_t> queue;
};

بیرونی std::string get_fileline();

#endif /* IVL_vvp_darray_H */
