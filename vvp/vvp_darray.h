#اگر_تعریف_نشده IVL_vvp_darray_H
#تعریف_کن IVL_vvp_darray_H
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

      مجازی size_t get_اندازه(فراغ) ثابت =0;

      مجازی فراغ مجموعه_کلمه(بیعلامت adr, ثابت vvp_vector4_t&مقدار);
      مجازی فراغ get_کلمه(بیعلامت adr, vvp_vector4_t&مقدار);

      مجازی فراغ مجموعه_کلمه(بیعلامت adr, مزدوج مقدار);
      مجازی فراغ get_کلمه(بیعلامت adr, مزدوج&مقدار);

      مجازی فراغ مجموعه_کلمه(بیعلامت adr, ثابت std::string&مقدار);
      مجازی فراغ get_کلمه(بیعلامت adr, std::string&مقدار);

      مجازی فراغ مجموعه_کلمه(بیعلامت adr, ثابت vvp_object_t&مقدار);
      مجازی فراغ get_کلمه(بیعلامت adr, vvp_object_t&مقدار);

      مجازی vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      مجازی فراغ معکوس_elems(فراغ);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      مجازی فراغ مرتب_سازی_elems(bool ascending);
      مجازی فراغ shuffle_elems(فراغ);
};

قالب <class TYPE> class vvp_darray_atom : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ معکوس_elems(فراغ) override;
      فراغ مرتب_سازی_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ مجموعه_کلمه(بیعلامت adr, ثابت vvp_vector4_t&مقدار) override;
      فراغ get_کلمه(بیعلامت adr, vvp_vector4_t&مقدار) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::وکتور<TYPE> array_;
};

class vvp_darray_vec4 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec4(size_t siz, بیعلامت کلمه_wid) :
                             array_(siz), کلمه_wid_2(کلمه_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ مجموعه_کلمه(بیعلامت adr, ثابت vvp_vector4_t&مقدار) override;
      فراغ get_کلمه(بیعلامت adr, vvp_vector4_t&مقدار) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ معکوس_elems(فراغ) override;
      فراغ مرتب_سازی_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::وکتور<vvp_vector4_t> array_;
      بیعلامت کلمه_wid_2;
};

class vvp_darray_vec2 : عام vvp_darray {

    عام:
      مضمن vvp_darray_vec2(size_t siz, بیعلامت کلمه_wid) :
                             array_(siz), کلمه_wid_2(کلمه_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ معکوس_elems(فراغ) override;
      فراغ مرتب_سازی_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ مجموعه_کلمه(بیعلامت adr, ثابت vvp_vector4_t&مقدار) override;
      فراغ get_کلمه(بیعلامت adr, vvp_vector4_t&مقدار) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    خاص:
      std::وکتور<vvp_vector2_t> array_;
      بیعلامت کلمه_wid_2;
};

class vvp_darray_real : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ مجموعه_کلمه(بیعلامت adr, مزدوج مقدار) override;
      فراغ get_کلمه(بیعلامت adr, مزدوج&مقدار) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      فراغ معکوس_elems(فراغ) override;
      فراغ مرتب_سازی_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::وکتور<مزدوج> array_;
};

class vvp_darray_string : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ مجموعه_کلمه(بیعلامت adr, ثابت std::string&مقدار) override;
      فراغ get_کلمه(بیعلامت adr, std::string&مقدار) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(فراغ) ثابت override;
      فراغ معکوس_elems(فراغ) override;
      فراغ مرتب_سازی_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::وکتور<std::string> array_;
};

class vvp_darray_object : عام vvp_darray {

    عام:
      explicit مضمن vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_اندازه(فراغ) ثابت override;
      فراغ معکوس_elems(فراغ) override;
      فراغ مرتب_سازی_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;
      فراغ مجموعه_کلمه(بیعلامت adr, ثابت vvp_object_t&مقدار) override;
      فراغ get_کلمه(بیعلامت adr, vvp_object_t&مقدار) override;
      فراغ shallow_copy(ثابت vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    خاص:
      std::وکتور<vvp_object_t> array_;
};

class vvp_صف : عام vvp_darray {

    عام:
      مضمن vvp_صف(فراغ) { }
      ~vvp_صف() override;

      مجازی size_t get_اندازه(فراغ) ثابت override =0;
      مجازی فراغ copy_elems(vvp_object_t src, بیعلامت max_اندازه);

      مجازی فراغ مجموعه_کلمه_max(بیعلامت adr, ثابت vvp_vector4_t&مقدار, بیعلامت max_اندازه);
      مجازی فراغ درج(بیعلامت idx, ثابت vvp_vector4_t&مقدار, بیعلامت max_اندازه);
      مجازی فراغ افزودن_پشت(ثابت vvp_vector4_t&مقدار, بیعلامت max_اندازه);
      مجازی فراغ افزودن_front(ثابت vvp_vector4_t&مقدار, بیعلامت max_اندازه);

      مجازی فراغ مجموعه_کلمه_max(بیعلامت adr, مزدوج مقدار, بیعلامت max_اندازه);
      مجازی فراغ درج(بیعلامت idx, مزدوج مقدار, بیعلامت max_اندازه);
      مجازی فراغ افزودن_پشت(مزدوج مقدار, بیعلامت max_اندازه);
      مجازی فراغ افزودن_front(مزدوج مقدار, بیعلامت max_اندازه);

      مجازی فراغ مجموعه_کلمه_max(بیعلامت adr, ثابت std::string&مقدار, بیعلامت max_اندازه);
      مجازی فراغ درج(بیعلامت idx, ثابت std::string&مقدار, بیعلامت max_اندازه);
      مجازی فراغ افزودن_پشت(ثابت std::string&مقدار, بیعلامت max_اندازه);
      مجازی فراغ افزودن_front(ثابت std::string&مقدار, بیعلامت max_اندازه);

      مجازی فراغ برداشت_پشت(فراغ) =0;
      مجازی فراغ برداشت_front(فراغ)=0;
      مجازی فراغ erase(بیعلامت idx)=0;
      مجازی فراغ erase_دم(بیعلامت idx)=0;
};

class vvp_صف_real : عام vvp_صف {

    عام:
      size_t get_اندازه(فراغ) ثابت override { أرجع صف.اندازه(); };
      فراغ copy_elems(vvp_object_t src, بیعلامت max_اندازه) override;
      فراغ مجموعه_کلمه_max(بیعلامت adr, مزدوج مقدار, بیعلامت max_اندازه) override;
      فراغ مجموعه_کلمه(بیعلامت adr, مزدوج مقدار) override;
      فراغ get_کلمه(بیعلامت adr, مزدوج&مقدار) override;
      فراغ درج(بیعلامت idx, مزدوج مقدار, بیعلامت max_اندازه) override;
      فراغ افزودن_پشت(مزدوج مقدار, بیعلامت max_اندازه) override;
      فراغ افزودن_front(مزدوج مقدار, بیعلامت max_اندازه) override;
      فراغ برداشت_پشت(فراغ) override { صف.برداشت_پشت(); };
      فراغ برداشت_front(فراغ) override { صف.برداشت_front(); };
      فراغ erase(بیعلامت idx) override;
      فراغ erase_دم(بیعلامت idx) override;
      فراغ معکوس_elems(فراغ) override;
      فراغ مرتب_سازی_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<مزدوج> صف;
};

class vvp_صف_string : عام vvp_صف {

    عام:
      size_t get_اندازه(فراغ) ثابت override { أرجع صف.اندازه(); };
      فراغ copy_elems(vvp_object_t src, بیعلامت max_اندازه) override;
      فراغ مجموعه_کلمه_max(بیعلامت adr, ثابت std::string&مقدار, بیعلامت max_اندازه) override;
      فراغ مجموعه_کلمه(بیعلامت adr, ثابت std::string&مقدار) override;
      فراغ get_کلمه(بیعلامت adr, std::string&مقدار) override;
      فراغ درج(بیعلامت idx, ثابت std::string&مقدار, بیعلامت max_اندازه) override;
      فراغ افزودن_پشت(ثابت std::string&مقدار, بیعلامت max_اندازه) override;
      فراغ افزودن_front(ثابت std::string&مقدار, بیعلامت max_اندازه) override;
      فراغ برداشت_پشت(فراغ) override { صف.برداشت_پشت(); };
      فراغ برداشت_front(فراغ) override { صف.برداشت_front(); };
      فراغ erase(بیعلامت idx) override;
      فراغ erase_دم(بیعلامت idx) override;
      فراغ معکوس_elems(فراغ) override;
      فراغ مرتب_سازی_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<std::string> صف;
};

class vvp_صف_vec4 : عام vvp_صف {

    عام:
      size_t get_اندازه(فراغ) ثابت override { أرجع صف.اندازه(); };
      فراغ copy_elems(vvp_object_t src, بیعلامت max_اندازه) override;
      فراغ مجموعه_کلمه_max(بیعلامت adr, ثابت vvp_vector4_t&مقدار, بیعلامت max_اندازه) override;
      فراغ مجموعه_کلمه(بیعلامت adr, ثابت vvp_vector4_t&مقدار) override;
      فراغ get_کلمه(بیعلامت adr, vvp_vector4_t&مقدار) override;
      فراغ درج(بیعلامت idx, ثابت vvp_vector4_t&مقدار, بیعلامت max_اندازه) override;
      فراغ افزودن_پشت(ثابت vvp_vector4_t&مقدار, بیعلامت max_اندازه) override;
      فراغ افزودن_front(ثابت vvp_vector4_t&مقدار, بیعلامت max_اندازه) override;
      فراغ برداشت_پشت(فراغ) override { صف.برداشت_پشت(); };
      فراغ برداشت_front(فراغ) override { صف.برداشت_front(); };
      فراغ erase(بیعلامت idx) override;
      فراغ erase_دم(بیعلامت idx) override;
      فراغ معکوس_elems(فراغ) override;
      فراغ مرتب_سازی_elems(bool ascending) override;
      فراغ shuffle_elems(فراغ) override;

    خاص:
      std::deque<vvp_vector4_t> صف;
};

خارجي std::string get_fileline();

#پایان_اگر /* IVL_vvp_darray_H */
