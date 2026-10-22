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

വർഗ്ഗം __vpiFileLine : പൊതു __vpiHandle {
    പൊതു:
      explicit __vpiFileLine(സ്ഥിരം അക്ഷരം*desc_, long file_idx_, long lineno_);
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;

      സ്ഥിരം അക്ഷരം* get_description() സ്ഥിരം { തിരികെ description; };
      unsigned get_file_idx() സ്ഥിരം { തിരികെ file_idx; };
      unsigned get_lineno() സ്ഥിരം { തിരികെ lineno; };

    സ്വകാര്യ:
      സ്ഥിരം അക്ഷരം *description;
      unsigned file_idx;
      unsigned lineno;
};

തർക്കം show_file_വരി = അസത്യം;
തർക്കം code_is_instrumented = അസത്യം;

സ്ഥിതം പൂർണ്ണസംഖ്യ file_വരി_get(പൂർണ്ണസംഖ്യ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ vpi_വരി_No:
	    തിരികെ rfp->get_lineno();
	സ്ഥിരസ്ഥിതി:
	    തിരികെ vpiUndefined;
      }
}

സ്ഥിതം അക്ഷരം *file_വരി_get_str(പൂർണ്ണസംഖ്യ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ vpiFile:
	    പ്രതിജ്ഞാ(rfp->get_file_idx() < file_names.size());
	    തിരികെ simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	അവസ്ഥ _vpiDescription:
	    എങ്കിൽ (rfp->get_description()) {
		  തിരികെ simple_set_rbuf_str(rfp->get_description());
	    } അല്ലെങ്കിൽ തിരികെ simple_set_rbuf_str("Procedural tracing.");
	സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

വരിയിൽ __vpiFileLine::__vpiFileLine(സ്ഥിരം അക്ഷരം*desc_, long file_idx_,
                                    long lineno_)
{
      എങ്കിൽ (desc_) description = vpip_name_string(desc_);
      അല്ലെങ്കിൽ description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

പൂർണ്ണസംഖ്യ __vpiFileLine::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpi_File_വരി; }

പൂർണ്ണസംഖ്യ __vpiFileLine::vpi_get(പൂർണ്ണസംഖ്യ code)
{ തിരികെ file_വരി_get(code, ഇത്); }

അക്ഷരം* __vpiFileLine::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{ തിരികെ file_വരി_get_str(code, ഇത്); }


vpiHandle vpip_build_file_വരി(സ്ഥിരം അക്ഷരം*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = പുതിയ __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = സത്യം;

      തിരികെ obj;
}
