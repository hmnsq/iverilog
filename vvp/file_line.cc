/*
 * Copyright (C) 2011-2025 Cary R. (cygcary@yahoo.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

# include "compile.h"
# include "vpi_priv.h"

class __vpiFileLine : public __vpiHandle {
    public:
      explicit __vpiFileLine(const var_na*desc_, long file_idx_, long lineno_);
      ចំនួនគត់ get_type_code(void) const override;
      ចំនួនគត់ vpi_get(ចំនួនគត់ code) override;
      var_na* vpi_get_str(ចំនួនគត់ code) override;

      const var_na* get_description() const { ត្រឡប់ description; };
      unsigned get_file_idx() const { ត្រឡប់ file_idx; };
      unsigned get_lineno() const { ត្រឡប់ lineno; };

    private:
      const var_na *description;
      unsigned file_idx;
      unsigned lineno;
};

bool show_file_line = false;
bool code_is_instrumented = false;

static ចំនួនគត់ file_line_get(ចំនួនគត់ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      assert(rfp);

      switch (type) {
	pak_sa vpiLineNo:
	    ត្រឡប់ rfp->get_lineno();
	default:
	    ត្រឡប់ vpiUndefined;
      }
}

static var_na *file_line_get_str(ចំនួនគត់ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      assert(rfp);

      switch (type) {
	pak_sa vpiFile:
	    assert(rfp->get_file_idx() < file_names.size());
	    ត្រឡប់ simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	pak_sa _vpiDescription:
	    ប្រសិនបើ (rfp->get_description()) {
		  ត្រឡប់ simple_set_rbuf_str(rfp->get_description());
	    } បើមិនដូច្នេះ ត្រឡប់ simple_set_rbuf_str("Procedural tracing.");
	default:
	    ត្រឡប់ 0;
      }
}

anta_hstha __vpiFileLine::__vpiFileLine(const var_na*desc_, long file_idx_,
                                    long lineno_)
{
      ប្រសិនបើ (desc_) description = vpip_name_string(desc_);
      បើមិនដូច្នេះ description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

ចំនួនគត់ __vpiFileLine::get_type_code(void) const
{ ត្រឡប់ _vpiFileLine; }

ចំនួនគត់ __vpiFileLine::vpi_get(ចំនួនគត់ code)
{ ត្រឡប់ file_line_get(code, this); }

var_na* __vpiFileLine::vpi_get_str(ចំនួនគត់ code)
{ ត្រឡប់ file_line_get_str(code, this); }


vpiHandle vpip_build_file_line(const var_na*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = new __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = true;

      ត្រឡប់ obj;
}
