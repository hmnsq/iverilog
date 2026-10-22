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

वग्ग __vpiFileLine : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      explicit __vpiFileLine(ध्रुव वर्ण*desc_, long file_idx_, long lineno_);
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;

      ध्रुव वर्ण* get_description() ध्रुव { पटिनिवत्त description; };
      unsigned get_file_idx() ध्रुव { पटिनिवत्त file_idx; };
      unsigned get_lineno() ध्रुव { पटिनिवत्त lineno; };

    गुप्त:
      ध्रुव वर्ण *description;
      unsigned file_idx;
      unsigned lineno;
};

तार्किक show_file_पङ्क्तिः = मिच्छा;
तार्किक code_is_instrumented = मिच्छा;

स्थिर पूर्णाङ्क file_पङ्क्तिः_get(पूर्णाङ्क type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (type) {
	पक्ष vpi_पङ्क्तिः_No:
	    पटिनिवत्त rfp->get_lineno();
	स्वभाव:
	    पटिनिवत्त vpiUndefined;
      }
}

स्थिर वर्ण *file_पङ्क्तिः_get_str(पूर्णाङ्क type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (type) {
	पक्ष vpiFile:
	    प्रतिज्ञा(rfp->get_file_idx() < file_names.size());
	    पटिनिवत्त simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	पक्ष _vpiDescription:
	    सचे (rfp->get_description()) {
		  पटिनिवत्त simple_set_rbuf_str(rfp->get_description());
	    } अञ्ञथा पटिनिवत्त simple_set_rbuf_str("Procedural tracing.");
	स्वभाव:
	    पटिनिवत्त 0;
      }
}

अन्तःस्थ __vpiFileLine::__vpiFileLine(ध्रुव वर्ण*desc_, long file_idx_,
                                    long lineno_)
{
      सचे (desc_) description = vpip_name_string(desc_);
      अञ्ञथा description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

पूर्णाङ्क __vpiFileLine::get_type_code(शून्य) ध्रुव
{ पटिनिवत्त vpi_File_पङ्क्तिः; }

पूर्णाङ्क __vpiFileLine::vpi_get(पूर्णाङ्क code)
{ पटिनिवत्त file_पङ्क्तिः_get(code, एतत्); }

वर्ण* __vpiFileLine::vpi_get_str(पूर्णाङ्क code)
{ पटिनिवत्त file_पङ्क्तिः_get_str(code, एतत्); }


vpiHandle vpip_build_file_पङ्क्तिः(ध्रुव वर्ण*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = नूतन __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = सच्च;

      पटिनिवत्त obj;
}
