/*
 * Copyright (c) 2012-2026 Stephen Williams (steve@icarus.com)
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

# include  "class_type.h"
# include  "compile.h"
# include  "vpi_priv.h"
# include  "config.h"
# include  <map>
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <cassert>

استعمال نامخلا std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
جماعت class_property_t {
    عوامی:
      اندرونی class_property_t() { انحراف = 0; }
      مجازی ~class_property_t() =0;
	// How much space does an instance of this property require?
      مجازی size_t instance_سائز() ثابت =0;

      خالی سیٹ_انحراف(size_t off) { انحراف = off; }

    عوامی:
      مجازی خالی construct(حرف*buf) ثابت;
      مجازی خالی destruct(حرف*buf) ثابت;

      مجازی خالی سیٹ_vec4(حرف*buf, ثابت vvp_vector4_t&val);
      مجازی خالی get_vec4(حرف*buf, vvp_vector4_t&val);

      مجازی خالی سیٹ_real(حرف*buf, دوہرا val);
      مجازی دوہرا get_real(حرف*buf);

      مجازی خالی سیٹ_string(حرف*buf, ثابت std::string&val);
      مجازی string get_string(حرف*buf);

      مجازی خالی سیٹ_object(حرف*buf, ثابت vvp_object_t&val, uint64_t element);
      مجازی خالی get_object(حرف*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      مجازی خالی copy(حرف*buf, حرف*src) = 0;

    محفوظ:
      size_t انحراف;
};

class_property_t::~class_property_t()
{
}

خالی class_property_t::construct(حرف*) ثابت
{
}

خالی class_property_t::destruct(حرف*) ثابت
{
}

خالی class_property_t::سیٹ_vec4(حرف*, ثابت vvp_vector4_t&)
{
      أكد(0);
}

خالی class_property_t::get_vec4(حرف*, vvp_vector4_t&)
{
      أكد(0);
}

خالی class_property_t::سیٹ_real(حرف*, دوہرا)
{
      أكد(0);
}

دوہرا class_property_t::get_real(حرف*)
{
      أكد(0);
      واپس 0.0;
}

خالی class_property_t::سیٹ_string(حرف*, ثابت string&)
{
      أكد(0);
}

string class_property_t::get_string(حرف*)
{
      أكد(0);
      واپس "";
}

خالی class_property_t::سیٹ_object(حرف*, ثابت vvp_object_t&, uint64_t)
{
      أكد(0);
}

خالی class_property_t::get_object(حرف*, vvp_object_t&, uint64_t)
{
      أكد(0);
}

/*
 */
سانچہ <جماعت T> جماعت property_atom : عوامی class_property_t {
    عوامی:
      اندرونی صریح property_atom(خالی) { }
      ~property_atom() override { }

      size_t instance_سائز() ثابت override { واپس حجم(T); }

    عوامی:
      خالی construct(حرف*buf) ثابت override
      { T*tmp = reinterpret_cast<T*> (buf+انحراف);
	*tmp = 0;
      }

      خالی سیٹ_vec4(حرف*buf, ثابت vvp_vector4_t&val) override;
      خالی get_vec4(حرف*buf, vvp_vector4_t&val) override;

      خالی copy(حرف*dst, حرف*src) override;
};

جماعت property_ثنائی_ہندسہ : عوامی class_property_t {
    عوامی:
      صریح اندرونی property_ثنائی_ہندسہ(size_t wid): wid_(wid) { }
      ~property_ثنائی_ہندسہ() override { }

      size_t instance_سائز() ثابت override { واپس حجم(vvp_vector2_t); }

    عوامی:
      خالی construct(حرف*buf) ثابت override
      { نیا (buf+انحراف) vvp_vector2_t (0, wid_); }

      خالی destruct(حرف*buf) ثابت override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+انحراف);
	tmp->~vvp_vector2_t();
      }

      خالی سیٹ_vec4(حرف*buf, ثابت vvp_vector4_t&val) override;
      خالی get_vec4(حرف*buf, vvp_vector4_t&val) override;

      خالی copy(حرف*dst, حرف*src) override;

    نجی:
      size_t wid_;
};

جماعت property_logic : عوامی class_property_t {
    عوامی:
      صریح اندرونی property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_سائز() ثابت override { واپس حجم(vvp_vector4_t); }

    عوامی:
      خالی construct(حرف*buf) ثابت override
      { نیا (buf+انحراف) vvp_vector4_t (wid_); }

      خالی destruct(حرف*buf) ثابت override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+انحراف);
	tmp->~vvp_vector4_t();
      }

      خالی سیٹ_vec4(حرف*buf, ثابت vvp_vector4_t&val) override;
      خالی get_vec4(حرف*buf, vvp_vector4_t&val) override;

      خالی copy(حرف*dst, حرف*src) override;

    نجی:
      size_t wid_;
};

سانچہ <جماعت T> جماعت property_real : عوامی class_property_t {
    عوامی:
      اندرونی صریح property_real(خالی) { }
      ~property_real() override { }

      size_t instance_سائز() ثابت override { واپس حجم(T); }

    عوامی:
      خالی construct(حرف*buf) ثابت override
      { T*tmp = reinterpret_cast<T*> (buf+انحراف);
	*tmp = 0.0;
      }

      خالی سیٹ_real(حرف*buf, دوہرا val) override;
      دوہرا get_real(حرف*buf) override;

      خالی copy(حرف*dst, حرف*src) override;
};

جماعت property_string : عوامی class_property_t {
    عوامی:
      اندرونی صریح property_string(خالی) { }
      ~property_string() override { }

      size_t instance_سائز() ثابت override { واپس حجم(std::string); }

    عوامی:
      خالی construct(حرف*buf) ثابت override
      { /* string*tmp = */ نیا (buf+انحراف) string; }

      خالی destruct(حرف*buf) ثابت override
      { string*tmp = reinterpret_cast<string*> (buf+انحراف);
	tmp->~string();
      }

      خالی سیٹ_string(حرف*buf, ثابت string&) override;
      string get_string(حرف*buf) override;

      خالی copy(حرف*dst, حرف*src) override;
};

جماعت property_object : عوامی class_property_t {
    عوامی:
      اندرونی صریح property_object(uint64_t as): array_سائز(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_سائز() ثابت override { واپس array_سائز * حجم(vvp_object_t); }

    عوامی:
      خالی construct(حرف*buf) ثابت override;

      خالی destruct(حرف*buf) ثابت override;

      خالی سیٹ_object(حرف*buf, ثابت vvp_object_t&, uint64_t) override;
      خالی get_object(حرف*buf, vvp_object_t&, uint64_t) override;

      خالی copy(حرف*dst, حرف*src) override;

    نجی:
      size_t array_سائز;
};

سانچہ <جماعت T> خالی property_atom<T>::سیٹ_vec4(حرف*buf, ثابت vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+انحراف);
      منطقی flag = vector4_مقصد_قدر(val, *tmp, سچ, جھوٹ);
      أكد(flag);
}

سانچہ <جماعت T> خالی property_atom<T>::get_vec4(حرف*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+انحراف);
      ثابت size_t tmp_cnt = حجم(T)<حجم(غیرعلامتی طویل)
				       ? 1
				       : حجم(T) / حجم(غیرعلامتی طویل);
      غیرعلامتی طویل tmp[tmp_cnt];
      tmp[0] = src[0];

      برائے (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * حجم(tmp[0]);

      val.resize(8*حجم(T));
      val.setarray(0, val.size(), tmp);
}

سانچہ <جماعت T> خالی property_atom<T>::copy(حرف*dst, حرف*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+انحراف);
      T*src_obj = reinterpret_cast<T*> (src+انحراف);
      *dst_obj = *src_obj;
}

خالی property_ثنائی_ہندسہ::سیٹ_vec4(حرف*buf, ثابت vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+انحراف);
      *obj = val;
}

خالی property_ثنائی_ہندسہ::get_vec4(حرف*buf, vvp_vector4_t&val)
{
      ثابت vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+انحراف);
      val = vector2_مقصد_vector4(*obj, obj->size());
}

خالی property_ثنائی_ہندسہ::copy(حرف*dst, حرف*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+انحراف);
      ثابت vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+انحراف);
      *dst_obj = *src_obj;
}

خالی property_logic::سیٹ_vec4(حرف*buf, ثابت vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+انحراف);
      *obj = val;
}

خالی property_logic::get_vec4(حرف*buf, vvp_vector4_t&val)
{
      ثابت vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+انحراف);
      val = *obj;
}

خالی property_logic::copy(حرف*dst, حرف*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+انحراف);
      ثابت vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+انحراف);
      *dst_obj = *src_obj;
}

سانچہ <جماعت T> خالی property_real<T>::سیٹ_real(حرف*buf, دوہرا val)
{
      T*tmp = reinterpret_cast<T*>(buf+انحراف);
      *tmp = val;
}

سانچہ <جماعت T> دوہرا property_real<T>::get_real(حرف*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+انحراف);
      واپس *tmp;
}

سانچہ <جماعت T> خالی property_real<T>::copy(حرف*dst, حرف*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+انحراف);
      T*src_obj = reinterpret_cast<T*> (src+انحراف);
      *dst_obj = *src_obj;
}

خالی property_string::سیٹ_string(حرف*buf, ثابت string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+انحراف);
      *tmp = val;
}

string property_string::get_string(حرف*buf)
{
      ثابت string*tmp = reinterpret_cast<string*>(buf+انحراف);
      واپس *tmp;
}

خالی property_string::copy(حرف*dst, حرف*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+انحراف);
      ثابت string*src_obj = reinterpret_cast<string*> (src+انحراف);
      *dst_obj = *src_obj;
}

خالی property_object::construct(حرف*buf) ثابت
{
      برائے (size_t idx = 0 ; idx < array_سائز ; idx += 1)
	    نیا (buf+انحراف + idx*حجم(vvp_object_t)) vvp_object_t;
}

خالی property_object::destruct(حرف*buf) ثابت
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+انحراف);
      برائے (size_t idx = 0 ; idx < array_سائز ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

خالی property_object::سیٹ_object(حرف*buf, ثابت vvp_object_t&val, uint64_t idx)
{
      أكد(idx < array_سائز);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+انحراف);
      tmp[idx] = val;
}

خالی property_object::get_object(حرف*buf, vvp_object_t&val, uint64_t idx)
{
      أكد(idx < array_سائز);
      ثابت vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+انحراف);
      val = tmp[idx];
}

خالی property_object::copy(حرف*dst, حرف*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+انحراف);
      ثابت vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+انحراف);
      برائے (size_t idx = 0 ; idx < array_سائز ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ثابت string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_سائز_2 = 0;
}

class_type::~class_type()
{
      برائے (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    حذف properties_[idx].type;
}

خالی class_type::سیٹ_property(size_t idx, ثابت string&name, ثابت string&type, uint64_t array_سائز_2)
{
      أكد(idx < properties_.size());
      properties_[idx].name = name;

      اگر (type == "b8")
	    properties_[idx].type = نیا property_atom<uint8_t>;
      ورنہ اگر (type == "b16")
	    properties_[idx].type = نیا property_atom<uint16_t>;
      ورنہ اگر (type == "b32")
	    properties_[idx].type = نیا property_atom<uint32_t>;
      ورنہ اگر (type == "b64")
	    properties_[idx].type = نیا property_atom<uint64_t>;
      ورنہ اگر (type == "sb8")
	    properties_[idx].type = نیا property_atom<int8_t>;
      ورنہ اگر (type == "sb16")
	    properties_[idx].type = نیا property_atom<int16_t>;
      ورنہ اگر (type == "sb32")
	    properties_[idx].type = نیا property_atom<int32_t>;
      ورنہ اگر (type == "sb64")
	    properties_[idx].type = نیا property_atom<int64_t>;
      ورنہ اگر (type == "r")
	    properties_[idx].type = نیا property_real<دوہرا>;
      ورنہ اگر (type == "S")
	    properties_[idx].type = نیا property_string;
      ورنہ اگر (type == "o")
	    properties_[idx].type = نیا property_object(array_سائز_2);
      ورنہ اگر (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = نیا property_ثنائی_ہندسہ(wid);
      } ورنہ اگر (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = نیا property_logic(wid);
      } ورنہ اگر (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = نیا property_logic(wid);
      } ورنہ {
	    properties_[idx].type = 0;
      }
}

خالی class_type::اختتام_تیاری(خالی)
{
      map<size_t, vector<size_t> > سائز_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      برائے (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    أكد(properties_[idx].type);
	    size_t instance_سائز = properties_[idx].type->instance_سائز();
	    accum += instance_سائز;
	    سائز_map[instance_سائز].push_back(idx);
      }

      instance_سائز_2 = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      برائے (map<size_t, vector<size_t> >::reverse_iterator cur = سائز_map.rbegin()
		 ; cur != سائز_map.rend() ; ++ cur) {
	    برائے (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  أكد(ptype->instance_سائز() == cur->پہلا);
		  ptype->سیٹ_انحراف(accum);
		  accum += cur->پہلا;
	    }
      }
}

class_type::inst_t class_type::instance_new() ثابت
{
      حرف*buf = نیا حرف [instance_سائز_2];

      برائے (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      واپس reinterpret_cast<inst_t> (buf);
}

خالی class_type::instance_delete(class_type::inst_t obj) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);

      برائے (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      حذف[]buf;
}

خالی class_type::سیٹ_vec4(class_type::inst_t obj, size_t pid,
			  ثابت vvp_vector4_t&val) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->سیٹ_vec4(buf, val);
}

خالی class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

خالی class_type::سیٹ_real(class_type::inst_t obj, size_t pid,
			  دوہرا val) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->سیٹ_real(buf, val);
}

دوہرا class_type::get_real(class_type::inst_t obj, size_t pid) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      واپس properties_[pid].type->get_real(buf);
}

خالی class_type::سیٹ_string(class_type::inst_t obj, size_t pid,
			    ثابت string&val) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->سیٹ_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      واپس properties_[pid].type->get_string(buf);
}

خالی class_type::سیٹ_object(class_type::inst_t obj, size_t pid,
			    ثابت vvp_object_t&val, size_t idx) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->سیٹ_object(buf, val, idx);
}

خالی class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

خالی class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ثابت
{
      حرف*dst_buf = reinterpret_cast<حرف*> (dst);
      حرف*src_buf = reinterpret_cast<حرف*> (src);

      أكد(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

صحیح class_type::get_type_رمز(خالی) ثابت
{
      واپس vpiClassDefn;
}

ساکن class_type*compile_class = 0;

خالی compile_class_آغاز(حرف*lab, حرف*nam, غیرعلامتی ntype)
{
      أكد(compile_class == 0);
      compile_class = نیا class_type(nam, ntype);
      compile_vpi_علامت(lab, compile_class);
      آزاد(lab);
      حذف[]nam;
}

خالی compile_class_property(غیرعلامتی idx, حرف*nam, حرف*typ, uint64_t array_سائز_2)
{
      أكد(compile_class);
      compile_class->سیٹ_property(idx, nam, typ, array_سائز_2);
      حذف[]nam;
      حذف[]typ;
}

خالی compile_class_مکمل(خالی)
{
      __vpiScope*scope = vpip_peek_current_scope();
      أكد(scope);
      أكد(compile_class);
      compile_class->اختتام_تیاری();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی class_def_delete(class_type *شے)
{
      حذف شے;
}
#اگر_ختم
