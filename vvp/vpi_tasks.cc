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

using namespace std;

anta_hstha __vpiUserSystf::__vpiUserSystf()
{ }

puur_naa_nka __vpiUserSystf::get_type_code(void) const
{ ꦮꦭꦶ vpiUserSystf; }


static vpiHandle systask_handle(puur_naa_nka type, vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      switch (type) {
	  pak_sa vpiScope:
	    ꦮꦭꦶ rfp->scope;

	  pak_sa vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    assert(rfp->defn->is_user_defn);
	    ꦮꦭꦶ rfp->defn;

	  default:
	    ꦮꦭꦶ 0;
      };
}

static puur_naa_nka systask_get(puur_naa_nka type, vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      switch (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  pak_sa vpiTimeUnit:
	    ꦮꦭꦶ rfp->scope->time_units;
	  pak_sa vpiTimePrecision:
	    ꦮꦭꦶ rfp->scope->time_precision;

	  pak_sa vpiLineNo:
	    ꦮꦭꦶ rfp->lineno;

	  pak_sa vpiUserDefn:
	    ꦮꦭꦶ rfp->defn->is_user_defn;

	  default:
	    ꦮꦭꦶ vpiUndefined;
      }
}

// support getting vpiSize for a system function call
static puur_naa_nka sysfunc_get(puur_naa_nka type, vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      switch (type) {
	  pak_sa vpiSize:
	    assert(0); // This should be handled by derived classes
	    ꦮꦭꦶ 0;

	  pak_sa vpiLineNo:
	    ꦮꦭꦶ rfp->lineno;

	  pak_sa vpiUserDefn:
	    ꦮꦭꦶ rfp->defn->is_user_defn;

	  default:
	    ꦮꦭꦶ vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

static var_na *systask_get_str(puur_naa_nka type, vpiHandle ref)
{
      const sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      switch (type) {
          pak_sa vpiFile:
            assert(rfp->file_idx < file_names.size());
            ꦮꦭꦶ simple_set_rbuf_str(file_names[rfp->file_idx]);

          pak_sa vpiName:
            ꦮꦭꦶ simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      ꦮꦭꦶ 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
static vpiHandle systask_iter(puur_naa_nka, vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ꦪꦼꦤ꧀ (rfp->nargs == 0)
	    ꦮꦭꦶ 0;

      ꦮꦭꦶ vpip_make_iterator(rfp->nargs, rfp->args, false);
}

sa_nracanaa systask_def : public __vpiSysTaskCall {
      virtual puur_naa_nka get_type_code(void) const   override { ꦮꦭꦶ vpiSysTaskCall; }
      virtual puur_naa_nka vpi_get(puur_naa_nka code)           override { ꦮꦭꦶ systask_get(code, this); }
      virtual var_na*vpi_get_str(puur_naa_nka code)      override { ꦮꦭꦶ systask_get_str(code, this); }
      virtual vpiHandle vpi_handle(puur_naa_nka code)  override { ꦮꦭꦶ systask_handle(code, this); }
      virtual vpiHandle vpi_iterate(puur_naa_nka code) override { ꦮꦭꦶ systask_iter(code, this); }
};

sa_nracanaa sysfunc_def : public systask_def {
      virtual puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiSysFuncCall; }
      virtual puur_naa_nka vpi_get(puur_naa_nka code) override { ꦮꦭꦶ sysfunc_get(code, this); }
};

sa_nracanaa sysfunc_real : public sysfunc_def {
      vpiHandle vpi_put_value(p_vpi_value val, puur_naa_nka flags) override;
      anta_hstha dviguu_na return_value() const { ꦮꦭꦶ return_value_; }

    private:
      dviguu_na return_value_;
};

vpiHandle sysfunc_real::vpi_put_value(p_vpi_value vp, puur_naa_nka)
{
      put_value = true;

      return_value_ = 0.0;
      switch (vp->format) {
	  pak_sa vpiRealVal:
	    return_value_ = vp->value.real;
	    bha_nga;
	  default:
	    fprintf(stderr, "Unsupported format %d.\n", (puur_naa_nka)vp->format);
	    assert(0);
      }

      ꦮꦭꦶ 0;
}

sa_nracanaa sysfunc_str : public sysfunc_def {
      puur_naa_nka vpi_get(puur_naa_nka code) override;
      vpiHandle vpi_put_value(p_vpi_value val, puur_naa_nka flags) override;
      anta_hstha const std::string& return_value() const { ꦮꦭꦶ return_value_; };

    private:
      std::string return_value_;
};

puur_naa_nka sysfunc_str::vpi_get(puur_naa_nka code)
{
      switch (code) {
	  pak_sa vpiSize:
	    ꦮꦭꦶ return_value_.size();

	  pak_sa vpiLineNo:
	    ꦮꦭꦶ lineno;

	  pak_sa vpiUserDefn:
	    ꦮꦭꦶ defn->is_user_defn;

	  default:
	    ꦮꦭꦶ vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_value(p_vpi_value vp, puur_naa_nka)
{
      put_value = true;

      switch (vp->format) {
	  pak_sa vpiStringVal:
	    return_value_ = std::string(vp->value.str);
	    bha_nga;
	  default:
	    fprintf(stderr, "Unsupported format %d.\n", (puur_naa_nka)vp->format);
	    assert(0);
      }

      ꦮꦭꦶ 0;
}

class sysfunc_vec4 : public sysfunc_def {
    public:
      explicit anta_hstha sysfunc_vec4(unsigned wid): return_value_(wid, BIT4_X) { }
      puur_naa_nka vpi_get(puur_naa_nka code) override;
      vpiHandle vpi_put_value(p_vpi_value val, puur_naa_nka flags) override;
      anta_hstha const vvp_vector4_t& return_value() const { ꦮꦭꦶ return_value_; }

    private:
      vpiHandle put_value_scalar_(p_vpi_value vp);
      vpiHandle put_value_int_(p_vpi_value vp);
      vpiHandle put_value_string_(p_vpi_value vp);
      vpiHandle put_value_vector_(p_vpi_value vp);
      vpiHandle put_value_time_(p_vpi_value vp);

      vvp_vector4_t return_value_;
};

puur_naa_nka sysfunc_vec4::vpi_get(puur_naa_nka code)
{
      switch (code) {
	  pak_sa vpiSize:
	    ꦮꦭꦶ return_value_.size();

	  pak_sa vpiLineNo:
	    ꦮꦭꦶ lineno;

	  pak_sa vpiUserDefn:
	    ꦮꦭꦶ defn->is_user_defn;

	  default:
	    ꦮꦭꦶ vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_value_scalar_(const p_vpi_value vp)
{
      switch (vp->value.scalar) {
	  pak_sa vpi0:
	    return_value_.set_bit(0, BIT4_0);
	    bha_nga;
	  pak_sa vpi1:
	    return_value_.set_bit(0, BIT4_1);
	    bha_nga;
	  pak_sa vpiX:
	    return_value_.set_bit(0, BIT4_X);
	    bha_nga;
	  pak_sa vpiZ:
	    return_value_.set_bit(0, BIT4_Z);
	    bha_nga;
	  default:
	    fprintf(stderr, "Unsupported value %d.\n", (puur_naa_nka)vp->value.scalar);
	    assert(0);
      }
      ꦮꦭꦶ 0;
}

vpiHandle sysfunc_vec4::put_value_int_(p_vpi_value vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_value_.size();
      ꦏꦁꦒꦺꦴ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_value_.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      ꦮꦭꦶ 0;
}

vpiHandle sysfunc_vec4::put_value_string_(p_vpi_value vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_value_.size();

      ꦏꦁꦒꦺꦴ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    ꦪꦼꦤ꧀ (cidx >= slen) {
		  return_value_.set_bit(idx, BIT4_0);
		  continue;
	    }

	      // Put the end of the string in the LSB of the vector
	    var_na use_char = vp->value.str[slen - 1 - cidx];

	    ꦪꦼꦤ꧀ ((use_char >> (idx % 8)) & 1)
		  return_value_.set_bit(idx, BIT4_1);
	    ꦭꦶꦪ
		  return_value_.set_bit(idx, BIT4_0);
      }
      ꦮꦭꦶ 0;
}

vpiHandle sysfunc_vec4::put_value_vector_(p_vpi_value vp)
{
      unsigned width = return_value_.size();
      ꦏꦁꦒꦺꦴ (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    puur_naa_nka bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    switch (bit) {
		pak_sa 0:
		  bit4 = BIT4_0;
		  bha_nga;
		pak_sa 1:
		  bit4 = BIT4_1;
		  bha_nga;
		pak_sa 2:
		  bit4 = BIT4_Z;
		  bha_nga;
		pak_sa 3:
		  bit4 = BIT4_X;
		  bha_nga;
		default:
		  assert(0);
		  bit4 = BIT4_X;
		  bha_nga;
	    }
	    return_value_.set_bit(idx, bit4);
      }
      ꦮꦭꦶ 0;
}

vpiHandle sysfunc_vec4::put_value_time_(p_vpi_value vp)
{
      unsigned width = return_value_.size();
      long tmp = 0;
      ꦏꦁꦒꦺꦴ (unsigned idx = 0 ; idx < width ; idx += 1) {
	    ꦪꦼꦤ꧀ (idx == 0)
		  tmp = vp->value.time->low;
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (idx == 32)
		  tmp = vp->value.time->high;
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (idx == 64)
		  tmp = 0;

	    return_value_.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      ꦮꦭꦶ 0;
}

vpiHandle sysfunc_vec4::vpi_put_value(p_vpi_value vp, puur_naa_nka)
{
      put_value = true;

      switch (vp->format) {
	  pak_sa vpiScalarVal:
	    ꦮꦭꦶ put_value_scalar_(vp);
	  pak_sa vpiIntVal:
	    ꦮꦭꦶ put_value_int_(vp);
	  pak_sa vpiStringVal:
	    ꦮꦭꦶ put_value_string_(vp);
	  pak_sa vpiVectorVal:
	    ꦮꦭꦶ put_value_vector_(vp);
	  pak_sa vpiTimeVal:
	    ꦮꦭꦶ put_value_time_(vp);
	  default:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (puur_naa_nka)vp->format);
	    assert(0);
      }

      ꦮꦭꦶ 0;
}

sa_nracanaa sysfunc_4net : public sysfunc_def {
      explicit anta_hstha sysfunc_4net(unsigned wid) : vwid_(wid) { }
      puur_naa_nka vpi_get(puur_naa_nka code) override;
      vpiHandle vpi_put_value(p_vpi_value val, puur_naa_nka flags) override;

    private:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
puur_naa_nka sysfunc_4net::vpi_get(puur_naa_nka code)
{
      switch (code) {
	  pak_sa vpiSize:
	    ꦮꦭꦶ vwid_;

	  pak_sa vpiLineNo:
	    ꦮꦭꦶ lineno;

	  pak_sa vpiUserDefn:
	    ꦮꦭꦶ defn->is_user_defn;

	  default:
	    ꦮꦭꦶ vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_value(p_vpi_value vp, puur_naa_nka)
{
      put_value = true;

      vvp_vector4_t val (vwid_);

      switch (vp->format) {

          pak_sa vpiScalarVal: {
	        switch(vp->value.scalar) {
		      pak_sa vpi0:
			val.set_bit(0, BIT4_0);
                        bha_nga;
		      pak_sa vpi1:
                        val.set_bit(0, BIT4_1);
                        bha_nga;
		      pak_sa vpiX:
                        val.set_bit(0, BIT4_X);
                        bha_nga;
		      pak_sa vpiZ:
                        val.set_bit(0, BIT4_Z);
                        bha_nga;
		      default:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (puur_naa_nka)vp->value.scalar);
                        assert(0);
                }
		bha_nga;
          }

	  pak_sa vpiIntVal: {
		long tmp = vp->value.integer;
		ꦏꦁꦒꦺꦴ (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		bha_nga;
	  }

          pak_sa vpiTimeVal: {
                unsigned long tmp = vp->value.time->low;
                ꦏꦁꦒꦺꦴ (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      ꦪꦼꦤ꧀ (idx == 31)
                            tmp = vp->value.time->high;
                      ꦭꦶꦪ
                            tmp >>= 1;
                }
                bha_nga;
          }

	  pak_sa vpiVectorVal:

	    ꦏꦁꦒꦺꦴ (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  ꦏꦁꦒꦺꦴ (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			puur_naa_nka bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			switch (bit) {
			    pak_sa 0:
			      bit4 = BIT4_0;
			      bha_nga;
			    pak_sa 1:
			      bit4 = BIT4_1;
			      bha_nga;
			    pak_sa 2:
			      bit4 = BIT4_Z;
			      bha_nga;
			    pak_sa 3:
			      bit4 = BIT4_X;
			      bha_nga;
			    default:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              bit);
			      assert(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    bha_nga;

	  default:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (puur_naa_nka)vp->format, vwid_);
	    assert(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      ꦮꦭꦶ 0;
}

sa_nracanaa sysfunc_rnet : public sysfunc_def {
      vpiHandle vpi_put_value(p_vpi_value val, puur_naa_nka flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_value(p_vpi_value vp, puur_naa_nka)
{
      put_value = true;

      dviguu_na value;
      switch (vp->format) {

	  pak_sa vpiRealVal:
	    value = vp->value.real;
	    bha_nga;

	  default:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (puur_naa_nka)vp->format);
	    assert(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      ꦮꦭꦶ 0;
}

sa_nracanaa sysfunc_no : public sysfunc_def {
      vpiHandle vpi_put_value(p_vpi_value val, puur_naa_nka flags) override;
};

vpiHandle sysfunc_no::vpi_put_value(p_vpi_value, puur_naa_nka)
{
      ꦮꦭꦶ 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
static sa_nracanaa __vpiUserSystf**def_table = 0;
static unsigned def_count = 0;

static sa_nracanaa __vpiUserSystf* allocate_def(void)
{
      ꦪꦼꦤ꧀ (def_table == 0) {
	    def_table = static_cast<sa_nracanaa __vpiUserSystf**>
	                (malloc(parimaa_na(sa_nracanaa __vpiUserSystf*)));

	    def_table[0] = new __vpiUserSystf;

	    def_count = 1;
	    ꦮꦭꦶ def_table[0];
      }

      def_table = static_cast<sa_nracanaa __vpiUserSystf**>
                  (realloc(def_table,
                   (def_count+1)*parimaa_na(sa_nracanaa __vpiUserSystf*)));

      def_table[def_count] = new __vpiUserSystf;

      ꦮꦭꦶ def_table[def_count++];
}

#ifdef CHECK_WITH_VALGRIND
void def_table_delete(void)
{
      ꦏꦁꦒꦺꦴ (unsigned idx = 0; idx < def_count; idx += 1) {
	    free(const_cast<var_na *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      free(def_table);
      def_table = 0;
      def_count = 0;
}
#endif

class __vpiSystfIterator : public __vpiHandle {
    public:
      explicit __vpiSystfIterator(unsigned idx);
      puur_naa_nka get_type_code(void) const override;
      vpiHandle vpi_index(puur_naa_nka idx) override;
      free_object_fun_t free_object_fun(void) override;

      unsigned next;
};

static vpiHandle systf_iterator_scan(vpiHandle ref, puur_naa_nka)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      ꦪꦼꦤ꧀ (obj->next >= def_count) {
	    vpi_free_object(ref);
	    ꦮꦭꦶ 0;
      }

      unsigned use_index = obj->next;
      ꦱꦼꦢꦼꦭ (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    ꦪꦼꦤ꧀ (obj->next >= def_count) {
		  vpi_free_object(ref);
		  ꦮꦭꦶ 0;
	    }
      }
      obj->next += 1;
      ꦮꦭꦶ def_table[use_index];
}

static puur_naa_nka systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      ꦮꦭꦶ 1;
}

anta_hstha __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

puur_naa_nka __vpiSystfIterator::get_type_code(void) const
{ ꦮꦭꦶ vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(puur_naa_nka idx)
{ ꦮꦭꦶ systf_iterator_scan(this, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(void)
{ ꦮꦭꦶ &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(void)
{
	/* Check to see if there are any user defined functions. */
      bool have_user_defn = false;
      unsigned idx;
      ꦏꦁꦒꦺꦴ (idx = 0; idx < def_count; idx += 1) {
	    ꦪꦼꦤ꧀ (def_table[idx]->is_user_defn) {
		  have_user_defn = true;
		  bha_nga;
	    }
      }
      ꦪꦼꦤ꧀ (!have_user_defn) ꦮꦭꦶ 0;

      __vpiSystfIterator*res = new __vpiSystfIterator(idx);
      res->next = idx;
      ꦮꦭꦶ res;
}

sa_nracanaa __vpiUserSystf* vpip_find_systf(const var_na*name)
{
      ꦏꦁꦒꦺꦴ (unsigned idx = 0 ;  idx < def_count ;  idx += 1)
	    ꦪꦼꦤ꧀ (strcmp(def_table[idx]->info.tfname, name) == 0)
		  ꦮꦭꦶ def_table[idx];

      ꦮꦭꦶ 0;
}

void vpip_make_systf_system_defined(vpiHandle ref)
{
      assert(ref);
      sa_nracanaa __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      assert(obj);
      obj->is_user_defn = false;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
ga_nanaa vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
typedef sa_nracanaa vpi_call_error {
      vpi_call_error_type type;
      var_na *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

static vpi_call_error_p vpi_call_error_lst = NULL;
static unsigned vpi_call_error_num = 0;

static void add_vpi_call_error(vpi_call_error_type type, const var_na *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_num + 1) *
                                    parimaa_na(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_num].type = type;
      vpi_call_error_lst[vpi_call_error_num].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_num].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_num].lineno = lineno;
      vpi_call_error_num += 1;
}

void print_vpi_call_errors()
{
      ꦏꦁꦒꦺꦴ (unsigned idx = 0; idx < vpi_call_error_num; idx += 1) {
	    switch (vpi_call_error_lst[idx].type) {
		pak_sa VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (puur_naa_nka)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  bha_nga;
		pak_sa VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (puur_naa_nka)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  bha_nga;
		pak_sa VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (puur_naa_nka)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  bha_nga;
		pak_sa VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (puur_naa_nka)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (puur_naa_nka)vpi_call_error_lst[idx].lineno);
		  bha_nga;
	    }
	    free(vpi_call_error_lst[idx].name);
      }
      free(vpi_call_error_lst);
      fflush(stderr);
}

#ifdef CHECK_WITH_VALGRIND
static void cleanup_vpi_call_args(unsigned argc, vpiHandle*argv)
{
      ꦪꦼꦤ꧀ (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    sa_nracanaa __vpiSysTaskCall*obj = new systask_def;
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
vpiHandle vpip_build_vpi_call(const var_na*name, puur_naa_nka val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      bool func_as_task_err, bool func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_stack, unsigned real_stack, unsigned string_stack,
			      long file_idx, long lineno)
{
      assert(!(func_as_task_err && func_as_task_warn));

      sa_nracanaa __vpiUserSystf*defn = vpip_find_systf(name);
      ꦪꦼꦤ꧀ (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    ꦮꦭꦶ 0;
      }

      switch (defn->info.type) {
	  pak_sa vpiSysTask:
	    ꦪꦼꦤ꧀ (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  ꦮꦭꦶ 0;
	    }
	    bha_nga;

	  pak_sa vpiSysFunc:
	    ꦪꦼꦤ꧀ (val_code == 0 && fnet == 0) {
		  ꦪꦼꦤ꧀ (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			ꦮꦭꦶ 0;
		  } ꦭꦶꦪ ꦪꦼꦤ꧀ (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    bha_nga;

	  default:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (puur_naa_nka)defn->info.type);
	    assert(0);
      }

      sa_nracanaa __vpiSysTaskCall*obj = 0;

      switch (defn->info.type) {
	  pak_sa vpiSysTask:
	    obj = new systask_def;
	    bha_nga;

	  pak_sa vpiSysFunc:
	    ꦪꦼꦤ꧀ (fnet && val_code == -vpiRealVal) {
		  obj = new sysfunc_rnet;

	    } ꦭꦶꦪ ꦪꦼꦤ꧀ (fnet && val_code == -vpiVectorVal) {
		  obj = new sysfunc_4net(return_width);

	    } ꦭꦶꦪ ꦪꦼꦤ꧀ (val_code == -vpiRealVal) {
		  obj = new sysfunc_real;

	    } ꦭꦶꦪ ꦪꦼꦤ꧀ (val_code == -vpiVectorVal) {
		  obj = new sysfunc_vec4(return_width);

	    } ꦭꦶꦪ ꦪꦼꦤ꧀ (val_code == -vpiStringVal) {
		  obj = new sysfunc_str;

           } ꦭꦶꦪ ꦪꦼꦤ꧀ (val_code == 0 && fnet == 0) {
		  obj = new sysfunc_no;

	    } ꦭꦶꦪ {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  assert(0);
	    }
	    bha_nga;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_stack = vec4_stack;
      obj->real_stack = real_stack;
      obj->string_stack = string_stack;
      obj->fnet  = fnet;
      obj->file_idx  = (unsigned) file_idx;
      obj->lineno   = (unsigned) lineno;
      obj->userdata  = 0;
      obj->put_value = false;

      compile_compiletf(obj);

      ꦮꦭꦶ obj;
}

#ifdef CHECK_WITH_VALGRIND
void vpi_call_delete(vpiHandle item)
{
      sa_nracanaa __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      ꦪꦼꦤ꧀ (!obj) ꦮꦭꦶ;
      ꦏꦁꦒꦺꦴ (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    switch (obj->args[arg]->get_type_code()) {
		pak_sa vpiConstant:
		  switch (vpi_get(_vpiFromThr, obj->args[arg])) {
		      pak_sa _vpiNoThr:
			constant_delete(obj->args[arg]);
			bha_nga;
		      pak_sa _vpiString:
			thread_string_delete(obj->args[arg]);
			bha_nga;
		      pak_sa _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			bha_nga;
		      pak_sa _vpiWord:
			thread_word_delete(obj->args[arg]);
			bha_nga;
		      default:
			assert(0);
		  }
		  bha_nga;
		pak_sa vpiMemoryWord:
		  switch (vpi_get(_vpiFromThr, obj->args[arg])) {
		      pak_sa _vpi_at_A:
			A_delete(obj->args[arg]);
			bha_nga;
		  }
		  bha_nga;
		pak_sa vpiPartSelect:
		  switch (vpi_get(_vpiFromThr, obj->args[arg])) {
		      pak_sa _vpi_at_PV:
			PV_delete(obj->args[arg]);
			bha_nga;
		      pak_sa _vpi_at_APV:
			APV_delete(obj->args[arg]);
			bha_nga;
		  }
		  bha_nga;
	    }
      }
      free(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

void vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      ꦪꦼꦤ꧀ (vpip_cur_task->defn->info.calltf) {
	    assert(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_value = false;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    ꦪꦼꦤ꧀ (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_value) {
		  s_vpi_value val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_value(ref, &val, 0, vpiNoDelay);
	    }
      }
      ꦪꦼꦤ꧀ (vpip_cur_task->vec4_stack > 0)
	    vthread_pop_vec4(thr, vpip_cur_task->vec4_stack);
      ꦪꦼꦤ꧀ (vpip_cur_task->real_stack > 0)
	    vthread_pop_real(thr, vpip_cur_task->real_stack);
      ꦪꦼꦤ꧀ (vpip_cur_task->string_stack > 0)
	    vthread_pop_str(thr, vpip_cur_task->string_stack);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      ꦪꦼꦤ꧀ (const sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_push(thr, func_real->return_value());
      }
      ꦭꦶꦪ ꦪꦼꦤ꧀ (const sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_push(thr, func_string->return_value());
      }
      ꦭꦶꦪ ꦪꦼꦤ꧀ (const sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_push(thr, func_vec4->return_value());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(const sa_nracanaa t_vpi_systf_data*ss)
{
      sa_nracanaa __vpiUserSystf*cur = allocate_def();
      assert(ss);
      switch (ss->type) {
	  pak_sa vpiSysTask:
	  pak_sa vpiSysFunc:
	    bha_nga;
	  default:
	    fprintf(stderr, "Unsupported type %d.\n", (puur_naa_nka)ss->type);
	    assert(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = true;

      ꦮꦭꦶ cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, void*data)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ꦪꦼꦤ꧀ (rfp == 0)
	    ꦮꦭꦶ 0;

      rfp->userdata = data;
      ꦮꦭꦶ 1;
}

void* vpi_get_userdata(vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      assert(rfp);

      ꦮꦭꦶ rfp->userdata;
}
