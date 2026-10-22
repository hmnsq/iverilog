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

using નામાકાશ std;

અંતઃરેખ __vpiUserSystf::__vpiUserSystf()
{ }

પૂર્ણાંક __vpiUserSystf::get_type_code(શૂન્ય) અચલ
{ પાછા vpiUserSystf; }


સ્થિર vpiHandle systask_handle(પૂર્ણાંક type, vpiHandle ref)
{
      સંરચના __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      પસંદગી (type) {
	  સ્થિતિ vpiScope:
	    પાછા rfp->scope;

	  સ્થિતિ vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    પ્રતિજ્ઞા(rfp->defn->is_user_defn);
	    પાછા rfp->defn;

	  મૂળભૂત:
	    પાછા 0;
      };
}

સ્થિર પૂર્ણાંક systask_get(પૂર્ણાંક type, vpiHandle ref)
{
      સંરચના __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      પસંદગી (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  સ્થિતિ vpiTimeUnit:
	    પાછા rfp->scope->time_units;
	  સ્થિતિ vpiTimePrecision:
	    પાછા rfp->scope->time_precision;

	  સ્થિતિ vpi_લીટી_No:
	    પાછા rfp->lineno;

	  સ્થિતિ vpiUserDefn:
	    પાછા rfp->defn->is_user_defn;

	  મૂળભૂત:
	    પાછા vpiUndefined;
      }
}

// support getting vpiSize for a system function call
સ્થિર પૂર્ણાંક sysfunc_get(પૂર્ણાંક type, vpiHandle ref)
{
      સંરચના __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      પસંદગી (type) {
	  સ્થિતિ vpiSize:
	    પ્રતિજ્ઞા(0); // This should be handled by derived classes
	    પાછા 0;

	  સ્થિતિ vpi_લીટી_No:
	    પાછા rfp->lineno;

	  સ્થિતિ vpiUserDefn:
	    પાછા rfp->defn->is_user_defn;

	  મૂળભૂત:
	    પાછા vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

સ્થિર અક્ષર *systask_get_str(પૂર્ણાંક type, vpiHandle ref)
{
      અચલ સંરચના __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      પસંદગી (type) {
          સ્થિતિ vpiFile:
            પ્રતિજ્ઞા(rfp->file_idx < file_names.size());
            પાછા simple_set_rbuf_str(file_names[rfp->file_idx]);

          સ્થિતિ vpiName:
            પાછા simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      પાછા 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
સ્થિર vpiHandle systask_iter(પૂર્ણાંક, vpiHandle ref)
{
      સંરચના __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      જો (rfp->nargs == 0)
	    પાછા 0;

      પાછા vpip_make_iterator(rfp->nargs, rfp->args, ખોટું);
}

સંરચના systask_def : જાહેર __vpiSysTaskCall {
      આભાસી પૂર્ણાંક get_type_code(શૂન્ય) અચલ   override { પાછા vpiSysTaskCall; }
      આભાસી પૂર્ણાંક vpi_get(પૂર્ણાંક code)           override { પાછા systask_get(code, આ); }
      આભાસી અક્ષર*vpi_get_str(પૂર્ણાંક code)      override { પાછા systask_get_str(code, આ); }
      આભાસી vpiHandle vpi_handle(પૂર્ણાંક code)  override { પાછા systask_handle(code, આ); }
      આભાસી vpiHandle vpi_iterate(પૂર્ણાંક code) override { પાછા systask_iter(code, આ); }
};

સંરચના sysfunc_def : જાહેર systask_def {
      આભાસી પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiSysFuncCall; }
      આભાસી પૂર્ણાંક vpi_get(પૂર્ણાંક code) override { પાછા sysfunc_get(code, આ); }
};

સંરચના sysfunc_real : જાહેર sysfunc_def {
      vpiHandle vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags) override;
      અંતઃરેખ દ્વિગુણ return_કિંમત() અચલ { પાછા return_કિંમત_2; }

    ખાનગી:
      દ્વિગુણ return_કિંમત_2;
};

vpiHandle sysfunc_real::vpi_put_કિંમત(p_vpi_કિંમત vp, પૂર્ણાંક)
{
      put_કિંમત = સાચું;

      return_કિંમત_2 = 0.0;
      પસંદગી (vp->format) {
	  સ્થિતિ vpiRealVal:
	    return_કિંમત_2 = vp->value.real;
	    તોડો;
	  મૂળભૂત:
	    fprintf(stderr, "Unsupported format %d.\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);
      }

      પાછા 0;
}

સંરચના sysfunc_str : જાહેર sysfunc_def {
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      vpiHandle vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags) override;
      અંતઃરેખ અચલ std::string& return_કિંમત() અચલ { પાછા return_કિંમત_2; };

    ખાનગી:
      std::string return_કિંમત_2;
};

પૂર્ણાંક sysfunc_str::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiSize:
	    પાછા return_કિંમત_2.size();

	  સ્થિતિ vpi_લીટી_No:
	    પાછા lineno;

	  સ્થિતિ vpiUserDefn:
	    પાછા defn->is_user_defn;

	  મૂળભૂત:
	    પાછા vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_કિંમત(p_vpi_કિંમત vp, પૂર્ણાંક)
{
      put_કિંમત = સાચું;

      પસંદગી (vp->format) {
	  સ્થિતિ vpiStringVal:
	    return_કિંમત_2 = std::string(vp->value.str);
	    તોડો;
	  મૂળભૂત:
	    fprintf(stderr, "Unsupported format %d.\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);
      }

      પાછા 0;
}

વર્ગ sysfunc_vec4 : જાહેર sysfunc_def {
    જાહેર:
      explicit અંતઃરેખ sysfunc_vec4(unsigned wid): return_કિંમત_2(wid, BIT4_X) { }
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      vpiHandle vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags) override;
      અંતઃરેખ અચલ vvp_vector4_t& return_કિંમત() અચલ { પાછા return_કિંમત_2; }

    ખાનગી:
      vpiHandle put_કિંમત_scalar(p_vpi_કિંમત vp);
      vpiHandle put_કિંમત_int(p_vpi_કિંમત vp);
      vpiHandle put_કિંમત_string(p_vpi_કિંમત vp);
      vpiHandle put_કિંમત_vector(p_vpi_કિંમત vp);
      vpiHandle put_કિંમત_time(p_vpi_કિંમત vp);

      vvp_vector4_t return_કિંમત_2;
};

પૂર્ણાંક sysfunc_vec4::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiSize:
	    પાછા return_કિંમત_2.size();

	  સ્થિતિ vpi_લીટી_No:
	    પાછા lineno;

	  સ્થિતિ vpiUserDefn:
	    પાછા defn->is_user_defn;

	  મૂળભૂત:
	    પાછા vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_કિંમત_scalar(અચલ p_vpi_કિંમત vp)
{
      પસંદગી (vp->value.scalar) {
	  સ્થિતિ vpi0:
	    return_કિંમત_2.set_bit(0, BIT4_0);
	    તોડો;
	  સ્થિતિ vpi1:
	    return_કિંમત_2.set_bit(0, BIT4_1);
	    તોડો;
	  સ્થિતિ vpiX:
	    return_કિંમત_2.set_bit(0, BIT4_X);
	    તોડો;
	  સ્થિતિ vpiZ:
	    return_કિંમત_2.set_bit(0, BIT4_Z);
	    તોડો;
	  મૂળભૂત:
	    fprintf(stderr, "Unsupported value %d.\n", (પૂર્ણાંક)vp->value.scalar);
	    પ્રતિજ્ઞા(0);
      }
      પાછા 0;
}

vpiHandle sysfunc_vec4::put_કિંમત_int(p_vpi_કિંમત vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_કિંમત_2.size();
      માટે (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_કિંમત_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      પાછા 0;
}

vpiHandle sysfunc_vec4::put_કિંમત_string(p_vpi_કિંમત vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_કિંમત_2.size();

      માટે (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    જો (cidx >= slen) {
		  return_કિંમત_2.set_bit(idx, BIT4_0);
		  ચાલુ;
	    }

	      // Put the end of the string in the LSB of the vector
	    અક્ષર use_char = vp->value.str[slen - 1 - cidx];

	    જો ((use_char >> (idx % 8)) & 1)
		  return_કિંમત_2.set_bit(idx, BIT4_1);
	    નહીંતર
		  return_કિંમત_2.set_bit(idx, BIT4_0);
      }
      પાછા 0;
}

vpiHandle sysfunc_vec4::put_કિંમત_vector(p_vpi_કિંમત vp)
{
      unsigned width = return_કિંમત_2.size();
      માટે (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    પૂર્ણાંક bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    પસંદગી (bit) {
		સ્થિતિ 0:
		  bit4 = BIT4_0;
		  તોડો;
		સ્થિતિ 1:
		  bit4 = BIT4_1;
		  તોડો;
		સ્થિતિ 2:
		  bit4 = BIT4_Z;
		  તોડો;
		સ્થિતિ 3:
		  bit4 = BIT4_X;
		  તોડો;
		મૂળભૂત:
		  પ્રતિજ્ઞા(0);
		  bit4 = BIT4_X;
		  તોડો;
	    }
	    return_કિંમત_2.set_bit(idx, bit4);
      }
      પાછા 0;
}

vpiHandle sysfunc_vec4::put_કિંમત_time(p_vpi_કિંમત vp)
{
      unsigned width = return_કિંમત_2.size();
      long tmp = 0;
      માટે (unsigned idx = 0 ; idx < width ; idx += 1) {
	    જો (idx == 0)
		  tmp = vp->value.time->નીચલું;
	    નહીંતર જો (idx == 32)
		  tmp = vp->value.time->ઉપલું;
	    નહીંતર જો (idx == 64)
		  tmp = 0;

	    return_કિંમત_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      પાછા 0;
}

vpiHandle sysfunc_vec4::vpi_put_કિંમત(p_vpi_કિંમત vp, પૂર્ણાંક)
{
      put_કિંમત = સાચું;

      પસંદગી (vp->format) {
	  સ્થિતિ vpiScalarVal:
	    પાછા put_કિંમત_scalar(vp);
	  સ્થિતિ vpiIntVal:
	    પાછા put_કિંમત_int(vp);
	  સ્થિતિ vpiStringVal:
	    પાછા put_કિંમત_string(vp);
	  સ્થિતિ vpiVectorVal:
	    પાછા put_કિંમત_vector(vp);
	  સ્થિતિ vpiTimeVal:
	    પાછા put_કિંમત_time(vp);
	  મૂળભૂત:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);
      }

      પાછા 0;
}

સંરચના sysfunc_4net : જાહેર sysfunc_def {
      explicit અંતઃરેખ sysfunc_4net(unsigned wid) : vwid_(wid) { }
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      vpiHandle vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags) override;

    ખાનગી:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
પૂર્ણાંક sysfunc_4net::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiSize:
	    પાછા vwid_;

	  સ્થિતિ vpi_લીટી_No:
	    પાછા lineno;

	  સ્થિતિ vpiUserDefn:
	    પાછા defn->is_user_defn;

	  મૂળભૂત:
	    પાછા vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_કિંમત(p_vpi_કિંમત vp, પૂર્ણાંક)
{
      put_કિંમત = સાચું;

      vvp_vector4_t val (vwid_);

      પસંદગી (vp->format) {

          સ્થિતિ vpiScalarVal: {
	        પસંદગી(vp->value.scalar) {
		      સ્થિતિ vpi0:
			val.set_bit(0, BIT4_0);
                        તોડો;
		      સ્થિતિ vpi1:
                        val.set_bit(0, BIT4_1);
                        તોડો;
		      સ્થિતિ vpiX:
                        val.set_bit(0, BIT4_X);
                        તોડો;
		      સ્થિતિ vpiZ:
                        val.set_bit(0, BIT4_Z);
                        તોડો;
		      મૂળભૂત:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (પૂર્ણાંક)vp->value.scalar);
                        પ્રતિજ્ઞા(0);
                }
		તોડો;
          }

	  સ્થિતિ vpiIntVal: {
		long tmp = vp->value.integer;
		માટે (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		તોડો;
	  }

          સ્થિતિ vpiTimeVal: {
                unsigned long tmp = vp->value.time->નીચલું;
                માટે (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      જો (idx == 31)
                            tmp = vp->value.time->ઉપલું;
                      નહીંતર
                            tmp >>= 1;
                }
                તોડો;
          }

	  સ્થિતિ vpiVectorVal:

	    માટે (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  માટે (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			પૂર્ણાંક bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			પસંદગી (bit) {
			    સ્થિતિ 0:
			      bit4 = BIT4_0;
			      તોડો;
			    સ્થિતિ 1:
			      bit4 = BIT4_1;
			      તોડો;
			    સ્થિતિ 2:
			      bit4 = BIT4_Z;
			      તોડો;
			    સ્થિતિ 3:
			      bit4 = BIT4_X;
			      તોડો;
			    મૂળભૂત:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              bit);
			      પ્રતિજ્ઞા(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (પૂર્ણાંક)vp->format, vwid_);
	    પ્રતિજ્ઞા(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      પાછા 0;
}

સંરચના sysfunc_rnet : જાહેર sysfunc_def {
      vpiHandle vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_કિંમત(p_vpi_કિંમત vp, પૂર્ણાંક)
{
      put_કિંમત = સાચું;

      દ્વિગુણ value;
      પસંદગી (vp->format) {

	  સ્થિતિ vpiRealVal:
	    value = vp->value.real;
	    તોડો;

	  મૂળભૂત:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      પાછા 0;
}

સંરચના sysfunc_no : જાહેર sysfunc_def {
      vpiHandle vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags) override;
};

vpiHandle sysfunc_no::vpi_put_કિંમત(p_vpi_કિંમત, પૂર્ણાંક)
{
      પાછા 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
સ્થિર સંરચના __vpiUserSystf**def_table = 0;
સ્થિર unsigned def_ગણતરી = 0;

સ્થિર સંરચના __vpiUserSystf* allocate_def(શૂન્ય)
{
      જો (def_table == 0) {
	    def_table = static_cast<સંરચના __vpiUserSystf**>
	                (malloc(કદ(સંરચના __vpiUserSystf*)));

	    def_table[0] = નવું __vpiUserSystf;

	    def_ગણતરી = 1;
	    પાછા def_table[0];
      }

      def_table = static_cast<સંરચના __vpiUserSystf**>
                  (realloc(def_table,
                   (def_ગણતરી+1)*કદ(સંરચના __vpiUserSystf*)));

      def_table[def_ગણતરી] = નવું __vpiUserSystf;

      પાછા def_table[def_ગણતરી++];
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય def_table_delete(શૂન્ય)
{
      માટે (unsigned idx = 0; idx < def_ગણતરી; idx += 1) {
	    મુક્ત(const_cast<અક્ષર *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      મુક્ત(def_table);
      def_table = 0;
      def_ગણતરી = 0;
}
#endif

વર્ગ __vpiSystfIterator : જાહેર __vpiHandle {
    જાહેર:
      explicit __vpiSystfIterator(unsigned idx);
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      vpiHandle vpi_index(પૂર્ણાંક idx) override;
      free_object_fun_t free_object_fun(શૂન્ય) override;

      unsigned next;
};

સ્થિર vpiHandle systf_iterator_scan(vpiHandle ref, પૂર્ણાંક)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      જો (obj->next >= def_ગણતરી) {
	    vpi_free_object(ref);
	    પાછા 0;
      }

      unsigned use_index = obj->next;
      જ્યારે (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    જો (obj->next >= def_ગણતરી) {
		  vpi_free_object(ref);
		  પાછા 0;
	    }
      }
      obj->next += 1;
      પાછા def_table[use_index];
}

સ્થિર પૂર્ણાંક systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      પાછા 1;
}

અંતઃરેખ __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

પૂર્ણાંક __vpiSystfIterator::get_type_code(શૂન્ય) અચલ
{ પાછા vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(પૂર્ણાંક idx)
{ પાછા systf_iterator_scan(આ, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(શૂન્ય)
{ પાછા &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(શૂન્ય)
{
	/* Check to see if there are any user defined functions. */
      તાર્કિક have_user_defn = ખોટું;
      unsigned idx;
      માટે (idx = 0; idx < def_ગણતરી; idx += 1) {
	    જો (def_table[idx]->is_user_defn) {
		  have_user_defn = સાચું;
		  તોડો;
	    }
      }
      જો (!have_user_defn) પાછા 0;

      __vpiSystfIterator*res = નવું __vpiSystfIterator(idx);
      res->next = idx;
      પાછા res;
}

સંરચના __vpiUserSystf* vpip_find_systf(અચલ અક્ષર*name)
{
      માટે (unsigned idx = 0 ;  idx < def_ગણતરી ;  idx += 1)
	    જો (strcmp(def_table[idx]->info.tfname, name) == 0)
		  પાછા def_table[idx];

      પાછા 0;
}

શૂન્ય vpip_make_systf_system_defined(vpiHandle ref)
{
      પ્રતિજ્ઞા(ref);
      સંરચના __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      પ્રતિજ્ઞા(obj);
      obj->is_user_defn = ખોટું;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
ગણના vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
પ્રકારવ્યાખ્યા સંરચના vpi_call_error {
      vpi_call_error_type type;
      અક્ષર *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

સ્થિર vpi_call_error_p vpi_call_error_lst = ખાલી;
સ્થિર unsigned vpi_call_error_અંક = 0;

સ્થિર શૂન્ય add_vpi_call_error(vpi_call_error_type type, અચલ અક્ષર *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_અંક + 1) *
                                    કદ(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_અંક].type = type;
      vpi_call_error_lst[vpi_call_error_અંક].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_અંક].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_અંક].lineno = lineno;
      vpi_call_error_અંક += 1;
}

શૂન્ય print_vpi_call_errors()
{
      માટે (unsigned idx = 0; idx < vpi_call_error_અંક; idx += 1) {
	    પસંદગી (vpi_call_error_lst[idx].type) {
		સ્થિતિ VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (પૂર્ણાંક)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  તોડો;
		સ્થિતિ VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (પૂર્ણાંક)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  તોડો;
		સ્થિતિ VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (પૂર્ણાંક)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  તોડો;
		સ્થિતિ VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (પૂર્ણાંક)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (પૂર્ણાંક)vpi_call_error_lst[idx].lineno);
		  તોડો;
	    }
	    મુક્ત(vpi_call_error_lst[idx].name);
      }
      મુક્ત(vpi_call_error_lst);
      fflush(stderr);
}

#ifdef CHECK_WITH_VALGRIND
સ્થિર શૂન્ય cleanup_vpi_call_args(unsigned argc, vpiHandle*argv)
{
      જો (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    સંરચના __vpiSysTaskCall*obj = નવું systask_def;
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
vpiHandle vpip_build_vpi_call(અચલ અક્ષર*name, પૂર્ણાંક val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      તાર્કિક func_as_task_err, તાર્કિક func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_ઢગલો, unsigned real_ઢગલો, unsigned string_ઢગલો,
			      long file_idx, long lineno)
{
      પ્રતિજ્ઞા(!(func_as_task_err && func_as_task_warn));

      સંરચના __vpiUserSystf*defn = vpip_find_systf(name);
      જો (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    પાછા 0;
      }

      પસંદગી (defn->info.type) {
	  સ્થિતિ vpiSysTask:
	    જો (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  પાછા 0;
	    }
	    તોડો;

	  સ્થિતિ vpiSysFunc:
	    જો (val_code == 0 && fnet == 0) {
		  જો (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			પાછા 0;
		  } નહીંતર જો (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (પૂર્ણાંક)defn->info.type);
	    પ્રતિજ્ઞા(0);
      }

      સંરચના __vpiSysTaskCall*obj = 0;

      પસંદગી (defn->info.type) {
	  સ્થિતિ vpiSysTask:
	    obj = નવું systask_def;
	    તોડો;

	  સ્થિતિ vpiSysFunc:
	    જો (fnet && val_code == -vpiRealVal) {
		  obj = નવું sysfunc_rnet;

	    } નહીંતર જો (fnet && val_code == -vpiVectorVal) {
		  obj = નવું sysfunc_4net(return_width);

	    } નહીંતર જો (val_code == -vpiRealVal) {
		  obj = નવું sysfunc_real;

	    } નહીંતર જો (val_code == -vpiVectorVal) {
		  obj = નવું sysfunc_vec4(return_width);

	    } નહીંતર જો (val_code == -vpiStringVal) {
		  obj = નવું sysfunc_str;

           } નહીંતર જો (val_code == 0 && fnet == 0) {
		  obj = નવું sysfunc_no;

	    } નહીંતર {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  પ્રતિજ્ઞા(0);
	    }
	    તોડો;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_ઢગલો = vec4_ઢગલો;
      obj->real_ઢગલો = real_ઢગલો;
      obj->string_ઢગલો = string_ઢગલો;
      obj->fnet  = fnet;
      obj->file_idx  = (unsigned) file_idx;
      obj->lineno   = (unsigned) lineno;
      obj->userdata  = 0;
      obj->put_કિંમત = ખોટું;

      compile_compiletf(obj);

      પાછા obj;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vpi_call_delete(vpiHandle item)
{
      સંરચના __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      જો (!obj) પાછા;
      માટે (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    પસંદગી (obj->args[arg]->get_type_code()) {
		સ્થિતિ vpiConstant:
		  પસંદગી (vpi_get(_vpiFromThr, obj->args[arg])) {
		      સ્થિતિ _vpiNoThr:
			constant_delete(obj->args[arg]);
			તોડો;
		      સ્થિતિ _vpiString:
			thread_string_delete(obj->args[arg]);
			તોડો;
		      સ્થિતિ _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			તોડો;
		      સ્થિતિ _vpiWord:
			thread_word_delete(obj->args[arg]);
			તોડો;
		      મૂળભૂત:
			પ્રતિજ્ઞા(0);
		  }
		  તોડો;
		સ્થિતિ vpiMemoryWord:
		  પસંદગી (vpi_get(_vpiFromThr, obj->args[arg])) {
		      સ્થિતિ _vpi_at_A:
			A_delete(obj->args[arg]);
			તોડો;
		  }
		  તોડો;
		સ્થિતિ vpiPartSelect:
		  પસંદગી (vpi_get(_vpiFromThr, obj->args[arg])) {
		      સ્થિતિ _vpi_at_PV:
			PV_delete(obj->args[arg]);
			તોડો;
		      સ્થિતિ _vpi_at_APV:
			APV_delete(obj->args[arg]);
			તોડો;
		  }
		  તોડો;
	    }
      }
      મુક્ત(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

શૂન્ય vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      જો (vpip_cur_task->defn->info.calltf) {
	    પ્રતિજ્ઞા(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_કિંમત = ખોટું;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    જો (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_કિંમત) {
		  s_vpi_કિંમત val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_કિંમત(ref, &val, 0, vpiNoDelay);
	    }
      }
      જો (vpip_cur_task->vec4_ઢગલો > 0)
	    vthread_કાઢો_vec4(thr, vpip_cur_task->vec4_ઢગલો);
      જો (vpip_cur_task->real_ઢગલો > 0)
	    vthread_કાઢો_real(thr, vpip_cur_task->real_ઢગલો);
      જો (vpip_cur_task->string_ઢગલો > 0)
	    vthread_કાઢો_str(thr, vpip_cur_task->string_ઢગલો);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      જો (અચલ sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ધકેલો(thr, func_real->return_કિંમત());
      }
      નહીંતર જો (અચલ sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ધકેલો(thr, func_string->return_કિંમત());
      }
      નહીંતર જો (અચલ sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_ધકેલો(thr, func_vec4->return_કિંમત());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(અચલ સંરચના t_vpi_systf_data*ss)
{
      સંરચના __vpiUserSystf*cur = allocate_def();
      પ્રતિજ્ઞા(ss);
      પસંદગી (ss->type) {
	  સ્થિતિ vpiSysTask:
	  સ્થિતિ vpiSysFunc:
	    તોડો;
	  મૂળભૂત:
	    fprintf(stderr, "Unsupported type %d.\n", (પૂર્ણાંક)ss->type);
	    પ્રતિજ્ઞા(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = સાચું;

      પાછા cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, શૂન્ય*data)
{
      સંરચના __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      જો (rfp == 0)
	    પાછા 0;

      rfp->userdata = data;
      પાછા 1;
}

શૂન્ય* vpi_get_userdata(vpiHandle ref)
{
      સંરચના __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      પ્રતિજ્ઞા(rfp);

      પાછા rfp->userdata;
}
