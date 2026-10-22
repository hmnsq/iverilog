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
      පූර්ණ_සංඛ්‍යා get_type_code(void) const override;
      පූර්ණ_සංඛ්‍යා vpi_get(පූර්ණ_සංඛ්‍යා code) override;
      var_na* vpi_get_str(පූර්ණ_සංඛ්‍යා code) override;

      const var_na* get_description() const { ආපසු description; };
      unsigned get_file_idx() const { ආපසු file_idx; };
      unsigned get_lineno() const { ආපසු lineno; };

    private:
      const var_na *description;
      unsigned file_idx;
      unsigned lineno;
};

bool show_file_line = false;
bool code_is_instrumented = false;

static පූර්ණ_සංඛ්‍යා file_line_get(පූර්ණ_සංඛ්‍යා type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      assert(rfp);

      switch (type) {
	pak_sa vpiLineNo:
	    ආපසු rfp->get_lineno();
	default:
	    ආපසු vpiUndefined;
      }
}

static var_na *file_line_get_str(පූර්ණ_සංඛ්‍යා type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      assert(rfp);

      switch (type) {
	pak_sa vpiFile:
	    assert(rfp->get_file_idx() < file_names.size());
	    ආපසු simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	pak_sa _vpiDescription:
	    නම් (rfp->get_description()) {
		  ආපසු simple_set_rbuf_str(rfp->get_description());
	    } නැතහොත් ආපසු simple_set_rbuf_str("Procedural tracing.");
	default:
	    ආපසු 0;
      }
}

anta_hstha __vpiFileLine::__vpiFileLine(const var_na*desc_, long file_idx_,
                                    long lineno_)
{
      නම් (desc_) description = vpip_name_string(desc_);
      නැතහොත් description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

පූර්ණ_සංඛ්‍යා __vpiFileLine::get_type_code(void) const
{ ආපසු _vpiFileLine; }

පූර්ණ_සංඛ්‍යා __vpiFileLine::vpi_get(පූර්ණ_සංඛ්‍යා code)
{ ආපසු file_line_get(code, this); }

var_na* __vpiFileLine::vpi_get_str(පූර්ණ_සංඛ්‍යා code)
{ ආපසු file_line_get_str(code, this); }


vpiHandle vpip_build_file_line(const var_na*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = new __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = true;

      ආපසු obj;
}
