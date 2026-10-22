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
      explicit __vpiFileLine(const char*desc_, long file_idx_, long lineno_);
      số_nguyên get_type_code(void) const override;
      số_nguyên vpi_get(số_nguyên code) override;
      char* vpi_get_str(số_nguyên code) override;

      const char* get_description() const { trả_về description; };
      unsigned get_file_idx() const { trả_về file_idx; };
      unsigned get_lineno() const { trả_về lineno; };

    private:
      const char *description;
      unsigned file_idx;
      unsigned lineno;
};

bool show_file_line = false;
bool code_is_instrumented = false;

static số_nguyên file_line_get(số_nguyên type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      assert(rfp);

      switch (type) {
	case vpiLineNo:
	    trả_về rfp->get_lineno();
	default:
	    trả_về vpiUndefined;
      }
}

static char *file_line_get_str(số_nguyên type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      assert(rfp);

      switch (type) {
	case vpiFile:
	    assert(rfp->get_file_idx() < file_names.size());
	    trả_về simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	case _vpiDescription:
	    nếu (rfp->get_description()) {
		  trả_về simple_set_rbuf_str(rfp->get_description());
	    } không_thì trả_về simple_set_rbuf_str("Procedural tracing.");
	default:
	    trả_về 0;
      }
}

inline __vpiFileLine::__vpiFileLine(const char*desc_, long file_idx_,
                                    long lineno_)
{
      nếu (desc_) description = vpip_name_string(desc_);
      không_thì description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

số_nguyên __vpiFileLine::get_type_code(void) const
{ trả_về _vpiFileLine; }

số_nguyên __vpiFileLine::vpi_get(số_nguyên code)
{ trả_về file_line_get(code, this); }

char* __vpiFileLine::vpi_get_str(số_nguyên code)
{ trả_về file_line_get_str(code, this); }


vpiHandle vpip_build_file_line(const char*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = new __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = true;

      trả_về obj;
}
