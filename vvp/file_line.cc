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

מחלקה __vpiFileLine : عام __vpiHandle {
    عام:
      explicit __vpiFileLine(ثابت حرف*desc_, long file_idx_, long lineno_);
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;

      ثابت حرف* get_description() ثابت { החזר description; };
      unsigned get_file_idx() ثابت { החזר file_idx; };
      unsigned get_lineno() ثابت { החזר lineno; };

    خاص:
      ثابت حرف *description;
      unsigned file_idx;
      unsigned lineno;
};

منطقي show_file_line = שקר;
منطقي code_is_instrumented = שקר;

ساكن صحيح file_line_get(صحيح type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      أكد(rfp);

      بدل (type) {
	حالة vpiLineNo:
	    החזר rfp->get_lineno();
	افتراضي:
	    החזר vpiUndefined;
      }
}

ساكن حرف *file_line_get_str(صحيح type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      أكد(rfp);

      بدل (type) {
	حالة vpiFile:
	    أكد(rfp->get_file_idx() < file_names.size());
	    החזר simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	حالة _vpiDescription:
	    אם (rfp->get_description()) {
		  החזר simple_set_rbuf_str(rfp->get_description());
	    } אחרת החזר simple_set_rbuf_str("Procedural tracing.");
	افتراضي:
	    החזר 0;
      }
}

مضمن __vpiFileLine::__vpiFileLine(ثابت حرف*desc_, long file_idx_,
                                    long lineno_)
{
      אם (desc_) description = vpip_name_string(desc_);
      אחרת description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

صحيح __vpiFileLine::get_type_code(فراغ) ثابت
{ החזר _vpiFileLine; }

صحيح __vpiFileLine::vpi_get(صحيح code)
{ החזר file_line_get(code, هذا); }

حرف* __vpiFileLine::vpi_get_str(صحيح code)
{ החזר file_line_get_str(code, هذا); }


vpiHandle vpip_build_file_line(ثابت حرف*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = جديد __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = אמת;

      החזר obj;
}
