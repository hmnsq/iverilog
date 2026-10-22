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

வகுப்பு __vpiFileLine : பொது __vpiHandle {
    பொது:
      explicit __vpiFileLine(மாறிலி எழுத்து*desc_, long file_idx_, long lineno_);
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;

      மாறிலி எழுத்து* get_description() மாறிலி { திரும்பு description; };
      unsigned get_file_idx() மாறிலி { திரும்பு file_idx; };
      unsigned get_lineno() மாறிலி { திரும்பு lineno; };

    தனிப்பட்ட:
      மாறிலி எழுத்து *description;
      unsigned file_idx;
      unsigned lineno;
};

தருக்க show_file_வரி = பொய்;
தருக்க code_is_instrumented = பொய்;

நிலையான முழு file_வரி_get(முழு type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      தேர்வு (type) {
	நிலை vpi_வரி_No:
	    திரும்பு rfp->get_lineno();
	இயல்பு:
	    திரும்பு vpiUndefined;
      }
}

நிலையான எழுத்து *file_வரி_get_str(முழு type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      தேர்வு (type) {
	நிலை vpiFile:
	    ப்ரதிஜ்ஞா(rfp->get_file_idx() < file_names.size());
	    திரும்பு simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	நிலை _vpiDescription:
	    எனில் (rfp->get_description()) {
		  திரும்பு simple_set_rbuf_str(rfp->get_description());
	    } இல்லையெனில் திரும்பு simple_set_rbuf_str("Procedural tracing.");
	இயல்பு:
	    திரும்பு 0;
      }
}

உள்வரி __vpiFileLine::__vpiFileLine(மாறிலி எழுத்து*desc_, long file_idx_,
                                    long lineno_)
{
      எனில் (desc_) description = vpip_name_string(desc_);
      இல்லையெனில் description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

முழு __vpiFileLine::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpi_File_வரி; }

முழு __vpiFileLine::vpi_get(முழு code)
{ திரும்பு file_வரி_get(code, இது); }

எழுத்து* __vpiFileLine::vpi_get_str(முழு code)
{ திரும்பு file_வரி_get_str(code, இது); }


vpiHandle vpip_build_file_வரி(மாறிலி எழுத்து*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = புதிய __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = உண்மை;

      திரும்பு obj;
}
