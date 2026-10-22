#अगर_अघोषित IVL_vvp_darray_H
#घोषणा IVL_vvp_darray_H
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

class vvp_darray : खुला vvp_object {

    खुला:
      अंतरभूत vvp_darray() { }
      भव ~vvp_darray() override;

      भव माप_प्रकार get_आकार(व्योम) स्थिर =0;

      भव व्योम set_word(अचिन्हित adr, स्थिर vvp_vector4_t&मूल्य);
      भव व्योम get_word(अचिन्हित adr, vvp_vector4_t&मूल्य);

      भव व्योम set_word(अचिन्हित adr, द्विगुन मूल्य);
      भव व्योम get_word(अचिन्हित adr, द्विगुन&मूल्य);

      भव व्योम set_word(अचिन्हित adr, स्थिर std::string&मूल्य);
      भव व्योम get_word(अचिन्हित adr, std::string&मूल्य);

      भव व्योम set_word(अचिन्हित adr, स्थिर vvp_object_t&मूल्य);
      भव व्योम get_word(अचिन्हित adr, vvp_object_t&मूल्य);

      भव vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      भव व्योम reverse_elems(व्योम);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      भव व्योम sort_elems(bool ascending);
      भव व्योम shuffle_elems(व्योम);
};

ढाँचा <class TYPE> class vvp_darray_atom : खुला vvp_darray {

    खुला:
      explicit अंतरभूत vvp_darray_atom(माप_प्रकार siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      माप_प्रकार get_आकार(व्योम) स्थिर override;
      व्योम reverse_elems(व्योम) override;
      व्योम sort_elems(bool ascending) override;
      व्योम shuffle_elems(व्योम) override;
      व्योम set_word(अचिन्हित adr, स्थिर vvp_vector4_t&मूल्य) override;
      व्योम get_word(अचिन्हित adr, vvp_vector4_t&मूल्य) override;
      व्योम shallow_copy(स्थिर vvp_object*obj) override;
      vvp_object* duplicate(व्योम) स्थिर override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    निजी:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : खुला vvp_darray {

    खुला:
      अंतरभूत vvp_darray_vec4(माप_प्रकार siz, अचिन्हित word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      माप_प्रकार get_आकार(व्योम) स्थिर override;
      व्योम set_word(अचिन्हित adr, स्थिर vvp_vector4_t&मूल्य) override;
      व्योम get_word(अचिन्हित adr, vvp_vector4_t&मूल्य) override;
      व्योम shallow_copy(स्थिर vvp_object*obj) override;
      vvp_object* duplicate(व्योम) स्थिर override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      व्योम reverse_elems(व्योम) override;
      व्योम sort_elems(bool ascending) override;
      व्योम shuffle_elems(व्योम) override;

    निजी:
      std::vector<vvp_vector4_t> array_;
      अचिन्हित word_wid_;
};

class vvp_darray_vec2 : खुला vvp_darray {

    खुला:
      अंतरभूत vvp_darray_vec2(माप_प्रकार siz, अचिन्हित word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      माप_प्रकार get_आकार(व्योम) स्थिर override;
      व्योम reverse_elems(व्योम) override;
      व्योम sort_elems(bool ascending) override;
      व्योम shuffle_elems(व्योम) override;
      व्योम set_word(अचिन्हित adr, स्थिर vvp_vector4_t&मूल्य) override;
      व्योम get_word(अचिन्हित adr, vvp_vector4_t&मूल्य) override;
      व्योम shallow_copy(स्थिर vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    निजी:
      std::vector<vvp_vector2_t> array_;
      अचिन्हित word_wid_;
};

class vvp_darray_real : खुला vvp_darray {

    खुला:
      explicit अंतरभूत vvp_darray_real(माप_प्रकार siz) : array_(siz) { }
      ~vvp_darray_real() override;

      माप_प्रकार get_आकार(व्योम) स्थिर override;
      व्योम set_word(अचिन्हित adr, द्विगुन मूल्य) override;
      व्योम get_word(अचिन्हित adr, द्विगुन&मूल्य) override;
      व्योम shallow_copy(स्थिर vvp_object*obj) override;
      vvp_object* duplicate(व्योम) स्थिर override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      व्योम reverse_elems(व्योम) override;
      व्योम sort_elems(bool ascending) override;
      व्योम shuffle_elems(व्योम) override;

    निजी:
      std::vector<द्विगुन> array_;
};

class vvp_darray_string : खुला vvp_darray {

    खुला:
      explicit अंतरभूत vvp_darray_string(माप_प्रकार siz) : array_(siz) { }
      ~vvp_darray_string() override;

      माप_प्रकार get_आकार(व्योम) स्थिर override;
      व्योम set_word(अचिन्हित adr, स्थिर std::string&मूल्य) override;
      व्योम get_word(अचिन्हित adr, std::string&मूल्य) override;
      व्योम shallow_copy(स्थिर vvp_object*obj) override;
      vvp_object* duplicate(व्योम) स्थिर override;
      व्योम reverse_elems(व्योम) override;
      व्योम sort_elems(bool ascending) override;
      व्योम shuffle_elems(व्योम) override;

    निजी:
      std::vector<std::string> array_;
};

class vvp_darray_object : खुला vvp_darray {

    खुला:
      explicit अंतरभूत vvp_darray_object(माप_प्रकार siz) : array_(siz) { }
      ~vvp_darray_object() override;

      माप_प्रकार get_आकार(व्योम) स्थिर override;
      व्योम reverse_elems(व्योम) override;
      व्योम sort_elems(bool ascending) override;
      व्योम shuffle_elems(व्योम) override;
      व्योम set_word(अचिन्हित adr, स्थिर vvp_object_t&मूल्य) override;
      व्योम get_word(अचिन्हित adr, vvp_object_t&मूल्य) override;
      व्योम shallow_copy(स्थिर vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    निजी:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : खुला vvp_darray {

    खुला:
      अंतरभूत vvp_queue(व्योम) { }
      ~vvp_queue() override;

      भव माप_प्रकार get_आकार(व्योम) स्थिर override =0;
      भव व्योम copy_elems(vvp_object_t src, अचिन्हित max_आकार);

      भव व्योम set_word_max(अचिन्हित adr, स्थिर vvp_vector4_t&मूल्य, अचिन्हित max_आकार);
      भव व्योम जोड़ो(अचिन्हित idx, स्थिर vvp_vector4_t&मूल्य, अचिन्हित max_आकार);
      भव व्योम धकेलो_back(स्थिर vvp_vector4_t&मूल्य, अचिन्हित max_आकार);
      भव व्योम धकेलो_front(स्थिर vvp_vector4_t&मूल्य, अचिन्हित max_आकार);

      भव व्योम set_word_max(अचिन्हित adr, द्विगुन मूल्य, अचिन्हित max_आकार);
      भव व्योम जोड़ो(अचिन्हित idx, द्विगुन मूल्य, अचिन्हित max_आकार);
      भव व्योम धकेलो_back(द्विगुन मूल्य, अचिन्हित max_आकार);
      भव व्योम धकेलो_front(द्विगुन मूल्य, अचिन्हित max_आकार);

      भव व्योम set_word_max(अचिन्हित adr, स्थिर std::string&मूल्य, अचिन्हित max_आकार);
      भव व्योम जोड़ो(अचिन्हित idx, स्थिर std::string&मूल्य, अचिन्हित max_आकार);
      भव व्योम धकेलो_back(स्थिर std::string&मूल्य, अचिन्हित max_आकार);
      भव व्योम धकेलो_front(स्थिर std::string&मूल्य, अचिन्हित max_आकार);

      भव व्योम निकालो_back(व्योम) =0;
      भव व्योम निकालो_front(व्योम)=0;
      भव व्योम erase(अचिन्हित idx)=0;
      भव व्योम erase_पुच्छ(अचिन्हित idx)=0;
};

class vvp_queue_real : खुला vvp_queue {

    खुला:
      माप_प्रकार get_आकार(व्योम) स्थिर override { वापस queue.आकार(); };
      व्योम copy_elems(vvp_object_t src, अचिन्हित max_आकार) override;
      व्योम set_word_max(अचिन्हित adr, द्विगुन मूल्य, अचिन्हित max_आकार) override;
      व्योम set_word(अचिन्हित adr, द्विगुन मूल्य) override;
      व्योम get_word(अचिन्हित adr, द्विगुन&मूल्य) override;
      व्योम जोड़ो(अचिन्हित idx, द्विगुन मूल्य, अचिन्हित max_आकार) override;
      व्योम धकेलो_back(द्विगुन मूल्य, अचिन्हित max_आकार) override;
      व्योम धकेलो_front(द्विगुन मूल्य, अचिन्हित max_आकार) override;
      व्योम निकालो_back(व्योम) override { queue.निकालो_back(); };
      व्योम निकालो_front(व्योम) override { queue.निकालो_front(); };
      व्योम erase(अचिन्हित idx) override;
      व्योम erase_पुच्छ(अचिन्हित idx) override;
      व्योम reverse_elems(व्योम) override;
      व्योम sort_elems(bool ascending) override;
      व्योम shuffle_elems(व्योम) override;

    निजी:
      std::deque<द्विगुन> queue;
};

class vvp_queue_string : खुला vvp_queue {

    खुला:
      माप_प्रकार get_आकार(व्योम) स्थिर override { वापस queue.आकार(); };
      व्योम copy_elems(vvp_object_t src, अचिन्हित max_आकार) override;
      व्योम set_word_max(अचिन्हित adr, स्थिर std::string&मूल्य, अचिन्हित max_आकार) override;
      व्योम set_word(अचिन्हित adr, स्थिर std::string&मूल्य) override;
      व्योम get_word(अचिन्हित adr, std::string&मूल्य) override;
      व्योम जोड़ो(अचिन्हित idx, स्थिर std::string&मूल्य, अचिन्हित max_आकार) override;
      व्योम धकेलो_back(स्थिर std::string&मूल्य, अचिन्हित max_आकार) override;
      व्योम धकेलो_front(स्थिर std::string&मूल्य, अचिन्हित max_आकार) override;
      व्योम निकालो_back(व्योम) override { queue.निकालो_back(); };
      व्योम निकालो_front(व्योम) override { queue.निकालो_front(); };
      व्योम erase(अचिन्हित idx) override;
      व्योम erase_पुच्छ(अचिन्हित idx) override;
      व्योम reverse_elems(व्योम) override;
      व्योम sort_elems(bool ascending) override;
      व्योम shuffle_elems(व्योम) override;

    निजी:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : खुला vvp_queue {

    खुला:
      माप_प्रकार get_आकार(व्योम) स्थिर override { वापस queue.आकार(); };
      व्योम copy_elems(vvp_object_t src, अचिन्हित max_आकार) override;
      व्योम set_word_max(अचिन्हित adr, स्थिर vvp_vector4_t&मूल्य, अचिन्हित max_आकार) override;
      व्योम set_word(अचिन्हित adr, स्थिर vvp_vector4_t&मूल्य) override;
      व्योम get_word(अचिन्हित adr, vvp_vector4_t&मूल्य) override;
      व्योम जोड़ो(अचिन्हित idx, स्थिर vvp_vector4_t&मूल्य, अचिन्हित max_आकार) override;
      व्योम धकेलो_back(स्थिर vvp_vector4_t&मूल्य, अचिन्हित max_आकार) override;
      व्योम धकेलो_front(स्थिर vvp_vector4_t&मूल्य, अचिन्हित max_आकार) override;
      व्योम निकालो_back(व्योम) override { queue.निकालो_back(); };
      व्योम निकालो_front(व्योम) override { queue.निकालो_front(); };
      व्योम erase(अचिन्हित idx) override;
      व्योम erase_पुच्छ(अचिन्हित idx) override;
      व्योम reverse_elems(व्योम) override;
      व्योम sort_elems(bool ascending) override;
      व्योम shuffle_elems(व्योम) override;

    निजी:
      std::deque<vvp_vector4_t> queue;
};

बाह्य std::string get_fileline();

#पूर्ण_अगर /* IVL_vvp_darray_H */
