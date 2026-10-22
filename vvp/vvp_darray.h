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

class vvp_darray : ସର୍ବସାଧାରଣ vvp_object {

    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vvp_darray() { }
      ଆଭାସୀ ~vvp_darray() override;

      ଆଭାସୀ size_t get_size(ଶୂନ୍ୟ) ଅଚଳ =0;

      ଆଭାସୀ ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ);
      ଆଭାସୀ ଶୂନ୍ୟ get_word(unsigned adr, vvp_vector4_t&ମୂଲ୍ୟ);

      ଆଭାସୀ ଶୂନ୍ୟ set_word(unsigned adr, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ);
      ଆଭାସୀ ଶୂନ୍ୟ get_word(unsigned adr, ଦ୍ୱିଗୁଣ&ମୂଲ୍ୟ);

      ଆଭାସୀ ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ std::string&ମୂଲ୍ୟ);
      ଆଭାସୀ ଶୂନ୍ୟ get_word(unsigned adr, std::string&ମୂଲ୍ୟ);

      ଆଭାସୀ ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ vvp_object_t&ମୂଲ୍ୟ);
      ଆଭାସୀ ଶୂନ୍ୟ get_word(unsigned adr, vvp_object_t&ମୂଲ୍ୟ);

      ଆଭାସୀ vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      ଆଭାସୀ ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      ଆଭାସୀ ଶୂନ୍ୟ sort_elems(bool ascending);
      ଆଭାସୀ ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ);
};

template <class TYPE> class vvp_darray_atom : ସର୍ବସାଧାରଣ vvp_darray {

    ସର୍ବସାଧାରଣ:
      explicit ଧାଡ଼ିଭିତରେ vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override;
      ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ sort_elems(bool ascending) override;
      ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ get_word(unsigned adr, vvp_vector4_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ shallow_copy(ଅଚଳ vvp_object*obj) override;
      vvp_object* duplicate(ଶୂନ୍ୟ) ଅଚଳ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ବ୍ୟକ୍ତିଗତ:
      std::vector<TYPE> array_;
};

class vvp_darray_vec4 : ସର୍ବସାଧାରଣ vvp_darray {

    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vvp_darray_vec4(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override;
      ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ get_word(unsigned adr, vvp_vector4_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ shallow_copy(ଅଚଳ vvp_object*obj) override;
      vvp_object* duplicate(ଶୂନ୍ୟ) ଅଚଳ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ sort_elems(bool ascending) override;
      ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ) override;

    ବ୍ୟକ୍ତିଗତ:
      std::vector<vvp_vector4_t> array_;
      unsigned word_wid_;
};

class vvp_darray_vec2 : ସର୍ବସାଧାରଣ vvp_darray {

    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vvp_darray_vec2(size_t siz, unsigned word_wid) :
                             array_(siz), word_wid_(word_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override;
      ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ sort_elems(bool ascending) override;
      ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ get_word(unsigned adr, vvp_vector4_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ shallow_copy(ଅଚଳ vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ବ୍ୟକ୍ତିଗତ:
      std::vector<vvp_vector2_t> array_;
      unsigned word_wid_;
};

class vvp_darray_real : ସର୍ବସାଧାରଣ vvp_darray {

    ସର୍ବସାଧାରଣ:
      explicit ଧାଡ଼ିଭିତରେ vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override;
      ଶୂନ୍ୟ set_word(unsigned adr, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ get_word(unsigned adr, ଦ୍ୱିଗୁଣ&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ shallow_copy(ଅଚଳ vvp_object*obj) override;
      vvp_object* duplicate(ଶୂନ୍ୟ) ଅଚଳ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ sort_elems(bool ascending) override;
      ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ) override;

    ବ୍ୟକ୍ତିଗତ:
      std::vector<ଦ୍ୱିଗୁଣ> array_;
};

class vvp_darray_string : ସର୍ବସାଧାରଣ vvp_darray {

    ସର୍ବସାଧାରଣ:
      explicit ଧାଡ଼ିଭିତରେ vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override;
      ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ std::string&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ get_word(unsigned adr, std::string&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ shallow_copy(ଅଚଳ vvp_object*obj) override;
      vvp_object* duplicate(ଶୂନ୍ୟ) ଅଚଳ override;
      ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ sort_elems(bool ascending) override;
      ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ) override;

    ବ୍ୟକ୍ତିଗତ:
      std::vector<std::string> array_;
};

class vvp_darray_object : ସର୍ବସାଧାରଣ vvp_darray {

    ସର୍ବସାଧାରଣ:
      explicit ଧାଡ଼ିଭିତରେ vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override;
      ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ sort_elems(bool ascending) override;
      ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ vvp_object_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ get_word(unsigned adr, vvp_object_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ shallow_copy(ଅଚଳ vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    ବ୍ୟକ୍ତିଗତ:
      std::vector<vvp_object_t> array_;
};

class vvp_queue : ସର୍ବସାଧାରଣ vvp_darray {

    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vvp_queue(ଶୂନ୍ୟ) { }
      ~vvp_queue() override;

      ଆଭାସୀ size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override =0;
      ଆଭାସୀ ଶୂନ୍ୟ copy_elems(vvp_object_t src, unsigned max_size);

      ଆଭାସୀ ଶୂନ୍ୟ set_word_max(unsigned adr, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned max_size);
      ଆଭାସୀ ଶୂନ୍ୟ ଭର୍ତ୍ତି(unsigned idx, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned max_size);
      ଆଭାସୀ ଶୂନ୍ୟ ଠେଲ_back(ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned max_size);
      ଆଭାସୀ ଶୂନ୍ୟ ଠେଲ_front(ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned max_size);

      ଆଭାସୀ ଶୂନ୍ୟ set_word_max(unsigned adr, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, unsigned max_size);
      ଆଭାସୀ ଶୂନ୍ୟ ଭର୍ତ୍ତି(unsigned idx, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, unsigned max_size);
      ଆଭାସୀ ଶୂନ୍ୟ ଠେଲ_back(ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, unsigned max_size);
      ଆଭାସୀ ଶୂନ୍ୟ ଠେଲ_front(ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, unsigned max_size);

      ଆଭାସୀ ଶୂନ୍ୟ set_word_max(unsigned adr, ଅଚଳ std::string&ମୂଲ୍ୟ, unsigned max_size);
      ଆଭାସୀ ଶୂନ୍ୟ ଭର୍ତ୍ତି(unsigned idx, ଅଚଳ std::string&ମୂଲ୍ୟ, unsigned max_size);
      ଆଭାସୀ ଶୂନ୍ୟ ଠେଲ_back(ଅଚଳ std::string&ମୂଲ୍ୟ, unsigned max_size);
      ଆଭାସୀ ଶୂନ୍ୟ ଠେଲ_front(ଅଚଳ std::string&ମୂଲ୍ୟ, unsigned max_size);

      ଆଭାସୀ ଶୂନ୍ୟ କାଢ଼_back(ଶୂନ୍ୟ) =0;
      ଆଭାସୀ ଶୂନ୍ୟ କାଢ଼_front(ଶୂନ୍ୟ)=0;
      ଆଭାସୀ ଶୂନ୍ୟ erase(unsigned idx)=0;
      ଆଭାସୀ ଶୂନ୍ୟ erase_tail(unsigned idx)=0;
};

class vvp_queue_real : ସର୍ବସାଧାରଣ vvp_queue {

    ସର୍ବସାଧାରଣ:
      size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ queue.size(); };
      ଶୂନ୍ୟ copy_elems(vvp_object_t src, unsigned max_size) override;
      ଶୂନ୍ୟ set_word_max(unsigned adr, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ set_word(unsigned adr, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ get_word(unsigned adr, ଦ୍ୱିଗୁଣ&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ ଭର୍ତ୍ତି(unsigned idx, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ ଠେଲ_back(ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ ଠେଲ_front(ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ କାଢ଼_back(ଶୂନ୍ୟ) override { queue.କାଢ଼_back(); };
      ଶୂନ୍ୟ କାଢ଼_front(ଶୂନ୍ୟ) override { queue.କାଢ଼_front(); };
      ଶୂନ୍ୟ erase(unsigned idx) override;
      ଶୂନ୍ୟ erase_tail(unsigned idx) override;
      ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ sort_elems(bool ascending) override;
      ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ) override;

    ବ୍ୟକ୍ତିଗତ:
      std::deque<ଦ୍ୱିଗୁଣ> queue;
};

class vvp_queue_string : ସର୍ବସାଧାରଣ vvp_queue {

    ସର୍ବସାଧାରଣ:
      size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ queue.size(); };
      ଶୂନ୍ୟ copy_elems(vvp_object_t src, unsigned max_size) override;
      ଶୂନ୍ୟ set_word_max(unsigned adr, ଅଚଳ std::string&ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ std::string&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ get_word(unsigned adr, std::string&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ ଭର୍ତ୍ତି(unsigned idx, ଅଚଳ std::string&ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ ଠେଲ_back(ଅଚଳ std::string&ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ ଠେଲ_front(ଅଚଳ std::string&ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ କାଢ଼_back(ଶୂନ୍ୟ) override { queue.କାଢ଼_back(); };
      ଶୂନ୍ୟ କାଢ଼_front(ଶୂନ୍ୟ) override { queue.କାଢ଼_front(); };
      ଶୂନ୍ୟ erase(unsigned idx) override;
      ଶୂନ୍ୟ erase_tail(unsigned idx) override;
      ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ sort_elems(bool ascending) override;
      ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ) override;

    ବ୍ୟକ୍ତିଗତ:
      std::deque<std::string> queue;
};

class vvp_queue_vec4 : ସର୍ବସାଧାରଣ vvp_queue {

    ସର୍ବସାଧାରଣ:
      size_t get_size(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ queue.size(); };
      ଶୂନ୍ୟ copy_elems(vvp_object_t src, unsigned max_size) override;
      ଶୂନ୍ୟ set_word_max(unsigned adr, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ set_word(unsigned adr, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ get_word(unsigned adr, vvp_vector4_t&ମୂଲ୍ୟ) override;
      ଶୂନ୍ୟ ଭର୍ତ୍ତି(unsigned idx, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ ଠେଲ_back(ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ ଠେଲ_front(ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned max_size) override;
      ଶୂନ୍ୟ କାଢ଼_back(ଶୂନ୍ୟ) override { queue.କାଢ଼_back(); };
      ଶୂନ୍ୟ କାଢ଼_front(ଶୂନ୍ୟ) override { queue.କାଢ଼_front(); };
      ଶୂନ୍ୟ erase(unsigned idx) override;
      ଶୂନ୍ୟ erase_tail(unsigned idx) override;
      ଶୂନ୍ୟ reverse_elems(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ sort_elems(bool ascending) override;
      ଶୂନ୍ୟ shuffle_elems(ଶୂନ୍ୟ) override;

    ବ୍ୟକ୍ତିଗତ:
      std::deque<vvp_vector4_t> queue;
};

ବାହ୍ୟ std::string get_fileline();

#endif /* IVL_vvp_darray_H */
