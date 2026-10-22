#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_vvp_darray_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_vvp_darray_H
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

class vvp_darray : ਜਨਤਕ vvp_object {

    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vvp_darray() { }
      ਆਭਾਸੀ ~vvp_darray() override;

      ਆਭਾਸੀ size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ =0;

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&ਮੁੱਲ);
      ਆਭਾਸੀ ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_vector4_t&ਮੁੱਲ);

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ ਮੁੱਲ);
      ਆਭਾਸੀ ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ&ਮੁੱਲ);

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ std::string&ਮੁੱਲ);
      ਆਭਾਸੀ ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, std::string&ਮੁੱਲ);

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_object_t&ਮੁੱਲ);
      ਆਭਾਸੀ ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_object_t&ਮੁੱਲ);

      ਆਭਾਸੀ vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      ਆਭਾਸੀ ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      ਆਭਾਸੀ ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending);
      ਆਭਾਸੀ ਖਾਲੀ shuffle_elems(ਖਾਲੀ);
};

ਸਾਂਚਾ <class TYPE> class vvp_darray_atom : ਜਨਤਕ vvp_darray {

    ਜਨਤਕ:
      explicit ਸਤਰਅੰਦਰ vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override;
      ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending) override;
      ਖਾਲੀ shuffle_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&ਮੁੱਲ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_vector4_t&ਮੁੱਲ) override;
      ਖਾਲੀ shallow_copy(ਅਚਲ vvp_object*obj) override;
      vvp_object* duplicate(ਖਾਲੀ) ਅਚਲ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ਨਿਜੀ:
      std::ਸਦਿਸ਼<TYPE> array_;
};

class vvp_darray_vec4 : ਜਨਤਕ vvp_darray {

    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vvp_darray_vec4(size_t siz, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_wid) :
                             array_(siz), ਸ਼ਬਦ_wid_2(ਸ਼ਬਦ_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&ਮੁੱਲ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_vector4_t&ਮੁੱਲ) override;
      ਖਾਲੀ shallow_copy(ਅਚਲ vvp_object*obj) override;
      vvp_object* duplicate(ਖਾਲੀ) ਅਚਲ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending) override;
      ਖਾਲੀ shuffle_elems(ਖਾਲੀ) override;

    ਨਿਜੀ:
      std::ਸਦਿਸ਼<vvp_vector4_t> array_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_wid_2;
};

class vvp_darray_vec2 : ਜਨਤਕ vvp_darray {

    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vvp_darray_vec2(size_t siz, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_wid) :
                             array_(siz), ਸ਼ਬਦ_wid_2(ਸ਼ਬਦ_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override;
      ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending) override;
      ਖਾਲੀ shuffle_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&ਮੁੱਲ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_vector4_t&ਮੁੱਲ) override;
      ਖਾਲੀ shallow_copy(ਅਚਲ vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    ਨਿਜੀ:
      std::ਸਦਿਸ਼<vvp_vector2_t> array_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_wid_2;
};

class vvp_darray_real : ਜਨਤਕ vvp_darray {

    ਜਨਤਕ:
      explicit ਸਤਰਅੰਦਰ vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ ਮੁੱਲ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ&ਮੁੱਲ) override;
      ਖਾਲੀ shallow_copy(ਅਚਲ vvp_object*obj) override;
      vvp_object* duplicate(ਖਾਲੀ) ਅਚਲ override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending) override;
      ਖਾਲੀ shuffle_elems(ਖਾਲੀ) override;

    ਨਿਜੀ:
      std::ਸਦਿਸ਼<ਦੁੱਗਣਾ> array_;
};

class vvp_darray_string : ਜਨਤਕ vvp_darray {

    ਜਨਤਕ:
      explicit ਸਤਰਅੰਦਰ vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ std::string&ਮੁੱਲ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, std::string&ਮੁੱਲ) override;
      ਖਾਲੀ shallow_copy(ਅਚਲ vvp_object*obj) override;
      vvp_object* duplicate(ਖਾਲੀ) ਅਚਲ override;
      ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending) override;
      ਖਾਲੀ shuffle_elems(ਖਾਲੀ) override;

    ਨਿਜੀ:
      std::ਸਦਿਸ਼<std::string> array_;
};

class vvp_darray_object : ਜਨਤਕ vvp_darray {

    ਜਨਤਕ:
      explicit ਸਤਰਅੰਦਰ vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override;
      ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending) override;
      ਖਾਲੀ shuffle_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_object_t&ਮੁੱਲ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_object_t&ਮੁੱਲ) override;
      ਖਾਲੀ shallow_copy(ਅਚਲ vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    ਨਿਜੀ:
      std::ਸਦਿਸ਼<vvp_object_t> array_;
};

class vvp_ਕਤਾਰ : ਜਨਤਕ vvp_darray {

    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vvp_ਕਤਾਰ(ਖਾਲੀ) { }
      ~vvp_ਕਤਾਰ() override;

      ਆਭਾਸੀ size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override =0;
      ਆਭਾਸੀ ਖਾਲੀ copy_elems(vvp_object_t src, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);
      ਆਭਾਸੀ ਖਾਲੀ ਪਾਓ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);
      ਆਭਾਸੀ ਖਾਲੀ ਧੱਕੋ_ਪਿੱਠ(ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);
      ਆਭਾਸੀ ਖਾਲੀ ਧੱਕੋ_front(ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);
      ਆਭਾਸੀ ਖਾਲੀ ਪਾਓ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਦੁੱਗਣਾ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);
      ਆਭਾਸੀ ਖਾਲੀ ਧੱਕੋ_ਪਿੱਠ(ਦੁੱਗਣਾ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);
      ਆਭਾਸੀ ਖਾਲੀ ਧੱਕੋ_front(ਦੁੱਗਣਾ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ std::string&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);
      ਆਭਾਸੀ ਖਾਲੀ ਪਾਓ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ std::string&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);
      ਆਭਾਸੀ ਖਾਲੀ ਧੱਕੋ_ਪਿੱਠ(ਅਚਲ std::string&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);
      ਆਭਾਸੀ ਖਾਲੀ ਧੱਕੋ_front(ਅਚਲ std::string&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ);

      ਆਭਾਸੀ ਖਾਲੀ ਕੱਢੋ_ਪਿੱਠ(ਖਾਲੀ) =0;
      ਆਭਾਸੀ ਖਾਲੀ ਕੱਢੋ_front(ਖਾਲੀ)=0;
      ਆਭਾਸੀ ਖਾਲੀ erase(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)=0;
      ਆਭਾਸੀ ਖਾਲੀ erase_ਪੂਛ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)=0;
};

class vvp_ਕਤਾਰ_real : ਜਨਤਕ vvp_ਕਤਾਰ {

    ਜਨਤਕ:
      size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ ਕਤਾਰ.ਮਾਪ(); };
      ਖਾਲੀ copy_elems(vvp_object_t src, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ ਮੁੱਲ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ&ਮੁੱਲ) override;
      ਖਾਲੀ ਪਾਓ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਦੁੱਗਣਾ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਧੱਕੋ_ਪਿੱਠ(ਦੁੱਗਣਾ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਧੱਕੋ_front(ਦੁੱਗਣਾ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਕੱਢੋ_ਪਿੱਠ(ਖਾਲੀ) override { ਕਤਾਰ.ਕੱਢੋ_ਪਿੱਠ(); };
      ਖਾਲੀ ਕੱਢੋ_front(ਖਾਲੀ) override { ਕਤਾਰ.ਕੱਢੋ_front(); };
      ਖਾਲੀ erase(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) override;
      ਖਾਲੀ erase_ਪੂਛ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) override;
      ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending) override;
      ਖਾਲੀ shuffle_elems(ਖਾਲੀ) override;

    ਨਿਜੀ:
      std::deque<ਦੁੱਗਣਾ> ਕਤਾਰ;
};

class vvp_ਕਤਾਰ_string : ਜਨਤਕ vvp_ਕਤਾਰ {

    ਜਨਤਕ:
      size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ ਕਤਾਰ.ਮਾਪ(); };
      ਖਾਲੀ copy_elems(vvp_object_t src, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ std::string&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ std::string&ਮੁੱਲ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, std::string&ਮੁੱਲ) override;
      ਖਾਲੀ ਪਾਓ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ std::string&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਧੱਕੋ_ਪਿੱਠ(ਅਚਲ std::string&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਧੱਕੋ_front(ਅਚਲ std::string&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਕੱਢੋ_ਪਿੱਠ(ਖਾਲੀ) override { ਕਤਾਰ.ਕੱਢੋ_ਪਿੱਠ(); };
      ਖਾਲੀ ਕੱਢੋ_front(ਖਾਲੀ) override { ਕਤਾਰ.ਕੱਢੋ_front(); };
      ਖਾਲੀ erase(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) override;
      ਖਾਲੀ erase_ਪੂਛ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) override;
      ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending) override;
      ਖਾਲੀ shuffle_elems(ਖਾਲੀ) override;

    ਨਿਜੀ:
      std::deque<std::string> ਕਤਾਰ;
};

class vvp_ਕਤਾਰ_vec4 : ਜਨਤਕ vvp_ਕਤਾਰ {

    ਜਨਤਕ:
      size_t get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ ਕਤਾਰ.ਮਾਪ(); };
      ਖਾਲੀ copy_elems(vvp_object_t src, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&ਮੁੱਲ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_vector4_t&ਮੁੱਲ) override;
      ਖਾਲੀ ਪਾਓ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਧੱਕੋ_ਪਿੱਠ(ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਧੱਕੋ_front(ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ) override;
      ਖਾਲੀ ਕੱਢੋ_ਪਿੱਠ(ਖਾਲੀ) override { ਕਤਾਰ.ਕੱਢੋ_ਪਿੱਠ(); };
      ਖਾਲੀ ਕੱਢੋ_front(ਖਾਲੀ) override { ਕਤਾਰ.ਕੱਢੋ_front(); };
      ਖਾਲੀ erase(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) override;
      ਖਾਲੀ erase_ਪੂਛ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) override;
      ਖਾਲੀ ਉਲਟ_elems(ਖਾਲੀ) override;
      ਖਾਲੀ ਛਾਂਟੋ_elems(bool ascending) override;
      ਖਾਲੀ shuffle_elems(ਖਾਲੀ) override;

    ਨਿਜੀ:
      std::deque<vvp_vector4_t> ਕਤਾਰ;
};

ਬਾਹਰੀ std::string get_fileline();

#ਜੇ_ਅੰਤ /* IVL_vvp_darray_H */
