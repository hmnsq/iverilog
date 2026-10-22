#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_vvp_darray_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_vvp_darray_H
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
      ಸಾಲೊಳಗೆ vvp_darray() { }
      ವಾಸ್ತವಿಕ ~vvp_darray() override;

      ವಾಸ್ತವಿಕ size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ =0;

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_vector4_t&ಮೌಲ್ಯ);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ&ಮೌಲ್ಯ);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, std::string&ಮೌಲ್ಯ);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_object_t&ಮೌಲ್ಯ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_object_t&ಮೌಲ್ಯ);

      ವಾಸ್ತವಿಕ vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ);
};

ಮಾದರಿ <class TYPE> class vvp_darray_atom : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      explicit ಸಾಲೊಳಗೆ vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj) override;
      vvp_object* duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ಖಾಸಗಿ:
      std::ಸದಿಶ<TYPE> array_;
};

class vvp_darray_vec4 : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vvp_darray_vec4(size_t siz, ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_wid) :
                             array_(siz), ಶಬ್ದ_wid_2(ಶಬ್ದ_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj) override;
      vvp_object* duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಖಾಸಗಿ:
      std::ಸದಿಶ<vvp_vector4_t> array_;
      ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_wid_2;
};

class vvp_darray_vec2 : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vvp_darray_vec2(size_t siz, ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_wid) :
                             array_(siz), ಶಬ್ದ_wid_2(ಶಬ್ದ_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ಖಾಸಗಿ:
      std::ಸದಿಶ<vvp_vector2_t> array_;
      ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_wid_2;
};

class vvp_darray_real : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      explicit ಸಾಲೊಳಗೆ vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj) override;
      vvp_object* duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಖಾಸಗಿ:
      std::ಸದಿಶ<ದ್ವಿಗುಣ> array_;
};

class vvp_darray_string : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      explicit ಸಾಲೊಳಗೆ vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, std::string&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj) override;
      vvp_object* duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಖಾಸಗಿ:
      std::ಸದಿಶ<std::string> array_;
};

class vvp_darray_object : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      explicit ಸಾಲೊಳಗೆ vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_object_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_object_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    ಖಾಸಗಿ:
      std::ಸದಿಶ<vvp_object_t> array_;
};

class vvp_ಸರತಿ : ಸಾರ್ವಜನಿಕ vvp_darray {

    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vvp_ಸರತಿ(ಶೂನ್ಯ) { }
      ~vvp_ಸರತಿ() override;

      ವಾಸ್ತವಿಕ size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override =0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ copy_elems(vvp_object_t src, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸೇರಿಸು(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ತಳ್ಳು_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ತಳ್ಳು_front(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸೇರಿಸು(ಚಿಹ್ನೆರಹಿತ idx, ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ತಳ್ಳು_ಹಿಂಭಾಗ(ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ತಳ್ಳು_front(ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸೇರಿಸು(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ತಳ್ಳು_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ತಳ್ಳು_front(ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ತೆಗೆ_ಹಿಂಭಾಗ(ಶೂನ್ಯ) =0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ತೆಗೆ_front(ಶೂನ್ಯ)=0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ erase(ಚಿಹ್ನೆರಹಿತ idx)=0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ erase_ಬಾಲ(ಚಿಹ್ನೆರಹಿತ idx)=0;
};

class vvp_ಸರತಿ_real : ಸಾರ್ವಜನಿಕ vvp_ಸರತಿ {

    ಸಾರ್ವಜನಿಕ:
      size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಸರತಿ.ಗಾತ್ರ_2(); };
      ಶೂನ್ಯ copy_elems(vvp_object_t src, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ ಸೇರಿಸು(ಚಿಹ್ನೆರಹಿತ idx, ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ತಳ್ಳು_ಹಿಂಭಾಗ(ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ತಳ್ಳು_front(ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ತೆಗೆ_ಹಿಂಭಾಗ(ಶೂನ್ಯ) override { ಸರತಿ.ತೆಗೆ_ಹಿಂಭಾಗ(); };
      ಶೂನ್ಯ ತೆಗೆ_front(ಶೂನ್ಯ) override { ಸರತಿ.ತೆಗೆ_front(); };
      ಶೂನ್ಯ erase(ಚಿಹ್ನೆರಹಿತ idx) override;
      ಶೂನ್ಯ erase_ಬಾಲ(ಚಿಹ್ನೆರಹಿತ idx) override;
      ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಖಾಸಗಿ:
      std::deque<ದ್ವಿಗುಣ> ಸರತಿ;
};

class vvp_ಸರತಿ_string : ಸಾರ್ವಜನಿಕ vvp_ಸರತಿ {

    ಸಾರ್ವಜನಿಕ:
      size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಸರತಿ.ಗಾತ್ರ_2(); };
      ಶೂನ್ಯ copy_elems(vvp_object_t src, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, std::string&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ ಸೇರಿಸು(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ತಳ್ಳು_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ತಳ್ಳು_front(ಸ್ಥಿರಾಂಕ std::string&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ತೆಗೆ_ಹಿಂಭಾಗ(ಶೂನ್ಯ) override { ಸರತಿ.ತೆಗೆ_ಹಿಂಭಾಗ(); };
      ಶೂನ್ಯ ತೆಗೆ_front(ಶೂನ್ಯ) override { ಸರತಿ.ತೆಗೆ_front(); };
      ಶೂನ್ಯ erase(ಚಿಹ್ನೆರಹಿತ idx) override;
      ಶೂನ್ಯ erase_ಬಾಲ(ಚಿಹ್ನೆರಹಿತ idx) override;
      ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಖಾಸಗಿ:
      std::deque<std::string> ಸರತಿ;
};

class vvp_ಸರತಿ_vec4 : ಸಾರ್ವಜನಿಕ vvp_ಸರತಿ {

    ಸಾರ್ವಜನಿಕ:
      size_t get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಸರತಿ.ಗಾತ್ರ_2(); };
      ಶೂನ್ಯ copy_elems(vvp_object_t src, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_vector4_t&ಮೌಲ್ಯ) override;
      ಶೂನ್ಯ ಸೇರಿಸು(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ತಳ್ಳು_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ತಳ್ಳು_front(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ) override;
      ಶೂನ್ಯ ತೆಗೆ_ಹಿಂಭಾಗ(ಶೂನ್ಯ) override { ಸರತಿ.ತೆಗೆ_ಹಿಂಭಾಗ(); };
      ಶೂನ್ಯ ತೆಗೆ_front(ಶೂನ್ಯ) override { ಸರತಿ.ತೆಗೆ_front(); };
      ಶೂನ್ಯ erase(ಚಿಹ್ನೆರಹಿತ idx) override;
      ಶೂನ್ಯ erase_ಬಾಲ(ಚಿಹ್ನೆರಹಿತ idx) override;
      ಶೂನ್ಯ ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ ವಿಂಗಡಿಸು_elems(bool ascending) override;
      ಶೂನ್ಯ shuffle_elems(ಶೂನ್ಯ) override;

    ಖಾಸಗಿ:
      std::deque<vvp_vector4_t> ಸರತಿ;
};

ಬಾಹ್ಯ std::string get_fileline();

#ಆದರೆ_ಅಂತ್ಯ /* IVL_vvp_darray_H */
