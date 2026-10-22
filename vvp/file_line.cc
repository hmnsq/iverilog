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

جماعت __vpiFileLine : عوامی __vpiHandle {
    عوامی:
      صریح __vpiFileLine(ثابت حرف*desc_, طویل file_idx_, طویل lineno_);
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      حرف* vpi_get_str(صحیح code) override;

      ثابت حرف* get_description() ثابت { واپس description; };
      غیرعلامتی get_file_idx() ثابت { واپس file_idx; };
      غیرعلامتی get_lineno() ثابت { واپس lineno; };

    نجی:
      ثابت حرف *description;
      غیرعلامتی file_idx;
      غیرعلامتی lineno;
};

منطقی show_file_سطر = جھوٹ;
منطقی رمز_is_instrumented = جھوٹ;

ساکن صحیح file_سطر_get(صحیح type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      أكد(rfp);

      انتخاب (type) {
	حالت vpi_سطر_No:
	    واپس rfp->get_lineno();
	طےشدہ:
	    واپس vpiUndefined;
      }
}

ساکن حرف *file_سطر_get_str(صحیح type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      أكد(rfp);

      انتخاب (type) {
	حالت vpiFile:
	    أكد(rfp->get_file_idx() < file_نام.size());
	    واپس simple_سیٹ_rbuf_str(file_نام[rfp->get_file_idx()]);
	حالت _vpiDescription:
	    اگر (rfp->get_description()) {
		  واپس simple_سیٹ_rbuf_str(rfp->get_description());
	    } ورنہ واپس simple_سیٹ_rbuf_str("Procedural tracing.");
	طےشدہ:
	    واپس 0;
      }
}

اندرونی __vpiFileLine::__vpiFileLine(ثابت حرف*desc_, طویل file_idx_,
                                    طویل lineno_)
{
      اگر (desc_) description = vpip_name_string(desc_);
      ورنہ description = 0;
      file_idx = (غیرعلامتی) file_idx_;
      lineno = (غیرعلامتی) lineno_;
}

صحیح __vpiFileLine::get_type_رمز(خالی) ثابت
{ واپس vpi_File_سطر; }

صحیح __vpiFileLine::vpi_get(صحیح code)
{ واپس file_سطر_get(code, یہ); }

حرف* __vpiFileLine::vpi_get_str(صحیح code)
{ واپس file_سطر_get_str(code, یہ); }


vpiHandle vpip_build_file_سطر(ثابت حرف*description, طویل file_idx, طویل lineno)
{
      __vpiFileLine*obj = نیا __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      رمز_is_instrumented = سچ;

      واپس obj;
}
