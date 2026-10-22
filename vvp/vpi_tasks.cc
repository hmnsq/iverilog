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

ຈຳນວນເຕັມ __vpiUserSystf::get_type_code(void) const
{ ສົ່ງຄືນ vpiUserSystf; }


static vpiHandle systask_handle(ຈຳນວນເຕັມ type, vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      switch (type) {
	  pak_sa vpiScope:
	    ສົ່ງຄືນ rfp->scope;

	  pak_sa vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    assert(rfp->defn->is_user_defn);
	    ສົ່ງຄືນ rfp->defn;

	  default:
	    ສົ່ງຄືນ 0;
      };
}

static ຈຳນວນເຕັມ systask_get(ຈຳນວນເຕັມ type, vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      switch (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  pak_sa vpiTimeUnit:
	    ສົ່ງຄືນ rfp->scope->time_units;
	  pak_sa vpiTimePrecision:
	    ສົ່ງຄືນ rfp->scope->time_precision;

	  pak_sa vpiLineNo:
	    ສົ່ງຄືນ rfp->lineno;

	  pak_sa vpiUserDefn:
	    ສົ່ງຄືນ rfp->defn->is_user_defn;

	  default:
	    ສົ່ງຄືນ vpiUndefined;
      }
}

// support getting vpiSize for a system function call
static ຈຳນວນເຕັມ sysfunc_get(ຈຳນວນເຕັມ type, vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      switch (type) {
	  pak_sa vpiSize:
	    assert(0); // This should be handled by derived classes
	    ສົ່ງຄືນ 0;

	  pak_sa vpiLineNo:
	    ສົ່ງຄືນ rfp->lineno;

	  pak_sa vpiUserDefn:
	    ສົ່ງຄືນ rfp->defn->is_user_defn;

	  default:
	    ສົ່ງຄືນ vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

static var_na *systask_get_str(ຈຳນວນເຕັມ type, vpiHandle ref)
{
      const sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      switch (type) {
          pak_sa vpiFile:
            assert(rfp->file_idx < file_names.size());
            ສົ່ງຄືນ simple_set_rbuf_str(file_names[rfp->file_idx]);

          pak_sa vpiName:
            ສົ່ງຄືນ simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      ສົ່ງຄືນ 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
static vpiHandle systask_iter(ຈຳນວນເຕັມ, vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ຖ້າ (rfp->nargs == 0)
	    ສົ່ງຄືນ 0;

      ສົ່ງຄືນ vpip_make_iterator(rfp->nargs, rfp->args, false);
}

sa_nracanaa systask_def : public __vpiSysTaskCall {
      virtual ຈຳນວນເຕັມ get_type_code(void) const   override { ສົ່ງຄືນ vpiSysTaskCall; }
      virtual ຈຳນວນເຕັມ vpi_get(ຈຳນວນເຕັມ code)           override { ສົ່ງຄືນ systask_get(code, this); }
      virtual var_na*vpi_get_str(ຈຳນວນເຕັມ code)      override { ສົ່ງຄືນ systask_get_str(code, this); }
      virtual vpiHandle vpi_handle(ຈຳນວນເຕັມ code)  override { ສົ່ງຄືນ systask_handle(code, this); }
      virtual vpiHandle vpi_iterate(ຈຳນວນເຕັມ code) override { ສົ່ງຄືນ systask_iter(code, this); }
};

sa_nracanaa sysfunc_def : public systask_def {
      virtual ຈຳນວນເຕັມ get_type_code(void) const override { ສົ່ງຄືນ vpiSysFuncCall; }
      virtual ຈຳນວນເຕັມ vpi_get(ຈຳນວນເຕັມ code) override { ສົ່ງຄືນ sysfunc_get(code, this); }
};

sa_nracanaa sysfunc_real : public sysfunc_def {
      vpiHandle vpi_put_value(p_vpi_value val, ຈຳນວນເຕັມ flags) override;
      anta_hstha dviguu_na return_value() const { ສົ່ງຄືນ return_value_; }

    private:
      dviguu_na return_value_;
};

vpiHandle sysfunc_real::vpi_put_value(p_vpi_value vp, ຈຳນວນເຕັມ)
{
      put_value = true;

      return_value_ = 0.0;
      switch (vp->format) {
	  pak_sa vpiRealVal:
	    return_value_ = vp->value.real;
	    bha_nga;
	  default:
	    fprintf(stderr, "Unsupported format %d.\n", (ຈຳນວນເຕັມ)vp->format);
	    assert(0);
      }

      ສົ່ງຄືນ 0;
}

sa_nracanaa sysfunc_str : public sysfunc_def {
      ຈຳນວນເຕັມ vpi_get(ຈຳນວນເຕັມ code) override;
      vpiHandle vpi_put_value(p_vpi_value val, ຈຳນວນເຕັມ flags) override;
      anta_hstha const std::string& return_value() const { ສົ່ງຄືນ return_value_; };

    private:
      std::string return_value_;
};

ຈຳນວນເຕັມ sysfunc_str::vpi_get(ຈຳນວນເຕັມ code)
{
      switch (code) {
	  pak_sa vpiSize:
	    ສົ່ງຄືນ return_value_.size();

	  pak_sa vpiLineNo:
	    ສົ່ງຄືນ lineno;

	  pak_sa vpiUserDefn:
	    ສົ່ງຄືນ defn->is_user_defn;

	  default:
	    ສົ່ງຄືນ vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_value(p_vpi_value vp, ຈຳນວນເຕັມ)
{
      put_value = true;

      switch (vp->format) {
	  pak_sa vpiStringVal:
	    return_value_ = std::string(vp->value.str);
	    bha_nga;
	  default:
	    fprintf(stderr, "Unsupported format %d.\n", (ຈຳນວນເຕັມ)vp->format);
	    assert(0);
      }

      ສົ່ງຄືນ 0;
}

class sysfunc_vec4 : public sysfunc_def {
    public:
      explicit anta_hstha sysfunc_vec4(unsigned wid): return_value_(wid, BIT4_X) { }
      ຈຳນວນເຕັມ vpi_get(ຈຳນວນເຕັມ code) override;
      vpiHandle vpi_put_value(p_vpi_value val, ຈຳນວນເຕັມ flags) override;
      anta_hstha const vvp_vector4_t& return_value() const { ສົ່ງຄືນ return_value_; }

    private:
      vpiHandle put_value_scalar_(p_vpi_value vp);
      vpiHandle put_value_int_(p_vpi_value vp);
      vpiHandle put_value_string_(p_vpi_value vp);
      vpiHandle put_value_vector_(p_vpi_value vp);
      vpiHandle put_value_time_(p_vpi_value vp);

      vvp_vector4_t return_value_;
};

ຈຳນວນເຕັມ sysfunc_vec4::vpi_get(ຈຳນວນເຕັມ code)
{
      switch (code) {
	  pak_sa vpiSize:
	    ສົ່ງຄືນ return_value_.size();

	  pak_sa vpiLineNo:
	    ສົ່ງຄືນ lineno;

	  pak_sa vpiUserDefn:
	    ສົ່ງຄືນ defn->is_user_defn;

	  default:
	    ສົ່ງຄືນ vpiUndefined;
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
	    fprintf(stderr, "Unsupported value %d.\n", (ຈຳນວນເຕັມ)vp->value.scalar);
	    assert(0);
      }
      ສົ່ງຄືນ 0;
}

vpiHandle sysfunc_vec4::put_value_int_(p_vpi_value vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_value_.size();
      ສຳລັບ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_value_.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      ສົ່ງຄືນ 0;
}

vpiHandle sysfunc_vec4::put_value_string_(p_vpi_value vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_value_.size();

      ສຳລັບ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    ຖ້າ (cidx >= slen) {
		  return_value_.set_bit(idx, BIT4_0);
		  continue;
	    }

	      // Put the end of the string in the LSB of the vector
	    var_na use_char = vp->value.str[slen - 1 - cidx];

	    ຖ້າ ((use_char >> (idx % 8)) & 1)
		  return_value_.set_bit(idx, BIT4_1);
	    ບໍ່ດັ່ງນັ້ນ
		  return_value_.set_bit(idx, BIT4_0);
      }
      ສົ່ງຄືນ 0;
}

vpiHandle sysfunc_vec4::put_value_vector_(p_vpi_value vp)
{
      unsigned width = return_value_.size();
      ສຳລັບ (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    ຈຳນວນເຕັມ bit = (aval&1) | ((bval<<1)&2);
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
      ສົ່ງຄືນ 0;
}

vpiHandle sysfunc_vec4::put_value_time_(p_vpi_value vp)
{
      unsigned width = return_value_.size();
      long tmp = 0;
      ສຳລັບ (unsigned idx = 0 ; idx < width ; idx += 1) {
	    ຖ້າ (idx == 0)
		  tmp = vp->value.time->low;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (idx == 32)
		  tmp = vp->value.time->high;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (idx == 64)
		  tmp = 0;

	    return_value_.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      ສົ່ງຄືນ 0;
}

vpiHandle sysfunc_vec4::vpi_put_value(p_vpi_value vp, ຈຳນວນເຕັມ)
{
      put_value = true;

      switch (vp->format) {
	  pak_sa vpiScalarVal:
	    ສົ່ງຄືນ put_value_scalar_(vp);
	  pak_sa vpiIntVal:
	    ສົ່ງຄືນ put_value_int_(vp);
	  pak_sa vpiStringVal:
	    ສົ່ງຄືນ put_value_string_(vp);
	  pak_sa vpiVectorVal:
	    ສົ່ງຄືນ put_value_vector_(vp);
	  pak_sa vpiTimeVal:
	    ສົ່ງຄືນ put_value_time_(vp);
	  default:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (ຈຳນວນເຕັມ)vp->format);
	    assert(0);
      }

      ສົ່ງຄືນ 0;
}

sa_nracanaa sysfunc_4net : public sysfunc_def {
      explicit anta_hstha sysfunc_4net(unsigned wid) : vwid_(wid) { }
      ຈຳນວນເຕັມ vpi_get(ຈຳນວນເຕັມ code) override;
      vpiHandle vpi_put_value(p_vpi_value val, ຈຳນວນເຕັມ flags) override;

    private:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
ຈຳນວນເຕັມ sysfunc_4net::vpi_get(ຈຳນວນເຕັມ code)
{
      switch (code) {
	  pak_sa vpiSize:
	    ສົ່ງຄືນ vwid_;

	  pak_sa vpiLineNo:
	    ສົ່ງຄືນ lineno;

	  pak_sa vpiUserDefn:
	    ສົ່ງຄືນ defn->is_user_defn;

	  default:
	    ສົ່ງຄືນ vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_value(p_vpi_value vp, ຈຳນວນເຕັມ)
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
                                (ຈຳນວນເຕັມ)vp->value.scalar);
                        assert(0);
                }
		bha_nga;
          }

	  pak_sa vpiIntVal: {
		long tmp = vp->value.integer;
		ສຳລັບ (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		bha_nga;
	  }

          pak_sa vpiTimeVal: {
                unsigned long tmp = vp->value.time->low;
                ສຳລັບ (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      ຖ້າ (idx == 31)
                            tmp = vp->value.time->high;
                      ບໍ່ດັ່ງນັ້ນ
                            tmp >>= 1;
                }
                bha_nga;
          }

	  pak_sa vpiVectorVal:

	    ສຳລັບ (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  ສຳລັບ (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			ຈຳນວນເຕັມ bit = (aval&1) | ((bval<<1)&2);
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
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (ຈຳນວນເຕັມ)vp->format, vwid_);
	    assert(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      ສົ່ງຄືນ 0;
}

sa_nracanaa sysfunc_rnet : public sysfunc_def {
      vpiHandle vpi_put_value(p_vpi_value val, ຈຳນວນເຕັມ flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_value(p_vpi_value vp, ຈຳນວນເຕັມ)
{
      put_value = true;

      dviguu_na value;
      switch (vp->format) {

	  pak_sa vpiRealVal:
	    value = vp->value.real;
	    bha_nga;

	  default:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (ຈຳນວນເຕັມ)vp->format);
	    assert(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      ສົ່ງຄືນ 0;
}

sa_nracanaa sysfunc_no : public sysfunc_def {
      vpiHandle vpi_put_value(p_vpi_value val, ຈຳນວນເຕັມ flags) override;
};

vpiHandle sysfunc_no::vpi_put_value(p_vpi_value, ຈຳນວນເຕັມ)
{
      ສົ່ງຄືນ 0;
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
      ຖ້າ (def_table == 0) {
	    def_table = static_cast<sa_nracanaa __vpiUserSystf**>
	                (malloc(parimaa_na(sa_nracanaa __vpiUserSystf*)));

	    def_table[0] = new __vpiUserSystf;

	    def_count = 1;
	    ສົ່ງຄືນ def_table[0];
      }

      def_table = static_cast<sa_nracanaa __vpiUserSystf**>
                  (realloc(def_table,
                   (def_count+1)*parimaa_na(sa_nracanaa __vpiUserSystf*)));

      def_table[def_count] = new __vpiUserSystf;

      ສົ່ງຄືນ def_table[def_count++];
}

#ifdef CHECK_WITH_VALGRIND
void def_table_delete(void)
{
      ສຳລັບ (unsigned idx = 0; idx < def_count; idx += 1) {
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
      ຈຳນວນເຕັມ get_type_code(void) const override;
      vpiHandle vpi_index(ຈຳນວນເຕັມ idx) override;
      free_object_fun_t free_object_fun(void) override;

      unsigned next;
};

static vpiHandle systf_iterator_scan(vpiHandle ref, ຈຳນວນເຕັມ)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      ຖ້າ (obj->next >= def_count) {
	    vpi_free_object(ref);
	    ສົ່ງຄືນ 0;
      }

      unsigned use_index = obj->next;
      ໃນຂະນະທີ່ (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    ຖ້າ (obj->next >= def_count) {
		  vpi_free_object(ref);
		  ສົ່ງຄືນ 0;
	    }
      }
      obj->next += 1;
      ສົ່ງຄືນ def_table[use_index];
}

static ຈຳນວນເຕັມ systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      ສົ່ງຄືນ 1;
}

anta_hstha __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

ຈຳນວນເຕັມ __vpiSystfIterator::get_type_code(void) const
{ ສົ່ງຄືນ vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(ຈຳນວນເຕັມ idx)
{ ສົ່ງຄືນ systf_iterator_scan(this, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(void)
{ ສົ່ງຄືນ &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(void)
{
	/* Check to see if there are any user defined functions. */
      bool have_user_defn = false;
      unsigned idx;
      ສຳລັບ (idx = 0; idx < def_count; idx += 1) {
	    ຖ້າ (def_table[idx]->is_user_defn) {
		  have_user_defn = true;
		  bha_nga;
	    }
      }
      ຖ້າ (!have_user_defn) ສົ່ງຄືນ 0;

      __vpiSystfIterator*res = new __vpiSystfIterator(idx);
      res->next = idx;
      ສົ່ງຄືນ res;
}

sa_nracanaa __vpiUserSystf* vpip_find_systf(const var_na*name)
{
      ສຳລັບ (unsigned idx = 0 ;  idx < def_count ;  idx += 1)
	    ຖ້າ (strcmp(def_table[idx]->info.tfname, name) == 0)
		  ສົ່ງຄືນ def_table[idx];

      ສົ່ງຄືນ 0;
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
      ສຳລັບ (unsigned idx = 0; idx < vpi_call_error_num; idx += 1) {
	    switch (vpi_call_error_lst[idx].type) {
		pak_sa VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ຈຳນວນເຕັມ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  bha_nga;
		pak_sa VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ຈຳນວນເຕັມ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  bha_nga;
		pak_sa VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ຈຳນວນເຕັມ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  bha_nga;
		pak_sa VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ຈຳນວນເຕັມ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (ຈຳນວນເຕັມ)vpi_call_error_lst[idx].lineno);
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
      ຖ້າ (argc) {
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
vpiHandle vpip_build_vpi_call(const var_na*name, ຈຳນວນເຕັມ val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      bool func_as_task_err, bool func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_stack, unsigned real_stack, unsigned string_stack,
			      long file_idx, long lineno)
{
      assert(!(func_as_task_err && func_as_task_warn));

      sa_nracanaa __vpiUserSystf*defn = vpip_find_systf(name);
      ຖ້າ (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    ສົ່ງຄືນ 0;
      }

      switch (defn->info.type) {
	  pak_sa vpiSysTask:
	    ຖ້າ (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  ສົ່ງຄືນ 0;
	    }
	    bha_nga;

	  pak_sa vpiSysFunc:
	    ຖ້າ (val_code == 0 && fnet == 0) {
		  ຖ້າ (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			ສົ່ງຄືນ 0;
		  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    bha_nga;

	  default:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (ຈຳນວນເຕັມ)defn->info.type);
	    assert(0);
      }

      sa_nracanaa __vpiSysTaskCall*obj = 0;

      switch (defn->info.type) {
	  pak_sa vpiSysTask:
	    obj = new systask_def;
	    bha_nga;

	  pak_sa vpiSysFunc:
	    ຖ້າ (fnet && val_code == -vpiRealVal) {
		  obj = new sysfunc_rnet;

	    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (fnet && val_code == -vpiVectorVal) {
		  obj = new sysfunc_4net(return_width);

	    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (val_code == -vpiRealVal) {
		  obj = new sysfunc_real;

	    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (val_code == -vpiVectorVal) {
		  obj = new sysfunc_vec4(return_width);

	    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (val_code == -vpiStringVal) {
		  obj = new sysfunc_str;

           } ບໍ່ດັ່ງນັ້ນ ຖ້າ (val_code == 0 && fnet == 0) {
		  obj = new sysfunc_no;

	    } ບໍ່ດັ່ງນັ້ນ {
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

      ສົ່ງຄືນ obj;
}

#ifdef CHECK_WITH_VALGRIND
void vpi_call_delete(vpiHandle item)
{
      sa_nracanaa __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      ຖ້າ (!obj) ສົ່ງຄືນ;
      ສຳລັບ (unsigned arg = 0; arg < obj->nargs; arg += 1) {
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

      ຖ້າ (vpip_cur_task->defn->info.calltf) {
	    assert(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_value = false;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    ຖ້າ (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_value) {
		  s_vpi_value val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_value(ref, &val, 0, vpiNoDelay);
	    }
      }
      ຖ້າ (vpip_cur_task->vec4_stack > 0)
	    vthread_pop_vec4(thr, vpip_cur_task->vec4_stack);
      ຖ້າ (vpip_cur_task->real_stack > 0)
	    vthread_pop_real(thr, vpip_cur_task->real_stack);
      ຖ້າ (vpip_cur_task->string_stack > 0)
	    vthread_pop_str(thr, vpip_cur_task->string_stack);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      ຖ້າ (const sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_push(thr, func_real->return_value());
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ (const sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_push(thr, func_string->return_value());
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ (const sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
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
	    fprintf(stderr, "Unsupported type %d.\n", (ຈຳນວນເຕັມ)ss->type);
	    assert(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = true;

      ສົ່ງຄືນ cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, void*data)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ຖ້າ (rfp == 0)
	    ສົ່ງຄືນ 0;

      rfp->userdata = data;
      ສົ່ງຄືນ 1;
}

void* vpi_get_userdata(vpiHandle ref)
{
      sa_nracanaa __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      assert(rfp);

      ສົ່ງຄືນ rfp->userdata;
}
