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

class vvp_darray : सार्वजनिक vvp_object {

    सार्वजनिक:
      अन्तःस्थ vvp_darray() { }
      आभास ~vvp_darray() override;

      आभास size_t get_size(शून्य) ध्रुव =0;

      आभास शून्य set_word(unsigned adr, ध्रुव vvp_vector4_t&मानम्);
      आभास शून्य get_word(unsigned adr, vvp_vector4_t&मानम्);

      आभास शून्य set_word(unsigned adr, द्विगुण मानम्);
      आभास शून्य get_word(unsigned adr, द्विगुण&मानम्);

      आभास शून्य set_word(unsigned adr, ध्रुव std::string&मानम्);
      आभास शून्य get_word(unsigned adr, std::string&मानम्);

      आभास शून्य set_word(unsigned adr, ध्रुव vvp_object_t&मानम्);
      आभास शून्य get_word(unsigned adr, vvp_object_t&मानम्);

      आभास vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      आभास शून्य reverse_elems(शून्य);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      आभास शून्य sort_elems(bool ascending);
      आभास शून्य shuffle_elems(शून्य);
};

template <class TYPE> class vvp_darray_atom : सार्वजनिक vvp_darray {

    सार्वजनिक:
      explicit अन्तःस्थ vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_size(शून्य) ध्रुव override;
      शून्य reverse_elems(शून्य) override;
      शून्य sort_elems(bool ascending) override;
      शून्य shuffle_elems(शून्य) override;
      शून्य set_word(unsigned adr, ध्रुव vvp_vector4_t&मानम्) override;
      शून्य get_word(unsigned adr, vvp_vector4_t&मानम्) override;
      शून्य shallow_copy(ध्रुव vvp_object*obj) override;
      vvp_object* duplicate(शून्य) ध्रुव override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    गुप्त:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : सार्वजनिक vvp_darray {

    सार्वजनिक:
      अन्तःस्थ vvp_darray_vec4(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_size(शून्य) ध्रुव override;
      शून्य set_word(unsigned adr, ध्रुव vvp_vector4_t&मानम्) override;
      शून्य get_word(unsigned adr, vvp_vector4_t&मानम्) override;
      शून्य shallow_copy(ध्रुव vvp_object*obj) override;
      vvp_object* duplicate(शून्य) ध्रुव override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      शून्य reverse_elems(शून्य) override;
      शून्य sort_elems(bool ascending) override;
      शून्य shuffle_elems(शून्य) override;

    गुप्त:
      std::vector<vvp_vector4_t> array_;
      unsigned word_wid_;
};

class vvp_darray_vec2 : सार्वजनिक vvp_darray {

    सार्वजनिक:
      अन्तःस्थ vvp_darray_vec2(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_size(शून्य) ध्रुव override;
      शून्य reverse_elems(शून्य) override;
      शून्य sort_elems(bool ascending) override;
      शून्य shuffle_elems(शून्य) override;
      शून्य set_word(unsigned adr, ध्रुव vvp_vector4_t&मानम्) override;
      शून्य get_word(unsigned adr, vvp_vector4_t&मानम्) override;
      शून्य shallow_copy(ध्रुव vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    गुप्त:
      std::vector<vvp_vector2_t> array_;
      unsigned word_wid_;
};

class vvp_darray_real : सार्वजनिक vvp_darray {

    सार्वजनिक:
      explicit अन्तःस्थ vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_size(शून्य) ध्रुव override;
      शून्य set_word(unsigned adr, द्विगुण मानम्) override;
      शून्य get_word(unsigned adr, द्विगुण&मानम्) override;
      शून्य shallow_copy(ध्रुव vvp_object*obj) override;
      vvp_object* duplicate(शून्य) ध्रुव override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      शून्य reverse_elems(शून्य) override;
      शून्य sort_elems(bool ascending) override;
      शून्य shuffle_elems(शून्य) override;

    गुप्त:
      std::vector<द्विगुण> array_;
};

class vvp_darray_string : सार्वजनिक vvp_darray {

    सार्वजनिक:
      explicit अन्तःस्थ vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_size(शून्य) ध्रुव override;
      शून्य set_word(unsigned adr, ध्रुव std::string&मानम्) override;
      शून्य get_word(unsigned adr, std::string&मानम्) override;
      शून्य shallow_copy(ध्रुव vvp_object*obj) override;
      vvp_object* duplicate(शून्य) ध्रुव override;
      शून्य reverse_elems(शून्य) override;
      शून्य sort_elems(bool ascending) override;
      शून्य shuffle_elems(शून्य) override;

    गुप्त:
      std::vector<std::string> array_;
};

class vvp_darray_object : सार्वजनिक vvp_darray {

    सार्वजनिक:
      explicit अन्तःस्थ vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_size(शून्य) ध्रुव override;
      शून्य reverse_elems(शून्य) override;
      शून्य sort_elems(bool ascending) override;
      शून्य shuffle_elems(शून्य) override;
      शून्य set_word(unsigned adr, ध्रुव vvp_object_t&मानम्) override;
      शून्य get_word(unsigned adr, vvp_object_t&मानम्) override;
      शून्य shallow_copy(ध्रुव vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    गुप्त:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : सार्वजनिक vvp_darray {

    सार्वजनिक:
      अन्तःस्थ vvp_queue(शून्य) { }
      ~vvp_queue() override;

      आभास size_t get_size(शून्य) ध्रुव override =0;
      आभास शून्य copy_elems(vvp_object_t src, unsigned max_size);

      आभास शून्य set_word_max(unsigned adr, ध्रुव vvp_vector4_t&मानम्, unsigned max_size);
      आभास शून्य निवेशय(unsigned idx, ध्रुव vvp_vector4_t&मानम्, unsigned max_size);
      आभास शून्य प्रक्षिप_back(ध्रुव vvp_vector4_t&मानम्, unsigned max_size);
      आभास शून्य प्रक्षिप_front(ध्रुव vvp_vector4_t&मानम्, unsigned max_size);

      आभास शून्य set_word_max(unsigned adr, द्विगुण मानम्, unsigned max_size);
      आभास शून्य निवेशय(unsigned idx, द्विगुण मानम्, unsigned max_size);
      आभास शून्य प्रक्षिप_back(द्विगुण मानम्, unsigned max_size);
      आभास शून्य प्रक्षिप_front(द्विगुण मानम्, unsigned max_size);

      आभास शून्य set_word_max(unsigned adr, ध्रुव std::string&मानम्, unsigned max_size);
      आभास शून्य निवेशय(unsigned idx, ध्रुव std::string&मानम्, unsigned max_size);
      आभास शून्य प्रक्षिप_back(ध्रुव std::string&मानम्, unsigned max_size);
      आभास शून्य प्रक्षिप_front(ध्रुव std::string&मानम्, unsigned max_size);

      आभास शून्य निष्कासय_back(शून्य) =0;
      आभास शून्य निष्कासय_front(शून्य)=0;
      आभास शून्य erase(unsigned idx)=0;
      आभास शून्य erase_tail(unsigned idx)=0;
};

class vvp_queue_real : सार्वजनिक vvp_queue {

    सार्वजनिक:
      size_t get_size(शून्य) ध्रुव override { प्रत्यागम queue.size(); };
      शून्य copy_elems(vvp_object_t src, unsigned max_size) override;
      शून्य set_word_max(unsigned adr, द्विगुण मानम्, unsigned max_size) override;
      शून्य set_word(unsigned adr, द्विगुण मानम्) override;
      शून्य get_word(unsigned adr, द्विगुण&मानम्) override;
      शून्य निवेशय(unsigned idx, द्विगुण मानम्, unsigned max_size) override;
      शून्य प्रक्षिप_back(द्विगुण मानम्, unsigned max_size) override;
      शून्य प्रक्षिप_front(द्विगुण मानम्, unsigned max_size) override;
      शून्य निष्कासय_back(शून्य) override { queue.निष्कासय_back(); };
      शून्य निष्कासय_front(शून्य) override { queue.निष्कासय_front(); };
      शून्य erase(unsigned idx) override;
      शून्य erase_tail(unsigned idx) override;
      शून्य reverse_elems(शून्य) override;
      शून्य sort_elems(bool ascending) override;
      शून्य shuffle_elems(शून्य) override;

    गुप्त:
      std::deque<द्विगुण> queue;
};

class vvp_queue_string : सार्वजनिक vvp_queue {

    सार्वजनिक:
      size_t get_size(शून्य) ध्रुव override { प्रत्यागम queue.size(); };
      शून्य copy_elems(vvp_object_t src, unsigned max_size) override;
      शून्य set_word_max(unsigned adr, ध्रुव std::string&मानम्, unsigned max_size) override;
      शून्य set_word(unsigned adr, ध्रुव std::string&मानम्) override;
      शून्य get_word(unsigned adr, std::string&मानम्) override;
      शून्य निवेशय(unsigned idx, ध्रुव std::string&मानम्, unsigned max_size) override;
      शून्य प्रक्षिप_back(ध्रुव std::string&मानम्, unsigned max_size) override;
      शून्य प्रक्षिप_front(ध्रुव std::string&मानम्, unsigned max_size) override;
      शून्य निष्कासय_back(शून्य) override { queue.निष्कासय_back(); };
      शून्य निष्कासय_front(शून्य) override { queue.निष्कासय_front(); };
      शून्य erase(unsigned idx) override;
      शून्य erase_tail(unsigned idx) override;
      शून्य reverse_elems(शून्य) override;
      शून्य sort_elems(bool ascending) override;
      शून्य shuffle_elems(शून्य) override;

    गुप्त:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : सार्वजनिक vvp_queue {

    सार्वजनिक:
      size_t get_size(शून्य) ध्रुव override { प्रत्यागम queue.size(); };
      शून्य copy_elems(vvp_object_t src, unsigned max_size) override;
      शून्य set_word_max(unsigned adr, ध्रुव vvp_vector4_t&मानम्, unsigned max_size) override;
      शून्य set_word(unsigned adr, ध्रुव vvp_vector4_t&मानम्) override;
      शून्य get_word(unsigned adr, vvp_vector4_t&मानम्) override;
      शून्य निवेशय(unsigned idx, ध्रुव vvp_vector4_t&मानम्, unsigned max_size) override;
      शून्य प्रक्षिप_back(ध्रुव vvp_vector4_t&मानम्, unsigned max_size) override;
      शून्य प्रक्षिप_front(ध्रुव vvp_vector4_t&मानम्, unsigned max_size) override;
      शून्य निष्कासय_back(शून्य) override { queue.निष्कासय_back(); };
      शून्य निष्कासय_front(शून्य) override { queue.निष्कासय_front(); };
      शून्य erase(unsigned idx) override;
      शून्य erase_tail(unsigned idx) override;
      शून्य reverse_elems(शून्य) override;
      शून्य sort_elems(bool ascending) override;
      शून्य shuffle_elems(शून्य) override;

    गुप्त:
      std::deque<vvp_vector4_t> queue;
};

बाह्य std::string get_fileline();

#endif /* IVL_vvp_darray_H */
