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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#د_که_پای
# include  <cassert>

کارول نطاق std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
صنف class_property_t {
    عام:
      مضمن class_property_t() { دننه_واټن = 0; }
      مجازي ~class_property_t() =0;
	// How much space does an instance of this property require?
      مجازي size_t instance_اندازه() ثابت =0;

      فراغ سېټ_دننه_واټن(size_t off) { دننه_واټن = off; }

    عام:
      مجازي فراغ construct(حرف*buf) ثابت;
      مجازي فراغ destruct(حرف*buf) ثابت;

      مجازي فراغ سېټ_vec4(حرف*buf, ثابت vvp_vector4_t&val);
      مجازي فراغ get_vec4(حرف*buf, vvp_vector4_t&val);

      مجازي فراغ سېټ_real(حرف*buf, مزدوج val);
      مجازي مزدوج get_real(حرف*buf);

      مجازي فراغ سېټ_string(حرف*buf, ثابت std::string&val);
      مجازي string get_string(حرف*buf);

      مجازي فراغ سېټ_object(حرف*buf, ثابت vvp_object_t&val, uint64_t element);
      مجازي فراغ get_object(حرف*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      مجازي فراغ copy(حرف*buf, حرف*src) = 0;

    محمي:
      size_t دننه_واټن;
};

class_property_t::~class_property_t()
{
}

فراغ class_property_t::construct(حرف*) ثابت
{
}

فراغ class_property_t::destruct(حرف*) ثابت
{
}

فراغ class_property_t::سېټ_vec4(حرف*, ثابت vvp_vector4_t&)
{
      أكد(0);
}

فراغ class_property_t::get_vec4(حرف*, vvp_vector4_t&)
{
      أكد(0);
}

فراغ class_property_t::سېټ_real(حرف*, مزدوج)
{
      أكد(0);
}

مزدوج class_property_t::get_real(حرف*)
{
      أكد(0);
      أرجع 0.0;
}

فراغ class_property_t::سېټ_string(حرف*, ثابت string&)
{
      أكد(0);
}

string class_property_t::get_string(حرف*)
{
      أكد(0);
      أرجع "";
}

فراغ class_property_t::سېټ_object(حرف*, ثابت vvp_object_t&, uint64_t)
{
      أكد(0);
}

فراغ class_property_t::get_object(حرف*, vvp_object_t&, uint64_t)
{
      أكد(0);
}

/*
 */
کينډۍ <صنف T> صنف property_atom : عام class_property_t {
    عام:
      مضمن څرګند property_atom(فراغ) { }
      ~property_atom() override { }

      size_t instance_اندازه() ثابت override { أرجع حجم(T); }

    عام:
      فراغ construct(حرف*buf) ثابت override
      { T*tmp = reinterpret_cast<T*> (buf+دننه_واټن);
	*tmp = 0;
      }

      فراغ سېټ_vec4(حرف*buf, ثابت vvp_vector4_t&val) override;
      فراغ get_vec4(حرف*buf, vvp_vector4_t&val) override;

      فراغ copy(حرف*dst, حرف*src) override;
};

صنف property_بټ : عام class_property_t {
    عام:
      څرګند مضمن property_بټ(size_t wid): wid_(wid) { }
      ~property_بټ() override { }

      size_t instance_اندازه() ثابت override { أرجع حجم(vvp_vector2_t); }

    عام:
      فراغ construct(حرف*buf) ثابت override
      { جديد (buf+دننه_واټن) vvp_vector2_t (0, wid_); }

      فراغ destruct(حرف*buf) ثابت override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+دننه_واټن);
	tmp->~vvp_vector2_t();
      }

      فراغ سېټ_vec4(حرف*buf, ثابت vvp_vector4_t&val) override;
      فراغ get_vec4(حرف*buf, vvp_vector4_t&val) override;

      فراغ copy(حرف*dst, حرف*src) override;

    خاص:
      size_t wid_;
};

صنف property_logic : عام class_property_t {
    عام:
      څرګند مضمن property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_اندازه() ثابت override { أرجع حجم(vvp_vector4_t); }

    عام:
      فراغ construct(حرف*buf) ثابت override
      { جديد (buf+دننه_واټن) vvp_vector4_t (wid_); }

      فراغ destruct(حرف*buf) ثابت override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+دننه_واټن);
	tmp->~vvp_vector4_t();
      }

      فراغ سېټ_vec4(حرف*buf, ثابت vvp_vector4_t&val) override;
      فراغ get_vec4(حرف*buf, vvp_vector4_t&val) override;

      فراغ copy(حرف*dst, حرف*src) override;

    خاص:
      size_t wid_;
};

کينډۍ <صنف T> صنف property_real : عام class_property_t {
    عام:
      مضمن څرګند property_real(فراغ) { }
      ~property_real() override { }

      size_t instance_اندازه() ثابت override { أرجع حجم(T); }

    عام:
      فراغ construct(حرف*buf) ثابت override
      { T*tmp = reinterpret_cast<T*> (buf+دننه_واټن);
	*tmp = 0.0;
      }

      فراغ سېټ_real(حرف*buf, مزدوج val) override;
      مزدوج get_real(حرف*buf) override;

      فراغ copy(حرف*dst, حرف*src) override;
};

صنف property_string : عام class_property_t {
    عام:
      مضمن څرګند property_string(فراغ) { }
      ~property_string() override { }

      size_t instance_اندازه() ثابت override { أرجع حجم(std::string); }

    عام:
      فراغ construct(حرف*buf) ثابت override
      { /* string*tmp = */ جديد (buf+دننه_واټن) string; }

      فراغ destruct(حرف*buf) ثابت override
      { string*tmp = reinterpret_cast<string*> (buf+دننه_واټن);
	tmp->~string();
      }

      فراغ سېټ_string(حرف*buf, ثابت string&) override;
      string get_string(حرف*buf) override;

      فراغ copy(حرف*dst, حرف*src) override;
};

صنف property_object : عام class_property_t {
    عام:
      مضمن څرګند property_object(uint64_t as): array_اندازه(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_اندازه() ثابت override { أرجع array_اندازه * حجم(vvp_object_t); }

    عام:
      فراغ construct(حرف*buf) ثابت override;

      فراغ destruct(حرف*buf) ثابت override;

      فراغ سېټ_object(حرف*buf, ثابت vvp_object_t&, uint64_t) override;
      فراغ get_object(حرف*buf, vvp_object_t&, uint64_t) override;

      فراغ copy(حرف*dst, حرف*src) override;

    خاص:
      size_t array_اندازه;
};

کينډۍ <صنف T> فراغ property_atom<T>::سېټ_vec4(حرف*buf, ثابت vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+دننه_واټن);
      منطقي flag = vector4_منزل_ارزښت(val, *tmp, ریښتیا, خطأ);
      أكد(flag);
}

کينډۍ <صنف T> فراغ property_atom<T>::get_vec4(حرف*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+دننه_واټن);
      ثابت size_t tmp_cnt = حجم(T)<حجم(بینښه اوږد)
				       ? 1
				       : حجم(T) / حجم(بینښه اوږد);
      بینښه اوږد tmp[tmp_cnt];
      tmp[0] = src[0];

      لكل (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * حجم(tmp[0]);

      val.resize(8*حجم(T));
      val.setarray(0, val.size(), tmp);
}

کينډۍ <صنف T> فراغ property_atom<T>::copy(حرف*dst, حرف*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+دننه_واټن);
      T*src_obj = reinterpret_cast<T*> (src+دننه_واټن);
      *dst_obj = *src_obj;
}

فراغ property_بټ::سېټ_vec4(حرف*buf, ثابت vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+دننه_واټن);
      *obj = val;
}

فراغ property_بټ::get_vec4(حرف*buf, vvp_vector4_t&val)
{
      ثابت vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+دننه_واټن);
      val = vector2_منزل_vector4(*obj, obj->size());
}

فراغ property_بټ::copy(حرف*dst, حرف*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+دننه_واټن);
      ثابت vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+دننه_واټن);
      *dst_obj = *src_obj;
}

فراغ property_logic::سېټ_vec4(حرف*buf, ثابت vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+دننه_واټن);
      *obj = val;
}

فراغ property_logic::get_vec4(حرف*buf, vvp_vector4_t&val)
{
      ثابت vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+دننه_واټن);
      val = *obj;
}

فراغ property_logic::copy(حرف*dst, حرف*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+دننه_واټن);
      ثابت vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+دننه_واټن);
      *dst_obj = *src_obj;
}

کينډۍ <صنف T> فراغ property_real<T>::سېټ_real(حرف*buf, مزدوج val)
{
      T*tmp = reinterpret_cast<T*>(buf+دننه_واټن);
      *tmp = val;
}

کينډۍ <صنف T> مزدوج property_real<T>::get_real(حرف*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+دننه_واټن);
      أرجع *tmp;
}

کينډۍ <صنف T> فراغ property_real<T>::copy(حرف*dst, حرف*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+دننه_واټن);
      T*src_obj = reinterpret_cast<T*> (src+دننه_واټن);
      *dst_obj = *src_obj;
}

فراغ property_string::سېټ_string(حرف*buf, ثابت string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+دننه_واټن);
      *tmp = val;
}

string property_string::get_string(حرف*buf)
{
      ثابت string*tmp = reinterpret_cast<string*>(buf+دننه_واټن);
      أرجع *tmp;
}

فراغ property_string::copy(حرف*dst, حرف*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+دننه_واټن);
      ثابت string*src_obj = reinterpret_cast<string*> (src+دننه_واټن);
      *dst_obj = *src_obj;
}

فراغ property_object::construct(حرف*buf) ثابت
{
      لكل (size_t idx = 0 ; idx < array_اندازه ; idx += 1)
	    جديد (buf+دننه_واټن + idx*حجم(vvp_object_t)) vvp_object_t;
}

فراغ property_object::destruct(حرف*buf) ثابت
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+دننه_واټن);
      لكل (size_t idx = 0 ; idx < array_اندازه ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

فراغ property_object::سېټ_object(حرف*buf, ثابت vvp_object_t&val, uint64_t idx)
{
      أكد(idx < array_اندازه);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+دننه_واټن);
      tmp[idx] = val;
}

فراغ property_object::get_object(حرف*buf, vvp_object_t&val, uint64_t idx)
{
      أكد(idx < array_اندازه);
      ثابت vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+دننه_واټن);
      val = tmp[idx];
}

فراغ property_object::copy(حرف*dst, حرف*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+دننه_واټن);
      ثابت vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+دننه_واټن);
      لكل (size_t idx = 0 ; idx < array_اندازه ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ثابت string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_اندازه_2 = 0;
}

class_type::~class_type()
{
      لكل (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    ړنګول properties_[idx].type;
}

فراغ class_type::سېټ_property(size_t idx, ثابت string&name, ثابت string&type, uint64_t array_اندازه_2)
{
      أكد(idx < properties_.size());
      properties_[idx].name = name;

      إذا (type == "b8")
	    properties_[idx].type = جديد property_atom<uint8_t>;
      وإلا إذا (type == "b16")
	    properties_[idx].type = جديد property_atom<uint16_t>;
      وإلا إذا (type == "b32")
	    properties_[idx].type = جديد property_atom<uint32_t>;
      وإلا إذا (type == "b64")
	    properties_[idx].type = جديد property_atom<uint64_t>;
      وإلا إذا (type == "sb8")
	    properties_[idx].type = جديد property_atom<int8_t>;
      وإلا إذا (type == "sb16")
	    properties_[idx].type = جديد property_atom<int16_t>;
      وإلا إذا (type == "sb32")
	    properties_[idx].type = جديد property_atom<int32_t>;
      وإلا إذا (type == "sb64")
	    properties_[idx].type = جديد property_atom<int64_t>;
      وإلا إذا (type == "r")
	    properties_[idx].type = جديد property_real<مزدوج>;
      وإلا إذا (type == "S")
	    properties_[idx].type = جديد property_string;
      وإلا إذا (type == "o")
	    properties_[idx].type = جديد property_object(array_اندازه_2);
      وإلا إذا (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = جديد property_بټ(wid);
      } وإلا إذا (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = جديد property_logic(wid);
      } وإلا إذا (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = جديد property_logic(wid);
      } وإلا {
	    properties_[idx].type = 0;
      }
}

فراغ class_type::پای_تیاری(فراغ)
{
      map<size_t, vector<size_t> > اندازه_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      لكل (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    أكد(properties_[idx].type);
	    size_t instance_اندازه = properties_[idx].type->instance_اندازه();
	    accum += instance_اندازه;
	    اندازه_map[instance_اندازه].push_back(idx);
      }

      instance_اندازه_2 = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      لكل (map<size_t, vector<size_t> >::reverse_iterator cur = اندازه_map.rbegin()
		 ; cur != اندازه_map.rend() ; ++ cur) {
	    لكل (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  أكد(ptype->instance_اندازه() == cur->لومړی);
		  ptype->سېټ_دننه_واټن(accum);
		  accum += cur->لومړی;
	    }
      }
}

class_type::inst_t class_type::instance_new() ثابت
{
      حرف*buf = جديد حرف [instance_اندازه_2];

      لكل (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      أرجع reinterpret_cast<inst_t> (buf);
}

فراغ class_type::instance_delete(class_type::inst_t obj) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);

      لكل (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      ړنګول[]buf;
}

فراغ class_type::سېټ_vec4(class_type::inst_t obj, size_t pid,
			  ثابت vvp_vector4_t&val) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->سېټ_vec4(buf, val);
}

فراغ class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

فراغ class_type::سېټ_real(class_type::inst_t obj, size_t pid,
			  مزدوج val) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->سېټ_real(buf, val);
}

مزدوج class_type::get_real(class_type::inst_t obj, size_t pid) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      أرجع properties_[pid].type->get_real(buf);
}

فراغ class_type::سېټ_string(class_type::inst_t obj, size_t pid,
			    ثابت string&val) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->سېټ_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      أرجع properties_[pid].type->get_string(buf);
}

فراغ class_type::سېټ_object(class_type::inst_t obj, size_t pid,
			    ثابت vvp_object_t&val, size_t idx) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->سېټ_object(buf, val, idx);
}

فراغ class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ثابت
{
      حرف*buf = reinterpret_cast<حرف*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

فراغ class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ثابت
{
      حرف*dst_buf = reinterpret_cast<حرف*> (dst);
      حرف*src_buf = reinterpret_cast<حرف*> (src);

      أكد(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

صحيح class_type::get_type_کوډ(فراغ) ثابت
{
      أرجع vpiClassDefn;
}

ساكن class_type*compile_class = 0;

فراغ compile_class_پیل(حرف*lab, حرف*nam, بینښه ntype)
{
      أكد(compile_class == 0);
      compile_class = جديد class_type(nam, ntype);
      compile_vpi_نښه(lab, compile_class);
      حرر(lab);
      ړنګول[]nam;
}

فراغ compile_class_property(بینښه idx, حرف*nam, حرف*typ, uint64_t array_اندازه_2)
{
      أكد(compile_class);
      compile_class->سېټ_property(idx, nam, typ, array_اندازه_2);
      ړنګول[]nam;
      ړنګول[]typ;
}

فراغ compile_class_بشپړ(فراغ)
{
      __vpiScope*scope = vpip_peek_current_scope();
      أكد(scope);
      أكد(compile_class);
      compile_class->پای_تیاری();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ class_def_delete(class_type *توکی)
{
      ړنګول توکی;
}
#د_که_پای
