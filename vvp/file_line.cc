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

صنف __vpiFileLine : عام __vpiHandle {
    عام:
      صریح __vpiFileLine(ثابت حرف*desc_, دراز file_idx_, دراز lineno_);
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;

      ثابت حرف* get_description() ثابت { أرجع description; };
      بیعلامت get_file_idx() ثابت { أرجع file_idx; };
      بیعلامت get_lineno() ثابت { أرجع lineno; };

    خاص:
      ثابت حرف *description;
      بیعلامت file_idx;
      بیعلامت lineno;
};

منطقي show_file_سطر = خطأ;
منطقي کد_is_instrumented = خطأ;

ساكن صحيح file_سطر_get(صحيح type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      أكد(rfp);

      بدل (type) {
	حالة vpi_سطر_No:
	    أرجع rfp->get_lineno();
	افتراضي:
	    أرجع vpiUndefined;
      }
}

ساكن حرف *file_سطر_get_str(صحيح type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      أكد(rfp);

      بدل (type) {
	حالة vpiFile:
	    أكد(rfp->get_file_idx() < file_نامها.size());
	    أرجع simple_مجموعه_rbuf_str(file_نامها[rfp->get_file_idx()]);
	حالة _vpiDescription:
	    إذا (rfp->get_description()) {
		  أرجع simple_مجموعه_rbuf_str(rfp->get_description());
	    } وإلا أرجع simple_مجموعه_rbuf_str("Procedural tracing.");
	افتراضي:
	    أرجع 0;
      }
}

مضمن __vpiFileLine::__vpiFileLine(ثابت حرف*desc_, دراز file_idx_,
                                    دراز lineno_)
{
      إذا (desc_) description = vpip_name_string(desc_);
      وإلا description = 0;
      file_idx = (بیعلامت) file_idx_;
      lineno = (بیعلامت) lineno_;
}

صحيح __vpiFileLine::get_type_کد(فراغ) ثابت
{ أرجع vpi_File_سطر; }

صحيح __vpiFileLine::vpi_get(صحيح code)
{ أرجع file_سطر_get(code, هذا); }

حرف* __vpiFileLine::vpi_get_str(صحيح code)
{ أرجع file_سطر_get_str(code, هذا); }


vpiHandle vpip_build_file_سطر(ثابت حرف*description, دراز file_idx, دراز lineno)
{
      __vpiFileLine*obj = جديد __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      کد_is_instrumented = درست;

      أرجع obj;
}
