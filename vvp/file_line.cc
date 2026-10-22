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

শ্রেণী __vpiFileLine : সর্বজনীন __vpiHandle {
    সর্বজনীন:
      explicit __vpiFileLine(ধ্রুবক অক্ষর*desc_, long file_idx_, long lineno_);
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      অক্ষর* vpi_get_str(পূর্ণসংখ্যা code) override;

      ধ্রুবক অক্ষর* get_description() ধ্রুবক { ফেরত description; };
      unsigned get_file_idx() ধ্রুবক { ফেরত file_idx; };
      unsigned get_lineno() ধ্রুবক { ফেরত lineno; };

    ব্যক্তিগত:
      ধ্রুবক অক্ষর *description;
      unsigned file_idx;
      unsigned lineno;
};

যৌক্তিক show_file_সারি = মিথ্যা;
যৌক্তিক code_is_instrumented = মিথ্যা;

স্থির পূর্ণসংখ্যা file_সারি_get(পূর্ণসংখ্যা type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (type) {
	ক্ষেত্র vpi_সারি_No:
	    ফেরত rfp->get_lineno();
	পূর্বনির্ধারিত:
	    ফেরত vpiUndefined;
      }
}

স্থির অক্ষর *file_সারি_get_str(পূর্ণসংখ্যা type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (type) {
	ক্ষেত্র vpiFile:
	    প্রতিজ্ঞা(rfp->get_file_idx() < file_names.size());
	    ফেরত simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	ক্ষেত্র _vpiDescription:
	    যদি (rfp->get_description()) {
		  ফেরত simple_set_rbuf_str(rfp->get_description());
	    } অন্যথা ফেরত simple_set_rbuf_str("Procedural tracing.");
	পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

অন্তঃরেখ __vpiFileLine::__vpiFileLine(ধ্রুবক অক্ষর*desc_, long file_idx_,
                                    long lineno_)
{
      যদি (desc_) description = vpip_name_string(desc_);
      অন্যথা description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

পূর্ণসংখ্যা __vpiFileLine::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpi_File_সারি; }

পূর্ণসংখ্যা __vpiFileLine::vpi_get(পূর্ণসংখ্যা code)
{ ফেরত file_সারি_get(code, এই); }

অক্ষর* __vpiFileLine::vpi_get_str(পূর্ণসংখ্যা code)
{ ফেরত file_সারি_get_str(code, এই); }


vpiHandle vpip_build_file_সারি(ধ্রুবক অক্ষর*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = নতুন __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = সত্য;

      ফেরত obj;
}
