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

class vvp_darray : ಸಾರ್ವಜನಿಕ vvp_object {

    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vvp_darray() { }
      ಆಭಾಸ ~vvp_darray() override;

      ಆಭಾಸ size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ =0;

      ಆಭಾಸ ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ);
      ಆಭಾಸ ಶೂನ್ಯ get_word(unsigned adr, vvp_vector4_t&ಮೌಲ್ಯ);

      ಆಭಾಸ ಶೂನ್ಯ set_word(unsigned adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ);
      ಆಭಾಸ ಶೂನ್ಯ get_word(unsigned adr, ದ್ವಿಗುಣ&ಮೌಲ್ಯ);

      ಆಭಾಸ ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ std::string&ಮೌಲ್ಯ);
      ಆಭಾಸ ಶೂನ್ಯ get_word(unsigned adr, std::string&ಮೌಲ್ಯ);

      ಆಭಾಸ ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ vvp_object_t&ಮೌಲ್ಯ);
      ಆಭಾಸ ಶೂನ್ಯ get_word(unsigned adr, vvp_object_t&ಮೌಲ್ಯ);

      ಆಭಾಸ vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      ಆಭಾಸ ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      ಆಭಾಸ ಶೂನ್ಯ sort_elems(bool ascending);
      ಆಭಾಸ ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ);
};

template <class TYPE> class vvp_darray_atom : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      explicit ಅನ್ತಃಸ್ಥ vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ sort_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_word(unsigned adr, vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಧ್ರುವ vvp_object*obj) override;
      vvp_object* duplicate(ಶೂನ್ಯ) ಧ್ರುವ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ಗುಪ್ತ:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vvp_darray_vec4(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_word(unsigned adr, vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಧ್ರುವ vvp_object*obj) override;
      vvp_object* duplicate(ಶೂನ್ಯ) ಧ್ರುವ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ sort_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಗುಪ್ತ:
      std::vector<vvp_vector4_t> array_;
      unsigned word_wid_;
};

class vvp_darray_vec2 : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vvp_darray_vec2(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ sort_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_word(unsigned adr, vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಧ್ರುವ vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ಗುಪ್ತ:
      std::vector<vvp_vector2_t> array_;
      unsigned word_wid_;
};

class vvp_darray_real : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      explicit ಅನ್ತಃಸ್ಥ vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಶೂನ್ಯ set_word(unsigned adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_word(unsigned adr, ದ್ವಿಗುಣ&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಧ್ರುವ vvp_object*obj) override;
      vvp_object* duplicate(ಶೂನ್ಯ) ಧ್ರುವ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ sort_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಗುಪ್ತ:
      std::vector<ದ್ವಿಗುಣ> array_;
};

class vvp_darray_string : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      explicit ಅನ್ತಃಸ್ಥ vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ std::string&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_word(unsigned adr, std::string&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಧ್ರುವ vvp_object*obj) override;
      vvp_object* duplicate(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ sort_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಗುಪ್ತ:
      std::vector<std::string> array_;
};

class vvp_darray_object : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      explicit ಅನ್ತಃಸ್ಥ vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ sort_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ vvp_object_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_word(unsigned adr, vvp_object_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಧ್ರುವ vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    ಗುಪ್ತ:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vvp_queue(ಶೂನ್ಯ) { }
      ~vvp_queue() override;

      ಆಭಾಸ size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override =0;
      ಆಭಾಸ ಶೂನ್ಯ copy_elems(vvp_object_t src, unsigned max_size);

      ಆಭಾಸ ಶೂನ್ಯ set_word_max(unsigned adr, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned max_size);
      ಆಭಾಸ ಶೂನ್ಯ ಸೇರಿಸು(unsigned idx, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned max_size);
      ಆಭಾಸ ಶೂನ್ಯ ತಳ್ಳು_back(ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned max_size);
      ಆಭಾಸ ಶೂನ್ಯ ತಳ್ಳು_front(ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned max_size);

      ಆಭಾಸ ಶೂನ್ಯ set_word_max(unsigned adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ, unsigned max_size);
      ಆಭಾಸ ಶೂನ್ಯ ಸೇರಿಸು(unsigned idx, ದ್ವಿಗುಣ ಮೌಲ್ಯ, unsigned max_size);
      ಆಭಾಸ ಶೂನ್ಯ ತಳ್ಳು_back(ದ್ವಿಗುಣ ಮೌಲ್ಯ, unsigned max_size);
      ಆಭಾಸ ಶೂನ್ಯ ತಳ್ಳು_front(ದ್ವಿಗುಣ ಮೌಲ್ಯ, unsigned max_size);

      ಆಭಾಸ ಶೂನ್ಯ set_word_max(unsigned adr, ಧ್ರುವ std::string&ಮೌಲ್ಯ, unsigned max_size);
      ಆಭಾಸ ಶೂನ್ಯ ಸೇರಿಸು(unsigned idx, ಧ್ರುವ std::string&ಮೌಲ್ಯ, unsigned max_size);
      ಆಭಾಸ ಶೂನ್ಯ ತಳ್ಳು_back(ಧ್ರುವ std::string&ಮೌಲ್ಯ, unsigned max_size);
      ಆಭಾಸ ಶೂನ್ಯ ತಳ್ಳು_front(ಧ್ರುವ std::string&ಮೌಲ್ಯ, unsigned max_size);

      ಆಭಾಸ ಶೂನ್ಯ ತೆಗೆ_back(ಶೂನ್ಯ) =0;
      ಆಭಾಸ ಶೂನ್ಯ ತೆಗೆ_front(ಶೂನ್ಯ)=0;
      ಆಭಾಸ ಶೂನ್ಯ erase(unsigned idx)=0;
      ಆಭಾಸ ಶೂನ್ಯ erase_tail(unsigned idx)=0;
};

class vvp_queue_real : ಸಾರ್ವಜನಿಕ vvp_queue {

    ಸಾರ್ವಜನಿಕ:
      size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ queue.size(); };
      ಶೂನ್ಯ copy_elems(vvp_object_t src, unsigned max_size) override;
      ಶೂನ್ಯ set_word_max(unsigned adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ set_word(unsigned adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_word(unsigned adr, ದ್ವಿಗುಣ&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ ಸೇರಿಸು(unsigned idx, ದ್ವಿಗುಣ ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ ತಳ್ಳು_back(ದ್ವಿಗುಣ ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ ತಳ್ಳು_front(ದ್ವಿಗುಣ ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ ತೆಗೆ_back(ಶೂನ್ಯ) override { queue.ತೆಗೆ_back(); };
      ಶೂನ್ಯ ತೆಗೆ_front(ಶೂನ್ಯ) override { queue.ತೆಗೆ_front(); };
      ಶೂನ್ಯ erase(unsigned idx) override;
      ಶೂನ್ಯ erase_tail(unsigned idx) override;
      ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ sort_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಗುಪ್ತ:
      std::deque<ದ್ವಿಗುಣ> queue;
};

class vvp_queue_string : ಸಾರ್ವಜನಿಕ vvp_queue {

    ಸಾರ್ವಜನಿಕ:
      size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ queue.size(); };
      ಶೂನ್ಯ copy_elems(vvp_object_t src, unsigned max_size) override;
      ಶೂನ್ಯ set_word_max(unsigned adr, ಧ್ರುವ std::string&ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ std::string&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_word(unsigned adr, std::string&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ ಸೇರಿಸು(unsigned idx, ಧ್ರುವ std::string&ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ ತಳ್ಳು_back(ಧ್ರುವ std::string&ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ ತಳ್ಳು_front(ಧ್ರುವ std::string&ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ ತೆಗೆ_back(ಶೂನ್ಯ) override { queue.ತೆಗೆ_back(); };
      ಶೂನ್ಯ ತೆಗೆ_front(ಶೂನ್ಯ) override { queue.ತೆಗೆ_front(); };
      ಶೂನ್ಯ erase(unsigned idx) override;
      ಶೂನ್ಯ erase_tail(unsigned idx) override;
      ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ sort_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಗುಪ್ತ:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : ಸಾರ್ವಜನಿಕ vvp_queue {

    ಸಾರ್ವಜನಿಕ:
      size_t get_size(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ queue.size(); };
      ಶೂನ್ಯ copy_elems(vvp_object_t src, unsigned max_size) override;
      ಶೂನ್ಯ set_word_max(unsigned adr, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ set_word(unsigned adr, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_word(unsigned adr, vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ ಸೇರಿಸು(unsigned idx, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ ತಳ್ಳು_back(ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ ತಳ್ಳು_front(ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned max_size) override;
      ಶೂನ್ಯ ತೆಗೆ_back(ಶೂನ್ಯ) override { queue.ತೆಗೆ_back(); };
      ಶೂನ್ಯ ತೆಗೆ_front(ಶೂನ್ಯ) override { queue.ತೆಗೆ_front(); };
      ಶೂನ್ಯ erase(unsigned idx) override;
      ಶೂನ್ಯ erase_tail(unsigned idx) override;
      ಶೂನ್ಯ reverse_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ sort_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಗುಪ್ತ:
      std::deque<vvp_vector4_t> queue;
};

ಬಾಹ್ಯ std::string get_fileline();

#endif /* IVL_vvp_darray_H */
