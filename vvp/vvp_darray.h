#که_نه_تعریف_شوی IVL_vvp_darray_H
#تعریف_کړه IVL_vvp_darray_H
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

      مجازي size_t get_اندازه(فراغ) ثابت =0;

      مجازي فراغ سېټ_کلمه(بینښه adr, ثابت vvp_vector4_t&ارزښت);
      مجازي فراغ get_کلمه(بینښه adr, vvp_vector4_t&ارزښت);

      مجازي فراغ سېټ_کلمه(بینښه adr, مزدوج ارزښت);
      مجازي فراغ get_کلمه(بینښه adr, مزدوج&ارزښت);

      مجازي فراغ سېټ_کلمه(بینښه adr, ثابت std::string&ارزښت);
      مجازي فراغ get_کلمه(بینښه adr, std::string&ارزښت);

      مجازي فراغ سېټ_کلمه(بینښه adr, ثابت vvp_object_t&ارزښت);
      مجازي فراغ get_کلمه(بینښه adr, vvp_object_t&ارزښت);

      مجازي vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      مجازي فراغ سرچپه_elems(فراغ);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      مجازي فراغ ترتیبول_elems(bool ascending);
      مجازي فراغ shuffle_elems(فراغ);
};

کينډۍ <class TYPE> class vvp_darray_atom : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ سرچپه_elems(فراغ) override;
      فراغ ترتیبول_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ سېټ_کلمه(بینښه adr, ثابت vvp_vector4_t&ارزښت) override;
      فراغ get_کلمه(بینښه adr, vvp_vector4_t&ارزښت) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::ویکټور<TYPE> array_;
};

class vvp_darray_vec4 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec4(size_t siz, بینښه کلمه_wid) :
                             array_(siz), کلمه_wid_2(کلمه_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ سېټ_کلمه(بینښه adr, ثابت vvp_vector4_t&ارزښت) override;
      فراغ get_کلمه(بینښه adr, vvp_vector4_t&ارزښت) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ سرچپه_elems(فراغ) override;
      فراغ ترتیبول_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::ویکټور<vvp_vector4_t> array_;
      بینښه کلمه_wid_2;
};

class vvp_darray_vec2 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec2(size_t siz, بینښه کلمه_wid) :
                             array_(siz), کلمه_wid_2(کلمه_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ سرچپه_elems(فراغ) override;
      فراغ ترتیبول_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ سېټ_کلمه(بینښه adr, ثابت vvp_vector4_t&ارزښت) override;
      فراغ get_کلمه(بینښه adr, vvp_vector4_t&ارزښت) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::ویکټور<vvp_vector2_t> array_;
      بینښه کلمه_wid_2;
};

class vvp_darray_real : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ سېټ_کلمه(بینښه adr, مزدوج ارزښت) override;
      فراغ get_کلمه(بینښه adr, مزدوج&ارزښت) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ سرچپه_elems(فراغ) override;
      فراغ ترتیبول_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::ویکټور<مزدوج> array_;
};

class vvp_darray_string : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ سېټ_کلمه(بینښه adr, ثابت std::string&ارزښت) override;
      فراغ get_کلمه(بینښه adr, std::string&ارزښت) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      فراغ سرچپه_elems(فراغ) override;
      فراغ ترتیبول_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::ویکټور<std::string> array_;
};

class vvp_darray_object : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ سرچپه_elems(فراغ) override;
      فراغ ترتیبول_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ سېټ_کلمه(بینښه adr, ثابت vvp_object_t&ارزښت) override;
      فراغ get_کلمه(بینښه adr, vvp_object_t&ارزښت) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    خاص:
      std::ویکټور<vvp_object_t> array_;
};

class vvp_کتار : عام vvp_darray {

    عام:
      مضمن vvp_کتار(فراغ) { }
      ~vvp_کتار() override;

      مجازي size_t get_اندازه(فراغ) ثابت override =0;
      مجازي فراغ copy_elems(vvp_object_t src, بینښه max_اندازه);

      مجازي فراغ سېټ_کلمه_max(بینښه adr, ثابت vvp_vector4_t&ارزښت, بینښه max_اندازه);
      مجازي فراغ ورننباسه(بینښه idx, ثابت vvp_vector4_t&ارزښت, بینښه max_اندازه);
      مجازي فراغ ټېل_شا(ثابت vvp_vector4_t&ارزښت, بینښه max_اندازه);
      مجازي فراغ ټېل_front(ثابت vvp_vector4_t&ارزښت, بینښه max_اندازه);

      مجازي فراغ سېټ_کلمه_max(بینښه adr, مزدوج ارزښت, بینښه max_اندازه);
      مجازي فراغ ورننباسه(بینښه idx, مزدوج ارزښت, بینښه max_اندازه);
      مجازي فراغ ټېل_شا(مزدوج ارزښت, بینښه max_اندازه);
      مجازي فراغ ټېل_front(مزدوج ارزښت, بینښه max_اندازه);

      مجازي فراغ سېټ_کلمه_max(بینښه adr, ثابت std::string&ارزښت, بینښه max_اندازه);
      مجازي فراغ ورننباسه(بینښه idx, ثابت std::string&ارزښت, بینښه max_اندازه);
      مجازي فراغ ټېل_شا(ثابت std::string&ارزښت, بینښه max_اندازه);
      مجازي فراغ ټېل_front(ثابت std::string&ارزښت, بینښه max_اندازه);

      مجازي فراغ راوباسه_شا(فراغ) =0;
      مجازي فراغ راوباسه_front(فراغ)=0;
      مجازي فراغ erase(بینښه idx)=0;
      مجازي فراغ erase_لکۍ(بینښه idx)=0;
};

class vvp_کتار_real : عام vvp_کتار {

    عام:
      size_t get_اندازه(فراغ) ثابت override { أرجع کتار.اندازه(); };
      فراغ copy_elems(vvp_object_t src, بینښه max_اندازه) override;
      فراغ سېټ_کلمه_max(بینښه adr, مزدوج ارزښت, بینښه max_اندازه) override;
      فراغ سېټ_کلمه(بینښه adr, مزدوج ارزښت) override;
      فراغ get_کلمه(بینښه adr, مزدوج&ارزښت) override;
      فراغ ورننباسه(بینښه idx, مزدوج ارزښت, بینښه max_اندازه) override;
      فراغ ټېل_شا(مزدوج ارزښت, بینښه max_اندازه) override;
      فراغ ټېل_front(مزدوج ارزښت, بینښه max_اندازه) override;
      فراغ راوباسه_شا(فراغ) override { کتار.راوباسه_شا(); };
      فراغ راوباسه_front(فراغ) override { کتار.راوباسه_front(); };
      فراغ erase(بینښه idx) override;
      فراغ erase_لکۍ(بینښه idx) override;
      فراغ سرچپه_elems(فراغ) override;
      فراغ ترتیبول_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<مزدوج> کتار;
};

class vvp_کتار_string : عام vvp_کتار {

    عام:
      size_t get_اندازه(فراغ) ثابت override { أرجع کتار.اندازه(); };
      فراغ copy_elems(vvp_object_t src, بینښه max_اندازه) override;
      فراغ سېټ_کلمه_max(بینښه adr, ثابت std::string&ارزښت, بینښه max_اندازه) override;
      فراغ سېټ_کلمه(بینښه adr, ثابت std::string&ارزښت) override;
      فراغ get_کلمه(بینښه adr, std::string&ارزښت) override;
      فراغ ورننباسه(بینښه idx, ثابت std::string&ارزښت, بینښه max_اندازه) override;
      فراغ ټېل_شا(ثابت std::string&ارزښت, بینښه max_اندازه) override;
      فراغ ټېل_front(ثابت std::string&ارزښت, بینښه max_اندازه) override;
      فراغ راوباسه_شا(فراغ) override { کتار.راوباسه_شا(); };
      فراغ راوباسه_front(فراغ) override { کتار.راوباسه_front(); };
      فراغ erase(بینښه idx) override;
      فراغ erase_لکۍ(بینښه idx) override;
      فراغ سرچپه_elems(فراغ) override;
      فراغ ترتیبول_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<std::string> کتار;
};

class vvp_کتار_vec4 : عام vvp_کتار {

    عام:
      size_t get_اندازه(فراغ) ثابت override { أرجع کتار.اندازه(); };
      فراغ copy_elems(vvp_object_t src, بینښه max_اندازه) override;
      فراغ سېټ_کلمه_max(بینښه adr, ثابت vvp_vector4_t&ارزښت, بینښه max_اندازه) override;
      فراغ سېټ_کلمه(بینښه adr, ثابت vvp_vector4_t&ارزښت) override;
      فراغ get_کلمه(بینښه adr, vvp_vector4_t&ارزښت) override;
      فراغ ورننباسه(بینښه idx, ثابت vvp_vector4_t&ارزښت, بینښه max_اندازه) override;
      فراغ ټېل_شا(ثابت vvp_vector4_t&ارزښت, بینښه max_اندازه) override;
      فراغ ټېل_front(ثابت vvp_vector4_t&ارزښت, بینښه max_اندازه) override;
      فراغ راوباسه_شا(فراغ) override { کتار.راوباسه_شا(); };
      فراغ راوباسه_front(فراغ) override { کتار.راوباسه_front(); };
      فراغ erase(بینښه idx) override;
      فراغ erase_لکۍ(بینښه idx) override;
      فراغ سرچپه_elems(فراغ) override;
      فراغ ترتیبول_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<vvp_vector4_t> کتار;
};

خارجي std::string get_fileline();

#د_که_پای /* IVL_vvp_darray_H */
