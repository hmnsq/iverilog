#جے_بنا_تعریف IVL_vvp_darray_H
#تعریف_کرو IVL_vvp_darray_H
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
      مجازی ~vvp_darray() override;

      مجازی size_t get_ناپ(فراغ) ثابت =0;

      مجازی فراغ سیٹ_لفظ(بنانشانی adr, ثابت vvp_vector4_t&قدر);
      مجازی فراغ get_لفظ(بنانشانی adr, vvp_vector4_t&قدر);

      مجازی فراغ سیٹ_لفظ(بنانشانی adr, مزدوج قدر);
      مجازی فراغ get_لفظ(بنانشانی adr, مزدوج&قدر);

      مجازی فراغ سیٹ_لفظ(بنانشانی adr, ثابت std::string&قدر);
      مجازی فراغ get_لفظ(بنانشانی adr, std::string&قدر);

      مجازی فراغ سیٹ_لفظ(بنانشانی adr, ثابت vvp_object_t&قدر);
      مجازی فراغ get_لفظ(بنانشانی adr, vvp_object_t&قدر);

      مجازی vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      مجازی فراغ الٹ_elems(فراغ);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      مجازی فراغ ترتیب_دیو_elems(bool ascending);
      مجازی فراغ shuffle_elems(فراغ);
};

سانچا <class TYPE> class vvp_darray_atom : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_ناپ(فراغ) ثابت override;
      فراغ الٹ_elems(فراغ) override;
      فراغ ترتیب_دیو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ سیٹ_لفظ(بنانشانی adr, ثابت vvp_vector4_t&قدر) override;
      فراغ get_لفظ(بنانشانی adr, vvp_vector4_t&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::سمتیہ<TYPE> array_;
};

class vvp_darray_vec4 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec4(size_t siz, بنانشانی لفظ_wid) :
                             array_(siz), لفظ_wid_2(لفظ_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_ناپ(فراغ) ثابت override;
      فراغ سیٹ_لفظ(بنانشانی adr, ثابت vvp_vector4_t&قدر) override;
      فراغ get_لفظ(بنانشانی adr, vvp_vector4_t&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ الٹ_elems(فراغ) override;
      فراغ ترتیب_دیو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::سمتیہ<vvp_vector4_t> array_;
      بنانشانی لفظ_wid_2;
};

class vvp_darray_vec2 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec2(size_t siz, بنانشانی لفظ_wid) :
                             array_(siz), لفظ_wid_2(لفظ_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_ناپ(فراغ) ثابت override;
      فراغ الٹ_elems(فراغ) override;
      فراغ ترتیب_دیو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ سیٹ_لفظ(بنانشانی adr, ثابت vvp_vector4_t&قدر) override;
      فراغ get_لفظ(بنانشانی adr, vvp_vector4_t&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::سمتیہ<vvp_vector2_t> array_;
      بنانشانی لفظ_wid_2;
};

class vvp_darray_real : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_ناپ(فراغ) ثابت override;
      فراغ سیٹ_لفظ(بنانشانی adr, مزدوج قدر) override;
      فراغ get_لفظ(بنانشانی adr, مزدوج&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ الٹ_elems(فراغ) override;
      فراغ ترتیب_دیو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::سمتیہ<مزدوج> array_;
};

class vvp_darray_string : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_ناپ(فراغ) ثابت override;
      فراغ سیٹ_لفظ(بنانشانی adr, ثابت std::string&قدر) override;
      فراغ get_لفظ(بنانشانی adr, std::string&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      فراغ الٹ_elems(فراغ) override;
      فراغ ترتیب_دیو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::سمتیہ<std::string> array_;
};

class vvp_darray_object : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_ناپ(فراغ) ثابت override;
      فراغ الٹ_elems(فراغ) override;
      فراغ ترتیب_دیو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ سیٹ_لفظ(بنانشانی adr, ثابت vvp_object_t&قدر) override;
      فراغ get_لفظ(بنانشانی adr, vvp_object_t&قدر) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    خاص:
      std::سمتیہ<vvp_object_t> array_;
};

class vvp_قطار : عام vvp_darray {

    عام:
      مضمن vvp_قطار(فراغ) { }
      ~vvp_قطار() override;

      مجازی size_t get_ناپ(فراغ) ثابت override =0;
      مجازی فراغ copy_elems(vvp_object_t src, بنانشانی max_ناپ);

      مجازی فراغ سیٹ_لفظ_max(بنانشانی adr, ثابت vvp_vector4_t&قدر, بنانشانی max_ناپ);
      مجازی فراغ داخل_کرو(بنانشانی idx, ثابت vvp_vector4_t&قدر, بنانشانی max_ناپ);
      مجازی فراغ ڈالو_پٹھ(ثابت vvp_vector4_t&قدر, بنانشانی max_ناپ);
      مجازی فراغ ڈالو_front(ثابت vvp_vector4_t&قدر, بنانشانی max_ناپ);

      مجازی فراغ سیٹ_لفظ_max(بنانشانی adr, مزدوج قدر, بنانشانی max_ناپ);
      مجازی فراغ داخل_کرو(بنانشانی idx, مزدوج قدر, بنانشانی max_ناپ);
      مجازی فراغ ڈالو_پٹھ(مزدوج قدر, بنانشانی max_ناپ);
      مجازی فراغ ڈالو_front(مزدوج قدر, بنانشانی max_ناپ);

      مجازی فراغ سیٹ_لفظ_max(بنانشانی adr, ثابت std::string&قدر, بنانشانی max_ناپ);
      مجازی فراغ داخل_کرو(بنانشانی idx, ثابت std::string&قدر, بنانشانی max_ناپ);
      مجازی فراغ ڈالو_پٹھ(ثابت std::string&قدر, بنانشانی max_ناپ);
      مجازی فراغ ڈالو_front(ثابت std::string&قدر, بنانشانی max_ناپ);

      مجازی فراغ نکالو_پٹھ(فراغ) =0;
      مجازی فراغ نکالو_front(فراغ)=0;
      مجازی فراغ erase(بنانشانی idx)=0;
      مجازی فراغ erase_پوچھ(بنانشانی idx)=0;
};

class vvp_قطار_real : عام vvp_قطار {

    عام:
      size_t get_ناپ(فراغ) ثابت override { أرجع قطار.ناپ(); };
      فراغ copy_elems(vvp_object_t src, بنانشانی max_ناپ) override;
      فراغ سیٹ_لفظ_max(بنانشانی adr, مزدوج قدر, بنانشانی max_ناپ) override;
      فراغ سیٹ_لفظ(بنانشانی adr, مزدوج قدر) override;
      فراغ get_لفظ(بنانشانی adr, مزدوج&قدر) override;
      فراغ داخل_کرو(بنانشانی idx, مزدوج قدر, بنانشانی max_ناپ) override;
      فراغ ڈالو_پٹھ(مزدوج قدر, بنانشانی max_ناپ) override;
      فراغ ڈالو_front(مزدوج قدر, بنانشانی max_ناپ) override;
      فراغ نکالو_پٹھ(فراغ) override { قطار.نکالو_پٹھ(); };
      فراغ نکالو_front(فراغ) override { قطار.نکالو_front(); };
      فراغ erase(بنانشانی idx) override;
      فراغ erase_پوچھ(بنانشانی idx) override;
      فراغ الٹ_elems(فراغ) override;
      فراغ ترتیب_دیو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<مزدوج> قطار;
};

class vvp_قطار_string : عام vvp_قطار {

    عام:
      size_t get_ناپ(فراغ) ثابت override { أرجع قطار.ناپ(); };
      فراغ copy_elems(vvp_object_t src, بنانشانی max_ناپ) override;
      فراغ سیٹ_لفظ_max(بنانشانی adr, ثابت std::string&قدر, بنانشانی max_ناپ) override;
      فراغ سیٹ_لفظ(بنانشانی adr, ثابت std::string&قدر) override;
      فراغ get_لفظ(بنانشانی adr, std::string&قدر) override;
      فراغ داخل_کرو(بنانشانی idx, ثابت std::string&قدر, بنانشانی max_ناپ) override;
      فراغ ڈالو_پٹھ(ثابت std::string&قدر, بنانشانی max_ناپ) override;
      فراغ ڈالو_front(ثابت std::string&قدر, بنانشانی max_ناپ) override;
      فراغ نکالو_پٹھ(فراغ) override { قطار.نکالو_پٹھ(); };
      فراغ نکالو_front(فراغ) override { قطار.نکالو_front(); };
      فراغ erase(بنانشانی idx) override;
      فراغ erase_پوچھ(بنانشانی idx) override;
      فراغ الٹ_elems(فراغ) override;
      فراغ ترتیب_دیو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<std::string> قطار;
};

class vvp_قطار_vec4 : عام vvp_قطار {

    عام:
      size_t get_ناپ(فراغ) ثابت override { أرجع قطار.ناپ(); };
      فراغ copy_elems(vvp_object_t src, بنانشانی max_ناپ) override;
      فراغ سیٹ_لفظ_max(بنانشانی adr, ثابت vvp_vector4_t&قدر, بنانشانی max_ناپ) override;
      فراغ سیٹ_لفظ(بنانشانی adr, ثابت vvp_vector4_t&قدر) override;
      فراغ get_لفظ(بنانشانی adr, vvp_vector4_t&قدر) override;
      فراغ داخل_کرو(بنانشانی idx, ثابت vvp_vector4_t&قدر, بنانشانی max_ناپ) override;
      فراغ ڈالو_پٹھ(ثابت vvp_vector4_t&قدر, بنانشانی max_ناپ) override;
      فراغ ڈالو_front(ثابت vvp_vector4_t&قدر, بنانشانی max_ناپ) override;
      فراغ نکالو_پٹھ(فراغ) override { قطار.نکالو_پٹھ(); };
      فراغ نکالو_front(فراغ) override { قطار.نکالو_front(); };
      فراغ erase(بنانشانی idx) override;
      فراغ erase_پوچھ(بنانشانی idx) override;
      فراغ الٹ_elems(فراغ) override;
      فراغ ترتیب_دیو_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<vvp_vector4_t> قطار;
};

خارجي std::string get_fileline();

#جے_انت /* IVL_vvp_darray_H */
