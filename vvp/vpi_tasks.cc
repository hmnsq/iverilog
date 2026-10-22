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

using नामाकाश std;

ओळीत __vpiUserSystf::__vpiUserSystf()
{ }

पूर्णांक __vpiUserSystf::get_type_code(शून्य) स्थिरांक
{ परत vpiUserSystf; }


स्थिर vpiHandle systask_handle(पूर्णांक type, vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      निवड (type) {
	  स्थिती vpiScope:
	    परत rfp->scope;

	  स्थिती vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    प्रतिज्ञा(rfp->defn->is_user_defn);
	    परत rfp->defn;

	  पूर्वनिर्धारित:
	    परत 0;
      };
}

स्थिर पूर्णांक systask_get(पूर्णांक type, vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      निवड (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  स्थिती vpiTimeUnit:
	    परत rfp->scope->time_units;
	  स्थिती vpiTimePrecision:
	    परत rfp->scope->time_precision;

	  स्थिती vpi_ओळ_No:
	    परत rfp->lineno;

	  स्थिती vpiUserDefn:
	    परत rfp->defn->is_user_defn;

	  पूर्वनिर्धारित:
	    परत vpiUndefined;
      }
}

// support getting vpiSize for a system function call
स्थिर पूर्णांक sysfunc_get(पूर्णांक type, vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      निवड (type) {
	  स्थिती vpiSize:
	    प्रतिज्ञा(0); // This should be handled by derived classes
	    परत 0;

	  स्थिती vpi_ओळ_No:
	    परत rfp->lineno;

	  स्थिती vpiUserDefn:
	    परत rfp->defn->is_user_defn;

	  पूर्वनिर्धारित:
	    परत vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

स्थिर अक्षर *systask_get_str(पूर्णांक type, vpiHandle ref)
{
      स्थिरांक संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      निवड (type) {
          स्थिती vpiFile:
            प्रतिज्ञा(rfp->file_idx < file_names.size());
            परत simple_set_rbuf_str(file_names[rfp->file_idx]);

          स्थिती vpiName:
            परत simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      परत 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
स्थिर vpiHandle systask_iter(पूर्णांक, vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      जर (rfp->nargs == 0)
	    परत 0;

      परत vpip_make_iterator(rfp->nargs, rfp->args, असत्य);
}

संरचना systask_def : सार्वजनिक __vpiSysTaskCall {
      आभासी पूर्णांक get_type_code(शून्य) स्थिरांक   override { परत vpiSysTaskCall; }
      आभासी पूर्णांक vpi_get(पूर्णांक code)           override { परत systask_get(code, हे); }
      आभासी अक्षर*vpi_get_str(पूर्णांक code)      override { परत systask_get_str(code, हे); }
      आभासी vpiHandle vpi_handle(पूर्णांक code)  override { परत systask_handle(code, हे); }
      आभासी vpiHandle vpi_iterate(पूर्णांक code) override { परत systask_iter(code, हे); }
};

संरचना sysfunc_def : सार्वजनिक systask_def {
      आभासी पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiSysFuncCall; }
      आभासी पूर्णांक vpi_get(पूर्णांक code) override { परत sysfunc_get(code, हे); }
};

संरचना sysfunc_real : सार्वजनिक sysfunc_def {
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      ओळीत द्विगुण return_मूल्य() स्थिरांक { परत return_मूल्य_2; }

    खाजगी:
      द्विगुण return_मूल्य_2;
};

vpiHandle sysfunc_real::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = सत्य;

      return_मूल्य_2 = 0.0;
      निवड (vp->format) {
	  स्थिती vpiRealVal:
	    return_मूल्य_2 = vp->value.real;
	    थांबा;
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Unsupported format %d.\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);
      }

      परत 0;
}

संरचना sysfunc_str : सार्वजनिक sysfunc_def {
      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      ओळीत स्थिरांक std::string& return_मूल्य() स्थिरांक { परत return_मूल्य_2; };

    खाजगी:
      std::string return_मूल्य_2;
};

पूर्णांक sysfunc_str::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiSize:
	    परत return_मूल्य_2.size();

	  स्थिती vpi_ओळ_No:
	    परत lineno;

	  स्थिती vpiUserDefn:
	    परत defn->is_user_defn;

	  पूर्वनिर्धारित:
	    परत vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = सत्य;

      निवड (vp->format) {
	  स्थिती vpiStringVal:
	    return_मूल्य_2 = std::string(vp->value.str);
	    थांबा;
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Unsupported format %d.\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);
      }

      परत 0;
}

वर्ग sysfunc_vec4 : सार्वजनिक sysfunc_def {
    सार्वजनिक:
      explicit ओळीत sysfunc_vec4(अचिन्हित wid): return_मूल्य_2(wid, BIT4_X) { }
      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      ओळीत स्थिरांक vvp_vector4_t& return_मूल्य() स्थिरांक { परत return_मूल्य_2; }

    खाजगी:
      vpiHandle put_मूल्य_scalar(p_vpi_मूल्य vp);
      vpiHandle put_मूल्य_int(p_vpi_मूल्य vp);
      vpiHandle put_मूल्य_string(p_vpi_मूल्य vp);
      vpiHandle put_मूल्य_vector(p_vpi_मूल्य vp);
      vpiHandle put_मूल्य_time(p_vpi_मूल्य vp);

      vvp_vector4_t return_मूल्य_2;
};

पूर्णांक sysfunc_vec4::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiSize:
	    परत return_मूल्य_2.size();

	  स्थिती vpi_ओळ_No:
	    परत lineno;

	  स्थिती vpiUserDefn:
	    परत defn->is_user_defn;

	  पूर्वनिर्धारित:
	    परत vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_मूल्य_scalar(स्थिरांक p_vpi_मूल्य vp)
{
      निवड (vp->value.scalar) {
	  स्थिती vpi0:
	    return_मूल्य_2.set_bit(0, BIT4_0);
	    थांबा;
	  स्थिती vpi1:
	    return_मूल्य_2.set_bit(0, BIT4_1);
	    थांबा;
	  स्थिती vpiX:
	    return_मूल्य_2.set_bit(0, BIT4_X);
	    थांबा;
	  स्थिती vpiZ:
	    return_मूल्य_2.set_bit(0, BIT4_Z);
	    थांबा;
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Unsupported value %d.\n", (पूर्णांक)vp->value.scalar);
	    प्रतिज्ञा(0);
      }
      परत 0;
}

vpiHandle sysfunc_vec4::put_मूल्य_int(p_vpi_मूल्य vp)
{
      दीर्घ tmp = vp->value.integer;
      अचिन्हित width = return_मूल्य_2.size();
      साठी (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
	    return_मूल्य_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      परत 0;
}

vpiHandle sysfunc_vec4::put_मूल्य_string(p_vpi_मूल्य vp)
{
      माप_प्रकार slen = म_माप(vp->value.str);
      अचिन्हित wid = return_मूल्य_2.size();

      साठी (अचिन्हित idx = 0 ; idx < wid ; idx += 1) {
	    अचिन्हित cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    जर (cidx >= slen) {
		  return_मूल्य_2.set_bit(idx, BIT4_0);
		  सुरूठेवा;
	    }

	      // Put the end of the string in the LSB of the vector
	    अक्षर use_char = vp->value.str[slen - 1 - cidx];

	    जर ((use_char >> (idx % 8)) & 1)
		  return_मूल्य_2.set_bit(idx, BIT4_1);
	    अन्यथा
		  return_मूल्य_2.set_bit(idx, BIT4_0);
      }
      परत 0;
}

vpiHandle sysfunc_vec4::put_मूल्य_vector(p_vpi_मूल्य vp)
{
      अचिन्हित width = return_मूल्य_2.size();
      साठी (अचिन्हित idx = 0 ; idx < width ; idx += 1) {
	    अचिन्हित word = idx / 32;
	    अचिन्हित bidx = idx % 32;

	    अचिन्हित दीर्घ aval = vp->value.vector[word].aval >> bidx;
	    अचिन्हित दीर्घ bval = vp->value.vector[word].bval >> bidx;

	    पूर्णांक bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    निवड (bit) {
		स्थिती 0:
		  bit4 = BIT4_0;
		  थांबा;
		स्थिती 1:
		  bit4 = BIT4_1;
		  थांबा;
		स्थिती 2:
		  bit4 = BIT4_Z;
		  थांबा;
		स्थिती 3:
		  bit4 = BIT4_X;
		  थांबा;
		पूर्वनिर्धारित:
		  प्रतिज्ञा(0);
		  bit4 = BIT4_X;
		  थांबा;
	    }
	    return_मूल्य_2.set_bit(idx, bit4);
      }
      परत 0;
}

vpiHandle sysfunc_vec4::put_मूल्य_time(p_vpi_मूल्य vp)
{
      अचिन्हित width = return_मूल्य_2.size();
      दीर्घ tmp = 0;
      साठी (अचिन्हित idx = 0 ; idx < width ; idx += 1) {
	    जर (idx == 0)
		  tmp = vp->value.समय->खालचा;
	    अन्यथा जर (idx == 32)
		  tmp = vp->value.समय->वरचा;
	    अन्यथा जर (idx == 64)
		  tmp = 0;

	    return_मूल्य_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      परत 0;
}

vpiHandle sysfunc_vec4::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = सत्य;

      निवड (vp->format) {
	  स्थिती vpiScalarVal:
	    परत put_मूल्य_scalar(vp);
	  स्थिती vpiIntVal:
	    परत put_मूल्य_int(vp);
	  स्थिती vpiStringVal:
	    परत put_मूल्य_string(vp);
	  स्थिती vpiVectorVal:
	    परत put_मूल्य_vector(vp);
	  स्थिती vpiTimeVal:
	    परत put_मूल्य_time(vp);
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Unsupported format %d setting sysfunc vec4 value.\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);
      }

      परत 0;
}

संरचना sysfunc_4net : सार्वजनिक sysfunc_def {
      explicit ओळीत sysfunc_4net(अचिन्हित wid) : vwid_(wid) { }
      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;

    खाजगी:
      अचिन्हित vwid_;
};

// support getting vpiSize for a system function call
पूर्णांक sysfunc_4net::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiSize:
	    परत vwid_;

	  स्थिती vpi_ओळ_No:
	    परत lineno;

	  स्थिती vpiUserDefn:
	    परत defn->is_user_defn;

	  पूर्वनिर्धारित:
	    परत vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = सत्य;

      vvp_vector4_t val (vwid_);

      निवड (vp->format) {

          स्थिती vpiScalarVal: {
	        निवड(vp->value.scalar) {
		      स्थिती vpi0:
			val.set_bit(0, BIT4_0);
                        थांबा;
		      स्थिती vpi1:
                        val.set_bit(0, BIT4_1);
                        थांबा;
		      स्थिती vpiX:
                        val.set_bit(0, BIT4_X);
                        थांबा;
		      स्थिती vpiZ:
                        val.set_bit(0, BIT4_Z);
                        थांबा;
		      पूर्वनिर्धारित:
                        ख_लिखो(मानक_त्रुटि, "Unsupported bit value %d.\n",
                                (पूर्णांक)vp->value.scalar);
                        प्रतिज्ञा(0);
                }
		थांबा;
          }

	  स्थिती vpiIntVal: {
		दीर्घ tmp = vp->value.integer;
		साठी (अचिन्हित idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		थांबा;
	  }

          स्थिती vpiTimeVal: {
                अचिन्हित दीर्घ tmp = vp->value.समय->खालचा;
                साठी (अचिन्हित idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      जर (idx == 31)
                            tmp = vp->value.समय->वरचा;
                      अन्यथा
                            tmp >>= 1;
                }
                थांबा;
          }

	  स्थिती vpiVectorVal:

	    साठी (अचिन्हित wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  अचिन्हित word = wdx / 32;
		  अचिन्हित दीर्घ aval = vp->value.vector[word].aval;
		  अचिन्हित दीर्घ bval = vp->value.vector[word].bval;

		  साठी (अचिन्हित idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			पूर्णांक bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			निवड (bit) {
			    स्थिती 0:
			      bit4 = BIT4_0;
			      थांबा;
			    स्थिती 1:
			      bit4 = BIT4_1;
			      थांबा;
			    स्थिती 2:
			      bit4 = BIT4_Z;
			      थांबा;
			    स्थिती 3:
			      bit4 = BIT4_X;
			      थांबा;
			    पूर्वनिर्धारित:
			      bit4 = BIT4_X;
			      ख_लिखो(मानक_त्रुटि, "Unsupported bit value %d.\n",
			              bit);
			      प्रतिज्ञा(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "XXXX format=%d, vwid_=%u\n", (पूर्णांक)vp->format, vwid_);
	    प्रतिज्ञा(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      परत 0;
}

संरचना sysfunc_rnet : सार्वजनिक sysfunc_def {
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = सत्य;

      द्विगुण value;
      निवड (vp->format) {

	  स्थिती vpiRealVal:
	    value = vp->value.real;
	    थांबा;

	  पूर्वनिर्धारित:
	    value = 0.0;
	    ख_लिखो(मानक_त्रुटि, "Unsupported format %d.\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      परत 0;
}

संरचना sysfunc_no : सार्वजनिक sysfunc_def {
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
};

vpiHandle sysfunc_no::vpi_put_मूल्य(p_vpi_मूल्य, पूर्णांक)
{
      परत 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
स्थिर संरचना __vpiUserSystf**def_table = 0;
स्थिर अचिन्हित def_मोजणी = 0;

स्थिर संरचना __vpiUserSystf* allocate_def(शून्य)
{
      जर (def_table == 0) {
	    def_table = static_cast<संरचना __vpiUserSystf**>
	                (दो_स्मृति(आकार(संरचना __vpiUserSystf*)));

	    def_table[0] = नवीन __vpiUserSystf;

	    def_मोजणी = 1;
	    परत def_table[0];
      }

      def_table = static_cast<संरचना __vpiUserSystf**>
                  (पुनः_स्मृति(def_table,
                   (def_मोजणी+1)*आकार(संरचना __vpiUserSystf*)));

      def_table[def_मोजणी] = नवीन __vpiUserSystf;

      परत def_table[def_मोजणी++];
}

#ifdef CHECK_WITH_VALGRIND
शून्य def_table_delete(शून्य)
{
      साठी (अचिन्हित idx = 0; idx < def_मोजणी; idx += 1) {
	    मुक्त(const_cast<अक्षर *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      मुक्त(def_table);
      def_table = 0;
      def_मोजणी = 0;
}
#endif

वर्ग __vpiSystfIterator : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      explicit __vpiSystfIterator(अचिन्हित idx);
      पूर्णांक get_type_code(शून्य) स्थिरांक override;
      vpiHandle vpi_index(पूर्णांक idx) override;
      free_object_fun_t free_object_fun(शून्य) override;

      अचिन्हित next;
};

स्थिर vpiHandle systf_iterator_scan(vpiHandle ref, पूर्णांक)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      जर (obj->next >= def_मोजणी) {
	    vpi_free_object(ref);
	    परत 0;
      }

      अचिन्हित use_index = obj->next;
      पर्यंत (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    जर (obj->next >= def_मोजणी) {
		  vpi_free_object(ref);
		  परत 0;
	    }
      }
      obj->next += 1;
      परत def_table[use_index];
}

स्थिर पूर्णांक systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      परत 1;
}

ओळीत __vpiSystfIterator::__vpiSystfIterator(अचिन्हित idx)
{ next = idx; }

पूर्णांक __vpiSystfIterator::get_type_code(शून्य) स्थिरांक
{ परत vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(पूर्णांक idx)
{ परत systf_iterator_scan(हे, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(शून्य)
{ परत &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(शून्य)
{
	/* Check to see if there are any user defined functions. */
      तार्किक have_user_defn = असत्य;
      अचिन्हित idx;
      साठी (idx = 0; idx < def_मोजणी; idx += 1) {
	    जर (def_table[idx]->is_user_defn) {
		  have_user_defn = सत्य;
		  थांबा;
	    }
      }
      जर (!have_user_defn) परत 0;

      __vpiSystfIterator*res = नवीन __vpiSystfIterator(idx);
      res->next = idx;
      परत res;
}

संरचना __vpiUserSystf* vpip_find_systf(स्थिरांक अक्षर*name)
{
      साठी (अचिन्हित idx = 0 ;  idx < def_मोजणी ;  idx += 1)
	    जर (म_भेद(def_table[idx]->info.tfname, name) == 0)
		  परत def_table[idx];

      परत 0;
}

शून्य vpip_make_systf_system_defined(vpiHandle ref)
{
      प्रतिज्ञा(ref);
      संरचना __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      प्रतिज्ञा(obj);
      obj->is_user_defn = असत्य;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
गणना vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
प्रकारव्याख्या संरचना vpi_call_error {
      vpi_call_error_type type;
      अक्षर *name;
      दीर्घ file_idx;
      दीर्घ lineno;
} vpi_call_error_s, *vpi_call_error_p;

स्थिर vpi_call_error_p vpi_call_error_lst = रिक्त;
स्थिर अचिन्हित vpi_call_error_अंक = 0;

स्थिर शून्य add_vpi_call_error(vpi_call_error_type type, स्थिरांक अक्षर *name,
                               दीर्घ file_idx, दीर्घ lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (पुनः_स्मृति(vpi_call_error_lst,
                                    (vpi_call_error_अंक + 1) *
                                    आकार(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_अंक].type = type;
      vpi_call_error_lst[vpi_call_error_अंक].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_अंक].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_अंक].lineno = lineno;
      vpi_call_error_अंक += 1;
}

शून्य print_vpi_call_errors()
{
      साठी (अचिन्हित idx = 0; idx < vpi_call_error_अंक; idx += 1) {
	    निवड (vpi_call_error_lst[idx].type) {
		स्थिती VPI_CALL_NO_DEF:
		  ख_लिखो(मानक_त्रुटि, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णांक)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  थांबा;
		स्थिती VPI_CALL_TASK_AS_FUNC:
		  ख_लिखो(मानक_त्रुटि, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णांक)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  थांबा;
		स्थिती VPI_CALL_FUNC_AS_TASK:
		  ख_लिखो(मानक_त्रुटि, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णांक)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  थांबा;
		स्थिती VPI_CALL_FUNC_AS_TASK_WARN:
                 ख_लिखो(मानक_त्रुटि, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णांक)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 ख_लिखो(मानक_त्रुटि, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (पूर्णांक)vpi_call_error_lst[idx].lineno);
		  थांबा;
	    }
	    मुक्त(vpi_call_error_lst[idx].name);
      }
      मुक्त(vpi_call_error_lst);
      ख_साफ(मानक_त्रुटि);
}

#ifdef CHECK_WITH_VALGRIND
स्थिर शून्य cleanup_vpi_call_args(अचिन्हित argc, vpiHandle*argv)
{
      जर (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    संरचना __vpiSysTaskCall*obj = नवीन systask_def;
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
vpiHandle vpip_build_vpi_call(स्थिरांक अक्षर*name, पूर्णांक val_code, अचिन्हित return_width,
			      vvp_net_t*fnet,
			      तार्किक func_as_task_err, तार्किक func_as_task_warn,
			      अचिन्हित argc, vpiHandle*argv,
			      अचिन्हित vec4_चळत, अचिन्हित real_चळत, अचिन्हित string_चळत,
			      दीर्घ file_idx, दीर्घ lineno)
{
      प्रतिज्ञा(!(func_as_task_err && func_as_task_warn));

      संरचना __vpiUserSystf*defn = vpip_find_systf(name);
      जर (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    परत 0;
      }

      निवड (defn->info.type) {
	  स्थिती vpiSysTask:
	    जर (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  परत 0;
	    }
	    थांबा;

	  स्थिती vpiSysFunc:
	    जर (val_code == 0 && fnet == 0) {
		  जर (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			परत 0;
		  } अन्यथा जर (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Unsupported vpi_call type %d.\n",
	                    (पूर्णांक)defn->info.type);
	    प्रतिज्ञा(0);
      }

      संरचना __vpiSysTaskCall*obj = 0;

      निवड (defn->info.type) {
	  स्थिती vpiSysTask:
	    obj = नवीन systask_def;
	    थांबा;

	  स्थिती vpiSysFunc:
	    जर (fnet && val_code == -vpiRealVal) {
		  obj = नवीन sysfunc_rnet;

	    } अन्यथा जर (fnet && val_code == -vpiVectorVal) {
		  obj = नवीन sysfunc_4net(return_width);

	    } अन्यथा जर (val_code == -vpiRealVal) {
		  obj = नवीन sysfunc_real;

	    } अन्यथा जर (val_code == -vpiVectorVal) {
		  obj = नवीन sysfunc_vec4(return_width);

	    } अन्यथा जर (val_code == -vpiStringVal) {
		  obj = नवीन sysfunc_str;

           } अन्यथा जर (val_code == 0 && fnet == 0) {
		  obj = नवीन sysfunc_no;

	    } अन्यथा {
		  ख_लिखो(मानक_त्रुटि, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  प्रतिज्ञा(0);
	    }
	    थांबा;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_चळत = vec4_चळत;
      obj->real_चळत = real_चळत;
      obj->string_चळत = string_चळत;
      obj->fnet  = fnet;
      obj->file_idx  = (अचिन्हित) file_idx;
      obj->lineno   = (अचिन्हित) lineno;
      obj->userdata  = 0;
      obj->put_मूल्य = असत्य;

      compile_compiletf(obj);

      परत obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य vpi_call_delete(vpiHandle item)
{
      संरचना __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      जर (!obj) परत;
      साठी (अचिन्हित arg = 0; arg < obj->nargs; arg += 1) {
	    निवड (obj->args[arg]->get_type_code()) {
		स्थिती vpiConstant:
		  निवड (vpi_get(_vpiFromThr, obj->args[arg])) {
		      स्थिती _vpiNoThr:
			constant_delete(obj->args[arg]);
			थांबा;
		      स्थिती _vpiString:
			thread_string_delete(obj->args[arg]);
			थांबा;
		      स्थिती _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			थांबा;
		      स्थिती _vpiWord:
			thread_word_delete(obj->args[arg]);
			थांबा;
		      पूर्वनिर्धारित:
			प्रतिज्ञा(0);
		  }
		  थांबा;
		स्थिती vpiMemoryWord:
		  निवड (vpi_get(_vpiFromThr, obj->args[arg])) {
		      स्थिती _vpi_at_A:
			A_delete(obj->args[arg]);
			थांबा;
		  }
		  थांबा;
		स्थिती vpiPartSelect:
		  निवड (vpi_get(_vpiFromThr, obj->args[arg])) {
		      स्थिती _vpi_at_PV:
			PV_delete(obj->args[arg]);
			थांबा;
		      स्थिती _vpi_at_APV:
			APV_delete(obj->args[arg]);
			थांबा;
		  }
		  थांबा;
	    }
      }
      मुक्त(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

शून्य vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      जर (vpip_cur_task->defn->info.calltf) {
	    प्रतिज्ञा(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_मूल्य = असत्य;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    जर (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_मूल्य) {
		  s_vpi_मूल्य val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_मूल्य(ref, &val, 0, vpiNoDelay);
	    }
      }
      जर (vpip_cur_task->vec4_चळत > 0)
	    vthread_काढा_vec4(thr, vpip_cur_task->vec4_चळत);
      जर (vpip_cur_task->real_चळत > 0)
	    vthread_काढा_real(thr, vpip_cur_task->real_चळत);
      जर (vpip_cur_task->string_चळत > 0)
	    vthread_काढा_str(thr, vpip_cur_task->string_चळत);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      जर (स्थिरांक sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ढकला(thr, func_real->return_मूल्य());
      }
      अन्यथा जर (स्थिरांक sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ढकला(thr, func_string->return_मूल्य());
      }
      अन्यथा जर (स्थिरांक sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_ढकला(thr, func_vec4->return_मूल्य());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(स्थिरांक संरचना t_vpi_systf_data*ss)
{
      संरचना __vpiUserSystf*cur = allocate_def();
      प्रतिज्ञा(ss);
      निवड (ss->type) {
	  स्थिती vpiSysTask:
	  स्थिती vpiSysFunc:
	    थांबा;
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Unsupported type %d.\n", (पूर्णांक)ss->type);
	    प्रतिज्ञा(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = सत्य;

      परत cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, शून्य*data)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      जर (rfp == 0)
	    परत 0;

      rfp->userdata = data;
      परत 1;
}

शून्य* vpi_get_userdata(vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      प्रतिज्ञा(rfp);

      परत rfp->userdata;
}
