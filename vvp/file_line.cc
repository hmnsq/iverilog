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

বর্গ __vpiFileLine : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      explicit __vpiFileLine(ধ্রুব বর্ণ*desc_, long file_idx_, long lineno_);
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;

      ধ্রুব বর্ণ* get_description() ধ্রুব { প্রত্যাগম description; };
      unsigned get_file_idx() ধ্রুব { প্রত্যাগম file_idx; };
      unsigned get_lineno() ধ্রুব { প্রত্যাগম lineno; };

    গুপ্ত:
      ধ্রুব বর্ণ *description;
      unsigned file_idx;
      unsigned lineno;
};

তার্কিক show_file_সারি = অসত্য;
তার্কিক code_is_instrumented = অসত্য;

স্থির পূর্ণাঙ্ক file_সারি_get(পূর্ণাঙ্ক type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      প্রতিজ্ঞা(rfp);

      বিকল্প (type) {
	পক্ষ vpi_সারি_No:
	    প্রত্যাগম rfp->get_lineno();
	স্বভাব:
	    প্রত্যাগম vpiUndefined;
      }
}

স্থির বর্ণ *file_সারি_get_str(পূর্ণাঙ্ক type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      প্রতিজ্ঞা(rfp);

      বিকল্প (type) {
	পক্ষ vpiFile:
	    প্রতিজ্ঞা(rfp->get_file_idx() < file_names.size());
	    প্রত্যাগম simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	পক্ষ _vpiDescription:
	    যদি (rfp->get_description()) {
		  প্রত্যাগম simple_set_rbuf_str(rfp->get_description());
	    } অন্যথা প্রত্যাগম simple_set_rbuf_str("Procedural tracing.");
	স্বভাব:
	    প্রত্যাগম 0;
      }
}

অন্তঃস্থ __vpiFileLine::__vpiFileLine(ধ্রুব বর্ণ*desc_, long file_idx_,
                                    long lineno_)
{
      যদি (desc_) description = vpip_name_string(desc_);
      অন্যথা description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

পূর্ণাঙ্ক __vpiFileLine::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpi_File_সারি; }

পূর্ণাঙ্ক __vpiFileLine::vpi_get(পূর্ণাঙ্ক code)
{ প্রত্যাগম file_সারি_get(code, এতত্); }

বর্ণ* __vpiFileLine::vpi_get_str(পূর্ণাঙ্ক code)
{ প্রত্যাগম file_সারি_get_str(code, এতত্); }


vpiHandle vpip_build_file_সারি(ধ্রুব বর্ণ*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = নূতন __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = সত্য;

      প্রত্যাগম obj;
}
