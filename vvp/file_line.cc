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

শ্ৰেণী __vpiFileLine : সার্ৱজনিক __vpiHandle {
    সার্ৱজনিক:
      explicit __vpiFileLine(ধ্রুৱ ৱর্ণ*desc_, long file_idx_, long lineno_);
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      ৱর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;

      ধ্রুৱ ৱর্ণ* get_description() ধ্রুৱ { ঘূৰাই description; };
      unsigned get_file_idx() ধ্রুৱ { ঘূৰাই file_idx; };
      unsigned get_lineno() ধ্রুৱ { ঘূৰাই lineno; };

    গুপ্ত:
      ধ্রুৱ ৱর্ণ *description;
      unsigned file_idx;
      unsigned lineno;
};

তার্কিক show_file_সাৰি = মিছা;
তার্কিক code_is_instrumented = মিছা;

স্থির পূর্ণাঙ্ক file_সাৰি_get(পূর্ণাঙ্ক type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      প্রতিজ্ঞা(rfp);

      ৱিকল্প (type) {
	পক্ষ vpi_সাৰি_No:
	    ঘূৰাই rfp->get_lineno();
	স্ৱভাৱ:
	    ঘূৰাই vpiUndefined;
      }
}

স্থির ৱর্ণ *file_সাৰি_get_str(পূর্ণাঙ্ক type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      প্রতিজ্ঞা(rfp);

      ৱিকল্প (type) {
	পক্ষ vpiFile:
	    প্রতিজ্ঞা(rfp->get_file_idx() < file_names.size());
	    ঘূৰাই simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	পক্ষ _vpiDescription:
	    যদি (rfp->get_description()) {
		  ঘূৰাই simple_set_rbuf_str(rfp->get_description());
	    } অন্যথা ঘূৰাই simple_set_rbuf_str("Procedural tracing.");
	স্ৱভাৱ:
	    ঘূৰাই 0;
      }
}

অন্তঃস্থ __vpiFileLine::__vpiFileLine(ধ্রুৱ ৱর্ণ*desc_, long file_idx_,
                                    long lineno_)
{
      যদি (desc_) description = vpip_name_string(desc_);
      অন্যথা description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

পূর্ণাঙ্ক __vpiFileLine::get_type_code(শূন্য) ধ্রুৱ
{ ঘূৰাই vpi_File_সাৰি; }

পূর্ণাঙ্ক __vpiFileLine::vpi_get(পূর্ণাঙ্ক code)
{ ঘূৰাই file_সাৰি_get(code, এতত্); }

ৱর্ণ* __vpiFileLine::vpi_get_str(পূর্ণাঙ্ক code)
{ ঘূৰাই file_সাৰি_get_str(code, এতত্); }


vpiHandle vpip_build_file_সাৰি(ধ্রুৱ ৱর্ণ*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = নূতন __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = সঁচা;

      ঘূৰাই obj;
}
