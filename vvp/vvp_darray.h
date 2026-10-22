#جيڪڏهن_وصف_نه_ٿيل IVL_vvp_darray_H
#وصف_ڪريو IVL_vvp_darray_H
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
      مجازي ~vvp_darray() override;

      مجازي size_t get_سائيز(فراغ) ثابت =0;

      مجازي فراغ سيٽ_لفظ(بنانشان adr, ثابت vvp_vector4_t&قدر);
      مجازي فراغ get_لفظ(بنانشان adr, vvp_vector4_t&قدر);

      مجازي فراغ سيٽ_لفظ(بنانشان adr, مزدوج قدر);
      مجازي فراغ get_لفظ(بنانشان adr, مزدوج&قدر);

      مجازي فراغ سيٽ_لفظ(بنانشان adr, ثابت std::string&قدر);
      مجازي فراغ get_لفظ(بنانشان adr, std::string&قدر);

      مجازي فراغ سيٽ_لفظ(بنانشان adr, ثابت vvp_object_t&قدر);
      مجازي فراغ get_لفظ(بنانشان adr, vvp_object_t&قدر);

      مجازي vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      مجازي فراغ ابتو_elems(فراغ);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      مجازي فراغ ترتيب_ڏيو_elems(bool ascending);
      مجازي فراغ shuffle_elems(فراغ);
};

سانچو <class TYPE> class vvp_darray_atom : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_سائيز(فراغ) ثابت override;
      فراغ ابتو_elems(فراغ) override;
      فراغ ترتيب_ڏيو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ سيٽ_لفظ(بنانشان adr, ثابت vvp_vector4_t&قدر) override;
      فراغ get_لفظ(بنانشان adr, vvp_vector4_t&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::ويڪٽر<TYPE> array_;
};

class vvp_darray_vec4 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec4(size_t siz, بنانشان لفظ_wid) :
                             array_(siz), لفظ_wid_2(لفظ_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_سائيز(فراغ) ثابت override;
      فراغ سيٽ_لفظ(بنانشان adr, ثابت vvp_vector4_t&قدر) override;
      فراغ get_لفظ(بنانشان adr, vvp_vector4_t&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ ابتو_elems(فراغ) override;
      فراغ ترتيب_ڏيو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::ويڪٽر<vvp_vector4_t> array_;
      بنانشان لفظ_wid_2;
};

class vvp_darray_vec2 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec2(size_t siz, بنانشان لفظ_wid) :
                             array_(siz), لفظ_wid_2(لفظ_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_سائيز(فراغ) ثابت override;
      فراغ ابتو_elems(فراغ) override;
      فراغ ترتيب_ڏيو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ سيٽ_لفظ(بنانشان adr, ثابت vvp_vector4_t&قدر) override;
      فراغ get_لفظ(بنانشان adr, vvp_vector4_t&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::ويڪٽر<vvp_vector2_t> array_;
      بنانشان لفظ_wid_2;
};

class vvp_darray_real : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_سائيز(فراغ) ثابت override;
      فراغ سيٽ_لفظ(بنانشان adr, مزدوج قدر) override;
      فراغ get_لفظ(بنانشان adr, مزدوج&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ ابتو_elems(فراغ) override;
      فراغ ترتيب_ڏيو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::ويڪٽر<مزدوج> array_;
};

class vvp_darray_string : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_سائيز(فراغ) ثابت override;
      فراغ سيٽ_لفظ(بنانشان adr, ثابت std::string&قدر) override;
      فراغ get_لفظ(بنانشان adr, std::string&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      فراغ ابتو_elems(فراغ) override;
      فراغ ترتيب_ڏيو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::ويڪٽر<std::string> array_;
};

class vvp_darray_object : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_سائيز(فراغ) ثابت override;
      فراغ ابتو_elems(فراغ) override;
      فراغ ترتيب_ڏيو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ سيٽ_لفظ(بنانشان adr, ثابت vvp_object_t&قدر) override;
      فراغ get_لفظ(بنانشان adr, vvp_object_t&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    خاص:
      std::ويڪٽر<vvp_object_t> array_;
};

class vvp_قطار : عام vvp_darray {

    عام:
      مضمن vvp_قطار(فراغ) { }
      ~vvp_قطار() override;

      مجازي size_t get_سائيز(فراغ) ثابت override =0;
      مجازي فراغ copy_elems(vvp_object_t src, بنانشان max_سائيز);

      مجازي فراغ سيٽ_لفظ_max(بنانشان adr, ثابت vvp_vector4_t&قدر, بنانشان max_سائيز);
      مجازي فراغ وجهو(بنانشان idx, ثابت vvp_vector4_t&قدر, بنانشان max_سائيز);
      مجازي فراغ ڌڪو_پٺ(ثابت vvp_vector4_t&قدر, بنانشان max_سائيز);
      مجازي فراغ ڌڪو_front(ثابت vvp_vector4_t&قدر, بنانشان max_سائيز);

      مجازي فراغ سيٽ_لفظ_max(بنانشان adr, مزدوج قدر, بنانشان max_سائيز);
      مجازي فراغ وجهو(بنانشان idx, مزدوج قدر, بنانشان max_سائيز);
      مجازي فراغ ڌڪو_پٺ(مزدوج قدر, بنانشان max_سائيز);
      مجازي فراغ ڌڪو_front(مزدوج قدر, بنانشان max_سائيز);

      مجازي فراغ سيٽ_لفظ_max(بنانشان adr, ثابت std::string&قدر, بنانشان max_سائيز);
      مجازي فراغ وجهو(بنانشان idx, ثابت std::string&قدر, بنانشان max_سائيز);
      مجازي فراغ ڌڪو_پٺ(ثابت std::string&قدر, بنانشان max_سائيز);
      مجازي فراغ ڌڪو_front(ثابت std::string&قدر, بنانشان max_سائيز);

      مجازي فراغ ڪڍو_پٺ(فراغ) =0;
      مجازي فراغ ڪڍو_front(فراغ)=0;
      مجازي فراغ erase(بنانشان idx)=0;
      مجازي فراغ erase_پڇ(بنانشان idx)=0;
};

class vvp_قطار_real : عام vvp_قطار {

    عام:
      size_t get_سائيز(فراغ) ثابت override { أرجع قطار.سائيز(); };
      فراغ copy_elems(vvp_object_t src, بنانشان max_سائيز) override;
      فراغ سيٽ_لفظ_max(بنانشان adr, مزدوج قدر, بنانشان max_سائيز) override;
      فراغ سيٽ_لفظ(بنانشان adr, مزدوج قدر) override;
      فراغ get_لفظ(بنانشان adr, مزدوج&قدر) override;
      فراغ وجهو(بنانشان idx, مزدوج قدر, بنانشان max_سائيز) override;
      فراغ ڌڪو_پٺ(مزدوج قدر, بنانشان max_سائيز) override;
      فراغ ڌڪو_front(مزدوج قدر, بنانشان max_سائيز) override;
      فراغ ڪڍو_پٺ(فراغ) override { قطار.ڪڍو_پٺ(); };
      فراغ ڪڍو_front(فراغ) override { قطار.ڪڍو_front(); };
      فراغ erase(بنانشان idx) override;
      فراغ erase_پڇ(بنانشان idx) override;
      فراغ ابتو_elems(فراغ) override;
      فراغ ترتيب_ڏيو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<مزدوج> قطار;
};

class vvp_قطار_string : عام vvp_قطار {

    عام:
      size_t get_سائيز(فراغ) ثابت override { أرجع قطار.سائيز(); };
      فراغ copy_elems(vvp_object_t src, بنانشان max_سائيز) override;
      فراغ سيٽ_لفظ_max(بنانشان adr, ثابت std::string&قدر, بنانشان max_سائيز) override;
      فراغ سيٽ_لفظ(بنانشان adr, ثابت std::string&قدر) override;
      فراغ get_لفظ(بنانشان adr, std::string&قدر) override;
      فراغ وجهو(بنانشان idx, ثابت std::string&قدر, بنانشان max_سائيز) override;
      فراغ ڌڪو_پٺ(ثابت std::string&قدر, بنانشان max_سائيز) override;
      فراغ ڌڪو_front(ثابت std::string&قدر, بنانشان max_سائيز) override;
      فراغ ڪڍو_پٺ(فراغ) override { قطار.ڪڍو_پٺ(); };
      فراغ ڪڍو_front(فراغ) override { قطار.ڪڍو_front(); };
      فراغ erase(بنانشان idx) override;
      فراغ erase_پڇ(بنانشان idx) override;
      فراغ ابتو_elems(فراغ) override;
      فراغ ترتيب_ڏيو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<std::string> قطار;
};

class vvp_قطار_vec4 : عام vvp_قطار {

    عام:
      size_t get_سائيز(فراغ) ثابت override { أرجع قطار.سائيز(); };
      فراغ copy_elems(vvp_object_t src, بنانشان max_سائيز) override;
      فراغ سيٽ_لفظ_max(بنانشان adr, ثابت vvp_vector4_t&قدر, بنانشان max_سائيز) override;
      فراغ سيٽ_لفظ(بنانشان adr, ثابت vvp_vector4_t&قدر) override;
      فراغ get_لفظ(بنانشان adr, vvp_vector4_t&قدر) override;
      فراغ وجهو(بنانشان idx, ثابت vvp_vector4_t&قدر, بنانشان max_سائيز) override;
      فراغ ڌڪو_پٺ(ثابت vvp_vector4_t&قدر, بنانشان max_سائيز) override;
      فراغ ڌڪو_front(ثابت vvp_vector4_t&قدر, بنانشان max_سائيز) override;
      فراغ ڪڍو_پٺ(فراغ) override { قطار.ڪڍو_پٺ(); };
      فراغ ڪڍو_front(فراغ) override { قطار.ڪڍو_front(); };
      فراغ erase(بنانشان idx) override;
      فراغ erase_پڇ(بنانشان idx) override;
      فراغ ابتو_elems(فراغ) override;
      فراغ ترتيب_ڏيو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<vvp_vector4_t> قطار;
};

خارجي std::string get_fileline();

#جيڪڏهن_پورو /* IVL_vvp_darray_H */
