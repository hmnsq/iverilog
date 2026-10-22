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

رده __vpiFileLine : عمومی __vpiHandle {
    عمومی:
      explicit __vpiFileLine(ثابت نویسه*desc_, long file_idx_, long lineno_);
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;

      ثابت نویسه* get_description() ثابت { بازگشت description; };
      unsigned get_file_idx() ثابت { بازگشت file_idx; };
      unsigned get_lineno() ثابت { بازگشت lineno; };

    خصوصی:
      ثابت نویسه *description;
      unsigned file_idx;
      unsigned lineno;
};

منطقی show_file_سطر = نادرست;
منطقی code_is_instrumented = نادرست;

ایستا صحیح file_سطر_get(صحیح type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      أكد(rfp);

      انتخاب (type) {
	حالت vpi_سطر_No:
	    بازگشت rfp->get_lineno();
	پیشفرض:
	    بازگشت vpiUndefined;
      }
}

ایستا نویسه *file_سطر_get_str(صحیح type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      أكد(rfp);

      انتخاب (type) {
	حالت vpiFile:
	    أكد(rfp->get_file_idx() < file_names.size());
	    بازگشت simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	حالت _vpiDescription:
	    اگر (rfp->get_description()) {
		  بازگشت simple_set_rbuf_str(rfp->get_description());
	    } وگرنه بازگشت simple_set_rbuf_str("Procedural tracing.");
	پیشفرض:
	    بازگشت 0;
      }
}

درونخطی __vpiFileLine::__vpiFileLine(ثابت نویسه*desc_, long file_idx_,
                                    long lineno_)
{
      اگر (desc_) description = vpip_name_string(desc_);
      وگرنه description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

صحیح __vpiFileLine::get_type_code(تهی) ثابت
{ بازگشت vpi_File_سطر; }

صحیح __vpiFileLine::vpi_get(صحیح code)
{ بازگشت file_سطر_get(code, این); }

نویسه* __vpiFileLine::vpi_get_str(صحیح code)
{ بازگشت file_سطر_get_str(code, این); }


vpiHandle vpip_build_file_سطر(ثابت نویسه*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = جدید __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = درست;

      بازگشت obj;
}
