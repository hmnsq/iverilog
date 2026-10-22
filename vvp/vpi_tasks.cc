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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#यदि_अन्तः
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

उपयुज्य नामाकाश std;

अन्तःस्थ __vpiUserSystf::__vpiUserSystf()
{ }

पूर्णाङ्क __vpiUserSystf::get_type_संहिता(शून्य) ध्रुव
{ प्रत्यागम vpiUserSystf; }


स्थिर vpiHandle systask_handle(पूर्णाङ्क type, vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      विकल्प (type) {
	  पक्ष vpiScope:
	    प्रत्यागम rfp->scope;

	  पक्ष vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    प्रतिज्ञा(rfp->defn->is_user_defn);
	    प्रत्यागम rfp->defn;

	  स्वभाव:
	    प्रत्यागम 0;
      };
}

स्थिर पूर्णाङ्क systask_get(पूर्णाङ्क type, vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      विकल्प (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  पक्ष vpiTimeUnit:
	    प्रत्यागम rfp->scope->time_units;
	  पक्ष vpiTimePrecision:
	    प्रत्यागम rfp->scope->time_precision;

	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम rfp->lineno;

	  पक्ष vpiUserDefn:
	    प्रत्यागम rfp->defn->is_user_defn;

	  स्वभाव:
	    प्रत्यागम vpiUndefined;
      }
}

// support getting vpiSize for a system function call
स्थिर पूर्णाङ्क sysfunc_get(पूर्णाङ्क type, vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      विकल्प (type) {
	  पक्ष vpi_आकार:
	    प्रतिज्ञा(0); // This should be handled by derived classes
	    प्रत्यागम 0;

	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम rfp->lineno;

	  पक्ष vpiUserDefn:
	    प्रत्यागम rfp->defn->is_user_defn;

	  स्वभाव:
	    प्रत्यागम vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

स्थिर वर्ण *systask_get_str(पूर्णाङ्क type, vpiHandle ref)
{
      ध्रुव संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      विकल्प (type) {
          पक्ष vpiFile:
            प्रतिज्ञा(rfp->file_idx < file_नामानि.size());
            प्रत्यागम simple_समुच्चयः_rbuf_str(file_नामानि[rfp->file_idx]);

          पक्ष vpiName:
            प्रत्यागम simple_समुच्चयः_rbuf_str(rfp->defn->info.tfname);
      }

      प्रत्यागम 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
स्थिर vpiHandle systask_iter(पूर्णाङ्क, vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      यदि (rfp->nargs == 0)
	    प्रत्यागम 0;

      प्रत्यागम vpip_make_iterator(rfp->nargs, rfp->तर्काः, असत्य);
}

संरचना systask_def : सार्वजनिक __vpiSysTaskCall {
      आभास पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव   override { प्रत्यागम vpiSysTaskCall; }
      आभास पूर्णाङ्क vpi_get(पूर्णाङ्क code)           override { प्रत्यागम systask_get(code, एतत्); }
      आभास वर्ण*vpi_get_str(पूर्णाङ्क code)      override { प्रत्यागम systask_get_str(code, एतत्); }
      आभास vpiHandle vpi_handle(पूर्णाङ्क code)  override { प्रत्यागम systask_handle(code, एतत्); }
      आभास vpiHandle vpi_iterate(पूर्णाङ्क code) override { प्रत्यागम systask_iter(code, एतत्); }
};

संरचना sysfunc_def : सार्वजनिक systask_def {
      आभास पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiSysFuncCall; }
      आभास पूर्णाङ्क vpi_get(पूर्णाङ्क code) override { प्रत्यागम sysfunc_get(code, एतत्); }
};

संरचना sysfunc_real : सार्वजनिक sysfunc_def {
      vpiHandle vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags) override;
      अन्तःस्थ द्विगुण return_मानम्() ध्रुव { प्रत्यागम return_मानम्_2; }

    गुप्त:
      द्विगुण return_मानम्_2;
};

vpiHandle sysfunc_real::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क)
{
      put_मानम् = सत्य;

      return_मानम्_2 = 0.0;
      विकल्प (vp->format) {
	  पक्ष vpiRealVal:
	    return_मानम्_2 = vp->value.real;
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "Unsupported format %d.\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);
      }

      प्रत्यागम 0;
}

संरचना sysfunc_str : सार्वजनिक sysfunc_def {
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      vpiHandle vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags) override;
      अन्तःस्थ ध्रुव std::string& return_मानम्() ध्रुव { प्रत्यागम return_मानम्_2; };

    गुप्त:
      std::string return_मानम्_2;
};

पूर्णाङ्क sysfunc_str::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_आकार:
	    प्रत्यागम return_मानम्_2.size();

	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम lineno;

	  पक्ष vpiUserDefn:
	    प्रत्यागम defn->is_user_defn;

	  स्वभाव:
	    प्रत्यागम vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क)
{
      put_मानम् = सत्य;

      विकल्प (vp->format) {
	  पक्ष vpiStringVal:
	    return_मानम्_2 = std::string(vp->value.str);
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "Unsupported format %d.\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);
      }

      प्रत्यागम 0;
}

वर्ग sysfunc_vec4 : सार्वजनिक sysfunc_def {
    सार्वजनिक:
      स्पष्ट अन्तःस्थ sysfunc_vec4(अचिह्नित wid): return_मानम्_2(wid, BIT4_X) { }
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      vpiHandle vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags) override;
      अन्तःस्थ ध्रुव vvp_vector4_t& return_मानम्() ध्रुव { प्रत्यागम return_मानम्_2; }

    गुप्त:
      vpiHandle put_मानम्_scalar(p_vpi_मानम् vp);
      vpiHandle put_मानम्_int(p_vpi_मानम् vp);
      vpiHandle put_मानम्_string(p_vpi_मानम् vp);
      vpiHandle put_मानम्_सदिशम्(p_vpi_मानम् vp);
      vpiHandle put_मानम्_time(p_vpi_मानम् vp);

      vvp_vector4_t return_मानम्_2;
};

पूर्णाङ्क sysfunc_vec4::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_आकार:
	    प्रत्यागम return_मानम्_2.size();

	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम lineno;

	  पक्ष vpiUserDefn:
	    प्रत्यागम defn->is_user_defn;

	  स्वभाव:
	    प्रत्यागम vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_मानम्_scalar(ध्रुव p_vpi_मानम् vp)
{
      विकल्प (vp->value.scalar) {
	  पक्ष vpi0:
	    return_मानम्_2.समुच्चयः_bit(0, BIT4_0);
	    भङ्ग;
	  पक्ष vpi1:
	    return_मानम्_2.समुच्चयः_bit(0, BIT4_1);
	    भङ्ग;
	  पक्ष vpiX:
	    return_मानम्_2.समुच्चयः_bit(0, BIT4_X);
	    भङ्ग;
	  पक्ष vpiZ:
	    return_मानम्_2.समुच्चयः_bit(0, BIT4_Z);
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "Unsupported value %d.\n", (पूर्णाङ्क)vp->value.scalar);
	    प्रतिज्ञा(0);
      }
      प्रत्यागम 0;
}

vpiHandle sysfunc_vec4::put_मानम्_int(p_vpi_मानम् vp)
{
      दीर्घ tmp = vp->value.integer;
      अचिह्नित width = return_मानम्_2.size();
      प्रति (अचिह्नित idx = 0 ;  idx < width ;  idx += 1) {
	    return_मानम्_2.समुच्चयः_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      प्रत्यागम 0;
}

vpiHandle sysfunc_vec4::put_मानम्_string(p_vpi_मानम् vp)
{
      size_t slen = वाक्यदैर्घ्यम्(vp->value.str);
      अचिह्नित wid = return_मानम्_2.size();

      प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1) {
	    अचिह्नित cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    यदि (cidx >= slen) {
		  return_मानम्_2.समुच्चयः_bit(idx, BIT4_0);
		  अनुवर्त;
	    }

	      // Put the end of the string in the LSB of the vector
	    वर्ण use_char = vp->value.str[slen - 1 - cidx];

	    यदि ((use_char >> (idx % 8)) & 1)
		  return_मानम्_2.समुच्चयः_bit(idx, BIT4_1);
	    अन्यथा
		  return_मानम्_2.समुच्चयः_bit(idx, BIT4_0);
      }
      प्रत्यागम 0;
}

vpiHandle sysfunc_vec4::put_मानम्_सदिशम्(p_vpi_मानम् vp)
{
      अचिह्नित width = return_मानम्_2.size();
      प्रति (अचिह्नित idx = 0 ; idx < width ; idx += 1) {
	    अचिह्नित word = idx / 32;
	    अचिह्नित bidx = idx % 32;

	    अचिह्नित दीर्घ aval = vp->value.vector[word].aval >> bidx;
	    अचिह्नित दीर्घ bval = vp->value.vector[word].bval >> bidx;

	    पूर्णाङ्क bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    विकल्प (bit) {
		पक्ष 0:
		  bit4 = BIT4_0;
		  भङ्ग;
		पक्ष 1:
		  bit4 = BIT4_1;
		  भङ्ग;
		पक्ष 2:
		  bit4 = BIT4_Z;
		  भङ्ग;
		पक्ष 3:
		  bit4 = BIT4_X;
		  भङ्ग;
		स्वभाव:
		  प्रतिज्ञा(0);
		  bit4 = BIT4_X;
		  भङ्ग;
	    }
	    return_मानम्_2.समुच्चयः_bit(idx, bit4);
      }
      प्रत्यागम 0;
}

vpiHandle sysfunc_vec4::put_मानम्_time(p_vpi_मानम् vp)
{
      अचिह्नित width = return_मानम्_2.size();
      दीर्घ tmp = 0;
      प्रति (अचिह्नित idx = 0 ; idx < width ; idx += 1) {
	    यदि (idx == 0)
		  tmp = vp->value.time->नीचः;
	    अन्यथा यदि (idx == 32)
		  tmp = vp->value.time->उच्चः;
	    अन्यथा यदि (idx == 64)
		  tmp = 0;

	    return_मानम्_2.समुच्चयः_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      प्रत्यागम 0;
}

vpiHandle sysfunc_vec4::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क)
{
      put_मानम् = सत्य;

      विकल्प (vp->format) {
	  पक्ष vpiScalarVal:
	    प्रत्यागम put_मानम्_scalar(vp);
	  पक्ष vpiIntVal:
	    प्रत्यागम put_मानम्_int(vp);
	  पक्ष vpiStringVal:
	    प्रत्यागम put_मानम्_string(vp);
	  पक्ष vpi_सदिशम्_Val:
	    प्रत्यागम put_मानम्_सदिशम्(vp);
	  पक्ष vpiTimeVal:
	    प्रत्यागम put_मानम्_time(vp);
	  स्वभाव:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);
      }

      प्रत्यागम 0;
}

संरचना sysfunc_4net : सार्वजनिक sysfunc_def {
      स्पष्ट अन्तःस्थ sysfunc_4net(अचिह्नित wid) : vwid_(wid) { }
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      vpiHandle vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags) override;

    गुप्त:
      अचिह्नित vwid_;
};

// support getting vpiSize for a system function call
पूर्णाङ्क sysfunc_4net::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_आकार:
	    प्रत्यागम vwid_;

	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम lineno;

	  पक्ष vpiUserDefn:
	    प्रत्यागम defn->is_user_defn;

	  स्वभाव:
	    प्रत्यागम vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क)
{
      put_मानम् = सत्य;

      vvp_vector4_t val (vwid_);

      विकल्प (vp->format) {

          पक्ष vpiScalarVal: {
	        विकल्प(vp->value.scalar) {
		      पक्ष vpi0:
			val.समुच्चयः_bit(0, BIT4_0);
                        भङ्ग;
		      पक्ष vpi1:
                        val.समुच्चयः_bit(0, BIT4_1);
                        भङ्ग;
		      पक्ष vpiX:
                        val.समुच्चयः_bit(0, BIT4_X);
                        भङ्ग;
		      पक्ष vpiZ:
                        val.समुच्चयः_bit(0, BIT4_Z);
                        भङ्ग;
		      स्वभाव:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (पूर्णाङ्क)vp->value.scalar);
                        प्रतिज्ञा(0);
                }
		भङ्ग;
          }

	  पक्ष vpiIntVal: {
		दीर्घ tmp = vp->value.integer;
		प्रति (अचिह्नित idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.समुच्चयः_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		भङ्ग;
	  }

          पक्ष vpiTimeVal: {
                अचिह्नित दीर्घ tmp = vp->value.time->नीचः;
                प्रति (अचिह्नित idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.समुच्चयः_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      यदि (idx == 31)
                            tmp = vp->value.time->उच्चः;
                      अन्यथा
                            tmp >>= 1;
                }
                भङ्ग;
          }

	  पक्ष vpi_सदिशम्_Val:

	    प्रति (अचिह्नित wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  अचिह्नित word = wdx / 32;
		  अचिह्नित दीर्घ aval = vp->value.vector[word].aval;
		  अचिह्नित दीर्घ bval = vp->value.vector[word].bval;

		  प्रति (अचिह्नित idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			पूर्णाङ्क bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			विकल्प (bit) {
			    पक्ष 0:
			      bit4 = BIT4_0;
			      भङ्ग;
			    पक्ष 1:
			      bit4 = BIT4_1;
			      भङ्ग;
			    पक्ष 2:
			      bit4 = BIT4_Z;
			      भङ्ग;
			    पक्ष 3:
			      bit4 = BIT4_X;
			      भङ्ग;
			    स्वभाव:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              bit);
			      प्रतिज्ञा(0);
			}
			val.समुच्चयः_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (पूर्णाङ्क)vp->format, vwid_);
	    प्रतिज्ञा(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      प्रत्यागम 0;
}

संरचना sysfunc_rnet : सार्वजनिक sysfunc_def {
      vpiHandle vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क)
{
      put_मानम् = सत्य;

      द्विगुण value;
      विकल्प (vp->format) {

	  पक्ष vpiRealVal:
	    value = vp->value.real;
	    भङ्ग;

	  स्वभाव:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      प्रत्यागम 0;
}

संरचना sysfunc_no : सार्वजनिक sysfunc_def {
      vpiHandle vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags) override;
};

vpiHandle sysfunc_no::vpi_put_मानम्(p_vpi_मानम्, पूर्णाङ्क)
{
      प्रत्यागम 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
स्थिर संरचना __vpiUserSystf**def_सारणी = 0;
स्थिर अचिह्नित def_गणनम् = 0;

स्थिर संरचना __vpiUserSystf* allocate_def(शून्य)
{
      यदि (def_सारणी == 0) {
	    def_सारणी = static_cast<संरचना __vpiUserSystf**>
	                (स्मृतिं_देहि(परिमाण(संरचना __vpiUserSystf*)));

	    def_सारणी[0] = नूतन __vpiUserSystf;

	    def_गणनम् = 1;
	    प्रत्यागम def_सारणी[0];
      }

      def_सारणी = static_cast<संरचना __vpiUserSystf**>
                  (realloc(def_सारणी,
                   (def_गणनम्+1)*परिमाण(संरचना __vpiUserSystf*)));

      def_सारणी[def_गणनम्] = नूतन __vpiUserSystf;

      प्रत्यागम def_सारणी[def_गणनम्++];
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य def_सारणी_delete(शून्य)
{
      प्रति (अचिह्नित idx = 0; idx < def_गणनम्; idx += 1) {
	    मुक्त(const_cast<वर्ण *>(def_सारणी[idx]->info.tfname));
	    विलोपय def_सारणी[idx];
      }
      मुक्त(def_सारणी);
      def_सारणी = 0;
      def_गणनम् = 0;
}
#यदि_अन्तः

वर्ग __vpiSystfIterator : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      स्पष्ट __vpiSystfIterator(अचिह्नित idx);
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override;
      vpiHandle vpi_सूचकांक(पूर्णाङ्क idx) override;
      free_object_fun_t free_object_fun(शून्य) override;

      अचिह्नित अग्र_2;
};

स्थिर vpiHandle systf_iterator_अवलोकनम्(vpiHandle ref, पूर्णाङ्क)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      यदि (obj->अग्र_2 >= def_गणनम्) {
	    vpi_free_object(ref);
	    प्रत्यागम 0;
      }

      अचिह्नित use_सूचकांक = obj->अग्र_2;
      यावत् (!def_सारणी[use_सूचकांक]->is_user_defn) {
	    obj->अग्र_2 += 1;
	    use_सूचकांक = obj->अग्र_2;
	    यदि (obj->अग्र_2 >= def_गणनम्) {
		  vpi_free_object(ref);
		  प्रत्यागम 0;
	    }
      }
      obj->अग्र_2 += 1;
      प्रत्यागम def_सारणी[use_सूचकांक];
}

स्थिर पूर्णाङ्क systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      विलोपय obj;
      प्रत्यागम 1;
}

अन्तःस्थ __vpiSystfIterator::__vpiSystfIterator(अचिह्नित idx)
{ अग्र_2 = idx; }

पूर्णाङ्क __vpiSystfIterator::get_type_संहिता(शून्य) ध्रुव
{ प्रत्यागम vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_सूचकांक(पूर्णाङ्क idx)
{ प्रत्यागम systf_iterator_अवलोकनम्(एतत्, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(शून्य)
{ प्रत्यागम &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(शून्य)
{
	/* Check to see if there are any user defined functions. */
      तार्किक have_user_defn = असत्य;
      अचिह्नित idx;
      प्रति (idx = 0; idx < def_गणनम्; idx += 1) {
	    यदि (def_सारणी[idx]->is_user_defn) {
		  have_user_defn = सत्य;
		  भङ्ग;
	    }
      }
      यदि (!have_user_defn) प्रत्यागम 0;

      __vpiSystfIterator*res = नूतन __vpiSystfIterator(idx);
      res->अग्र_2 = idx;
      प्रत्यागम res;
}

संरचना __vpiUserSystf* vpip_find_systf(ध्रुव वर्ण*name)
{
      प्रति (अचिह्नित idx = 0 ;  idx < def_गणनम् ;  idx += 1)
	    यदि (वाक्यं_तुलय(def_सारणी[idx]->info.tfname, name) == 0)
		  प्रत्यागम def_सारणी[idx];

      प्रत्यागम 0;
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
प्रकारनिर्देश संरचना vpi_call_error {
      vpi_call_error_type type;
      वर्ण *name;
      दीर्घ file_idx;
      दीर्घ lineno;
} vpi_call_error_s, *vpi_call_error_p;

स्थिर vpi_call_error_p vpi_call_error_lst = रिक्त;
स्थिर अचिह्नित vpi_call_error_अङ्कः = 0;

स्थिर शून्य सङ्कलय_vpi_call_error(vpi_call_error_type type, ध्रुव वर्ण *name,
                               दीर्घ file_idx, दीर्घ lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_अङ्कः + 1) *
                                    परिमाण(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_अङ्कः].type = type;
      vpi_call_error_lst[vpi_call_error_अङ्कः].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_अङ्कः].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_अङ्कः].lineno = lineno;
      vpi_call_error_अङ्कः += 1;
}

शून्य print_vpi_call_errors()
{
      प्रति (अचिह्नित idx = 0; idx < vpi_call_error_अङ्कः; idx += 1) {
	    विकल्प (vpi_call_error_lst[idx].type) {
		पक्ष VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_नामानि[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णाङ्क)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  भङ्ग;
		पक्ष VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_नामानि[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णाङ्क)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  भङ्ग;
		पक्ष VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_नामानि[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णाङ्क)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  भङ्ग;
		पक्ष VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_नामानि[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णाङ्क)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_नामानि[vpi_call_error_lst[idx].file_idx],
                                 (पूर्णाङ्क)vpi_call_error_lst[idx].lineno);
		  भङ्ग;
	    }
	    मुक्त(vpi_call_error_lst[idx].name);
      }
      मुक्त(vpi_call_error_lst);
      fflush(stderr);
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
स्थिर शून्य cleanup_vpi_call_तर्काः(अचिह्नित argc, vpiHandle*argv)
{
      यदि (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    संरचना __vpiSysTaskCall*obj = नूतन systask_def;
	    obj->nargs = argc;
	    obj->तर्काः  = argv;
	    vpi_call_delete(obj);
      }
}
#यदि_अन्तः

/*
 * A vpi_call is actually built up into a vpiSysTaskCall VPI object
 * that refers back to the vpiUserSystf VPI object that is the
 * definition. So this function is called by the compiler when a
 * %vpi_call statement is encountered. Create here a vpiHandle that
 * describes the call, and return it. The %vpi_call instruction will
 * store this handle for when it is executed.
 */
vpiHandle vpip_build_vpi_call(ध्रुव वर्ण*name, पूर्णाङ्क val_संहिता, अचिह्नित return_विस्तारः,
			      vvp_net_t*fnet,
			      तार्किक func_as_task_err, तार्किक func_as_task_warn,
			      अचिह्नित argc, vpiHandle*argv,
			      अचिह्नित vec4_राशिः, अचिह्नित real_राशिः, अचिह्नित string_राशिः,
			      दीर्घ file_idx, दीर्घ lineno)
{
      प्रतिज्ञा(!(func_as_task_err && func_as_task_warn));

      संरचना __vpiUserSystf*defn = vpip_find_systf(name);
      यदि (defn == 0) {
	    सङ्कलय_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
	    cleanup_vpi_call_तर्काः(argc, argv);
#यदि_अन्तः
	    प्रत्यागम 0;
      }

      विकल्प (defn->info.type) {
	  पक्ष vpiSysTask:
	    यदि (val_संहिता != 0 || fnet != 0) {
		  सङ्कलय_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
		  cleanup_vpi_call_तर्काः(argc, argv);
#यदि_अन्तः
		  प्रत्यागम 0;
	    }
	    भङ्ग;

	  पक्ष vpiSysFunc:
	    यदि (val_संहिता == 0 && fnet == 0) {
		  यदि (func_as_task_err) {
			सङ्कलय_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
			cleanup_vpi_call_तर्काः(argc, argv);
#यदि_अन्तः
			प्रत्यागम 0;
		  } अन्यथा यदि (func_as_task_warn) {
			सङ्कलय_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (पूर्णाङ्क)defn->info.type);
	    प्रतिज्ञा(0);
      }

      संरचना __vpiSysTaskCall*obj = 0;

      विकल्प (defn->info.type) {
	  पक्ष vpiSysTask:
	    obj = नूतन systask_def;
	    भङ्ग;

	  पक्ष vpiSysFunc:
	    यदि (fnet && val_संहिता == -vpiRealVal) {
		  obj = नूतन sysfunc_rnet;

	    } अन्यथा यदि (fnet && val_संहिता == -vpi_सदिशम्_Val) {
		  obj = नूतन sysfunc_4net(return_विस्तारः);

	    } अन्यथा यदि (val_संहिता == -vpiRealVal) {
		  obj = नूतन sysfunc_real;

	    } अन्यथा यदि (val_संहिता == -vpi_सदिशम्_Val) {
		  obj = नूतन sysfunc_vec4(return_विस्तारः);

	    } अन्यथा यदि (val_संहिता == -vpiStringVal) {
		  obj = नूतन sysfunc_str;

           } अन्यथा यदि (val_संहिता == 0 && fnet == 0) {
		  obj = नूतन sysfunc_no;

	    } अन्यथा {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_संहिता);
		  प्रतिज्ञा(0);
	    }
	    भङ्ग;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->तर्काः  = argv;
      obj->vec4_राशिः = vec4_राशिः;
      obj->real_राशिः = real_राशिः;
      obj->string_राशिः = string_राशिः;
      obj->fnet  = fnet;
      obj->file_idx  = (अचिह्नित) file_idx;
      obj->lineno   = (अचिह्नित) lineno;
      obj->userdata  = 0;
      obj->put_मानम् = असत्य;

      compile_compiletf(obj);

      प्रत्यागम obj;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vpi_call_delete(vpiHandle item)
{
      संरचना __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      यदि (!obj) प्रत्यागम;
      प्रति (अचिह्नित arg = 0; arg < obj->nargs; arg += 1) {
	    विकल्प (obj->तर्काः[arg]->get_type_संहिता()) {
		पक्ष vpiConstant:
		  विकल्प (vpi_get(_vpiFromThr, obj->तर्काः[arg])) {
		      पक्ष _vpiNoThr:
			constant_delete(obj->तर्काः[arg]);
			भङ्ग;
		      पक्ष _vpiString:
			thread_string_delete(obj->तर्काः[arg]);
			भङ्ग;
		      पक्ष _vpiVThr:
			thread_vthr_delete(obj->तर्काः[arg]);
			भङ्ग;
		      पक्ष _vpiWord:
			thread_word_delete(obj->तर्काः[arg]);
			भङ्ग;
		      स्वभाव:
			प्रतिज्ञा(0);
		  }
		  भङ्ग;
		पक्ष vpi_स्मृतिः_Word:
		  विकल्प (vpi_get(_vpiFromThr, obj->तर्काः[arg])) {
		      पक्ष _vpi_at_A:
			A_delete(obj->तर्काः[arg]);
			भङ्ग;
		  }
		  भङ्ग;
		पक्ष vpiPartSelect:
		  विकल्प (vpi_get(_vpiFromThr, obj->तर्काः[arg])) {
		      पक्ष _vpi_at_PV:
			PV_delete(obj->तर्काः[arg]);
			भङ्ग;
		      पक्ष _vpi_at_APV:
			APV_delete(obj->तर्काः[arg]);
			भङ्ग;
		  }
		  भङ्ग;
	    }
      }
      मुक्त(obj->तर्काः);
      विलोपय obj;
}
#यदि_अन्तः

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

      यदि (vpip_cur_task->defn->info.calltf) {
	    प्रतिज्ञा(vpi_बहुलकम्_flag == VPI_बहुलकम्_NONE);
	    vpi_बहुलकम्_flag = VPI_बहुलकम्_CALLTF;
	    vpip_cur_task->put_मानम् = असत्य;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_दत्त);
	    vpi_बहुलकम्_flag = VPI_बहुलकम्_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    यदि (ref->get_type_संहिता() == vpiSysFuncCall &&
	        !vpip_cur_task->put_मानम्) {
		  s_vpi_मानम् val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_मानम्(ref, &val, 0, vpiNoDelay);
	    }
      }
      यदि (vpip_cur_task->vec4_राशिः > 0)
	    vthread_निष्कासय_vec4(thr, vpip_cur_task->vec4_राशिः);
      यदि (vpip_cur_task->real_राशिः > 0)
	    vthread_निष्कासय_real(thr, vpip_cur_task->real_राशिः);
      यदि (vpip_cur_task->string_राशिः > 0)
	    vthread_निष्कासय_str(thr, vpip_cur_task->string_राशिः);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      यदि (ध्रुव sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_प्रक्षिप(thr, func_real->return_मानम्());
      }
      अन्यथा यदि (ध्रुव sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_प्रक्षिप(thr, func_string->return_मानम्());
      }
      अन्यथा यदि (ध्रुव sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_प्रक्षिप(thr, func_vec4->return_मानम्());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_पञ्जिकाकोशः_systf(ध्रुव संरचना t_vpi_systf_दत्त*ss)
{
      संरचना __vpiUserSystf*cur = allocate_def();
      प्रतिज्ञा(ss);
      विकल्प (ss->type) {
	  पक्ष vpiSysTask:
	  पक्ष vpiSysFunc:
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "Unsupported type %d.\n", (पूर्णाङ्क)ss->type);
	    प्रतिज्ञा(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = सत्य;

      प्रत्यागम cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, शून्य*दत्त)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      यदि (rfp == 0)
	    प्रत्यागम 0;

      rfp->userdata = दत्त;
      प्रत्यागम 1;
}

शून्य* vpi_get_userdata(vpiHandle ref)
{
      संरचना __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      प्रतिज्ञा(rfp);

      प्रत्यागम rfp->userdata;
}
