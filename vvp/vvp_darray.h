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

class vvp_darray : જાહેર vvp_object {

    જાહેર:
      અંતઃરેખ vvp_darray() { }
      આભાસી ~vvp_darray() override;

      આભાસી size_t get_size(શૂન્ય) અચલ =0;

      આભાસી શૂન્ય set_word(unsigned adr, અચલ vvp_vector4_t&કિંમત);
      આભાસી શૂન્ય get_word(unsigned adr, vvp_vector4_t&કિંમત);

      આભાસી શૂન્ય set_word(unsigned adr, દ્વિગુણ કિંમત);
      આભાસી શૂન્ય get_word(unsigned adr, દ્વિગુણ&કિંમત);

      આભાસી શૂન્ય set_word(unsigned adr, અચલ std::string&કિંમત);
      આભાસી શૂન્ય get_word(unsigned adr, std::string&કિંમત);

      આભાસી શૂન્ય set_word(unsigned adr, અચલ vvp_object_t&કિંમત);
      આભાસી શૂન્ય get_word(unsigned adr, vvp_object_t&કિંમત);

      આભાસી vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      આભાસી શૂન્ય reverse_elems(શૂન્ય);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      આભાસી શૂન્ય sort_elems(bool ascending);
      આભાસી શૂન્ય shuffle_elems(શૂન્ય);
};

template <class TYPE> class vvp_darray_atom : જાહેર vvp_darray {

    જાહેર:
      explicit અંતઃરેખ vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_size(શૂન્ય) અચલ override;
      શૂન્ય reverse_elems(શૂન્ય) override;
      શૂન્ય sort_elems(bool ascending) override;
      શૂન્ય shuffle_elems(શૂન્ય) override;
      શૂન્ય set_word(unsigned adr, અચલ vvp_vector4_t&કિંમત) override;
      શૂન્ય get_word(unsigned adr, vvp_vector4_t&કિંમત) override;
      શૂન્ય shallow_copy(અચલ vvp_object*obj) override;
      vvp_object* duplicate(શૂન્ય) અચલ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ખાનગી:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : જાહેર vvp_darray {

    જાહેર:
      અંતઃરેખ vvp_darray_vec4(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_size(શૂન્ય) અચલ override;
      શૂન્ય set_word(unsigned adr, અચલ vvp_vector4_t&કિંમત) override;
      શૂન્ય get_word(unsigned adr, vvp_vector4_t&કિંમત) override;
      શૂન્ય shallow_copy(અચલ vvp_object*obj) override;
      vvp_object* duplicate(શૂન્ય) અચલ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      શૂન્ય reverse_elems(શૂન્ય) override;
      શૂન્ય sort_elems(bool ascending) override;
      શૂન્ય shuffle_elems(શૂન્ય) override;

    ખાનગી:
      std::vector<vvp_vector4_t> array_;
      unsigned word_wid_;
};

class vvp_darray_vec2 : જાહેર vvp_darray {

    જાહેર:
      અંતઃરેખ vvp_darray_vec2(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_size(શૂન્ય) અચલ override;
      શૂન્ય reverse_elems(શૂન્ય) override;
      શૂન્ય sort_elems(bool ascending) override;
      શૂન્ય shuffle_elems(શૂન્ય) override;
      શૂન્ય set_word(unsigned adr, અચલ vvp_vector4_t&કિંમત) override;
      શૂન્ય get_word(unsigned adr, vvp_vector4_t&કિંમત) override;
      શૂન્ય shallow_copy(અચલ vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ખાનગી:
      std::vector<vvp_vector2_t> array_;
      unsigned word_wid_;
};

class vvp_darray_real : જાહેર vvp_darray {

    જાહેર:
      explicit અંતઃરેખ vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_size(શૂન્ય) અચલ override;
      શૂન્ય set_word(unsigned adr, દ્વિગુણ કિંમત) override;
      શૂન્ય get_word(unsigned adr, દ્વિગુણ&કિંમત) override;
      શૂન્ય shallow_copy(અચલ vvp_object*obj) override;
      vvp_object* duplicate(શૂન્ય) અચલ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      શૂન્ય reverse_elems(શૂન્ય) override;
      શૂન્ય sort_elems(bool ascending) override;
      શૂન્ય shuffle_elems(શૂન્ય) override;

    ખાનગી:
      std::vector<દ્વિગુણ> array_;
};

class vvp_darray_string : જાહેર vvp_darray {

    જાહેર:
      explicit અંતઃરેખ vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_size(શૂન્ય) અચલ override;
      શૂન્ય set_word(unsigned adr, અચલ std::string&કિંમત) override;
      શૂન્ય get_word(unsigned adr, std::string&કિંમત) override;
      શૂન્ય shallow_copy(અચલ vvp_object*obj) override;
      vvp_object* duplicate(શૂન્ય) અચલ override;
      શૂન્ય reverse_elems(શૂન્ય) override;
      શૂન્ય sort_elems(bool ascending) override;
      શૂન્ય shuffle_elems(શૂન્ય) override;

    ખાનગી:
      std::vector<std::string> array_;
};

class vvp_darray_object : જાહેર vvp_darray {

    જાહેર:
      explicit અંતઃરેખ vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_size(શૂન્ય) અચલ override;
      શૂન્ય reverse_elems(શૂન્ય) override;
      શૂન્ય sort_elems(bool ascending) override;
      શૂન્ય shuffle_elems(શૂન્ય) override;
      શૂન્ય set_word(unsigned adr, અચલ vvp_object_t&કિંમત) override;
      શૂન્ય get_word(unsigned adr, vvp_object_t&કિંમત) override;
      શૂન્ય shallow_copy(અચલ vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    ખાનગી:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : જાહેર vvp_darray {

    જાહેર:
      અંતઃરેખ vvp_queue(શૂન્ય) { }
      ~vvp_queue() override;

      આભાસી size_t get_size(શૂન્ય) અચલ override =0;
      આભાસી શૂન્ય copy_elems(vvp_object_t src, unsigned max_size);

      આભાસી શૂન્ય set_word_max(unsigned adr, અચલ vvp_vector4_t&કિંમત, unsigned max_size);
      આભાસી શૂન્ય ઉમેરો(unsigned idx, અચલ vvp_vector4_t&કિંમત, unsigned max_size);
      આભાસી શૂન્ય ધકેલો_back(અચલ vvp_vector4_t&કિંમત, unsigned max_size);
      આભાસી શૂન્ય ધકેલો_front(અચલ vvp_vector4_t&કિંમત, unsigned max_size);

      આભાસી શૂન્ય set_word_max(unsigned adr, દ્વિગુણ કિંમત, unsigned max_size);
      આભાસી શૂન્ય ઉમેરો(unsigned idx, દ્વિગુણ કિંમત, unsigned max_size);
      આભાસી શૂન્ય ધકેલો_back(દ્વિગુણ કિંમત, unsigned max_size);
      આભાસી શૂન્ય ધકેલો_front(દ્વિગુણ કિંમત, unsigned max_size);

      આભાસી શૂન્ય set_word_max(unsigned adr, અચલ std::string&કિંમત, unsigned max_size);
      આભાસી શૂન્ય ઉમેરો(unsigned idx, અચલ std::string&કિંમત, unsigned max_size);
      આભાસી શૂન્ય ધકેલો_back(અચલ std::string&કિંમત, unsigned max_size);
      આભાસી શૂન્ય ધકેલો_front(અચલ std::string&કિંમત, unsigned max_size);

      આભાસી શૂન્ય કાઢો_back(શૂન્ય) =0;
      આભાસી શૂન્ય કાઢો_front(શૂન્ય)=0;
      આભાસી શૂન્ય erase(unsigned idx)=0;
      આભાસી શૂન્ય erase_tail(unsigned idx)=0;
};

class vvp_queue_real : જાહેર vvp_queue {

    જાહેર:
      size_t get_size(શૂન્ય) અચલ override { પાછા queue.size(); };
      શૂન્ય copy_elems(vvp_object_t src, unsigned max_size) override;
      શૂન્ય set_word_max(unsigned adr, દ્વિગુણ કિંમત, unsigned max_size) override;
      શૂન્ય set_word(unsigned adr, દ્વિગુણ કિંમત) override;
      શૂન્ય get_word(unsigned adr, દ્વિગુણ&કિંમત) override;
      શૂન્ય ઉમેરો(unsigned idx, દ્વિગુણ કિંમત, unsigned max_size) override;
      શૂન્ય ધકેલો_back(દ્વિગુણ કિંમત, unsigned max_size) override;
      શૂન્ય ધકેલો_front(દ્વિગુણ કિંમત, unsigned max_size) override;
      શૂન્ય કાઢો_back(શૂન્ય) override { queue.કાઢો_back(); };
      શૂન્ય કાઢો_front(શૂન્ય) override { queue.કાઢો_front(); };
      શૂન્ય erase(unsigned idx) override;
      શૂન્ય erase_tail(unsigned idx) override;
      શૂન્ય reverse_elems(શૂન્ય) override;
      શૂન્ય sort_elems(bool ascending) override;
      શૂન્ય shuffle_elems(શૂન્ય) override;

    ખાનગી:
      std::deque<દ્વિગુણ> queue;
};

class vvp_queue_string : જાહેર vvp_queue {

    જાહેર:
      size_t get_size(શૂન્ય) અચલ override { પાછા queue.size(); };
      શૂન્ય copy_elems(vvp_object_t src, unsigned max_size) override;
      શૂન્ય set_word_max(unsigned adr, અચલ std::string&કિંમત, unsigned max_size) override;
      શૂન્ય set_word(unsigned adr, અચલ std::string&કિંમત) override;
      શૂન્ય get_word(unsigned adr, std::string&કિંમત) override;
      શૂન્ય ઉમેરો(unsigned idx, અચલ std::string&કિંમત, unsigned max_size) override;
      શૂન્ય ધકેલો_back(અચલ std::string&કિંમત, unsigned max_size) override;
      શૂન્ય ધકેલો_front(અચલ std::string&કિંમત, unsigned max_size) override;
      શૂન્ય કાઢો_back(શૂન્ય) override { queue.કાઢો_back(); };
      શૂન્ય કાઢો_front(શૂન્ય) override { queue.કાઢો_front(); };
      શૂન્ય erase(unsigned idx) override;
      શૂન્ય erase_tail(unsigned idx) override;
      શૂન્ય reverse_elems(શૂન્ય) override;
      શૂન્ય sort_elems(bool ascending) override;
      શૂન્ય shuffle_elems(શૂન્ય) override;

    ખાનગી:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : જાહેર vvp_queue {

    જાહેર:
      size_t get_size(શૂન્ય) અચલ override { પાછા queue.size(); };
      શૂન્ય copy_elems(vvp_object_t src, unsigned max_size) override;
      શૂન્ય set_word_max(unsigned adr, અચલ vvp_vector4_t&કિંમત, unsigned max_size) override;
      શૂન્ય set_word(unsigned adr, અચલ vvp_vector4_t&કિંમત) override;
      શૂન્ય get_word(unsigned adr, vvp_vector4_t&કિંમત) override;
      શૂન્ય ઉમેરો(unsigned idx, અચલ vvp_vector4_t&કિંમત, unsigned max_size) override;
      શૂન્ય ધકેલો_back(અચલ vvp_vector4_t&કિંમત, unsigned max_size) override;
      શૂન્ય ધકેલો_front(અચલ vvp_vector4_t&કિંમત, unsigned max_size) override;
      શૂન્ય કાઢો_back(શૂન્ય) override { queue.કાઢો_back(); };
      શૂન્ય કાઢો_front(શૂન્ય) override { queue.કાઢો_front(); };
      શૂન્ય erase(unsigned idx) override;
      શૂન્ય erase_tail(unsigned idx) override;
      શૂન્ય reverse_elems(શૂન્ય) override;
      શૂન્ય sort_elems(bool ascending) override;
      શૂન્ય shuffle_elems(શૂન્ય) override;

    ખાનગી:
      std::deque<vvp_vector4_t> queue;
};

બાહ્ય std::string get_fileline();

#endif /* IVL_vvp_darray_H */
