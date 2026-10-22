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
      ціле get_type_code(void) const override;
      ціле vpi_get(ціле code) override;
      char* vpi_get_str(ціле code) override;

      const char* get_description() const { повернути description; };
      unsigned get_file_idx() const { повернути file_idx; };
      unsigned get_lineno() const { повернути lineno; };

    private:
      const char *description;
      unsigned file_idx;
      unsigned lineno;
};

bool show_file_line = false;
bool code_is_instrumented = false;

static ціле file_line_get(ціле type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      assert(rfp);

      switch (type) {
	case vpiLineNo:
	    повернути rfp->get_lineno();
	default:
	    повернути vpiUndefined;
      }
}

static char *file_line_get_str(ціле type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      assert(rfp);

      switch (type) {
	case vpiFile:
	    assert(rfp->get_file_idx() < file_names.size());
	    повернути simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	case _vpiDescription:
	    якщо (rfp->get_description()) {
		  повернути simple_set_rbuf_str(rfp->get_description());
	    } інакше повернути simple_set_rbuf_str("Procedural tracing.");
	default:
	    повернути 0;
      }
}

inline __vpiFileLine::__vpiFileLine(const char*desc_, long file_idx_,
                                    long lineno_)
{
      якщо (desc_) description = vpip_name_string(desc_);
      інакше description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

ціле __vpiFileLine::get_type_code(void) const
{ повернути _vpiFileLine; }

ціле __vpiFileLine::vpi_get(ціле code)
{ повернути file_line_get(code, this); }

char* __vpiFileLine::vpi_get_str(ціле code)
{ повернути file_line_get_str(code, this); }


vpiHandle vpip_build_file_line(const char*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = new __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = true;

      повернути obj;
}
