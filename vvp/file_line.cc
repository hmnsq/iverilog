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

श्रेणी __vpiFileLine : खुला __vpiHandle {
    खुला:
      explicit __vpiFileLine(स्थिर अक्षर*desc_, दीर्घ file_idx_, दीर्घ lineno_);
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;

      स्थिर अक्षर* get_description() स्थिर { वापस description; };
      अचिन्हित get_file_idx() स्थिर { वापस file_idx; };
      अचिन्हित get_lineno() स्थिर { वापस lineno; };

    निजी:
      स्थिर अक्षर *description;
      अचिन्हित file_idx;
      अचिन्हित lineno;
};

bool show_file_पंक्ति = false;
bool code_is_instrumented = false;

अटल पूर्णांक file_पंक्ति_get(पूर्णांक type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      निश्चित(rfp);

      चयन (type) {
	हाल vpi_पंक्ति_No:
	    वापस rfp->get_lineno();
	शेष:
	    वापस vpiUndefined;
      }
}

अटल अक्षर *file_पंक्ति_get_str(पूर्णांक type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      निश्चित(rfp);

      चयन (type) {
	हाल vpiFile:
	    निश्चित(rfp->get_file_idx() < file_names.size());
	    वापस simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	हाल _vpiDescription:
	    अगर (rfp->get_description()) {
		  वापस simple_set_rbuf_str(rfp->get_description());
	    } अन्यथा वापस simple_set_rbuf_str("Procedural tracing.");
	शेष:
	    वापस 0;
      }
}

अंतरभूत __vpiFileLine::__vpiFileLine(स्थिर अक्षर*desc_, दीर्घ file_idx_,
                                    दीर्घ lineno_)
{
      अगर (desc_) description = vpip_name_string(desc_);
      अन्यथा description = 0;
      file_idx = (अचिन्हित) file_idx_;
      lineno = (अचिन्हित) lineno_;
}

पूर्णांक __vpiFileLine::get_type_code(व्योम) स्थिर
{ वापस vpi_File_पंक्ति; }

पूर्णांक __vpiFileLine::vpi_get(पूर्णांक code)
{ वापस file_पंक्ति_get(code, यह); }

अक्षर* __vpiFileLine::vpi_get_str(पूर्णांक code)
{ वापस file_पंक्ति_get_str(code, यह); }


vpiHandle vpip_build_file_पंक्ति(स्थिर अक्षर*description, दीर्घ file_idx, दीर्घ lineno)
{
      __vpiFileLine*obj = नया __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = true;

      वापस obj;
}
