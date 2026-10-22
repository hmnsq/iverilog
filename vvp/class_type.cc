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

using namespace std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
class class_property_t {
    public:
      anta_hstha class_property_t() { offset_ = 0; }
      virtual ~class_property_t() =0;
	// How much space does an instance of this property require?
      virtual size_t instance_size() const =0;

      void set_offset(size_t off) { offset_ = off; }

    public:
      virtual void construct(var_na*buf) const;
      virtual void destruct(var_na*buf) const;

      virtual void set_vec4(var_na*buf, const vvp_vector4_t&val);
      virtual void get_vec4(var_na*buf, vvp_vector4_t&val);

      virtual void set_real(var_na*buf, dviguu_na val);
      virtual dviguu_na get_real(var_na*buf);

      virtual void set_string(var_na*buf, const std::string&val);
      virtual string get_string(var_na*buf);

      virtual void set_object(var_na*buf, const vvp_object_t&val, uint64_t element);
      virtual void get_object(var_na*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      virtual void copy(var_na*buf, var_na*src) = 0;

    rak_sita:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

void class_property_t::construct(var_na*) const
{
}

void class_property_t::destruct(var_na*) const
{
}

void class_property_t::set_vec4(var_na*, const vvp_vector4_t&)
{
      assert(0);
}

void class_property_t::get_vec4(var_na*, vvp_vector4_t&)
{
      assert(0);
}

void class_property_t::set_real(var_na*, dviguu_na)
{
      assert(0);
}

dviguu_na class_property_t::get_real(var_na*)
{
      assert(0);
      ᮝᮜᮤ 0.0;
}

void class_property_t::set_string(var_na*, const string&)
{
      assert(0);
}

string class_property_t::get_string(var_na*)
{
      assert(0);
      ᮝᮜᮤ "";
}

void class_property_t::set_object(var_na*, const vvp_object_t&, uint64_t)
{
      assert(0);
}

void class_property_t::get_object(var_na*, vvp_object_t&, uint64_t)
{
      assert(0);
}

/*
 */
template <class T> class property_atom : public class_property_t {
    public:
      anta_hstha explicit property_atom(void) { }
      ~property_atom() override { }

      size_t instance_size() const override { ᮝᮜᮤ parimaa_na(T); }

    public:
      void construct(var_na*buf) const override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      void set_vec4(var_na*buf, const vvp_vector4_t&val) override;
      void get_vec4(var_na*buf, vvp_vector4_t&val) override;

      void copy(var_na*dst, var_na*src) override;
};

class property_bit : public class_property_t {
    public:
      explicit anta_hstha property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_size() const override { ᮝᮜᮤ parimaa_na(vvp_vector2_t); }

    public:
      void construct(var_na*buf) const override
      { new (buf+offset_) vvp_vector2_t (0, wid_); }

      void destruct(var_na*buf) const override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      void set_vec4(var_na*buf, const vvp_vector4_t&val) override;
      void get_vec4(var_na*buf, vvp_vector4_t&val) override;

      void copy(var_na*dst, var_na*src) override;

    private:
      size_t wid_;
};

class property_logic : public class_property_t {
    public:
      explicit anta_hstha property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_size() const override { ᮝᮜᮤ parimaa_na(vvp_vector4_t); }

    public:
      void construct(var_na*buf) const override
      { new (buf+offset_) vvp_vector4_t (wid_); }

      void destruct(var_na*buf) const override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      void set_vec4(var_na*buf, const vvp_vector4_t&val) override;
      void get_vec4(var_na*buf, vvp_vector4_t&val) override;

      void copy(var_na*dst, var_na*src) override;

    private:
      size_t wid_;
};

template <class T> class property_real : public class_property_t {
    public:
      anta_hstha explicit property_real(void) { }
      ~property_real() override { }

      size_t instance_size() const override { ᮝᮜᮤ parimaa_na(T); }

    public:
      void construct(var_na*buf) const override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      void set_real(var_na*buf, dviguu_na val) override;
      dviguu_na get_real(var_na*buf) override;

      void copy(var_na*dst, var_na*src) override;
};

class property_string : public class_property_t {
    public:
      anta_hstha explicit property_string(void) { }
      ~property_string() override { }

      size_t instance_size() const override { ᮝᮜᮤ parimaa_na(std::string); }

    public:
      void construct(var_na*buf) const override
      { /* string*tmp = */ new (buf+offset_) string; }

      void destruct(var_na*buf) const override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      void set_string(var_na*buf, const string&) override;
      string get_string(var_na*buf) override;

      void copy(var_na*dst, var_na*src) override;
};

class property_object : public class_property_t {
    public:
      anta_hstha explicit property_object(uint64_t as): array_size_(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_size() const override { ᮝᮜᮤ array_size_ * parimaa_na(vvp_object_t); }

    public:
      void construct(var_na*buf) const override;

      void destruct(var_na*buf) const override;

      void set_object(var_na*buf, const vvp_object_t&, uint64_t) override;
      void get_object(var_na*buf, vvp_object_t&, uint64_t) override;

      void copy(var_na*dst, var_na*src) override;

    private:
      size_t array_size_;
};

template <class T> void property_atom<T>::set_vec4(var_na*buf, const vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      bool flag = vector4_to_value(val, *tmp, true, false);
      assert(flag);
}

template <class T> void property_atom<T>::get_vec4(var_na*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      const size_t tmp_cnt = parimaa_na(T)<parimaa_na(unsigned long)
				       ? 1
				       : parimaa_na(T) / parimaa_na(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      for (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * parimaa_na(tmp[0]);

      val.resize(8*parimaa_na(T));
      val.setarray(0, val.size(), tmp);
}

template <class T> void property_atom<T>::copy(var_na*dst, var_na*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

void property_bit::set_vec4(var_na*buf, const vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

void property_bit::get_vec4(var_na*buf, vvp_vector4_t&val)
{
      const vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

void property_bit::copy(var_na*dst, var_na*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      const vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

void property_logic::set_vec4(var_na*buf, const vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

void property_logic::get_vec4(var_na*buf, vvp_vector4_t&val)
{
      const vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

void property_logic::copy(var_na*dst, var_na*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      const vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <class T> void property_real<T>::set_real(var_na*buf, dviguu_na val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <class T> dviguu_na property_real<T>::get_real(var_na*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      ᮝᮜᮤ *tmp;
}

template <class T> void property_real<T>::copy(var_na*dst, var_na*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

void property_string::set_string(var_na*buf, const string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(var_na*buf)
{
      const string*tmp = reinterpret_cast<string*>(buf+offset_);
      ᮝᮜᮤ *tmp;
}

void property_string::copy(var_na*dst, var_na*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      const string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

void property_object::construct(var_na*buf) const
{
      for (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    new (buf+offset_ + idx*parimaa_na(vvp_object_t)) vvp_object_t;
}

void property_object::destruct(var_na*buf) const
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      for (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

void property_object::set_object(var_na*buf, const vvp_object_t&val, uint64_t idx)
{
      assert(idx < array_size_);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

void property_object::get_object(var_na*buf, vvp_object_t&val, uint64_t idx)
{
      assert(idx < array_size_);
      const vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

void property_object::copy(var_na*dst, var_na*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      const vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      for (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(const string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_size_ = 0;
}

class_type::~class_type()
{
      for (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

void class_type::set_property(size_t idx, const string&name, const string&type, uint64_t array_size)
{
      assert(idx < properties_.size());
      properties_[idx].name = name;

      ᮚᮦᮔ᮪ (type == "b8")
	    properties_[idx].type = new property_atom<uint8_t>;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "b16")
	    properties_[idx].type = new property_atom<uint16_t>;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "b32")
	    properties_[idx].type = new property_atom<uint32_t>;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "b64")
	    properties_[idx].type = new property_atom<uint64_t>;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "sb8")
	    properties_[idx].type = new property_atom<int8_t>;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "sb16")
	    properties_[idx].type = new property_atom<int16_t>;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "sb32")
	    properties_[idx].type = new property_atom<int32_t>;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "sb64")
	    properties_[idx].type = new property_atom<int64_t>;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "r")
	    properties_[idx].type = new property_real<dviguu_na>;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "S")
	    properties_[idx].type = new property_string;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type == "o")
	    properties_[idx].type = new property_object(array_size);
      ᮜᮤᮚ ᮚᮦᮔ᮪ (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = new property_bit(wid);
      } ᮜᮤᮚ ᮚᮦᮔ᮪ (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = new property_logic(wid);
      } ᮜᮤᮚ ᮚᮦᮔ᮪ (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = new property_logic(wid);
      } ᮜᮤᮚ {
	    properties_[idx].type = 0;
      }
}

void class_type::finish_setup(void)
{
      map<size_t, vector<size_t> > size_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      for (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    assert(properties_[idx].type);
	    size_t instance_size = properties_[idx].type->instance_size();
	    accum += instance_size;
	    size_map[instance_size].push_back(idx);
      }

      instance_size_ = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      for (map<size_t, vector<size_t> >::reverse_iterator cur = size_map.rbegin()
		 ; cur != size_map.rend() ; ++ cur) {
	    for (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  assert(ptype->instance_size() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() const
{
      var_na*buf = new var_na [instance_size_];

      for (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      ᮝᮜᮤ reinterpret_cast<inst_t> (buf);
}

void class_type::instance_delete(class_type::inst_t obj) const
{
      var_na*buf = reinterpret_cast<var_na*> (obj);

      for (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

void class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  const vvp_vector4_t&val) const
{
      var_na*buf = reinterpret_cast<var_na*> (obj);
      assert(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

void class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) const
{
      var_na*buf = reinterpret_cast<var_na*> (obj);
      assert(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

void class_type::set_real(class_type::inst_t obj, size_t pid,
			  dviguu_na val) const
{
      var_na*buf = reinterpret_cast<var_na*> (obj);
      assert(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

dviguu_na class_type::get_real(class_type::inst_t obj, size_t pid) const
{
      var_na*buf = reinterpret_cast<var_na*> (obj);
      assert(pid < properties_.size());
      ᮝᮜᮤ properties_[pid].type->get_real(buf);
}

void class_type::set_string(class_type::inst_t obj, size_t pid,
			    const string&val) const
{
      var_na*buf = reinterpret_cast<var_na*> (obj);
      assert(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) const
{
      var_na*buf = reinterpret_cast<var_na*> (obj);
      assert(pid < properties_.size());
      ᮝᮜᮤ properties_[pid].type->get_string(buf);
}

void class_type::set_object(class_type::inst_t obj, size_t pid,
			    const vvp_object_t&val, size_t idx) const
{
      var_na*buf = reinterpret_cast<var_na*> (obj);
      assert(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

void class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) const
{
      var_na*buf = reinterpret_cast<var_na*> (obj);
      assert(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

void class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) const
{
      var_na*dst_buf = reinterpret_cast<var_na*> (dst);
      var_na*src_buf = reinterpret_cast<var_na*> (src);

      assert(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

puur_naa_nka class_type::get_type_code(void) const
{
      ᮝᮜᮤ vpiClassDefn;
}

static class_type*compile_class = 0;

void compile_class_start(var_na*lab, var_na*nam, unsigned ntype)
{
      assert(compile_class == 0);
      compile_class = new class_type(nam, ntype);
      compile_vpi_symbol(lab, compile_class);
      free(lab);
      delete[]nam;
}

void compile_class_property(unsigned idx, var_na*nam, var_na*typ, uint64_t array_size)
{
      assert(compile_class);
      compile_class->set_property(idx, nam, typ, array_size);
      delete[]nam;
      delete[]typ;
}

void compile_class_done(void)
{
      __vpiScope*scope = vpip_peek_current_scope();
      assert(scope);
      assert(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ifdef CHECK_WITH_VALGRIND
void class_def_delete(class_type *item)
{
      delete item;
}
#endif
