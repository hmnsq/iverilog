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
      explicit __vpiFileLine(अचर वर्ण*desc_, दीर्घ file_idx_, दीर्घ lineno_);
      पूर्णाङ्क get_type_code(शून्य) अचर override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;

      अचर वर्ण* get_description() अचर { फर्काऊ description; };
      अचिन्हित get_file_idx() अचर { फर्काऊ file_idx; };
      अचिन्हित get_lineno() अचर { फर्काऊ lineno; };

    गुप्त:
      अचर वर्ण *description;
      अचिन्हित file_idx;
      अचिन्हित lineno;
};

तार्किक show_file_पङ्क्ति = असत्य;
तार्किक code_is_instrumented = असत्य;

स्थिर पूर्णाङ्क file_पङ्क्ति_get(पूर्णाङ्क type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (type) {
	पक्ष vpi_पङ्क्ति_No:
	    फर्काऊ rfp->get_lineno();
	स्वभाव:
	    फर्काऊ vpiUndefined;
      }
}

स्थिर वर्ण *file_पङ्क्ति_get_str(पूर्णाङ्क type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (type) {
	पक्ष vpiFile:
	    प्रतिज्ञा(rfp->get_file_idx() < file_names.size());
	    फर्काऊ simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	पक्ष _vpiDescription:
	    यदि (rfp->get_description()) {
		  फर्काऊ simple_set_rbuf_str(rfp->get_description());
	    } अन्यथा फर्काऊ simple_set_rbuf_str("Procedural tracing.");
	स्वभाव:
	    फर्काऊ 0;
      }
}

अन्तःस्थ __vpiFileLine::__vpiFileLine(अचर वर्ण*desc_, दीर्घ file_idx_,
                                    दीर्घ lineno_)
{
      यदि (desc_) description = vpip_name_string(desc_);
      अन्यथा description = 0;
      file_idx = (अचिन्हित) file_idx_;
      lineno = (अचिन्हित) lineno_;
}

पूर्णाङ्क __vpiFileLine::get_type_code(शून्य) अचर
{ फर्काऊ vpi_File_पङ्क्ति; }

पूर्णाङ्क __vpiFileLine::vpi_get(पूर्णाङ्क code)
{ फर्काऊ file_पङ्क्ति_get(code, एतत्); }

वर्ण* __vpiFileLine::vpi_get_str(पूर्णाङ्क code)
{ फर्काऊ file_पङ्क्ति_get_str(code, एतत्); }


vpiHandle vpip_build_file_पङ्क्ति(अचर वर्ण*description, दीर्घ file_idx, दीर्घ lineno)
{
      __vpiFileLine*obj = नूतन __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = सत्य;

      फर्काऊ obj;
}
