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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <cassert>

using namespace std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
श्रेणी class_property_t {
    खुला:
      अंतरभूत class_property_t() { offset_ = 0; }
      भव ~class_property_t() =0;
	// How much space does an instance of this property require?
      भव माप_प्रकार instance_आकार() स्थिर =0;

      व्योम set_offset(माप_प्रकार off) { offset_ = off; }

    खुला:
      भव व्योम construct(अक्षर*buf) स्थिर;
      भव व्योम destruct(अक्षर*buf) स्थिर;

      भव व्योम set_vec4(अक्षर*buf, स्थिर vvp_vector4_t&val);
      भव व्योम get_vec4(अक्षर*buf, vvp_vector4_t&val);

      भव व्योम set_real(अक्षर*buf, द्विगुन val);
      भव द्विगुन get_real(अक्षर*buf);

      भव व्योम set_string(अक्षर*buf, स्थिर std::string&val);
      भव string get_string(अक्षर*buf);

      भव व्योम set_object(अक्षर*buf, स्थिर vvp_object_t&val, uint64_t element);
      भव व्योम get_object(अक्षर*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      भव व्योम copy(अक्षर*buf, अक्षर*src) = 0;

    रक्षित:
      माप_प्रकार offset_;
};

class_property_t::~class_property_t()
{
}

व्योम class_property_t::construct(अक्षर*) स्थिर
{
}

व्योम class_property_t::destruct(अक्षर*) स्थिर
{
}

व्योम class_property_t::set_vec4(अक्षर*, स्थिर vvp_vector4_t&)
{
      निश्चित(0);
}

व्योम class_property_t::get_vec4(अक्षर*, vvp_vector4_t&)
{
      निश्चित(0);
}

व्योम class_property_t::set_real(अक्षर*, द्विगुन)
{
      निश्चित(0);
}

द्विगुन class_property_t::get_real(अक्षर*)
{
      निश्चित(0);
      वापस 0.0;
}

व्योम class_property_t::set_string(अक्षर*, स्थिर string&)
{
      निश्चित(0);
}

string class_property_t::get_string(अक्षर*)
{
      निश्चित(0);
      वापस "";
}

व्योम class_property_t::set_object(अक्षर*, स्थिर vvp_object_t&, uint64_t)
{
      निश्चित(0);
}

व्योम class_property_t::get_object(अक्षर*, vvp_object_t&, uint64_t)
{
      निश्चित(0);
}

/*
 */
ढाँचा <श्रेणी T> श्रेणी property_atom : खुला class_property_t {
    खुला:
      अंतरभूत explicit property_atom(व्योम) { }
      ~property_atom() override { }

      माप_प्रकार instance_आकार() स्थिर override { वापस माप(T); }

    खुला:
      व्योम construct(अक्षर*buf) स्थिर override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      व्योम set_vec4(अक्षर*buf, स्थिर vvp_vector4_t&val) override;
      व्योम get_vec4(अक्षर*buf, vvp_vector4_t&val) override;

      व्योम copy(अक्षर*dst, अक्षर*src) override;
};

श्रेणी property_bit : खुला class_property_t {
    खुला:
      explicit अंतरभूत property_bit(माप_प्रकार wid): wid_(wid) { }
      ~property_bit() override { }

      माप_प्रकार instance_आकार() स्थिर override { वापस माप(vvp_vector2_t); }

    खुला:
      व्योम construct(अक्षर*buf) स्थिर override
      { नया (buf+offset_) vvp_vector2_t (0, wid_); }

      व्योम destruct(अक्षर*buf) स्थिर override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      व्योम set_vec4(अक्षर*buf, स्थिर vvp_vector4_t&val) override;
      व्योम get_vec4(अक्षर*buf, vvp_vector4_t&val) override;

      व्योम copy(अक्षर*dst, अक्षर*src) override;

    निजी:
      माप_प्रकार wid_;
};

श्रेणी property_logic : खुला class_property_t {
    खुला:
      explicit अंतरभूत property_logic(माप_प्रकार wid): wid_(wid) { }
      ~property_logic() override { }

      माप_प्रकार instance_आकार() स्थिर override { वापस माप(vvp_vector4_t); }

    खुला:
      व्योम construct(अक्षर*buf) स्थिर override
      { नया (buf+offset_) vvp_vector4_t (wid_); }

      व्योम destruct(अक्षर*buf) स्थिर override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      व्योम set_vec4(अक्षर*buf, स्थिर vvp_vector4_t&val) override;
      व्योम get_vec4(अक्षर*buf, vvp_vector4_t&val) override;

      व्योम copy(अक्षर*dst, अक्षर*src) override;

    निजी:
      माप_प्रकार wid_;
};

ढाँचा <श्रेणी T> श्रेणी property_real : खुला class_property_t {
    खुला:
      अंतरभूत explicit property_real(व्योम) { }
      ~property_real() override { }

      माप_प्रकार instance_आकार() स्थिर override { वापस माप(T); }

    खुला:
      व्योम construct(अक्षर*buf) स्थिर override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      व्योम set_real(अक्षर*buf, द्विगुन val) override;
      द्विगुन get_real(अक्षर*buf) override;

      व्योम copy(अक्षर*dst, अक्षर*src) override;
};

श्रेणी property_string : खुला class_property_t {
    खुला:
      अंतरभूत explicit property_string(व्योम) { }
      ~property_string() override { }

      माप_प्रकार instance_आकार() स्थिर override { वापस माप(std::string); }

    खुला:
      व्योम construct(अक्षर*buf) स्थिर override
      { /* string*tmp = */ नया (buf+offset_) string; }

      व्योम destruct(अक्षर*buf) स्थिर override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      व्योम set_string(अक्षर*buf, स्थिर string&) override;
      string get_string(अक्षर*buf) override;

      व्योम copy(अक्षर*dst, अक्षर*src) override;
};

श्रेणी property_object : खुला class_property_t {
    खुला:
      अंतरभूत explicit property_object(uint64_t as): array_आकार(as==0? 1 : as) { }
      ~property_object() override { }

      माप_प्रकार instance_आकार() स्थिर override { वापस array_आकार * माप(vvp_object_t); }

    खुला:
      व्योम construct(अक्षर*buf) स्थिर override;

      व्योम destruct(अक्षर*buf) स्थिर override;

      व्योम set_object(अक्षर*buf, स्थिर vvp_object_t&, uint64_t) override;
      व्योम get_object(अक्षर*buf, vvp_object_t&, uint64_t) override;

      व्योम copy(अक्षर*dst, अक्षर*src) override;

    निजी:
      माप_प्रकार array_आकार;
};

ढाँचा <श्रेणी T> व्योम property_atom<T>::set_vec4(अक्षर*buf, स्थिर vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      bool flag = vector4_to_मूल्य(val, *tmp, true, false);
      निश्चित(flag);
}

ढाँचा <श्रेणी T> व्योम property_atom<T>::get_vec4(अक्षर*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      स्थिर माप_प्रकार tmp_cnt = माप(T)<माप(अचिन्हित दीर्घ)
				       ? 1
				       : माप(T) / माप(अचिन्हित दीर्घ);
      अचिन्हित दीर्घ tmp[tmp_cnt];
      tmp[0] = src[0];

      क्रम (माप_प्रकार idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * माप(tmp[0]);

      val.resize(8*माप(T));
      val.setarray(0, val.size(), tmp);
}

ढाँचा <श्रेणी T> व्योम property_atom<T>::copy(अक्षर*dst, अक्षर*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

व्योम property_bit::set_vec4(अक्षर*buf, स्थिर vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

व्योम property_bit::get_vec4(अक्षर*buf, vvp_vector4_t&val)
{
      स्थिर vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

व्योम property_bit::copy(अक्षर*dst, अक्षर*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      स्थिर vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

व्योम property_logic::set_vec4(अक्षर*buf, स्थिर vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

व्योम property_logic::get_vec4(अक्षर*buf, vvp_vector4_t&val)
{
      स्थिर vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

व्योम property_logic::copy(अक्षर*dst, अक्षर*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      स्थिर vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

ढाँचा <श्रेणी T> व्योम property_real<T>::set_real(अक्षर*buf, द्विगुन val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

ढाँचा <श्रेणी T> द्विगुन property_real<T>::get_real(अक्षर*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      वापस *tmp;
}

ढाँचा <श्रेणी T> व्योम property_real<T>::copy(अक्षर*dst, अक्षर*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

व्योम property_string::set_string(अक्षर*buf, स्थिर string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(अक्षर*buf)
{
      स्थिर string*tmp = reinterpret_cast<string*>(buf+offset_);
      वापस *tmp;
}

व्योम property_string::copy(अक्षर*dst, अक्षर*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      स्थिर string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

व्योम property_object::construct(अक्षर*buf) स्थिर
{
      क्रम (माप_प्रकार idx = 0 ; idx < array_आकार ; idx += 1)
	    नया (buf+offset_ + idx*माप(vvp_object_t)) vvp_object_t;
}

व्योम property_object::destruct(अक्षर*buf) स्थिर
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      क्रम (माप_प्रकार idx = 0 ; idx < array_आकार ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

व्योम property_object::set_object(अक्षर*buf, स्थिर vvp_object_t&val, uint64_t idx)
{
      निश्चित(idx < array_आकार);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

व्योम property_object::get_object(अक्षर*buf, vvp_object_t&val, uint64_t idx)
{
      निश्चित(idx < array_आकार);
      स्थिर vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

व्योम property_object::copy(अक्षर*dst, अक्षर*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      स्थिर vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      क्रम (माप_प्रकार idx = 0 ; idx < array_आकार ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(स्थिर string&nam, माप_प्रकार nprop)
: class_name_(nam), properties_(nprop)
{
      instance_आकार_2 = 0;
}

class_type::~class_type()
{
      क्रम (माप_प्रकार idx = 0 ; idx < properties_.size() ; idx += 1)
	    मिटाओ properties_[idx].type;
}

व्योम class_type::set_property(माप_प्रकार idx, स्थिर string&name, स्थिर string&type, uint64_t array_आकार_2)
{
      निश्चित(idx < properties_.size());
      properties_[idx].name = name;

      अगर (type == "b8")
	    properties_[idx].type = नया property_atom<uint8_t>;
      अन्यथा अगर (type == "b16")
	    properties_[idx].type = नया property_atom<uint16_t>;
      अन्यथा अगर (type == "b32")
	    properties_[idx].type = नया property_atom<uint32_t>;
      अन्यथा अगर (type == "b64")
	    properties_[idx].type = नया property_atom<uint64_t>;
      अन्यथा अगर (type == "sb8")
	    properties_[idx].type = नया property_atom<int8_t>;
      अन्यथा अगर (type == "sb16")
	    properties_[idx].type = नया property_atom<int16_t>;
      अन्यथा अगर (type == "sb32")
	    properties_[idx].type = नया property_atom<int32_t>;
      अन्यथा अगर (type == "sb64")
	    properties_[idx].type = नया property_atom<int64_t>;
      अन्यथा अगर (type == "r")
	    properties_[idx].type = नया property_real<द्विगुन>;
      अन्यथा अगर (type == "S")
	    properties_[idx].type = नया property_string;
      अन्यथा अगर (type == "o")
	    properties_[idx].type = नया property_object(array_आकार_2);
      अन्यथा अगर (type[0] == 'b') {
	    माप_प्रकार wid = म_से_अदीर्घ(type.c_str()+1, 0, 0);
	    properties_[idx].type = नया property_bit(wid);
      } अन्यथा अगर (type[0] == 'L') {
	    माप_प्रकार wid = म_से_अदीर्घ(type.c_str()+1,0,0);
	    properties_[idx].type = नया property_logic(wid);
      } अन्यथा अगर (type[0] == 's' && type[1] == 'L') {
	    माप_प्रकार wid = म_से_अदीर्घ(type.c_str()+2,0,0);
	    properties_[idx].type = नया property_logic(wid);
      } अन्यथा {
	    properties_[idx].type = 0;
      }
}

व्योम class_type::finish_setup(व्योम)
{
      map<माप_प्रकार, vector<माप_प्रकार> > आकार_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      माप_प्रकार accum = 0;
      क्रम (माप_प्रकार idx = 0 ; idx < properties_.size() ; idx += 1) {
	    निश्चित(properties_[idx].type);
	    माप_प्रकार instance_आकार = properties_[idx].type->instance_आकार();
	    accum += instance_आकार;
	    आकार_map[instance_आकार].push_back(idx);
      }

      instance_आकार_2 = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      क्रम (map<माप_प्रकार, vector<माप_प्रकार> >::reverse_iterator cur = आकार_map.rbegin()
		 ; cur != आकार_map.rend() ; ++ cur) {
	    क्रम (माप_प्रकार idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  माप_प्रकार pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  निश्चित(ptype->instance_आकार() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() स्थिर
{
      अक्षर*buf = नया अक्षर [instance_आकार_2];

      क्रम (माप_प्रकार idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      वापस reinterpret_cast<inst_t> (buf);
}

व्योम class_type::instance_delete(class_type::inst_t obj) स्थिर
{
      अक्षर*buf = reinterpret_cast<अक्षर*> (obj);

      क्रम (माप_प्रकार idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      मिटाओ[]buf;
}

व्योम class_type::set_vec4(class_type::inst_t obj, माप_प्रकार pid,
			  स्थिर vvp_vector4_t&val) स्थिर
{
      अक्षर*buf = reinterpret_cast<अक्षर*> (obj);
      निश्चित(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

व्योम class_type::get_vec4(class_type::inst_t obj, माप_प्रकार pid,
			  vvp_vector4_t&val) स्थिर
{
      अक्षर*buf = reinterpret_cast<अक्षर*> (obj);
      निश्चित(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

व्योम class_type::set_real(class_type::inst_t obj, माप_प्रकार pid,
			  द्विगुन val) स्थिर
{
      अक्षर*buf = reinterpret_cast<अक्षर*> (obj);
      निश्चित(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

द्विगुन class_type::get_real(class_type::inst_t obj, माप_प्रकार pid) स्थिर
{
      अक्षर*buf = reinterpret_cast<अक्षर*> (obj);
      निश्चित(pid < properties_.size());
      वापस properties_[pid].type->get_real(buf);
}

व्योम class_type::set_string(class_type::inst_t obj, माप_प्रकार pid,
			    स्थिर string&val) स्थिर
{
      अक्षर*buf = reinterpret_cast<अक्षर*> (obj);
      निश्चित(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, माप_प्रकार pid) स्थिर
{
      अक्षर*buf = reinterpret_cast<अक्षर*> (obj);
      निश्चित(pid < properties_.size());
      वापस properties_[pid].type->get_string(buf);
}

व्योम class_type::set_object(class_type::inst_t obj, माप_प्रकार pid,
			    स्थिर vvp_object_t&val, माप_प्रकार idx) स्थिर
{
      अक्षर*buf = reinterpret_cast<अक्षर*> (obj);
      निश्चित(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

व्योम class_type::get_object(class_type::inst_t obj, माप_प्रकार pid,
			    vvp_object_t&val, माप_प्रकार idx) स्थिर
{
      अक्षर*buf = reinterpret_cast<अक्षर*> (obj);
      निश्चित(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

व्योम class_type::copy_property(class_type::inst_t dst, माप_प्रकार pid, class_type::inst_t src) स्थिर
{
      अक्षर*dst_buf = reinterpret_cast<अक्षर*> (dst);
      अक्षर*src_buf = reinterpret_cast<अक्षर*> (src);

      निश्चित(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

पूर्णांक class_type::get_type_code(व्योम) स्थिर
{
      वापस vpiClassDefn;
}

अटल class_type*compile_class = 0;

व्योम compile_class_start(अक्षर*lab, अक्षर*nam, अचिन्हित ntype)
{
      निश्चित(compile_class == 0);
      compile_class = नया class_type(nam, ntype);
      compile_vpi_प्रतीक(lab, compile_class);
      मुक्त(lab);
      मिटाओ[]nam;
}

व्योम compile_class_property(अचिन्हित idx, अक्षर*nam, अक्षर*typ, uint64_t array_आकार_2)
{
      निश्चित(compile_class);
      compile_class->set_property(idx, nam, typ, array_आकार_2);
      मिटाओ[]nam;
      मिटाओ[]typ;
}

व्योम compile_class_done(व्योम)
{
      __vpiScope*scope = vpip_peek_current_scope();
      निश्चित(scope);
      निश्चित(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम class_def_delete(class_type *item)
{
      मिटाओ item;
}
#पूर्ण_अगर
