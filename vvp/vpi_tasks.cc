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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ఉపయోగించి నామాకాశ std;

అంతఃవరుస __vpiUserSystf::__vpiUserSystf()
{ }

పూర్ణాంకం __vpiUserSystf::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiUserSystf; }


స్థిర vpiHandle systask_handle(పూర్ణాంకం type, vpiHandle ref)
{
      నిర్మాణం __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ఎంపిక (type) {
	  స్థితి vpiScope:
	    తిరిగి rfp->scope;

	  స్థితి vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    ప్రతిజ్ఞా(rfp->defn->is_user_defn);
	    తిరిగి rfp->defn;

	  అప్రమేయ:
	    తిరిగి 0;
      };
}

స్థిర పూర్ణాంకం systask_get(పూర్ణాంకం type, vpiHandle ref)
{
      నిర్మాణం __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ఎంపిక (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  స్థితి vpi_సమయం_Unit:
	    తిరిగి rfp->scope->సమయం_units;
	  స్థితి vpi_సమయం_Precision:
	    తిరిగి rfp->scope->సమయం_precision;

	  స్థితి vpi_పంక్తి_No:
	    తిరిగి rfp->lineno;

	  స్థితి vpiUserDefn:
	    తిరిగి rfp->defn->is_user_defn;

	  అప్రమేయ:
	    తిరిగి vpiUndefined;
      }
}

// support getting vpiSize for a system function call
స్థిర పూర్ణాంకం sysfunc_get(పూర్ణాంకం type, vpiHandle ref)
{
      నిర్మాణం __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ఎంపిక (type) {
	  స్థితి vpi_కొలత:
	    ప్రతిజ్ఞా(0); // This should be handled by derived classes
	    తిరిగి 0;

	  స్థితి vpi_పంక్తి_No:
	    తిరిగి rfp->lineno;

	  స్థితి vpiUserDefn:
	    తిరిగి rfp->defn->is_user_defn;

	  అప్రమేయ:
	    తిరిగి vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

స్థిర అక్షరం *systask_get_str(పూర్ణాంకం type, vpiHandle ref)
{
      స్థిరరాశి నిర్మాణం __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ఎంపిక (type) {
          స్థితి vpiFile:
            ప్రతిజ్ఞా(rfp->file_idx < file_పేర్లు.size());
            తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[rfp->file_idx]);

          స్థితి vpiName:
            తిరిగి simple_సమితి_rbuf_str(rfp->defn->info.tfname);
      }

      తిరిగి 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
స్థిర vpiHandle systask_iter(పూర్ణాంకం, vpiHandle ref)
{
      నిర్మాణం __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ఒకవేళ (rfp->nargs == 0)
	    తిరిగి 0;

      తిరిగి vpip_make_iterator(rfp->nargs, rfp->వాదనలు, అబద్ధం);
}

నిర్మాణం systask_def : బహిరంగ __vpiSysTaskCall {
      మాయా పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి   override { తిరిగి vpiSysTaskCall; }
      మాయా పూర్ణాంకం vpi_get(పూర్ణాంకం code)           override { తిరిగి systask_get(code, ఇది); }
      మాయా అక్షరం*vpi_get_str(పూర్ణాంకం code)      override { తిరిగి systask_get_str(code, ఇది); }
      మాయా vpiHandle vpi_handle(పూర్ణాంకం code)  override { తిరిగి systask_handle(code, ఇది); }
      మాయా vpiHandle vpi_iterate(పూర్ణాంకం code) override { తిరిగి systask_iter(code, ఇది); }
};

నిర్మాణం sysfunc_def : బహిరంగ systask_def {
      మాయా పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiSysFuncCall; }
      మాయా పూర్ణాంకం vpi_get(పూర్ణాంకం code) override { తిరిగి sysfunc_get(code, ఇది); }
};

నిర్మాణం sysfunc_real : బహిరంగ sysfunc_def {
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      అంతఃవరుస ద్విగుణ return_విలువ() స్థిరరాశి { తిరిగి return_విలువ_2; }

    అంతరంగ:
      ద్విగుణ return_విలువ_2;
};

vpiHandle sysfunc_real::vpi_put_విలువ(p_vpi_విలువ vp, పూర్ణాంకం)
{
      put_విలువ = నిజం;

      return_విలువ_2 = 0.0;
      ఎంపిక (vp->format) {
	  స్థితి vpiRealVal:
	    return_విలువ_2 = vp->value.real;
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported format %d.\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);
      }

      తిరిగి 0;
}

నిర్మాణం sysfunc_str : బహిరంగ sysfunc_def {
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      అంతఃవరుస స్థిరరాశి std::string& return_విలువ() స్థిరరాశి { తిరిగి return_విలువ_2; };

    అంతరంగ:
      std::string return_విలువ_2;
};

పూర్ణాంకం sysfunc_str::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_కొలత:
	    తిరిగి return_విలువ_2.size();

	  స్థితి vpi_పంక్తి_No:
	    తిరిగి lineno;

	  స్థితి vpiUserDefn:
	    తిరిగి defn->is_user_defn;

	  అప్రమేయ:
	    తిరిగి vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_విలువ(p_vpi_విలువ vp, పూర్ణాంకం)
{
      put_విలువ = నిజం;

      ఎంపిక (vp->format) {
	  స్థితి vpiStringVal:
	    return_విలువ_2 = std::string(vp->value.str);
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported format %d.\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);
      }

      తిరిగి 0;
}

తరగతి sysfunc_vec4 : బహిరంగ sysfunc_def {
    బహిరంగ:
      స్పష్టమైన అంతఃవరుస sysfunc_vec4(గుర్తులేని wid): return_విలువ_2(wid, BIT4_X) { }
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      అంతఃవరుస స్థిరరాశి vvp_vector4_t& return_విలువ() స్థిరరాశి { తిరిగి return_విలువ_2; }

    అంతరంగ:
      vpiHandle put_విలువ_scalar(p_vpi_విలువ vp);
      vpiHandle put_విలువ_int(p_vpi_విలువ vp);
      vpiHandle put_విలువ_string(p_vpi_విలువ vp);
      vpiHandle put_విలువ_సదిశ(p_vpi_విలువ vp);
      vpiHandle put_విలువ_సమయం(p_vpi_విలువ vp);

      vvp_vector4_t return_విలువ_2;
};

పూర్ణాంకం sysfunc_vec4::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_కొలత:
	    తిరిగి return_విలువ_2.size();

	  స్థితి vpi_పంక్తి_No:
	    తిరిగి lineno;

	  స్థితి vpiUserDefn:
	    తిరిగి defn->is_user_defn;

	  అప్రమేయ:
	    తిరిగి vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_విలువ_scalar(స్థిరరాశి p_vpi_విలువ vp)
{
      ఎంపిక (vp->value.scalar) {
	  స్థితి vpi0:
	    return_విలువ_2.సమితి_బిట్టు(0, BIT4_0);
	    ఆపు;
	  స్థితి vpi1:
	    return_విలువ_2.సమితి_బిట్టు(0, BIT4_1);
	    ఆపు;
	  స్థితి vpiX:
	    return_విలువ_2.సమితి_బిట్టు(0, BIT4_X);
	    ఆపు;
	  స్థితి vpiZ:
	    return_విలువ_2.సమితి_బిట్టు(0, BIT4_Z);
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported value %d.\n", (పూర్ణాంకం)vp->value.scalar);
	    ప్రతిజ్ఞా(0);
      }
      తిరిగి 0;
}

vpiHandle sysfunc_vec4::put_విలువ_int(p_vpi_విలువ vp)
{
      దీర్ఘ tmp = vp->value.integer;
      గుర్తులేని width = return_విలువ_2.size();
      కొరకు (గుర్తులేని idx = 0 ;  idx < width ;  idx += 1) {
	    return_విలువ_2.సమితి_బిట్టు(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      తిరిగి 0;
}

vpiHandle sysfunc_vec4::put_విలువ_string(p_vpi_విలువ vp)
{
      size_t slen = అక్షరశ్రేణి_పొడవు(vp->value.str);
      గుర్తులేని wid = return_విలువ_2.size();

      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1) {
	    గుర్తులేని cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    ఒకవేళ (cidx >= slen) {
		  return_విలువ_2.సమితి_బిట్టు(idx, BIT4_0);
		  కొనసాగు;
	    }

	      // Put the end of the string in the LSB of the vector
	    అక్షరం use_char = vp->value.str[slen - 1 - cidx];

	    ఒకవేళ ((use_char >> (idx % 8)) & 1)
		  return_విలువ_2.సమితి_బిట్టు(idx, BIT4_1);
	    లేకపోతే
		  return_విలువ_2.సమితి_బిట్టు(idx, BIT4_0);
      }
      తిరిగి 0;
}

vpiHandle sysfunc_vec4::put_విలువ_సదిశ(p_vpi_విలువ vp)
{
      గుర్తులేని width = return_విలువ_2.size();
      కొరకు (గుర్తులేని idx = 0 ; idx < width ; idx += 1) {
	    గుర్తులేని మాట = idx / 32;
	    గుర్తులేని bidx = idx % 32;

	    గుర్తులేని దీర్ఘ aval = vp->value.vector[మాట].aval >> bidx;
	    గుర్తులేని దీర్ఘ bval = vp->value.vector[మాట].bval >> bidx;

	    పూర్ణాంకం బిట్టు = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    ఎంపిక (బిట్టు) {
		స్థితి 0:
		  bit4 = BIT4_0;
		  ఆపు;
		స్థితి 1:
		  bit4 = BIT4_1;
		  ఆపు;
		స్థితి 2:
		  bit4 = BIT4_Z;
		  ఆపు;
		స్థితి 3:
		  bit4 = BIT4_X;
		  ఆపు;
		అప్రమేయ:
		  ప్రతిజ్ఞా(0);
		  bit4 = BIT4_X;
		  ఆపు;
	    }
	    return_విలువ_2.సమితి_బిట్టు(idx, bit4);
      }
      తిరిగి 0;
}

vpiHandle sysfunc_vec4::put_విలువ_సమయం(p_vpi_విలువ vp)
{
      గుర్తులేని width = return_విలువ_2.size();
      దీర్ఘ tmp = 0;
      కొరకు (గుర్తులేని idx = 0 ; idx < width ; idx += 1) {
	    ఒకవేళ (idx == 0)
		  tmp = vp->value.time->దిగువ;
	    లేకపోతే ఒకవేళ (idx == 32)
		  tmp = vp->value.time->ఎగువ;
	    లేకపోతే ఒకవేళ (idx == 64)
		  tmp = 0;

	    return_విలువ_2.సమితి_బిట్టు(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      తిరిగి 0;
}

vpiHandle sysfunc_vec4::vpi_put_విలువ(p_vpi_విలువ vp, పూర్ణాంకం)
{
      put_విలువ = నిజం;

      ఎంపిక (vp->format) {
	  స్థితి vpiScalarVal:
	    తిరిగి put_విలువ_scalar(vp);
	  స్థితి vpiIntVal:
	    తిరిగి put_విలువ_int(vp);
	  స్థితి vpiStringVal:
	    తిరిగి put_విలువ_string(vp);
	  స్థితి vpi_సదిశ_Val:
	    తిరిగి put_విలువ_సదిశ(vp);
	  స్థితి vpi_సమయం_Val:
	    తిరిగి put_విలువ_సమయం(vp);
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);
      }

      తిరిగి 0;
}

నిర్మాణం sysfunc_4net : బహిరంగ sysfunc_def {
      స్పష్టమైన అంతఃవరుస sysfunc_4net(గుర్తులేని wid) : vwid_(wid) { }
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;

    అంతరంగ:
      గుర్తులేని vwid_;
};

// support getting vpiSize for a system function call
పూర్ణాంకం sysfunc_4net::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_కొలత:
	    తిరిగి vwid_;

	  స్థితి vpi_పంక్తి_No:
	    తిరిగి lineno;

	  స్థితి vpiUserDefn:
	    తిరిగి defn->is_user_defn;

	  అప్రమేయ:
	    తిరిగి vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_విలువ(p_vpi_విలువ vp, పూర్ణాంకం)
{
      put_విలువ = నిజం;

      vvp_vector4_t val (vwid_);

      ఎంపిక (vp->format) {

          స్థితి vpiScalarVal: {
	        ఎంపిక(vp->value.scalar) {
		      స్థితి vpi0:
			val.సమితి_బిట్టు(0, BIT4_0);
                        ఆపు;
		      స్థితి vpi1:
                        val.సమితి_బిట్టు(0, BIT4_1);
                        ఆపు;
		      స్థితి vpiX:
                        val.సమితి_బిట్టు(0, BIT4_X);
                        ఆపు;
		      స్థితి vpiZ:
                        val.సమితి_బిట్టు(0, BIT4_Z);
                        ఆపు;
		      అప్రమేయ:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (పూర్ణాంకం)vp->value.scalar);
                        ప్రతిజ్ఞా(0);
                }
		ఆపు;
          }

	  స్థితి vpiIntVal: {
		దీర్ఘ tmp = vp->value.integer;
		కొరకు (గుర్తులేని idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.సమితి_బిట్టు(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		ఆపు;
	  }

          స్థితి vpi_సమయం_Val: {
                గుర్తులేని దీర్ఘ tmp = vp->value.time->దిగువ;
                కొరకు (గుర్తులేని idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.సమితి_బిట్టు(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      ఒకవేళ (idx == 31)
                            tmp = vp->value.time->ఎగువ;
                      లేకపోతే
                            tmp >>= 1;
                }
                ఆపు;
          }

	  స్థితి vpi_సదిశ_Val:

	    కొరకు (గుర్తులేని wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  గుర్తులేని మాట = wdx / 32;
		  గుర్తులేని దీర్ఘ aval = vp->value.vector[మాట].aval;
		  గుర్తులేని దీర్ఘ bval = vp->value.vector[మాట].bval;

		  కొరకు (గుర్తులేని idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			పూర్ణాంకం బిట్టు = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			ఎంపిక (బిట్టు) {
			    స్థితి 0:
			      bit4 = BIT4_0;
			      ఆపు;
			    స్థితి 1:
			      bit4 = BIT4_1;
			      ఆపు;
			    స్థితి 2:
			      bit4 = BIT4_Z;
			      ఆపు;
			    స్థితి 3:
			      bit4 = BIT4_X;
			      ఆపు;
			    అప్రమేయ:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              బిట్టు);
			      ప్రతిజ్ఞా(0);
			}
			val.సమితి_బిట్టు(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (పూర్ణాంకం)vp->format, vwid_);
	    ప్రతిజ్ఞా(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      తిరిగి 0;
}

నిర్మాణం sysfunc_rnet : బహిరంగ sysfunc_def {
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_విలువ(p_vpi_విలువ vp, పూర్ణాంకం)
{
      put_విలువ = నిజం;

      ద్విగుణ value;
      ఎంపిక (vp->format) {

	  స్థితి vpiRealVal:
	    value = vp->value.real;
	    ఆపు;

	  అప్రమేయ:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      తిరిగి 0;
}

నిర్మాణం sysfunc_no : బహిరంగ sysfunc_def {
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
};

vpiHandle sysfunc_no::vpi_put_విలువ(p_vpi_విలువ, పూర్ణాంకం)
{
      తిరిగి 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
స్థిర నిర్మాణం __vpiUserSystf**def_పట్టిక = 0;
స్థిర గుర్తులేని def_లెక్క = 0;

స్థిర నిర్మాణం __vpiUserSystf* allocate_def(శూన్య)
{
      ఒకవేళ (def_పట్టిక == 0) {
	    def_పట్టిక = static_cast<నిర్మాణం __vpiUserSystf**>
	                (జ్ఞాపకం_ఇవ్వు(పరిమాణం(నిర్మాణం __vpiUserSystf*)));

	    def_పట్టిక[0] = కొత్త __vpiUserSystf;

	    def_లెక్క = 1;
	    తిరిగి def_పట్టిక[0];
      }

      def_పట్టిక = static_cast<నిర్మాణం __vpiUserSystf**>
                  (realloc(def_పట్టిక,
                   (def_లెక్క+1)*పరిమాణం(నిర్మాణం __vpiUserSystf*)));

      def_పట్టిక[def_లెక్క] = కొత్త __vpiUserSystf;

      తిరిగి def_పట్టిక[def_లెక్క++];
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య def_పట్టిక_delete(శూన్య)
{
      కొరకు (గుర్తులేని idx = 0; idx < def_లెక్క; idx += 1) {
	    విడుదల(const_cast<అక్షరం *>(def_పట్టిక[idx]->info.tfname));
	    తొలగించు def_పట్టిక[idx];
      }
      విడుదల(def_పట్టిక);
      def_పట్టిక = 0;
      def_లెక్క = 0;
}
#అయితే_ముగింపు

తరగతి __vpiSystfIterator : బహిరంగ __vpiHandle {
    బహిరంగ:
      స్పష్టమైన __vpiSystfIterator(గుర్తులేని idx);
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      vpiHandle vpi_సూచీ(పూర్ణాంకం idx) override;
      free_object_fun_t free_object_fun(శూన్య) override;

      గుర్తులేని తదుపరి_2;
};

స్థిర vpiHandle systf_iterator_పరిశీలన(vpiHandle ref, పూర్ణాంకం)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      ఒకవేళ (obj->తదుపరి_2 >= def_లెక్క) {
	    vpi_free_object(ref);
	    తిరిగి 0;
      }

      గుర్తులేని use_సూచీ = obj->తదుపరి_2;
      అయితే (!def_పట్టిక[use_సూచీ]->is_user_defn) {
	    obj->తదుపరి_2 += 1;
	    use_సూచీ = obj->తదుపరి_2;
	    ఒకవేళ (obj->తదుపరి_2 >= def_లెక్క) {
		  vpi_free_object(ref);
		  తిరిగి 0;
	    }
      }
      obj->తదుపరి_2 += 1;
      తిరిగి def_పట్టిక[use_సూచీ];
}

స్థిర పూర్ణాంకం systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      తొలగించు obj;
      తిరిగి 1;
}

అంతఃవరుస __vpiSystfIterator::__vpiSystfIterator(గుర్తులేని idx)
{ తదుపరి_2 = idx; }

పూర్ణాంకం __vpiSystfIterator::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_సూచీ(పూర్ణాంకం idx)
{ తిరిగి systf_iterator_పరిశీలన(ఇది, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(శూన్య)
{ తిరిగి &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(శూన్య)
{
	/* Check to see if there are any user defined functions. */
      తార్కిక have_user_defn = అబద్ధం;
      గుర్తులేని idx;
      కొరకు (idx = 0; idx < def_లెక్క; idx += 1) {
	    ఒకవేళ (def_పట్టిక[idx]->is_user_defn) {
		  have_user_defn = నిజం;
		  ఆపు;
	    }
      }
      ఒకవేళ (!have_user_defn) తిరిగి 0;

      __vpiSystfIterator*res = కొత్త __vpiSystfIterator(idx);
      res->తదుపరి_2 = idx;
      తిరిగి res;
}

నిర్మాణం __vpiUserSystf* vpip_కనుగొను_systf(స్థిరరాశి అక్షరం*name)
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < def_లెక్క ;  idx += 1)
	    ఒకవేళ (strcmp(def_పట్టిక[idx]->info.tfname, name) == 0)
		  తిరిగి def_పట్టిక[idx];

      తిరిగి 0;
}

శూన్య vpip_make_systf_system_defined(vpiHandle ref)
{
      ప్రతిజ్ఞా(ref);
      నిర్మాణం __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      ప్రతిజ్ఞా(obj);
      obj->is_user_defn = అబద్ధం;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
గణన vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
రకనిర్వచనం నిర్మాణం vpi_call_error {
      vpi_call_error_type type;
      అక్షరం *name;
      దీర్ఘ file_idx;
      దీర్ఘ lineno;
} vpi_call_error_s, *vpi_call_error_p;

స్థిర vpi_call_error_p vpi_call_error_lst = ఖాళీ;
స్థిర గుర్తులేని vpi_call_error_అంకె = 0;

స్థిర శూన్య కూడు_vpi_call_error(vpi_call_error_type type, స్థిరరాశి అక్షరం *name,
                               దీర్ఘ file_idx, దీర్ఘ lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_అంకె + 1) *
                                    పరిమాణం(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_అంకె].type = type;
      vpi_call_error_lst[vpi_call_error_అంకె].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_అంకె].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_అంకె].lineno = lineno;
      vpi_call_error_అంకె += 1;
}

శూన్య print_vpi_call_errors()
{
      కొరకు (గుర్తులేని idx = 0; idx < vpi_call_error_అంకె; idx += 1) {
	    ఎంపిక (vpi_call_error_lst[idx].type) {
		స్థితి VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_పేర్లు[vpi_call_error_lst[idx].file_idx],
		                  (పూర్ణాంకం)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ఆపు;
		స్థితి VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_పేర్లు[vpi_call_error_lst[idx].file_idx],
		                  (పూర్ణాంకం)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ఆపు;
		స్థితి VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_పేర్లు[vpi_call_error_lst[idx].file_idx],
		                  (పూర్ణాంకం)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ఆపు;
		స్థితి VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_పేర్లు[vpi_call_error_lst[idx].file_idx],
		                  (పూర్ణాంకం)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_పేర్లు[vpi_call_error_lst[idx].file_idx],
                                 (పూర్ణాంకం)vpi_call_error_lst[idx].lineno);
		  ఆపు;
	    }
	    విడుదల(vpi_call_error_lst[idx].name);
      }
      విడుదల(vpi_call_error_lst);
      fflush(stderr);
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర శూన్య cleanup_vpi_call_వాదనలు(గుర్తులేని argc, vpiHandle*argv)
{
      ఒకవేళ (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    నిర్మాణం __vpiSysTaskCall*obj = కొత్త systask_def;
	    obj->nargs = argc;
	    obj->వాదనలు  = argv;
	    vpi_call_delete(obj);
      }
}
#అయితే_ముగింపు

/*
 * A vpi_call is actually built up into a vpiSysTaskCall VPI object
 * that refers back to the vpiUserSystf VPI object that is the
 * definition. So this function is called by the compiler when a
 * %vpi_call statement is encountered. Create here a vpiHandle that
 * describes the call, and return it. The %vpi_call instruction will
 * store this handle for when it is executed.
 */
vpiHandle vpip_build_vpi_call(స్థిరరాశి అక్షరం*name, పూర్ణాంకం val_సంకేతం, గుర్తులేని return_వెడల్పు,
			      vvp_net_t*fnet,
			      తార్కిక func_as_task_err, తార్కిక func_as_task_warn,
			      గుర్తులేని argc, vpiHandle*argv,
			      గుర్తులేని vec4_దొంతర, గుర్తులేని real_దొంతర, గుర్తులేని string_దొంతర,
			      దీర్ఘ file_idx, దీర్ఘ lineno)
{
      ప్రతిజ్ఞా(!(func_as_task_err && func_as_task_warn));

      నిర్మాణం __vpiUserSystf*defn = vpip_కనుగొను_systf(name);
      ఒకవేళ (defn == 0) {
	    కూడు_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#నిర్వచితమైతే CHECK_WITH_VALGRIND
	    cleanup_vpi_call_వాదనలు(argc, argv);
#అయితే_ముగింపు
	    తిరిగి 0;
      }

      ఎంపిక (defn->info.type) {
	  స్థితి vpiSysTask:
	    ఒకవేళ (val_సంకేతం != 0 || fnet != 0) {
		  కూడు_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#నిర్వచితమైతే CHECK_WITH_VALGRIND
		  cleanup_vpi_call_వాదనలు(argc, argv);
#అయితే_ముగింపు
		  తిరిగి 0;
	    }
	    ఆపు;

	  స్థితి vpiSysFunc:
	    ఒకవేళ (val_సంకేతం == 0 && fnet == 0) {
		  ఒకవేళ (func_as_task_err) {
			కూడు_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#నిర్వచితమైతే CHECK_WITH_VALGRIND
			cleanup_vpi_call_వాదనలు(argc, argv);
#అయితే_ముగింపు
			తిరిగి 0;
		  } లేకపోతే ఒకవేళ (func_as_task_warn) {
			కూడు_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (పూర్ణాంకం)defn->info.type);
	    ప్రతిజ్ఞా(0);
      }

      నిర్మాణం __vpiSysTaskCall*obj = 0;

      ఎంపిక (defn->info.type) {
	  స్థితి vpiSysTask:
	    obj = కొత్త systask_def;
	    ఆపు;

	  స్థితి vpiSysFunc:
	    ఒకవేళ (fnet && val_సంకేతం == -vpiRealVal) {
		  obj = కొత్త sysfunc_rnet;

	    } లేకపోతే ఒకవేళ (fnet && val_సంకేతం == -vpi_సదిశ_Val) {
		  obj = కొత్త sysfunc_4net(return_వెడల్పు);

	    } లేకపోతే ఒకవేళ (val_సంకేతం == -vpiRealVal) {
		  obj = కొత్త sysfunc_real;

	    } లేకపోతే ఒకవేళ (val_సంకేతం == -vpi_సదిశ_Val) {
		  obj = కొత్త sysfunc_vec4(return_వెడల్పు);

	    } లేకపోతే ఒకవేళ (val_సంకేతం == -vpiStringVal) {
		  obj = కొత్త sysfunc_str;

           } లేకపోతే ఒకవేళ (val_సంకేతం == 0 && fnet == 0) {
		  obj = కొత్త sysfunc_no;

	    } లేకపోతే {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_సంకేతం);
		  ప్రతిజ్ఞా(0);
	    }
	    ఆపు;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->వాదనలు  = argv;
      obj->vec4_దొంతర = vec4_దొంతర;
      obj->real_దొంతర = real_దొంతర;
      obj->string_దొంతర = string_దొంతర;
      obj->fnet  = fnet;
      obj->file_idx  = (గుర్తులేని) file_idx;
      obj->lineno   = (గుర్తులేని) lineno;
      obj->userdata  = 0;
      obj->put_విలువ = అబద్ధం;

      compile_compiletf(obj);

      తిరిగి obj;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vpi_call_delete(vpiHandle అంశం)
{
      నిర్మాణం __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(అంశం);
	/* The object can be NULL if there was an error. */
      ఒకవేళ (!obj) తిరిగి;
      కొరకు (గుర్తులేని arg = 0; arg < obj->nargs; arg += 1) {
	    ఎంపిక (obj->వాదనలు[arg]->get_type_సంకేతం()) {
		స్థితి vpiConstant:
		  ఎంపిక (vpi_get(_vpiFromThr, obj->వాదనలు[arg])) {
		      స్థితి _vpiNoThr:
			constant_delete(obj->వాదనలు[arg]);
			ఆపు;
		      స్థితి _vpiString:
			thread_string_delete(obj->వాదనలు[arg]);
			ఆపు;
		      స్థితి _vpiVThr:
			thread_vthr_delete(obj->వాదనలు[arg]);
			ఆపు;
		      స్థితి vpi_మాట:
			thread_మాట_delete(obj->వాదనలు[arg]);
			ఆపు;
		      అప్రమేయ:
			ప్రతిజ్ఞా(0);
		  }
		  ఆపు;
		స్థితి vpi_జ్ఞాపకం_మాట:
		  ఎంపిక (vpi_get(_vpiFromThr, obj->వాదనలు[arg])) {
		      స్థితి _vpi_at_A:
			A_delete(obj->వాదనలు[arg]);
			ఆపు;
		  }
		  ఆపు;
		స్థితి vpiPartSelect:
		  ఎంపిక (vpi_get(_vpiFromThr, obj->వాదనలు[arg])) {
		      స్థితి _vpi_at_PV:
			PV_delete(obj->వాదనలు[arg]);
			ఆపు;
		      స్థితి _vpi_at_APV:
			APV_delete(obj->వాదనలు[arg]);
			ఆపు;
		  }
		  ఆపు;
	    }
      }
      విడుదల(obj->వాదనలు);
      తొలగించు obj;
}
#అయితే_ముగింపు

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

శూన్య vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      ఒకవేళ (vpip_cur_task->defn->info.calltf) {
	    ప్రతిజ్ఞా(vpi_బహుళకం_flag == VPI_బహుళకం_NONE);
	    vpi_బహుళకం_flag = VPI_బహుళకం_CALLTF;
	    vpip_cur_task->put_విలువ = అబద్ధం;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_దత్తాంశం);
	    vpi_బహుళకం_flag = VPI_బహుళకం_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    ఒకవేళ (ref->get_type_సంకేతం() == vpiSysFuncCall &&
	        !vpip_cur_task->put_విలువ) {
		  s_vpi_విలువ val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_విలువ(ref, &val, 0, vpiNoDelay);
	    }
      }
      ఒకవేళ (vpip_cur_task->vec4_దొంతర > 0)
	    vthread_తీయి_vec4(thr, vpip_cur_task->vec4_దొంతర);
      ఒకవేళ (vpip_cur_task->real_దొంతర > 0)
	    vthread_తీయి_real(thr, vpip_cur_task->real_దొంతర);
      ఒకవేళ (vpip_cur_task->string_దొంతర > 0)
	    vthread_తీయి_str(thr, vpip_cur_task->string_దొంతర);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      ఒకవేళ (స్థిరరాశి sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_నెట్టు(thr, func_real->return_విలువ());
      }
      లేకపోతే ఒకవేళ (స్థిరరాశి sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_నెట్టు(thr, func_string->return_విలువ());
      }
      లేకపోతే ఒకవేళ (స్థిరరాశి sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_నెట్టు(thr, func_vec4->return_విలువ());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_రిజిస్టరు_systf(స్థిరరాశి నిర్మాణం t_vpi_systf_దత్తాంశం*ss)
{
      నిర్మాణం __vpiUserSystf*cur = allocate_def();
      ప్రతిజ్ఞా(ss);
      ఎంపిక (ss->type) {
	  స్థితి vpiSysTask:
	  స్థితి vpiSysFunc:
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported type %d.\n", (పూర్ణాంకం)ss->type);
	    ప్రతిజ్ఞా(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = నిజం;

      తిరిగి cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, శూన్య*దత్తాంశం)
{
      నిర్మాణం __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ఒకవేళ (rfp == 0)
	    తిరిగి 0;

      rfp->userdata = దత్తాంశం;
      తిరిగి 1;
}

శూన్య* vpi_get_userdata(vpiHandle ref)
{
      నిర్మాణం __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ప్రతిజ్ఞా(rfp);

      తిరిగి rfp->userdata;
}
