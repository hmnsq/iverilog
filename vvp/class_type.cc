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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cassert>

ఉపయోగించి నామాకాశ std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
తరగతి class_property_t {
    బహిరంగ:
      అంతఃవరుస class_property_t() { స్థానభ్రంశం = 0; }
      మాయా ~class_property_t() =0;
	// How much space does an instance of this property require?
      మాయా size_t instance_కొలత() స్థిరరాశి =0;

      శూన్య సమితి_స్థానభ్రంశం(size_t off) { స్థానభ్రంశం = off; }

    బహిరంగ:
      మాయా శూన్య construct(అక్షరం*buf) స్థిరరాశి;
      మాయా శూన్య destruct(అక్షరం*buf) స్థిరరాశి;

      మాయా శూన్య సమితి_vec4(అక్షరం*buf, స్థిరరాశి vvp_vector4_t&val);
      మాయా శూన్య get_vec4(అక్షరం*buf, vvp_vector4_t&val);

      మాయా శూన్య సమితి_real(అక్షరం*buf, ద్విగుణ val);
      మాయా ద్విగుణ get_real(అక్షరం*buf);

      మాయా శూన్య సమితి_string(అక్షరం*buf, స్థిరరాశి std::string&val);
      మాయా string get_string(అక్షరం*buf);

      మాయా శూన్య సమితి_object(అక్షరం*buf, స్థిరరాశి vvp_object_t&val, uint64_t element);
      మాయా శూన్య get_object(అక్షరం*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      మాయా శూన్య copy(అక్షరం*buf, అక్షరం*src) = 0;

    రక్షిత:
      size_t స్థానభ్రంశం;
};

class_property_t::~class_property_t()
{
}

శూన్య class_property_t::construct(అక్షరం*) స్థిరరాశి
{
}

శూన్య class_property_t::destruct(అక్షరం*) స్థిరరాశి
{
}

శూన్య class_property_t::సమితి_vec4(అక్షరం*, స్థిరరాశి vvp_vector4_t&)
{
      ప్రతిజ్ఞా(0);
}

శూన్య class_property_t::get_vec4(అక్షరం*, vvp_vector4_t&)
{
      ప్రతిజ్ఞా(0);
}

శూన్య class_property_t::సమితి_real(అక్షరం*, ద్విగుణ)
{
      ప్రతిజ్ఞా(0);
}

ద్విగుణ class_property_t::get_real(అక్షరం*)
{
      ప్రతిజ్ఞా(0);
      తిరిగి 0.0;
}

శూన్య class_property_t::సమితి_string(అక్షరం*, స్థిరరాశి string&)
{
      ప్రతిజ్ఞా(0);
}

string class_property_t::get_string(అక్షరం*)
{
      ప్రతిజ్ఞా(0);
      తిరిగి "";
}

శూన్య class_property_t::సమితి_object(అక్షరం*, స్థిరరాశి vvp_object_t&, uint64_t)
{
      ప్రతిజ్ఞా(0);
}

శూన్య class_property_t::get_object(అక్షరం*, vvp_object_t&, uint64_t)
{
      ప్రతిజ్ఞా(0);
}

/*
 */
మూస <తరగతి T> తరగతి property_atom : బహిరంగ class_property_t {
    బహిరంగ:
      అంతఃవరుస స్పష్టమైన property_atom(శూన్య) { }
      ~property_atom() override { }

      size_t instance_కొలత() స్థిరరాశి override { తిరిగి పరిమాణం(T); }

    బహిరంగ:
      శూన్య construct(అక్షరం*buf) స్థిరరాశి override
      { T*tmp = reinterpret_cast<T*> (buf+స్థానభ్రంశం);
	*tmp = 0;
      }

      శూన్య సమితి_vec4(అక్షరం*buf, స్థిరరాశి vvp_vector4_t&val) override;
      శూన్య get_vec4(అక్షరం*buf, vvp_vector4_t&val) override;

      శూన్య copy(అక్షరం*dst, అక్షరం*src) override;
};

తరగతి property_బిట్టు : బహిరంగ class_property_t {
    బహిరంగ:
      స్పష్టమైన అంతఃవరుస property_బిట్టు(size_t wid): wid_(wid) { }
      ~property_బిట్టు() override { }

      size_t instance_కొలత() స్థిరరాశి override { తిరిగి పరిమాణం(vvp_vector2_t); }

    బహిరంగ:
      శూన్య construct(అక్షరం*buf) స్థిరరాశి override
      { కొత్త (buf+స్థానభ్రంశం) vvp_vector2_t (0, wid_); }

      శూన్య destruct(అక్షరం*buf) స్థిరరాశి override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+స్థానభ్రంశం);
	tmp->~vvp_vector2_t();
      }

      శూన్య సమితి_vec4(అక్షరం*buf, స్థిరరాశి vvp_vector4_t&val) override;
      శూన్య get_vec4(అక్షరం*buf, vvp_vector4_t&val) override;

      శూన్య copy(అక్షరం*dst, అక్షరం*src) override;

    అంతరంగ:
      size_t wid_;
};

తరగతి property_logic : బహిరంగ class_property_t {
    బహిరంగ:
      స్పష్టమైన అంతఃవరుస property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_కొలత() స్థిరరాశి override { తిరిగి పరిమాణం(vvp_vector4_t); }

    బహిరంగ:
      శూన్య construct(అక్షరం*buf) స్థిరరాశి override
      { కొత్త (buf+స్థానభ్రంశం) vvp_vector4_t (wid_); }

      శూన్య destruct(అక్షరం*buf) స్థిరరాశి override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+స్థానభ్రంశం);
	tmp->~vvp_vector4_t();
      }

      శూన్య సమితి_vec4(అక్షరం*buf, స్థిరరాశి vvp_vector4_t&val) override;
      శూన్య get_vec4(అక్షరం*buf, vvp_vector4_t&val) override;

      శూన్య copy(అక్షరం*dst, అక్షరం*src) override;

    అంతరంగ:
      size_t wid_;
};

మూస <తరగతి T> తరగతి property_real : బహిరంగ class_property_t {
    బహిరంగ:
      అంతఃవరుస స్పష్టమైన property_real(శూన్య) { }
      ~property_real() override { }

      size_t instance_కొలత() స్థిరరాశి override { తిరిగి పరిమాణం(T); }

    బహిరంగ:
      శూన్య construct(అక్షరం*buf) స్థిరరాశి override
      { T*tmp = reinterpret_cast<T*> (buf+స్థానభ్రంశం);
	*tmp = 0.0;
      }

      శూన్య సమితి_real(అక్షరం*buf, ద్విగుణ val) override;
      ద్విగుణ get_real(అక్షరం*buf) override;

      శూన్య copy(అక్షరం*dst, అక్షరం*src) override;
};

తరగతి property_string : బహిరంగ class_property_t {
    బహిరంగ:
      అంతఃవరుస స్పష్టమైన property_string(శూన్య) { }
      ~property_string() override { }

      size_t instance_కొలత() స్థిరరాశి override { తిరిగి పరిమాణం(std::string); }

    బహిరంగ:
      శూన్య construct(అక్షరం*buf) స్థిరరాశి override
      { /* string*tmp = */ కొత్త (buf+స్థానభ్రంశం) string; }

      శూన్య destruct(అక్షరం*buf) స్థిరరాశి override
      { string*tmp = reinterpret_cast<string*> (buf+స్థానభ్రంశం);
	tmp->~string();
      }

      శూన్య సమితి_string(అక్షరం*buf, స్థిరరాశి string&) override;
      string get_string(అక్షరం*buf) override;

      శూన్య copy(అక్షరం*dst, అక్షరం*src) override;
};

తరగతి property_object : బహిరంగ class_property_t {
    బహిరంగ:
      అంతఃవరుస స్పష్టమైన property_object(uint64_t as): array_కొలత(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_కొలత() స్థిరరాశి override { తిరిగి array_కొలత * పరిమాణం(vvp_object_t); }

    బహిరంగ:
      శూన్య construct(అక్షరం*buf) స్థిరరాశి override;

      శూన్య destruct(అక్షరం*buf) స్థిరరాశి override;

      శూన్య సమితి_object(అక్షరం*buf, స్థిరరాశి vvp_object_t&, uint64_t) override;
      శూన్య get_object(అక్షరం*buf, vvp_object_t&, uint64_t) override;

      శూన్య copy(అక్షరం*dst, అక్షరం*src) override;

    అంతరంగ:
      size_t array_కొలత;
};

మూస <తరగతి T> శూన్య property_atom<T>::సమితి_vec4(అక్షరం*buf, స్థిరరాశి vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+స్థానభ్రంశం);
      తార్కిక flag = vector4_గమ్యం_విలువ(val, *tmp, నిజం, అబద్ధం);
      ప్రతిజ్ఞా(flag);
}

మూస <తరగతి T> శూన్య property_atom<T>::get_vec4(అక్షరం*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+స్థానభ్రంశం);
      స్థిరరాశి size_t tmp_cnt = పరిమాణం(T)<పరిమాణం(గుర్తులేని దీర్ఘ)
				       ? 1
				       : పరిమాణం(T) / పరిమాణం(గుర్తులేని దీర్ఘ);
      గుర్తులేని దీర్ఘ tmp[tmp_cnt];
      tmp[0] = src[0];

      కొరకు (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * పరిమాణం(tmp[0]);

      val.resize(8*పరిమాణం(T));
      val.setarray(0, val.size(), tmp);
}

మూస <తరగతి T> శూన్య property_atom<T>::copy(అక్షరం*dst, అక్షరం*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+స్థానభ్రంశం);
      T*src_obj = reinterpret_cast<T*> (src+స్థానభ్రంశం);
      *dst_obj = *src_obj;
}

శూన్య property_బిట్టు::సమితి_vec4(అక్షరం*buf, స్థిరరాశి vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+స్థానభ్రంశం);
      *obj = val;
}

శూన్య property_బిట్టు::get_vec4(అక్షరం*buf, vvp_vector4_t&val)
{
      స్థిరరాశి vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+స్థానభ్రంశం);
      val = vector2_గమ్యం_vector4(*obj, obj->size());
}

శూన్య property_బిట్టు::copy(అక్షరం*dst, అక్షరం*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+స్థానభ్రంశం);
      స్థిరరాశి vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+స్థానభ్రంశం);
      *dst_obj = *src_obj;
}

శూన్య property_logic::సమితి_vec4(అక్షరం*buf, స్థిరరాశి vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+స్థానభ్రంశం);
      *obj = val;
}

శూన్య property_logic::get_vec4(అక్షరం*buf, vvp_vector4_t&val)
{
      స్థిరరాశి vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+స్థానభ్రంశం);
      val = *obj;
}

శూన్య property_logic::copy(అక్షరం*dst, అక్షరం*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+స్థానభ్రంశం);
      స్థిరరాశి vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+స్థానభ్రంశం);
      *dst_obj = *src_obj;
}

మూస <తరగతి T> శూన్య property_real<T>::సమితి_real(అక్షరం*buf, ద్విగుణ val)
{
      T*tmp = reinterpret_cast<T*>(buf+స్థానభ్రంశం);
      *tmp = val;
}

మూస <తరగతి T> ద్విగుణ property_real<T>::get_real(అక్షరం*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+స్థానభ్రంశం);
      తిరిగి *tmp;
}

మూస <తరగతి T> శూన్య property_real<T>::copy(అక్షరం*dst, అక్షరం*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+స్థానభ్రంశం);
      T*src_obj = reinterpret_cast<T*> (src+స్థానభ్రంశం);
      *dst_obj = *src_obj;
}

శూన్య property_string::సమితి_string(అక్షరం*buf, స్థిరరాశి string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+స్థానభ్రంశం);
      *tmp = val;
}

string property_string::get_string(అక్షరం*buf)
{
      స్థిరరాశి string*tmp = reinterpret_cast<string*>(buf+స్థానభ్రంశం);
      తిరిగి *tmp;
}

శూన్య property_string::copy(అక్షరం*dst, అక్షరం*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+స్థానభ్రంశం);
      స్థిరరాశి string*src_obj = reinterpret_cast<string*> (src+స్థానభ్రంశం);
      *dst_obj = *src_obj;
}

శూన్య property_object::construct(అక్షరం*buf) స్థిరరాశి
{
      కొరకు (size_t idx = 0 ; idx < array_కొలత ; idx += 1)
	    కొత్త (buf+స్థానభ్రంశం + idx*పరిమాణం(vvp_object_t)) vvp_object_t;
}

శూన్య property_object::destruct(అక్షరం*buf) స్థిరరాశి
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+స్థానభ్రంశం);
      కొరకు (size_t idx = 0 ; idx < array_కొలత ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

శూన్య property_object::సమితి_object(అక్షరం*buf, స్థిరరాశి vvp_object_t&val, uint64_t idx)
{
      ప్రతిజ్ఞా(idx < array_కొలత);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+స్థానభ్రంశం);
      tmp[idx] = val;
}

శూన్య property_object::get_object(అక్షరం*buf, vvp_object_t&val, uint64_t idx)
{
      ప్రతిజ్ఞా(idx < array_కొలత);
      స్థిరరాశి vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+స్థానభ్రంశం);
      val = tmp[idx];
}

శూన్య property_object::copy(అక్షరం*dst, అక్షరం*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+స్థానభ్రంశం);
      స్థిరరాశి vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+స్థానభ్రంశం);
      కొరకు (size_t idx = 0 ; idx < array_కొలత ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(స్థిరరాశి string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_కొలత_2 = 0;
}

class_type::~class_type()
{
      కొరకు (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    తొలగించు properties_[idx].type;
}

శూన్య class_type::సమితి_property(size_t idx, స్థిరరాశి string&name, స్థిరరాశి string&type, uint64_t array_కొలత_2)
{
      ప్రతిజ్ఞా(idx < properties_.size());
      properties_[idx].name = name;

      ఒకవేళ (type == "b8")
	    properties_[idx].type = కొత్త property_atom<uint8_t>;
      లేకపోతే ఒకవేళ (type == "b16")
	    properties_[idx].type = కొత్త property_atom<uint16_t>;
      లేకపోతే ఒకవేళ (type == "b32")
	    properties_[idx].type = కొత్త property_atom<uint32_t>;
      లేకపోతే ఒకవేళ (type == "b64")
	    properties_[idx].type = కొత్త property_atom<uint64_t>;
      లేకపోతే ఒకవేళ (type == "sb8")
	    properties_[idx].type = కొత్త property_atom<int8_t>;
      లేకపోతే ఒకవేళ (type == "sb16")
	    properties_[idx].type = కొత్త property_atom<int16_t>;
      లేకపోతే ఒకవేళ (type == "sb32")
	    properties_[idx].type = కొత్త property_atom<int32_t>;
      లేకపోతే ఒకవేళ (type == "sb64")
	    properties_[idx].type = కొత్త property_atom<int64_t>;
      లేకపోతే ఒకవేళ (type == "r")
	    properties_[idx].type = కొత్త property_real<ద్విగుణ>;
      లేకపోతే ఒకవేళ (type == "S")
	    properties_[idx].type = కొత్త property_string;
      లేకపోతే ఒకవేళ (type == "o")
	    properties_[idx].type = కొత్త property_object(array_కొలత_2);
      లేకపోతే ఒకవేళ (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = కొత్త property_బిట్టు(wid);
      } లేకపోతే ఒకవేళ (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = కొత్త property_logic(wid);
      } లేకపోతే ఒకవేళ (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = కొత్త property_logic(wid);
      } లేకపోతే {
	    properties_[idx].type = 0;
      }
}

శూన్య class_type::ముగింపు_అమరిక(శూన్య)
{
      map<size_t, vector<size_t> > కొలత_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      కొరకు (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    ప్రతిజ్ఞా(properties_[idx].type);
	    size_t instance_కొలత = properties_[idx].type->instance_కొలత();
	    accum += instance_కొలత;
	    కొలత_map[instance_కొలత].push_back(idx);
      }

      instance_కొలత_2 = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      కొరకు (map<size_t, vector<size_t> >::reverse_iterator cur = కొలత_map.rbegin()
		 ; cur != కొలత_map.rend() ; ++ cur) {
	    కొరకు (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  ప్రతిజ్ఞా(ptype->instance_కొలత() == cur->మొదటి);
		  ptype->సమితి_స్థానభ్రంశం(accum);
		  accum += cur->మొదటి;
	    }
      }
}

class_type::inst_t class_type::instance_new() స్థిరరాశి
{
      అక్షరం*buf = కొత్త అక్షరం [instance_కొలత_2];

      కొరకు (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      తిరిగి reinterpret_cast<inst_t> (buf);
}

శూన్య class_type::instance_delete(class_type::inst_t obj) స్థిరరాశి
{
      అక్షరం*buf = reinterpret_cast<అక్షరం*> (obj);

      కొరకు (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      తొలగించు[]buf;
}

శూన్య class_type::సమితి_vec4(class_type::inst_t obj, size_t pid,
			  స్థిరరాశి vvp_vector4_t&val) స్థిరరాశి
{
      అక్షరం*buf = reinterpret_cast<అక్షరం*> (obj);
      ప్రతిజ్ఞా(pid < properties_.size());
      properties_[pid].type->సమితి_vec4(buf, val);
}

శూన్య class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) స్థిరరాశి
{
      అక్షరం*buf = reinterpret_cast<అక్షరం*> (obj);
      ప్రతిజ్ఞా(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

శూన్య class_type::సమితి_real(class_type::inst_t obj, size_t pid,
			  ద్విగుణ val) స్థిరరాశి
{
      అక్షరం*buf = reinterpret_cast<అక్షరం*> (obj);
      ప్రతిజ్ఞా(pid < properties_.size());
      properties_[pid].type->సమితి_real(buf, val);
}

ద్విగుణ class_type::get_real(class_type::inst_t obj, size_t pid) స్థిరరాశి
{
      అక్షరం*buf = reinterpret_cast<అక్షరం*> (obj);
      ప్రతిజ్ఞా(pid < properties_.size());
      తిరిగి properties_[pid].type->get_real(buf);
}

శూన్య class_type::సమితి_string(class_type::inst_t obj, size_t pid,
			    స్థిరరాశి string&val) స్థిరరాశి
{
      అక్షరం*buf = reinterpret_cast<అక్షరం*> (obj);
      ప్రతిజ్ఞా(pid < properties_.size());
      properties_[pid].type->సమితి_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) స్థిరరాశి
{
      అక్షరం*buf = reinterpret_cast<అక్షరం*> (obj);
      ప్రతిజ్ఞా(pid < properties_.size());
      తిరిగి properties_[pid].type->get_string(buf);
}

శూన్య class_type::సమితి_object(class_type::inst_t obj, size_t pid,
			    స్థిరరాశి vvp_object_t&val, size_t idx) స్థిరరాశి
{
      అక్షరం*buf = reinterpret_cast<అక్షరం*> (obj);
      ప్రతిజ్ఞా(pid < properties_.size());
      properties_[pid].type->సమితి_object(buf, val, idx);
}

శూన్య class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) స్థిరరాశి
{
      అక్షరం*buf = reinterpret_cast<అక్షరం*> (obj);
      ప్రతిజ్ఞా(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

శూన్య class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) స్థిరరాశి
{
      అక్షరం*dst_buf = reinterpret_cast<అక్షరం*> (dst);
      అక్షరం*src_buf = reinterpret_cast<అక్షరం*> (src);

      ప్రతిజ్ఞా(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

పూర్ణాంకం class_type::get_type_సంకేతం(శూన్య) స్థిరరాశి
{
      తిరిగి vpiClassDefn;
}

స్థిర class_type*compile_class = 0;

శూన్య compile_class_ప్రారంభం(అక్షరం*lab, అక్షరం*nam, గుర్తులేని ntype)
{
      ప్రతిజ్ఞా(compile_class == 0);
      compile_class = కొత్త class_type(nam, ntype);
      compile_vpi_చిహ్నం(lab, compile_class);
      విడుదల(lab);
      తొలగించు[]nam;
}

శూన్య compile_class_property(గుర్తులేని idx, అక్షరం*nam, అక్షరం*typ, uint64_t array_కొలత_2)
{
      ప్రతిజ్ఞా(compile_class);
      compile_class->సమితి_property(idx, nam, typ, array_కొలత_2);
      తొలగించు[]nam;
      తొలగించు[]typ;
}

శూన్య compile_class_పూర్తి(శూన్య)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ప్రతిజ్ఞా(scope);
      ప్రతిజ్ఞా(compile_class);
      compile_class->ముగింపు_అమరిక();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య class_def_delete(class_type *అంశం)
{
      తొలగించు అంశం;
}
#అయితే_ముగింపు
