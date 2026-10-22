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

वर्ग __vpiFileLine : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      explicit __vpiFileLine(स्थिरांक अक्षर*desc_, दीर्घ file_idx_, दीर्घ lineno_);
      पूर्णांक get_type_code(शून्य) स्थिरांक override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;

      स्थिरांक अक्षर* get_description() स्थिरांक { परत description; };
      अचिन्हित get_file_idx() स्थिरांक { परत file_idx; };
      अचिन्हित get_lineno() स्थिरांक { परत lineno; };

    खाजगी:
      स्थिरांक अक्षर *description;
      अचिन्हित file_idx;
      अचिन्हित lineno;
};

तार्किक show_file_ओळ = असत्य;
तार्किक code_is_instrumented = असत्य;

स्थिर पूर्णांक file_ओळ_get(पूर्णांक type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      प्रतिज्ञा(rfp);

      निवड (type) {
	स्थिती vpi_ओळ_No:
	    परत rfp->get_lineno();
	पूर्वनिर्धारित:
	    परत vpiUndefined;
      }
}

स्थिर अक्षर *file_ओळ_get_str(पूर्णांक type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      प्रतिज्ञा(rfp);

      निवड (type) {
	स्थिती vpiFile:
	    प्रतिज्ञा(rfp->get_file_idx() < file_names.size());
	    परत simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	स्थिती _vpiDescription:
	    जर (rfp->get_description()) {
		  परत simple_set_rbuf_str(rfp->get_description());
	    } अन्यथा परत simple_set_rbuf_str("Procedural tracing.");
	पूर्वनिर्धारित:
	    परत 0;
      }
}

ओळीत __vpiFileLine::__vpiFileLine(स्थिरांक अक्षर*desc_, दीर्घ file_idx_,
                                    दीर्घ lineno_)
{
      जर (desc_) description = vpip_name_string(desc_);
      अन्यथा description = 0;
      file_idx = (अचिन्हित) file_idx_;
      lineno = (अचिन्हित) lineno_;
}

पूर्णांक __vpiFileLine::get_type_code(शून्य) स्थिरांक
{ परत vpi_File_ओळ; }

पूर्णांक __vpiFileLine::vpi_get(पूर्णांक code)
{ परत file_ओळ_get(code, हे); }

अक्षर* __vpiFileLine::vpi_get_str(पूर्णांक code)
{ परत file_ओळ_get_str(code, हे); }


vpiHandle vpip_build_file_ओळ(स्थिरांक अक्षर*description, दीर्घ file_idx, दीर्घ lineno)
{
      __vpiFileLine*obj = नवीन __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = सत्य;

      परत obj;
}
