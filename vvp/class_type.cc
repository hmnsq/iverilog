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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cassert>

using ନାମାକାଶ std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
ଶ୍ରେଣୀ class_property_t {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ class_property_t() { offset_ = 0; }
      ଆଭାସୀ ~class_property_t() =0;
	// How much space does an instance of this property require?
      ଆଭାସୀ size_t instance_size() ଅଚଳ =0;

      ଶୂନ୍ୟ set_offset(size_t off) { offset_ = off; }

    ସର୍ବସାଧାରଣ:
      ଆଭାସୀ ଶୂନ୍ୟ construct(ଅକ୍ଷର*buf) ଅଚଳ;
      ଆଭାସୀ ଶୂନ୍ୟ destruct(ଅକ୍ଷର*buf) ଅଚଳ;

      ଆଭାସୀ ଶୂନ୍ୟ set_vec4(ଅକ୍ଷର*buf, ଅଚଳ vvp_vector4_t&val);
      ଆଭାସୀ ଶୂନ୍ୟ get_vec4(ଅକ୍ଷର*buf, vvp_vector4_t&val);

      ଆଭାସୀ ଶୂନ୍ୟ set_real(ଅକ୍ଷର*buf, ଦ୍ୱିଗୁଣ val);
      ଆଭାସୀ ଦ୍ୱିଗୁଣ get_real(ଅକ୍ଷର*buf);

      ଆଭାସୀ ଶୂନ୍ୟ set_string(ଅକ୍ଷର*buf, ଅଚଳ std::string&val);
      ଆଭାସୀ string get_string(ଅକ୍ଷର*buf);

      ଆଭାସୀ ଶୂନ୍ୟ set_object(ଅକ୍ଷର*buf, ଅଚଳ vvp_object_t&val, uint64_t element);
      ଆଭାସୀ ଶୂନ୍ୟ get_object(ଅକ୍ଷର*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      ଆଭାସୀ ଶୂନ୍ୟ copy(ଅକ୍ଷର*buf, ଅକ୍ଷର*src) = 0;

    ସୁରକ୍ଷିତ:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

ଶୂନ୍ୟ class_property_t::construct(ଅକ୍ଷର*) ଅଚଳ
{
}

ଶୂନ୍ୟ class_property_t::destruct(ଅକ୍ଷର*) ଅଚଳ
{
}

ଶୂନ୍ୟ class_property_t::set_vec4(ଅକ୍ଷର*, ଅଚଳ vvp_vector4_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ class_property_t::get_vec4(ଅକ୍ଷର*, vvp_vector4_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ class_property_t::set_real(ଅକ୍ଷର*, ଦ୍ୱିଗୁଣ)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଦ୍ୱିଗୁଣ class_property_t::get_real(ଅକ୍ଷର*)
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 0.0;
}

ଶୂନ୍ୟ class_property_t::set_string(ଅକ୍ଷର*, ଅଚଳ string&)
{
      ପ୍ରତିଜ୍ଞା(0);
}

string class_property_t::get_string(ଅକ୍ଷର*)
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ "";
}

ଶୂନ୍ୟ class_property_t::set_object(ଅକ୍ଷର*, ଅଚଳ vvp_object_t&, uint64_t)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ class_property_t::get_object(ଅକ୍ଷର*, vvp_object_t&, uint64_t)
{
      ପ୍ରତିଜ୍ଞା(0);
}

/*
 */
template <ଶ୍ରେଣୀ T> ଶ୍ରେଣୀ property_atom : ସର୍ବସାଧାରଣ class_property_t {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ explicit property_atom(ଶୂନ୍ୟ) { }
      ~property_atom() override { }

      size_t instance_size() ଅଚଳ override { ଫେରନ୍ତୁ ଆକାର(T); }

    ସର୍ବସାଧାରଣ:
      ଶୂନ୍ୟ construct(ଅକ୍ଷର*buf) ଅଚଳ override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      ଶୂନ୍ୟ set_vec4(ଅକ୍ଷର*buf, ଅଚଳ vvp_vector4_t&val) override;
      ଶୂନ୍ୟ get_vec4(ଅକ୍ଷର*buf, vvp_vector4_t&val) override;

      ଶୂନ୍ୟ copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src) override;
};

ଶ୍ରେଣୀ property_bit : ସର୍ବସାଧାରଣ class_property_t {
    ସର୍ବସାଧାରଣ:
      explicit ଧାଡ଼ିଭିତରେ property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_size() ଅଚଳ override { ଫେରନ୍ତୁ ଆକାର(vvp_vector2_t); }

    ସର୍ବସାଧାରଣ:
      ଶୂନ୍ୟ construct(ଅକ୍ଷର*buf) ଅଚଳ override
      { ନୂତନ (buf+offset_) vvp_vector2_t (0, wid_); }

      ଶୂନ୍ୟ destruct(ଅକ୍ଷର*buf) ଅଚଳ override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      ଶୂନ୍ୟ set_vec4(ଅକ୍ଷର*buf, ଅଚଳ vvp_vector4_t&val) override;
      ଶୂନ୍ୟ get_vec4(ଅକ୍ଷର*buf, vvp_vector4_t&val) override;

      ଶୂନ୍ୟ copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src) override;

    ବ୍ୟକ୍ତିଗତ:
      size_t wid_;
};

ଶ୍ରେଣୀ property_logic : ସର୍ବସାଧାରଣ class_property_t {
    ସର୍ବସାଧାରଣ:
      explicit ଧାଡ଼ିଭିତରେ property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_size() ଅଚଳ override { ଫେରନ୍ତୁ ଆକାର(vvp_vector4_t); }

    ସର୍ବସାଧାରଣ:
      ଶୂନ୍ୟ construct(ଅକ୍ଷର*buf) ଅଚଳ override
      { ନୂତନ (buf+offset_) vvp_vector4_t (wid_); }

      ଶୂନ୍ୟ destruct(ଅକ୍ଷର*buf) ଅଚଳ override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      ଶୂନ୍ୟ set_vec4(ଅକ୍ଷର*buf, ଅଚଳ vvp_vector4_t&val) override;
      ଶୂନ୍ୟ get_vec4(ଅକ୍ଷର*buf, vvp_vector4_t&val) override;

      ଶୂନ୍ୟ copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src) override;

    ବ୍ୟକ୍ତିଗତ:
      size_t wid_;
};

template <ଶ୍ରେଣୀ T> ଶ୍ରେଣୀ property_real : ସର୍ବସାଧାରଣ class_property_t {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ explicit property_real(ଶୂନ୍ୟ) { }
      ~property_real() override { }

      size_t instance_size() ଅଚଳ override { ଫେରନ୍ତୁ ଆକାର(T); }

    ସର୍ବସାଧାରଣ:
      ଶୂନ୍ୟ construct(ଅକ୍ଷର*buf) ଅଚଳ override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      ଶୂନ୍ୟ set_real(ଅକ୍ଷର*buf, ଦ୍ୱିଗୁଣ val) override;
      ଦ୍ୱିଗୁଣ get_real(ଅକ୍ଷର*buf) override;

      ଶୂନ୍ୟ copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src) override;
};

ଶ୍ରେଣୀ property_string : ସର୍ବସାଧାରଣ class_property_t {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ explicit property_string(ଶୂନ୍ୟ) { }
      ~property_string() override { }

      size_t instance_size() ଅଚଳ override { ଫେରନ୍ତୁ ଆକାର(std::string); }

    ସର୍ବସାଧାରଣ:
      ଶୂନ୍ୟ construct(ଅକ୍ଷର*buf) ଅଚଳ override
      { /* string*tmp = */ ନୂତନ (buf+offset_) string; }

      ଶୂନ୍ୟ destruct(ଅକ୍ଷର*buf) ଅଚଳ override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      ଶୂନ୍ୟ set_string(ଅକ୍ଷର*buf, ଅଚଳ string&) override;
      string get_string(ଅକ୍ଷର*buf) override;

      ଶୂନ୍ୟ copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src) override;
};

ଶ୍ରେଣୀ property_object : ସର୍ବସାଧାରଣ class_property_t {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ explicit property_object(uint64_t as): array_size_(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_size() ଅଚଳ override { ଫେରନ୍ତୁ array_size_ * ଆକାର(vvp_object_t); }

    ସର୍ବସାଧାରଣ:
      ଶୂନ୍ୟ construct(ଅକ୍ଷର*buf) ଅଚଳ override;

      ଶୂନ୍ୟ destruct(ଅକ୍ଷର*buf) ଅଚଳ override;

      ଶୂନ୍ୟ set_object(ଅକ୍ଷର*buf, ଅଚଳ vvp_object_t&, uint64_t) override;
      ଶୂନ୍ୟ get_object(ଅକ୍ଷର*buf, vvp_object_t&, uint64_t) override;

      ଶୂନ୍ୟ copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src) override;

    ବ୍ୟକ୍ତିଗତ:
      size_t array_size_;
};

template <ଶ୍ରେଣୀ T> ଶୂନ୍ୟ property_atom<T>::set_vec4(ଅକ୍ଷର*buf, ଅଚଳ vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      ତାର୍କିକ flag = vector4_to_ମୂଲ୍ୟ(val, *tmp, ସତ୍ୟ, ମିଥ୍ୟା);
      ପ୍ରତିଜ୍ଞା(flag);
}

template <ଶ୍ରେଣୀ T> ଶୂନ୍ୟ property_atom<T>::get_vec4(ଅକ୍ଷର*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      ଅଚଳ size_t tmp_cnt = ଆକାର(T)<ଆକାର(unsigned long)
				       ? 1
				       : ଆକାର(T) / ଆକାର(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      ପାଇଁ (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * ଆକାର(tmp[0]);

      val.resize(8*ଆକାର(T));
      val.setarray(0, val.size(), tmp);
}

template <ଶ୍ରେଣୀ T> ଶୂନ୍ୟ property_atom<T>::copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

ଶୂନ୍ୟ property_bit::set_vec4(ଅକ୍ଷର*buf, ଅଚଳ vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

ଶୂନ୍ୟ property_bit::get_vec4(ଅକ୍ଷର*buf, vvp_vector4_t&val)
{
      ଅଚଳ vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

ଶୂନ୍ୟ property_bit::copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      ଅଚଳ vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

ଶୂନ୍ୟ property_logic::set_vec4(ଅକ୍ଷର*buf, ଅଚଳ vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

ଶୂନ୍ୟ property_logic::get_vec4(ଅକ୍ଷର*buf, vvp_vector4_t&val)
{
      ଅଚଳ vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

ଶୂନ୍ୟ property_logic::copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      ଅଚଳ vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <ଶ୍ରେଣୀ T> ଶୂନ୍ୟ property_real<T>::set_real(ଅକ୍ଷର*buf, ଦ୍ୱିଗୁଣ val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <ଶ୍ରେଣୀ T> ଦ୍ୱିଗୁଣ property_real<T>::get_real(ଅକ୍ଷର*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      ଫେରନ୍ତୁ *tmp;
}

template <ଶ୍ରେଣୀ T> ଶୂନ୍ୟ property_real<T>::copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

ଶୂନ୍ୟ property_string::set_string(ଅକ୍ଷର*buf, ଅଚଳ string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(ଅକ୍ଷର*buf)
{
      ଅଚଳ string*tmp = reinterpret_cast<string*>(buf+offset_);
      ଫେରନ୍ତୁ *tmp;
}

ଶୂନ୍ୟ property_string::copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      ଅଚଳ string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

ଶୂନ୍ୟ property_object::construct(ଅକ୍ଷର*buf) ଅଚଳ
{
      ପାଇଁ (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    ନୂତନ (buf+offset_ + idx*ଆକାର(vvp_object_t)) vvp_object_t;
}

ଶୂନ୍ୟ property_object::destruct(ଅକ୍ଷର*buf) ଅଚଳ
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      ପାଇଁ (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

ଶୂନ୍ୟ property_object::set_object(ଅକ୍ଷର*buf, ଅଚଳ vvp_object_t&val, uint64_t idx)
{
      ପ୍ରତିଜ୍ଞା(idx < array_size_);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

ଶୂନ୍ୟ property_object::get_object(ଅକ୍ଷର*buf, vvp_object_t&val, uint64_t idx)
{
      ପ୍ରତିଜ୍ଞା(idx < array_size_);
      ଅଚଳ vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

ଶୂନ୍ୟ property_object::copy(ଅକ୍ଷର*dst, ଅକ୍ଷର*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      ଅଚଳ vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      ପାଇଁ (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ଅଚଳ string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_size_ = 0;
}

class_type::~class_type()
{
      ପାଇଁ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

ଶୂନ୍ୟ class_type::set_property(size_t idx, ଅଚଳ string&name, ଅଚଳ string&type, uint64_t array_size)
{
      ପ୍ରତିଜ୍ଞା(idx < properties_.size());
      properties_[idx].name = name;

      ଯଦି (type == "b8")
	    properties_[idx].type = ନୂତନ property_atom<uint8_t>;
      ଅନ୍ୟଥା ଯଦି (type == "b16")
	    properties_[idx].type = ନୂତନ property_atom<uint16_t>;
      ଅନ୍ୟଥା ଯଦି (type == "b32")
	    properties_[idx].type = ନୂତନ property_atom<uint32_t>;
      ଅନ୍ୟଥା ଯଦି (type == "b64")
	    properties_[idx].type = ନୂତନ property_atom<uint64_t>;
      ଅନ୍ୟଥା ଯଦି (type == "sb8")
	    properties_[idx].type = ନୂତନ property_atom<int8_t>;
      ଅନ୍ୟଥା ଯଦି (type == "sb16")
	    properties_[idx].type = ନୂତନ property_atom<int16_t>;
      ଅନ୍ୟଥା ଯଦି (type == "sb32")
	    properties_[idx].type = ନୂତନ property_atom<int32_t>;
      ଅନ୍ୟଥା ଯଦି (type == "sb64")
	    properties_[idx].type = ନୂତନ property_atom<int64_t>;
      ଅନ୍ୟଥା ଯଦି (type == "r")
	    properties_[idx].type = ନୂତନ property_real<ଦ୍ୱିଗୁଣ>;
      ଅନ୍ୟଥା ଯଦି (type == "S")
	    properties_[idx].type = ନୂତନ property_string;
      ଅନ୍ୟଥା ଯଦି (type == "o")
	    properties_[idx].type = ନୂତନ property_object(array_size);
      ଅନ୍ୟଥା ଯଦି (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = ନୂତନ property_bit(wid);
      } ଅନ୍ୟଥା ଯଦି (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = ନୂତନ property_logic(wid);
      } ଅନ୍ୟଥା ଯଦି (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = ନୂତନ property_logic(wid);
      } ଅନ୍ୟଥା {
	    properties_[idx].type = 0;
      }
}

ଶୂନ୍ୟ class_type::finish_setup(ଶୂନ୍ୟ)
{
      map<size_t, vector<size_t> > size_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      ପାଇଁ (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    ପ୍ରତିଜ୍ଞା(properties_[idx].type);
	    size_t instance_size = properties_[idx].type->instance_size();
	    accum += instance_size;
	    size_map[instance_size].push_back(idx);
      }

      instance_size_ = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      ପାଇଁ (map<size_t, vector<size_t> >::reverse_iterator cur = size_map.rbegin()
		 ; cur != size_map.rend() ; ++ cur) {
	    ପାଇଁ (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  ପ୍ରତିଜ୍ଞା(ptype->instance_size() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() ଅଚଳ
{
      ଅକ୍ଷର*buf = ନୂତନ ଅକ୍ଷର [instance_size_];

      ପାଇଁ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      ଫେରନ୍ତୁ reinterpret_cast<inst_t> (buf);
}

ଶୂନ୍ୟ class_type::instance_delete(class_type::inst_t obj) ଅଚଳ
{
      ଅକ୍ଷର*buf = reinterpret_cast<ଅକ୍ଷର*> (obj);

      ପାଇଁ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

ଶୂନ୍ୟ class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  ଅଚଳ vvp_vector4_t&val) ଅଚଳ
{
      ଅକ୍ଷର*buf = reinterpret_cast<ଅକ୍ଷର*> (obj);
      ପ୍ରତିଜ୍ଞା(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

ଶୂନ୍ୟ class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ଅଚଳ
{
      ଅକ୍ଷର*buf = reinterpret_cast<ଅକ୍ଷର*> (obj);
      ପ୍ରତିଜ୍ଞା(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

ଶୂନ୍ୟ class_type::set_real(class_type::inst_t obj, size_t pid,
			  ଦ୍ୱିଗୁଣ val) ଅଚଳ
{
      ଅକ୍ଷର*buf = reinterpret_cast<ଅକ୍ଷର*> (obj);
      ପ୍ରତିଜ୍ଞା(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

ଦ୍ୱିଗୁଣ class_type::get_real(class_type::inst_t obj, size_t pid) ଅଚଳ
{
      ଅକ୍ଷର*buf = reinterpret_cast<ଅକ୍ଷର*> (obj);
      ପ୍ରତିଜ୍ଞା(pid < properties_.size());
      ଫେରନ୍ତୁ properties_[pid].type->get_real(buf);
}

ଶୂନ୍ୟ class_type::set_string(class_type::inst_t obj, size_t pid,
			    ଅଚଳ string&val) ଅଚଳ
{
      ଅକ୍ଷର*buf = reinterpret_cast<ଅକ୍ଷର*> (obj);
      ପ୍ରତିଜ୍ଞା(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ଅଚଳ
{
      ଅକ୍ଷର*buf = reinterpret_cast<ଅକ୍ଷର*> (obj);
      ପ୍ରତିଜ୍ଞା(pid < properties_.size());
      ଫେରନ୍ତୁ properties_[pid].type->get_string(buf);
}

ଶୂନ୍ୟ class_type::set_object(class_type::inst_t obj, size_t pid,
			    ଅଚଳ vvp_object_t&val, size_t idx) ଅଚଳ
{
      ଅକ୍ଷର*buf = reinterpret_cast<ଅକ୍ଷର*> (obj);
      ପ୍ରତିଜ୍ଞା(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

ଶୂନ୍ୟ class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ଅଚଳ
{
      ଅକ୍ଷର*buf = reinterpret_cast<ଅକ୍ଷର*> (obj);
      ପ୍ରତିଜ୍ଞା(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

ଶୂନ୍ୟ class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ଅଚଳ
{
      ଅକ୍ଷର*dst_buf = reinterpret_cast<ଅକ୍ଷର*> (dst);
      ଅକ୍ଷର*src_buf = reinterpret_cast<ଅକ୍ଷର*> (src);

      ପ୍ରତିଜ୍ଞା(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା class_type::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{
      ଫେରନ୍ତୁ vpiClassDefn;
}

ସ୍ଥିର class_type*compile_class = 0;

ଶୂନ୍ୟ compile_class_start(ଅକ୍ଷର*lab, ଅକ୍ଷର*nam, unsigned ntype)
{
      ପ୍ରତିଜ୍ଞା(compile_class == 0);
      compile_class = ନୂତନ class_type(nam, ntype);
      compile_vpi_ପ୍ରତୀକ(lab, compile_class);
      ମୁକ୍ତ(lab);
      delete[]nam;
}

ଶୂନ୍ୟ compile_class_property(unsigned idx, ଅକ୍ଷର*nam, ଅକ୍ଷର*typ, uint64_t array_size)
{
      ପ୍ରତିଜ୍ଞା(compile_class);
      compile_class->set_property(idx, nam, typ, array_size);
      delete[]nam;
      delete[]typ;
}

ଶୂନ୍ୟ compile_class_done(ଶୂନ୍ୟ)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ପ୍ରତିଜ୍ଞା(scope);
      ପ୍ରତିଜ୍ଞା(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ class_def_delete(class_type *item)
{
      delete item;
}
#endif
