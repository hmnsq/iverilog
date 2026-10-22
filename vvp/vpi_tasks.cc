/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
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

/*
 * This file keeps the table of system/task definitions. This table is
 * built up before the input source file is parsed, and is used by the
 * compiler when %vpi_call statements are encountered.
 */
# include  "vpi_priv.h"
# include  "vthread.h"
# include  "compile.h"
# include  "config.h"
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

using நாமாகாஶ std;

உள்வரி __vpiUserSystf::__vpiUserSystf()
{ }

முழு __vpiUserSystf::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiUserSystf; }


நிலையான vpiHandle systask_handle(முழு type, vpiHandle ref)
{
      கட்டமைப்பு __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      தேர்வு (type) {
	  நிலை vpiScope:
	    திரும்பு rfp->scope;

	  நிலை vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    ப்ரதிஜ்ஞா(rfp->defn->is_user_defn);
	    திரும்பு rfp->defn;

	  இயல்பு:
	    திரும்பு 0;
      };
}

நிலையான முழு systask_get(முழு type, vpiHandle ref)
{
      கட்டமைப்பு __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      தேர்வு (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  நிலை vpiTimeUnit:
	    திரும்பு rfp->scope->time_units;
	  நிலை vpiTimePrecision:
	    திரும்பு rfp->scope->time_precision;

	  நிலை vpi_வரி_No:
	    திரும்பு rfp->lineno;

	  நிலை vpiUserDefn:
	    திரும்பு rfp->defn->is_user_defn;

	  இயல்பு:
	    திரும்பு vpiUndefined;
      }
}

// support getting vpiSize for a system function call
நிலையான முழு sysfunc_get(முழு type, vpiHandle ref)
{
      கட்டமைப்பு __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      தேர்வு (type) {
	  நிலை vpiSize:
	    ப்ரதிஜ்ஞா(0); // This should be handled by derived classes
	    திரும்பு 0;

	  நிலை vpi_வரி_No:
	    திரும்பு rfp->lineno;

	  நிலை vpiUserDefn:
	    திரும்பு rfp->defn->is_user_defn;

	  இயல்பு:
	    திரும்பு vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

நிலையான எழுத்து *systask_get_str(முழு type, vpiHandle ref)
{
      மாறிலி கட்டமைப்பு __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      தேர்வு (type) {
          நிலை vpiFile:
            ப்ரதிஜ்ஞா(rfp->file_idx < file_names.size());
            திரும்பு simple_set_rbuf_str(file_names[rfp->file_idx]);

          நிலை vpiName:
            திரும்பு simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      திரும்பு 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
நிலையான vpiHandle systask_iter(முழு, vpiHandle ref)
{
      கட்டமைப்பு __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      எனில் (rfp->nargs == 0)
	    திரும்பு 0;

      திரும்பு vpip_make_iterator(rfp->nargs, rfp->args, பொய்);
}

கட்டமைப்பு systask_def : பொது __vpiSysTaskCall {
      மெய்நிகர் முழு get_type_code(வெறுமை) மாறிலி   override { திரும்பு vpiSysTaskCall; }
      மெய்நிகர் முழு vpi_get(முழு code)           override { திரும்பு systask_get(code, இது); }
      மெய்நிகர் எழுத்து*vpi_get_str(முழு code)      override { திரும்பு systask_get_str(code, இது); }
      மெய்நிகர் vpiHandle vpi_handle(முழு code)  override { திரும்பு systask_handle(code, இது); }
      மெய்நிகர் vpiHandle vpi_iterate(முழு code) override { திரும்பு systask_iter(code, இது); }
};

கட்டமைப்பு sysfunc_def : பொது systask_def {
      மெய்நிகர் முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiSysFuncCall; }
      மெய்நிகர் முழு vpi_get(முழு code) override { திரும்பு sysfunc_get(code, இது); }
};

கட்டமைப்பு sysfunc_real : பொது sysfunc_def {
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      உள்வரி இரட்டை return_மதிப்பு() மாறிலி { திரும்பு return_மதிப்பு_2; }

    தனிப்பட்ட:
      இரட்டை return_மதிப்பு_2;
};

vpiHandle sysfunc_real::vpi_put_மதிப்பு(p_vpi_மதிப்பு vp, முழு)
{
      put_மதிப்பு = உண்மை;

      return_மதிப்பு_2 = 0.0;
      தேர்வு (vp->format) {
	  நிலை vpiRealVal:
	    return_மதிப்பு_2 = vp->value.real;
	    நிறுத்து;
	  இயல்பு:
	    fprintf(stderr, "Unsupported format %d.\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);
      }

      திரும்பு 0;
}

கட்டமைப்பு sysfunc_str : பொது sysfunc_def {
      முழு vpi_get(முழு code) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      உள்வரி மாறிலி std::string& return_மதிப்பு() மாறிலி { திரும்பு return_மதிப்பு_2; };

    தனிப்பட்ட:
      std::string return_மதிப்பு_2;
};

முழு sysfunc_str::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiSize:
	    திரும்பு return_மதிப்பு_2.size();

	  நிலை vpi_வரி_No:
	    திரும்பு lineno;

	  நிலை vpiUserDefn:
	    திரும்பு defn->is_user_defn;

	  இயல்பு:
	    திரும்பு vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_மதிப்பு(p_vpi_மதிப்பு vp, முழு)
{
      put_மதிப்பு = உண்மை;

      தேர்வு (vp->format) {
	  நிலை vpiStringVal:
	    return_மதிப்பு_2 = std::string(vp->value.str);
	    நிறுத்து;
	  இயல்பு:
	    fprintf(stderr, "Unsupported format %d.\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);
      }

      திரும்பு 0;
}

வகுப்பு sysfunc_vec4 : பொது sysfunc_def {
    பொது:
      explicit உள்வரி sysfunc_vec4(unsigned wid): return_மதிப்பு_2(wid, BIT4_X) { }
      முழு vpi_get(முழு code) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      உள்வரி மாறிலி vvp_vector4_t& return_மதிப்பு() மாறிலி { திரும்பு return_மதிப்பு_2; }

    தனிப்பட்ட:
      vpiHandle put_மதிப்பு_scalar(p_vpi_மதிப்பு vp);
      vpiHandle put_மதிப்பு_int(p_vpi_மதிப்பு vp);
      vpiHandle put_மதிப்பு_string(p_vpi_மதிப்பு vp);
      vpiHandle put_மதிப்பு_vector(p_vpi_மதிப்பு vp);
      vpiHandle put_மதிப்பு_time(p_vpi_மதிப்பு vp);

      vvp_vector4_t return_மதிப்பு_2;
};

முழு sysfunc_vec4::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiSize:
	    திரும்பு return_மதிப்பு_2.size();

	  நிலை vpi_வரி_No:
	    திரும்பு lineno;

	  நிலை vpiUserDefn:
	    திரும்பு defn->is_user_defn;

	  இயல்பு:
	    திரும்பு vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_மதிப்பு_scalar(மாறிலி p_vpi_மதிப்பு vp)
{
      தேர்வு (vp->value.scalar) {
	  நிலை vpi0:
	    return_மதிப்பு_2.set_bit(0, BIT4_0);
	    நிறுத்து;
	  நிலை vpi1:
	    return_மதிப்பு_2.set_bit(0, BIT4_1);
	    நிறுத்து;
	  நிலை vpiX:
	    return_மதிப்பு_2.set_bit(0, BIT4_X);
	    நிறுத்து;
	  நிலை vpiZ:
	    return_மதிப்பு_2.set_bit(0, BIT4_Z);
	    நிறுத்து;
	  இயல்பு:
	    fprintf(stderr, "Unsupported value %d.\n", (முழு)vp->value.scalar);
	    ப்ரதிஜ்ஞா(0);
      }
      திரும்பு 0;
}

vpiHandle sysfunc_vec4::put_மதிப்பு_int(p_vpi_மதிப்பு vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_மதிப்பு_2.size();
      க்கு (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_மதிப்பு_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      திரும்பு 0;
}

vpiHandle sysfunc_vec4::put_மதிப்பு_string(p_vpi_மதிப்பு vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_மதிப்பு_2.size();

      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    எனில் (cidx >= slen) {
		  return_மதிப்பு_2.set_bit(idx, BIT4_0);
		  தொடர்;
	    }

	      // Put the end of the string in the LSB of the vector
	    எழுத்து use_char = vp->value.str[slen - 1 - cidx];

	    எனில் ((use_char >> (idx % 8)) & 1)
		  return_மதிப்பு_2.set_bit(idx, BIT4_1);
	    இல்லையெனில்
		  return_மதிப்பு_2.set_bit(idx, BIT4_0);
      }
      திரும்பு 0;
}

vpiHandle sysfunc_vec4::put_மதிப்பு_vector(p_vpi_மதிப்பு vp)
{
      unsigned width = return_மதிப்பு_2.size();
      க்கு (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    முழு bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    தேர்வு (bit) {
		நிலை 0:
		  bit4 = BIT4_0;
		  நிறுத்து;
		நிலை 1:
		  bit4 = BIT4_1;
		  நிறுத்து;
		நிலை 2:
		  bit4 = BIT4_Z;
		  நிறுத்து;
		நிலை 3:
		  bit4 = BIT4_X;
		  நிறுத்து;
		இயல்பு:
		  ப்ரதிஜ்ஞா(0);
		  bit4 = BIT4_X;
		  நிறுத்து;
	    }
	    return_மதிப்பு_2.set_bit(idx, bit4);
      }
      திரும்பு 0;
}

vpiHandle sysfunc_vec4::put_மதிப்பு_time(p_vpi_மதிப்பு vp)
{
      unsigned width = return_மதிப்பு_2.size();
      long tmp = 0;
      க்கு (unsigned idx = 0 ; idx < width ; idx += 1) {
	    எனில் (idx == 0)
		  tmp = vp->value.time->கீழ்;
	    இல்லையெனில் எனில் (idx == 32)
		  tmp = vp->value.time->உயர்வு;
	    இல்லையெனில் எனில் (idx == 64)
		  tmp = 0;

	    return_மதிப்பு_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      திரும்பு 0;
}

vpiHandle sysfunc_vec4::vpi_put_மதிப்பு(p_vpi_மதிப்பு vp, முழு)
{
      put_மதிப்பு = உண்மை;

      தேர்வு (vp->format) {
	  நிலை vpiScalarVal:
	    திரும்பு put_மதிப்பு_scalar(vp);
	  நிலை vpiIntVal:
	    திரும்பு put_மதிப்பு_int(vp);
	  நிலை vpiStringVal:
	    திரும்பு put_மதிப்பு_string(vp);
	  நிலை vpiVectorVal:
	    திரும்பு put_மதிப்பு_vector(vp);
	  நிலை vpiTimeVal:
	    திரும்பு put_மதிப்பு_time(vp);
	  இயல்பு:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);
      }

      திரும்பு 0;
}

கட்டமைப்பு sysfunc_4net : பொது sysfunc_def {
      explicit உள்வரி sysfunc_4net(unsigned wid) : vwid_(wid) { }
      முழு vpi_get(முழு code) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;

    தனிப்பட்ட:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
முழு sysfunc_4net::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiSize:
	    திரும்பு vwid_;

	  நிலை vpi_வரி_No:
	    திரும்பு lineno;

	  நிலை vpiUserDefn:
	    திரும்பு defn->is_user_defn;

	  இயல்பு:
	    திரும்பு vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_மதிப்பு(p_vpi_மதிப்பு vp, முழு)
{
      put_மதிப்பு = உண்மை;

      vvp_vector4_t val (vwid_);

      தேர்வு (vp->format) {

          நிலை vpiScalarVal: {
	        தேர்வு(vp->value.scalar) {
		      நிலை vpi0:
			val.set_bit(0, BIT4_0);
                        நிறுத்து;
		      நிலை vpi1:
                        val.set_bit(0, BIT4_1);
                        நிறுத்து;
		      நிலை vpiX:
                        val.set_bit(0, BIT4_X);
                        நிறுத்து;
		      நிலை vpiZ:
                        val.set_bit(0, BIT4_Z);
                        நிறுத்து;
		      இயல்பு:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (முழு)vp->value.scalar);
                        ப்ரதிஜ்ஞா(0);
                }
		நிறுத்து;
          }

	  நிலை vpiIntVal: {
		long tmp = vp->value.integer;
		க்கு (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		நிறுத்து;
	  }

          நிலை vpiTimeVal: {
                unsigned long tmp = vp->value.time->கீழ்;
                க்கு (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      எனில் (idx == 31)
                            tmp = vp->value.time->உயர்வு;
                      இல்லையெனில்
                            tmp >>= 1;
                }
                நிறுத்து;
          }

	  நிலை vpiVectorVal:

	    க்கு (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  க்கு (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			முழு bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			தேர்வு (bit) {
			    நிலை 0:
			      bit4 = BIT4_0;
			      நிறுத்து;
			    நிலை 1:
			      bit4 = BIT4_1;
			      நிறுத்து;
			    நிலை 2:
			      bit4 = BIT4_Z;
			      நிறுத்து;
			    நிலை 3:
			      bit4 = BIT4_X;
			      நிறுத்து;
			    இயல்பு:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              bit);
			      ப்ரதிஜ்ஞா(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (முழு)vp->format, vwid_);
	    ப்ரதிஜ்ஞா(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      திரும்பு 0;
}

கட்டமைப்பு sysfunc_rnet : பொது sysfunc_def {
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_மதிப்பு(p_vpi_மதிப்பு vp, முழு)
{
      put_மதிப்பு = உண்மை;

      இரட்டை value;
      தேர்வு (vp->format) {

	  நிலை vpiRealVal:
	    value = vp->value.real;
	    நிறுத்து;

	  இயல்பு:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      திரும்பு 0;
}

கட்டமைப்பு sysfunc_no : பொது sysfunc_def {
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
};

vpiHandle sysfunc_no::vpi_put_மதிப்பு(p_vpi_மதிப்பு, முழு)
{
      திரும்பு 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
நிலையான கட்டமைப்பு __vpiUserSystf**def_table = 0;
நிலையான unsigned def_மொத்தம் = 0;

நிலையான கட்டமைப்பு __vpiUserSystf* allocate_def(வெறுமை)
{
      எனில் (def_table == 0) {
	    def_table = static_cast<கட்டமைப்பு __vpiUserSystf**>
	                (malloc(அளவு(கட்டமைப்பு __vpiUserSystf*)));

	    def_table[0] = புதிய __vpiUserSystf;

	    def_மொத்தம் = 1;
	    திரும்பு def_table[0];
      }

      def_table = static_cast<கட்டமைப்பு __vpiUserSystf**>
                  (realloc(def_table,
                   (def_மொத்தம்+1)*அளவு(கட்டமைப்பு __vpiUserSystf*)));

      def_table[def_மொத்தம்] = புதிய __vpiUserSystf;

      திரும்பு def_table[def_மொத்தம்++];
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை def_table_delete(வெறுமை)
{
      க்கு (unsigned idx = 0; idx < def_மொத்தம்; idx += 1) {
	    விடுவி(const_cast<எழுத்து *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      விடுவி(def_table);
      def_table = 0;
      def_மொத்தம் = 0;
}
#endif

வகுப்பு __vpiSystfIterator : பொது __vpiHandle {
    பொது:
      explicit __vpiSystfIterator(unsigned idx);
      முழு get_type_code(வெறுமை) மாறிலி override;
      vpiHandle vpi_index(முழு idx) override;
      free_object_fun_t free_object_fun(வெறுமை) override;

      unsigned next;
};

நிலையான vpiHandle systf_iterator_scan(vpiHandle ref, முழு)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      எனில் (obj->next >= def_மொத்தம்) {
	    vpi_free_object(ref);
	    திரும்பு 0;
      }

      unsigned use_index = obj->next;
      போது (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    எனில் (obj->next >= def_மொத்தம்) {
		  vpi_free_object(ref);
		  திரும்பு 0;
	    }
      }
      obj->next += 1;
      திரும்பு def_table[use_index];
}

நிலையான முழு systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      திரும்பு 1;
}

உள்வரி __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

முழு __vpiSystfIterator::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(முழு idx)
{ திரும்பு systf_iterator_scan(இது, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(வெறுமை)
{ திரும்பு &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(வெறுமை)
{
	/* Check to see if there are any user defined functions. */
      தருக்க have_user_defn = பொய்;
      unsigned idx;
      க்கு (idx = 0; idx < def_மொத்தம்; idx += 1) {
	    எனில் (def_table[idx]->is_user_defn) {
		  have_user_defn = உண்மை;
		  நிறுத்து;
	    }
      }
      எனில் (!have_user_defn) திரும்பு 0;

      __vpiSystfIterator*res = புதிய __vpiSystfIterator(idx);
      res->next = idx;
      திரும்பு res;
}

கட்டமைப்பு __vpiUserSystf* vpip_find_systf(மாறிலி எழுத்து*name)
{
      க்கு (unsigned idx = 0 ;  idx < def_மொத்தம் ;  idx += 1)
	    எனில் (strcmp(def_table[idx]->info.tfname, name) == 0)
		  திரும்பு def_table[idx];

      திரும்பு 0;
}

வெறுமை vpip_make_systf_system_defined(vpiHandle ref)
{
      ப்ரதிஜ்ஞா(ref);
      கட்டமைப்பு __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      ப்ரதிஜ்ஞா(obj);
      obj->is_user_defn = பொய்;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
எண்ணிக்கை vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
வகைவரை கட்டமைப்பு vpi_call_error {
      vpi_call_error_type type;
      எழுத்து *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

நிலையான vpi_call_error_p vpi_call_error_lst = வெற்று;
நிலையான unsigned vpi_call_error_இலக்கம் = 0;

நிலையான வெறுமை add_vpi_call_error(vpi_call_error_type type, மாறிலி எழுத்து *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_இலக்கம் + 1) *
                                    அளவு(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_இலக்கம்].type = type;
      vpi_call_error_lst[vpi_call_error_இலக்கம்].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_இலக்கம்].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_இலக்கம்].lineno = lineno;
      vpi_call_error_இலக்கம் += 1;
}

வெறுமை print_vpi_call_errors()
{
      க்கு (unsigned idx = 0; idx < vpi_call_error_இலக்கம்; idx += 1) {
	    தேர்வு (vpi_call_error_lst[idx].type) {
		நிலை VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (முழு)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  நிறுத்து;
		நிலை VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (முழு)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  நிறுத்து;
		நிலை VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (முழு)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  நிறுத்து;
		நிலை VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (முழு)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (முழு)vpi_call_error_lst[idx].lineno);
		  நிறுத்து;
	    }
	    விடுவி(vpi_call_error_lst[idx].name);
      }
      விடுவி(vpi_call_error_lst);
      fflush(stderr);
}

#ifdef CHECK_WITH_VALGRIND
நிலையான வெறுமை cleanup_vpi_call_args(unsigned argc, vpiHandle*argv)
{
      எனில் (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    கட்டமைப்பு __vpiSysTaskCall*obj = புதிய systask_def;
	    obj->nargs = argc;
	    obj->args  = argv;
	    vpi_call_delete(obj);
      }
}
#endif

/*
 * A vpi_call is actually built up into a vpiSysTaskCall VPI object
 * that refers back to the vpiUserSystf VPI object that is the
 * definition. So this function is called by the compiler when a
 * %vpi_call statement is encountered. Create here a vpiHandle that
 * describes the call, and return it. The %vpi_call instruction will
 * store this handle for when it is executed.
 */
vpiHandle vpip_build_vpi_call(மாறிலி எழுத்து*name, முழு val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      தருக்க func_as_task_err, தருக்க func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_அடுக்கு, unsigned real_அடுக்கு, unsigned string_அடுக்கு,
			      long file_idx, long lineno)
{
      ப்ரதிஜ்ஞா(!(func_as_task_err && func_as_task_warn));

      கட்டமைப்பு __vpiUserSystf*defn = vpip_find_systf(name);
      எனில் (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    திரும்பு 0;
      }

      தேர்வு (defn->info.type) {
	  நிலை vpiSysTask:
	    எனில் (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  திரும்பு 0;
	    }
	    நிறுத்து;

	  நிலை vpiSysFunc:
	    எனில் (val_code == 0 && fnet == 0) {
		  எனில் (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			திரும்பு 0;
		  } இல்லையெனில் எனில் (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (முழு)defn->info.type);
	    ப்ரதிஜ்ஞா(0);
      }

      கட்டமைப்பு __vpiSysTaskCall*obj = 0;

      தேர்வு (defn->info.type) {
	  நிலை vpiSysTask:
	    obj = புதிய systask_def;
	    நிறுத்து;

	  நிலை vpiSysFunc:
	    எனில் (fnet && val_code == -vpiRealVal) {
		  obj = புதிய sysfunc_rnet;

	    } இல்லையெனில் எனில் (fnet && val_code == -vpiVectorVal) {
		  obj = புதிய sysfunc_4net(return_width);

	    } இல்லையெனில் எனில் (val_code == -vpiRealVal) {
		  obj = புதிய sysfunc_real;

	    } இல்லையெனில் எனில் (val_code == -vpiVectorVal) {
		  obj = புதிய sysfunc_vec4(return_width);

	    } இல்லையெனில் எனில் (val_code == -vpiStringVal) {
		  obj = புதிய sysfunc_str;

           } இல்லையெனில் எனில் (val_code == 0 && fnet == 0) {
		  obj = புதிய sysfunc_no;

	    } இல்லையெனில் {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  ப்ரதிஜ்ஞா(0);
	    }
	    நிறுத்து;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_அடுக்கு = vec4_அடுக்கு;
      obj->real_அடுக்கு = real_அடுக்கு;
      obj->string_அடுக்கு = string_அடுக்கு;
      obj->fnet  = fnet;
      obj->file_idx  = (unsigned) file_idx;
      obj->lineno   = (unsigned) lineno;
      obj->userdata  = 0;
      obj->put_மதிப்பு = பொய்;

      compile_compiletf(obj);

      திரும்பு obj;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vpi_call_delete(vpiHandle item)
{
      கட்டமைப்பு __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      எனில் (!obj) திரும்பு;
      க்கு (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    தேர்வு (obj->args[arg]->get_type_code()) {
		நிலை vpiConstant:
		  தேர்வு (vpi_get(_vpiFromThr, obj->args[arg])) {
		      நிலை _vpiNoThr:
			constant_delete(obj->args[arg]);
			நிறுத்து;
		      நிலை _vpiString:
			thread_string_delete(obj->args[arg]);
			நிறுத்து;
		      நிலை _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			நிறுத்து;
		      நிலை _vpiWord:
			thread_word_delete(obj->args[arg]);
			நிறுத்து;
		      இயல்பு:
			ப்ரதிஜ்ஞா(0);
		  }
		  நிறுத்து;
		நிலை vpiMemoryWord:
		  தேர்வு (vpi_get(_vpiFromThr, obj->args[arg])) {
		      நிலை _vpi_at_A:
			A_delete(obj->args[arg]);
			நிறுத்து;
		  }
		  நிறுத்து;
		நிலை vpiPartSelect:
		  தேர்வு (vpi_get(_vpiFromThr, obj->args[arg])) {
		      நிலை _vpi_at_PV:
			PV_delete(obj->args[arg]);
			நிறுத்து;
		      நிலை _vpi_at_APV:
			APV_delete(obj->args[arg]);
			நிறுத்து;
		  }
		  நிறுத்து;
	    }
      }
      விடுவி(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

வெறுமை vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      எனில் (vpip_cur_task->defn->info.calltf) {
	    ப்ரதிஜ்ஞா(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_மதிப்பு = பொய்;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    எனில் (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_மதிப்பு) {
		  s_vpi_மதிப்பு val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_மதிப்பு(ref, &val, 0, vpiNoDelay);
	    }
      }
      எனில் (vpip_cur_task->vec4_அடுக்கு > 0)
	    vthread_எடு_vec4(thr, vpip_cur_task->vec4_அடுக்கு);
      எனில் (vpip_cur_task->real_அடுக்கு > 0)
	    vthread_எடு_real(thr, vpip_cur_task->real_அடுக்கு);
      எனில் (vpip_cur_task->string_அடுக்கு > 0)
	    vthread_எடு_str(thr, vpip_cur_task->string_அடுக்கு);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      எனில் (மாறிலி sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_தள்ளு(thr, func_real->return_மதிப்பு());
      }
      இல்லையெனில் எனில் (மாறிலி sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_தள்ளு(thr, func_string->return_மதிப்பு());
      }
      இல்லையெனில் எனில் (மாறிலி sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_தள்ளு(thr, func_vec4->return_மதிப்பு());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(மாறிலி கட்டமைப்பு t_vpi_systf_data*ss)
{
      கட்டமைப்பு __vpiUserSystf*cur = allocate_def();
      ப்ரதிஜ்ஞா(ss);
      தேர்வு (ss->type) {
	  நிலை vpiSysTask:
	  நிலை vpiSysFunc:
	    நிறுத்து;
	  இயல்பு:
	    fprintf(stderr, "Unsupported type %d.\n", (முழு)ss->type);
	    ப்ரதிஜ்ஞா(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = உண்மை;

      திரும்பு cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, வெறுமை*data)
{
      கட்டமைப்பு __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      எனில் (rfp == 0)
	    திரும்பு 0;

      rfp->userdata = data;
      திரும்பு 1;
}

வெறுமை* vpi_get_userdata(vpiHandle ref)
{
      கட்டமைப்பு __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      திரும்பு rfp->userdata;
}
