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

class vvp_darray : പൊതു vvp_object {

    പൊതു:
      വരിയിൽ vvp_darray() { }
      മായാ ~vvp_darray() override;

      മായാ size_t get_size(ശൂന്യം) സ്ഥിരം =0;

      മായാ ശൂന്യം set_word(unsigned adr, സ്ഥിരം vvp_vector4_t&മൂല്യം);
      മായാ ശൂന്യം get_word(unsigned adr, vvp_vector4_t&മൂല്യം);

      മായാ ശൂന്യം set_word(unsigned adr, ഇരട്ട മൂല്യം);
      മായാ ശൂന്യം get_word(unsigned adr, ഇരട്ട&മൂല്യം);

      മായാ ശൂന്യം set_word(unsigned adr, സ്ഥിരം std::string&മൂല്യം);
      മായാ ശൂന്യം get_word(unsigned adr, std::string&മൂല്യം);

      മായാ ശൂന്യം set_word(unsigned adr, സ്ഥിരം vvp_object_t&മൂല്യം);
      മായാ ശൂന്യം get_word(unsigned adr, vvp_object_t&മൂല്യം);

      മായാ vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      മായാ ശൂന്യം reverse_elems(ശൂന്യം);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      മായാ ശൂന്യം sort_elems(bool ascending);
      മായാ ശൂന്യം shuffle_elems(ശൂന്യം);
};

template <class TYPE> class vvp_darray_atom : പൊതു vvp_darray {

    പൊതു:
      explicit വരിയിൽ vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_size(ശൂന്യം) സ്ഥിരം override;
      ശൂന്യം reverse_elems(ശൂന്യം) override;
      ശൂന്യം sort_elems(bool ascending) override;
      ശൂന്യം shuffle_elems(ശൂന്യം) override;
      ശൂന്യം set_word(unsigned adr, സ്ഥിരം vvp_vector4_t&മൂല്യം) override;
      ശൂന്യം get_word(unsigned adr, vvp_vector4_t&മൂല്യം) override;
      ശൂന്യം shallow_copy(സ്ഥിരം vvp_object*obj) override;
      vvp_object* duplicate(ശൂന്യം) സ്ഥിരം override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    സ്വകാര്യ:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : പൊതു vvp_darray {

    പൊതു:
      വരിയിൽ vvp_darray_vec4(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_size(ശൂന്യം) സ്ഥിരം override;
      ശൂന്യം set_word(unsigned adr, സ്ഥിരം vvp_vector4_t&മൂല്യം) override;
      ശൂന്യം get_word(unsigned adr, vvp_vector4_t&മൂല്യം) override;
      ശൂന്യം shallow_copy(സ്ഥിരം vvp_object*obj) override;
      vvp_object* duplicate(ശൂന്യം) സ്ഥിരം override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ശൂന്യം reverse_elems(ശൂന്യം) override;
      ശൂന്യം sort_elems(bool ascending) override;
      ശൂന്യം shuffle_elems(ശൂന്യം) override;

    സ്വകാര്യ:
      std::vector<vvp_vector4_t> array_;
      unsigned word_wid_;
};

class vvp_darray_vec2 : പൊതു vvp_darray {

    പൊതു:
      വരിയിൽ vvp_darray_vec2(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_size(ശൂന്യം) സ്ഥിരം override;
      ശൂന്യം reverse_elems(ശൂന്യം) override;
      ശൂന്യം sort_elems(bool ascending) override;
      ശൂന്യം shuffle_elems(ശൂന്യം) override;
      ശൂന്യം set_word(unsigned adr, സ്ഥിരം vvp_vector4_t&മൂല്യം) override;
      ശൂന്യം get_word(unsigned adr, vvp_vector4_t&മൂല്യം) override;
      ശൂന്യം shallow_copy(സ്ഥിരം vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    സ്വകാര്യ:
      std::vector<vvp_vector2_t> array_;
      unsigned word_wid_;
};

class vvp_darray_real : പൊതു vvp_darray {

    പൊതു:
      explicit വരിയിൽ vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_size(ശൂന്യം) സ്ഥിരം override;
      ശൂന്യം set_word(unsigned adr, ഇരട്ട മൂല്യം) override;
      ശൂന്യം get_word(unsigned adr, ഇരട്ട&മൂല്യം) override;
      ശൂന്യം shallow_copy(സ്ഥിരം vvp_object*obj) override;
      vvp_object* duplicate(ശൂന്യം) സ്ഥിരം override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ശൂന്യം reverse_elems(ശൂന്യം) override;
      ശൂന്യം sort_elems(bool ascending) override;
      ശൂന്യം shuffle_elems(ശൂന്യം) override;

    സ്വകാര്യ:
      std::vector<ഇരട്ട> array_;
};

class vvp_darray_string : പൊതു vvp_darray {

    പൊതു:
      explicit വരിയിൽ vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_size(ശൂന്യം) സ്ഥിരം override;
      ശൂന്യം set_word(unsigned adr, സ്ഥിരം std::string&മൂല്യം) override;
      ശൂന്യം get_word(unsigned adr, std::string&മൂല്യം) override;
      ശൂന്യം shallow_copy(സ്ഥിരം vvp_object*obj) override;
      vvp_object* duplicate(ശൂന്യം) സ്ഥിരം override;
      ശൂന്യം reverse_elems(ശൂന്യം) override;
      ശൂന്യം sort_elems(bool ascending) override;
      ശൂന്യം shuffle_elems(ശൂന്യം) override;

    സ്വകാര്യ:
      std::vector<std::string> array_;
};

class vvp_darray_object : പൊതു vvp_darray {

    പൊതു:
      explicit വരിയിൽ vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_size(ശൂന്യം) സ്ഥിരം override;
      ശൂന്യം reverse_elems(ശൂന്യം) override;
      ശൂന്യം sort_elems(bool ascending) override;
      ശൂന്യം shuffle_elems(ശൂന്യം) override;
      ശൂന്യം set_word(unsigned adr, സ്ഥിരം vvp_object_t&മൂല്യം) override;
      ശൂന്യം get_word(unsigned adr, vvp_object_t&മൂല്യം) override;
      ശൂന്യം shallow_copy(സ്ഥിരം vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    സ്വകാര്യ:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : പൊതു vvp_darray {

    പൊതു:
      വരിയിൽ vvp_queue(ശൂന്യം) { }
      ~vvp_queue() override;

      മായാ size_t get_size(ശൂന്യം) സ്ഥിരം override =0;
      മായാ ശൂന്യം copy_elems(vvp_object_t src, unsigned max_size);

      മായാ ശൂന്യം set_word_max(unsigned adr, സ്ഥിരം vvp_vector4_t&മൂല്യം, unsigned max_size);
      മായാ ശൂന്യം തിരുകുക(unsigned idx, സ്ഥിരം vvp_vector4_t&മൂല്യം, unsigned max_size);
      മായാ ശൂന്യം തള്ളുക_back(സ്ഥിരം vvp_vector4_t&മൂല്യം, unsigned max_size);
      മായാ ശൂന്യം തള്ളുക_front(സ്ഥിരം vvp_vector4_t&മൂല്യം, unsigned max_size);

      മായാ ശൂന്യം set_word_max(unsigned adr, ഇരട്ട മൂല്യം, unsigned max_size);
      മായാ ശൂന്യം തിരുകുക(unsigned idx, ഇരട്ട മൂല്യം, unsigned max_size);
      മായാ ശൂന്യം തള്ളുക_back(ഇരട്ട മൂല്യം, unsigned max_size);
      മായാ ശൂന്യം തള്ളുക_front(ഇരട്ട മൂല്യം, unsigned max_size);

      മായാ ശൂന്യം set_word_max(unsigned adr, സ്ഥിരം std::string&മൂല്യം, unsigned max_size);
      മായാ ശൂന്യം തിരുകുക(unsigned idx, സ്ഥിരം std::string&മൂല്യം, unsigned max_size);
      മായാ ശൂന്യം തള്ളുക_back(സ്ഥിരം std::string&മൂല്യം, unsigned max_size);
      മായാ ശൂന്യം തള്ളുക_front(സ്ഥിരം std::string&മൂല്യം, unsigned max_size);

      മായാ ശൂന്യം എടുക്കുക_back(ശൂന്യം) =0;
      മായാ ശൂന്യം എടുക്കുക_front(ശൂന്യം)=0;
      മായാ ശൂന്യം erase(unsigned idx)=0;
      മായാ ശൂന്യം erase_tail(unsigned idx)=0;
};

class vvp_queue_real : പൊതു vvp_queue {

    പൊതു:
      size_t get_size(ശൂന്യം) സ്ഥിരം override { തിരികെ queue.size(); };
      ശൂന്യം copy_elems(vvp_object_t src, unsigned max_size) override;
      ശൂന്യം set_word_max(unsigned adr, ഇരട്ട മൂല്യം, unsigned max_size) override;
      ശൂന്യം set_word(unsigned adr, ഇരട്ട മൂല്യം) override;
      ശൂന്യം get_word(unsigned adr, ഇരട്ട&മൂല്യം) override;
      ശൂന്യം തിരുകുക(unsigned idx, ഇരട്ട മൂല്യം, unsigned max_size) override;
      ശൂന്യം തള്ളുക_back(ഇരട്ട മൂല്യം, unsigned max_size) override;
      ശൂന്യം തള്ളുക_front(ഇരട്ട മൂല്യം, unsigned max_size) override;
      ശൂന്യം എടുക്കുക_back(ശൂന്യം) override { queue.എടുക്കുക_back(); };
      ശൂന്യം എടുക്കുക_front(ശൂന്യം) override { queue.എടുക്കുക_front(); };
      ശൂന്യം erase(unsigned idx) override;
      ശൂന്യം erase_tail(unsigned idx) override;
      ശൂന്യം reverse_elems(ശൂന്യം) override;
      ശൂന്യം sort_elems(bool ascending) override;
      ശൂന്യം shuffle_elems(ശൂന്യം) override;

    സ്വകാര്യ:
      std::deque<ഇരട്ട> queue;
};

class vvp_queue_string : പൊതു vvp_queue {

    പൊതു:
      size_t get_size(ശൂന്യം) സ്ഥിരം override { തിരികെ queue.size(); };
      ശൂന്യം copy_elems(vvp_object_t src, unsigned max_size) override;
      ശൂന്യം set_word_max(unsigned adr, സ്ഥിരം std::string&മൂല്യം, unsigned max_size) override;
      ശൂന്യം set_word(unsigned adr, സ്ഥിരം std::string&മൂല്യം) override;
      ശൂന്യം get_word(unsigned adr, std::string&മൂല്യം) override;
      ശൂന്യം തിരുകുക(unsigned idx, സ്ഥിരം std::string&മൂല്യം, unsigned max_size) override;
      ശൂന്യം തള്ളുക_back(സ്ഥിരം std::string&മൂല്യം, unsigned max_size) override;
      ശൂന്യം തള്ളുക_front(സ്ഥിരം std::string&മൂല്യം, unsigned max_size) override;
      ശൂന്യം എടുക്കുക_back(ശൂന്യം) override { queue.എടുക്കുക_back(); };
      ശൂന്യം എടുക്കുക_front(ശൂന്യം) override { queue.എടുക്കുക_front(); };
      ശൂന്യം erase(unsigned idx) override;
      ശൂന്യം erase_tail(unsigned idx) override;
      ശൂന്യം reverse_elems(ശൂന്യം) override;
      ശൂന്യം sort_elems(bool ascending) override;
      ശൂന്യം shuffle_elems(ശൂന്യം) override;

    സ്വകാര്യ:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : പൊതു vvp_queue {

    പൊതു:
      size_t get_size(ശൂന്യം) സ്ഥിരം override { തിരികെ queue.size(); };
      ശൂന്യം copy_elems(vvp_object_t src, unsigned max_size) override;
      ശൂന്യം set_word_max(unsigned adr, സ്ഥിരം vvp_vector4_t&മൂല്യം, unsigned max_size) override;
      ശൂന്യം set_word(unsigned adr, സ്ഥിരം vvp_vector4_t&മൂല്യം) override;
      ശൂന്യം get_word(unsigned adr, vvp_vector4_t&മൂല്യം) override;
      ശൂന്യം തിരുകുക(unsigned idx, സ്ഥിരം vvp_vector4_t&മൂല്യം, unsigned max_size) override;
      ശൂന്യം തള്ളുക_back(സ്ഥിരം vvp_vector4_t&മൂല്യം, unsigned max_size) override;
      ശൂന്യം തള്ളുക_front(സ്ഥിരം vvp_vector4_t&മൂല്യം, unsigned max_size) override;
      ശൂന്യം എടുക്കുക_back(ശൂന്യം) override { queue.എടുക്കുക_back(); };
      ശൂന്യം എടുക്കുക_front(ശൂന്യം) override { queue.എടുക്കുക_front(); };
      ശൂന്യം erase(unsigned idx) override;
      ശൂന്യം erase_tail(unsigned idx) override;
      ശൂന്യം reverse_elems(ശൂന്യം) override;
      ശൂന്യം sort_elems(bool ascending) override;
      ശൂന്യം shuffle_elems(ശൂന്യം) override;

    സ്വകാര്യ:
      std::deque<vvp_vector4_t> queue;
};

ബാഹ്യ std::string get_fileline();

#endif /* IVL_vvp_darray_H */
