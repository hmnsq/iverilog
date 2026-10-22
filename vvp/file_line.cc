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

વર્ગ __vpiFileLine : જાહેર __vpiHandle {
    જાહેર:
      explicit __vpiFileLine(અચલ અક્ષર*desc_, long file_idx_, long lineno_);
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      અક્ષર* vpi_get_str(પૂર્ણાંક code) override;

      અચલ અક્ષર* get_description() અચલ { પાછા description; };
      unsigned get_file_idx() અચલ { પાછા file_idx; };
      unsigned get_lineno() અચલ { પાછા lineno; };

    ખાનગી:
      અચલ અક્ષર *description;
      unsigned file_idx;
      unsigned lineno;
};

તાર્કિક show_file_લીટી = ખોટું;
તાર્કિક code_is_instrumented = ખોટું;

સ્થિર પૂર્ણાંક file_લીટી_get(પૂર્ણાંક type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (type) {
	સ્થિતિ vpi_લીટી_No:
	    પાછા rfp->get_lineno();
	મૂળભૂત:
	    પાછા vpiUndefined;
      }
}

સ્થિર અક્ષર *file_લીટી_get_str(પૂર્ણાંક type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (type) {
	સ્થિતિ vpiFile:
	    પ્રતિજ્ઞા(rfp->get_file_idx() < file_names.size());
	    પાછા simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	સ્થિતિ _vpiDescription:
	    જો (rfp->get_description()) {
		  પાછા simple_set_rbuf_str(rfp->get_description());
	    } નહીંતર પાછા simple_set_rbuf_str("Procedural tracing.");
	મૂળભૂત:
	    પાછા 0;
      }
}

અંતઃરેખ __vpiFileLine::__vpiFileLine(અચલ અક્ષર*desc_, long file_idx_,
                                    long lineno_)
{
      જો (desc_) description = vpip_name_string(desc_);
      નહીંતર description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

પૂર્ણાંક __vpiFileLine::get_type_code(શૂન્ય) અચલ
{ પાછા vpi_File_લીટી; }

પૂર્ણાંક __vpiFileLine::vpi_get(પૂર્ણાંક code)
{ પાછા file_લીટી_get(code, આ); }

અક્ષર* __vpiFileLine::vpi_get_str(પૂર્ણાંક code)
{ પાછા file_લીટી_get_str(code, આ); }


vpiHandle vpip_build_file_લીટી(અચલ અક્ષર*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = નવું __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = સાચું;

      પાછા obj;
}
