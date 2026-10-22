#اگر_غیر_تعریف_شدہ IVL_vvp_darray_H
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

class vvp_darray : عوامی vvp_object {

    عوامی:
      اندرونی vvp_darray() { }
      مجازی ~vvp_darray() override;

      مجازی size_t get_سائز(خالی) ثابت =0;

      مجازی خالی سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_vector4_t&قدر);
      مجازی خالی get_لفظ(غیرعلامتی adr, vvp_vector4_t&قدر);

      مجازی خالی سیٹ_لفظ(غیرعلامتی adr, دوہرا قدر);
      مجازی خالی get_لفظ(غیرعلامتی adr, دوہرا&قدر);

      مجازی خالی سیٹ_لفظ(غیرعلامتی adr, ثابت std::string&قدر);
      مجازی خالی get_لفظ(غیرعلامتی adr, std::string&قدر);

      مجازی خالی سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_object_t&قدر);
      مجازی خالی get_لفظ(غیرعلامتی adr, vvp_object_t&قدر);

      مجازی vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      مجازی خالی الٹ_elems(خالی);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      مجازی خالی ترتیب_دو_elems(bool ascending);
      مجازی خالی shuffle_elems(خالی);
};

سانچہ <class TYPE> class vvp_darray_atom : عوامی vvp_darray {

    عوامی:
      explicit اندرونی vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_سائز(خالی) ثابت override;
      خالی الٹ_elems(خالی) override;
      خالی ترتیب_دو_elems(bool ascending) override;
      خالی shuffle_elems(خالی) override;
      خالی سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_vector4_t&قدر) override;
      خالی get_لفظ(غیرعلامتی adr, vvp_vector4_t&قدر) override;
      خالی shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(خالی) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    نجی:
      std::سمتیہ<TYPE> array_;
};

class vvp_darray_vec4 : عوامی vvp_darray {

    عوامی:
      اندرونی vvp_darray_vec4(size_t siz, غیرعلامتی لفظ_wid) :
                             array_(siz), لفظ_wid_2(لفظ_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_سائز(خالی) ثابت override;
      خالی سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_vector4_t&قدر) override;
      خالی get_لفظ(غیرعلامتی adr, vvp_vector4_t&قدر) override;
      خالی shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(خالی) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      خالی الٹ_elems(خالی) override;
      خالی ترتیب_دو_elems(bool ascending) override;
      خالی shuffle_elems(خالی) override;

    نجی:
      std::سمتیہ<vvp_vector4_t> array_;
      غیرعلامتی لفظ_wid_2;
};

class vvp_darray_vec2 : عوامی vvp_darray {

    عوامی:
      اندرونی vvp_darray_vec2(size_t siz, غیرعلامتی لفظ_wid) :
                             array_(siz), لفظ_wid_2(لفظ_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_سائز(خالی) ثابت override;
      خالی الٹ_elems(خالی) override;
      خالی ترتیب_دو_elems(bool ascending) override;
      خالی shuffle_elems(خالی) override;
      خالی سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_vector4_t&قدر) override;
      خالی get_لفظ(غیرعلامتی adr, vvp_vector4_t&قدر) override;
      خالی shallow_copy(ثابت vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    نجی:
      std::سمتیہ<vvp_vector2_t> array_;
      غیرعلامتی لفظ_wid_2;
};

class vvp_darray_real : عوامی vvp_darray {

    عوامی:
      explicit اندرونی vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_سائز(خالی) ثابت override;
      خالی سیٹ_لفظ(غیرعلامتی adr, دوہرا قدر) override;
      خالی get_لفظ(غیرعلامتی adr, دوہرا&قدر) override;
      خالی shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(خالی) ثابت override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      خالی الٹ_elems(خالی) override;
      خالی ترتیب_دو_elems(bool ascending) override;
      خالی shuffle_elems(خالی) override;

    نجی:
      std::سمتیہ<دوہرا> array_;
};

class vvp_darray_string : عوامی vvp_darray {

    عوامی:
      explicit اندرونی vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_سائز(خالی) ثابت override;
      خالی سیٹ_لفظ(غیرعلامتی adr, ثابت std::string&قدر) override;
      خالی get_لفظ(غیرعلامتی adr, std::string&قدر) override;
      خالی shallow_copy(ثابت vvp_object*obj) override;
      vvp_object* duplicate(خالی) ثابت override;
      خالی الٹ_elems(خالی) override;
      خالی ترتیب_دو_elems(bool ascending) override;
      خالی shuffle_elems(خالی) override;

    نجی:
      std::سمتیہ<std::string> array_;
};

class vvp_darray_object : عوامی vvp_darray {

    عوامی:
      explicit اندرونی vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_سائز(خالی) ثابت override;
      خالی الٹ_elems(خالی) override;
      خالی ترتیب_دو_elems(bool ascending) override;
      خالی shuffle_elems(خالی) override;
      خالی سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_object_t&قدر) override;
      خالی get_لفظ(غیرعلامتی adr, vvp_object_t&قدر) override;
      خالی shallow_copy(ثابت vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    نجی:
      std::سمتیہ<vvp_object_t> array_;
};

class vvp_قطار : عوامی vvp_darray {

    عوامی:
      اندرونی vvp_قطار(خالی) { }
      ~vvp_قطار() override;

      مجازی size_t get_سائز(خالی) ثابت override =0;
      مجازی خالی copy_elems(vvp_object_t src, غیرعلامتی max_سائز);

      مجازی خالی سیٹ_لفظ_max(غیرعلامتی adr, ثابت vvp_vector4_t&قدر, غیرعلامتی max_سائز);
      مجازی خالی داخل_کرو(غیرعلامتی idx, ثابت vvp_vector4_t&قدر, غیرعلامتی max_سائز);
      مجازی خالی ڈالو_عقب(ثابت vvp_vector4_t&قدر, غیرعلامتی max_سائز);
      مجازی خالی ڈالو_front(ثابت vvp_vector4_t&قدر, غیرعلامتی max_سائز);

      مجازی خالی سیٹ_لفظ_max(غیرعلامتی adr, دوہرا قدر, غیرعلامتی max_سائز);
      مجازی خالی داخل_کرو(غیرعلامتی idx, دوہرا قدر, غیرعلامتی max_سائز);
      مجازی خالی ڈالو_عقب(دوہرا قدر, غیرعلامتی max_سائز);
      مجازی خالی ڈالو_front(دوہرا قدر, غیرعلامتی max_سائز);

      مجازی خالی سیٹ_لفظ_max(غیرعلامتی adr, ثابت std::string&قدر, غیرعلامتی max_سائز);
      مجازی خالی داخل_کرو(غیرعلامتی idx, ثابت std::string&قدر, غیرعلامتی max_سائز);
      مجازی خالی ڈالو_عقب(ثابت std::string&قدر, غیرعلامتی max_سائز);
      مجازی خالی ڈالو_front(ثابت std::string&قدر, غیرعلامتی max_سائز);

      مجازی خالی نکالو_عقب(خالی) =0;
      مجازی خالی نکالو_front(خالی)=0;
      مجازی خالی erase(غیرعلامتی idx)=0;
      مجازی خالی erase_دم(غیرعلامتی idx)=0;
};

class vvp_قطار_real : عوامی vvp_قطار {

    عوامی:
      size_t get_سائز(خالی) ثابت override { واپس قطار.سائز(); };
      خالی copy_elems(vvp_object_t src, غیرعلامتی max_سائز) override;
      خالی سیٹ_لفظ_max(غیرعلامتی adr, دوہرا قدر, غیرعلامتی max_سائز) override;
      خالی سیٹ_لفظ(غیرعلامتی adr, دوہرا قدر) override;
      خالی get_لفظ(غیرعلامتی adr, دوہرا&قدر) override;
      خالی داخل_کرو(غیرعلامتی idx, دوہرا قدر, غیرعلامتی max_سائز) override;
      خالی ڈالو_عقب(دوہرا قدر, غیرعلامتی max_سائز) override;
      خالی ڈالو_front(دوہرا قدر, غیرعلامتی max_سائز) override;
      خالی نکالو_عقب(خالی) override { قطار.نکالو_عقب(); };
      خالی نکالو_front(خالی) override { قطار.نکالو_front(); };
      خالی erase(غیرعلامتی idx) override;
      خالی erase_دم(غیرعلامتی idx) override;
      خالی الٹ_elems(خالی) override;
      خالی ترتیب_دو_elems(bool ascending) override;
      خالی shuffle_elems(خالی) override;

    نجی:
      std::deque<دوہرا> قطار;
};

class vvp_قطار_string : عوامی vvp_قطار {

    عوامی:
      size_t get_سائز(خالی) ثابت override { واپس قطار.سائز(); };
      خالی copy_elems(vvp_object_t src, غیرعلامتی max_سائز) override;
      خالی سیٹ_لفظ_max(غیرعلامتی adr, ثابت std::string&قدر, غیرعلامتی max_سائز) override;
      خالی سیٹ_لفظ(غیرعلامتی adr, ثابت std::string&قدر) override;
      خالی get_لفظ(غیرعلامتی adr, std::string&قدر) override;
      خالی داخل_کرو(غیرعلامتی idx, ثابت std::string&قدر, غیرعلامتی max_سائز) override;
      خالی ڈالو_عقب(ثابت std::string&قدر, غیرعلامتی max_سائز) override;
      خالی ڈالو_front(ثابت std::string&قدر, غیرعلامتی max_سائز) override;
      خالی نکالو_عقب(خالی) override { قطار.نکالو_عقب(); };
      خالی نکالو_front(خالی) override { قطار.نکالو_front(); };
      خالی erase(غیرعلامتی idx) override;
      خالی erase_دم(غیرعلامتی idx) override;
      خالی الٹ_elems(خالی) override;
      خالی ترتیب_دو_elems(bool ascending) override;
      خالی shuffle_elems(خالی) override;

    نجی:
      std::deque<std::string> قطار;
};

class vvp_قطار_vec4 : عوامی vvp_قطار {

    عوامی:
      size_t get_سائز(خالی) ثابت override { واپس قطار.سائز(); };
      خالی copy_elems(vvp_object_t src, غیرعلامتی max_سائز) override;
      خالی سیٹ_لفظ_max(غیرعلامتی adr, ثابت vvp_vector4_t&قدر, غیرعلامتی max_سائز) override;
      خالی سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_vector4_t&قدر) override;
      خالی get_لفظ(غیرعلامتی adr, vvp_vector4_t&قدر) override;
      خالی داخل_کرو(غیرعلامتی idx, ثابت vvp_vector4_t&قدر, غیرعلامتی max_سائز) override;
      خالی ڈالو_عقب(ثابت vvp_vector4_t&قدر, غیرعلامتی max_سائز) override;
      خالی ڈالو_front(ثابت vvp_vector4_t&قدر, غیرعلامتی max_سائز) override;
      خالی نکالو_عقب(خالی) override { قطار.نکالو_عقب(); };
      خالی نکالو_front(خالی) override { قطار.نکالو_front(); };
      خالی erase(غیرعلامتی idx) override;
      خالی erase_دم(غیرعلامتی idx) override;
      خالی الٹ_elems(خالی) override;
      خالی ترتیب_دو_elems(bool ascending) override;
      خالی shuffle_elems(خالی) override;

    نجی:
      std::deque<vvp_vector4_t> قطار;
};

خارجی std::string get_fileline();

#اگر_ختم /* IVL_vvp_darray_H */
