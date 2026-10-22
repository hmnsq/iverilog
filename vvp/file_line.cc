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
      स्पष्ट __vpiFileLine(ध्रुव वर्ण*desc_, दीर्घ file_idx_, दीर्घ lineno_);
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;

      ध्रुव वर्ण* get_description() ध्रुव { प्रत्यागम description; };
      अचिह्नित get_file_idx() ध्रुव { प्रत्यागम file_idx; };
      अचिह्नित get_lineno() ध्रुव { प्रत्यागम lineno; };

    गुप्त:
      ध्रुव वर्ण *description;
      अचिह्नित file_idx;
      अचिह्नित lineno;
};

तार्किक show_file_पङ्क्तिः = असत्य;
तार्किक संहिता_is_instrumented = असत्य;

स्थिर पूर्णाङ्क file_पङ्क्तिः_get(पूर्णाङ्क type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (type) {
	पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम rfp->get_lineno();
	स्वभाव:
	    प्रत्यागम vpiUndefined;
      }
}

स्थिर वर्ण *file_पङ्क्तिः_get_str(पूर्णाङ्क type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (type) {
	पक्ष vpiFile:
	    प्रतिज्ञा(rfp->get_file_idx() < file_नामानि.size());
	    प्रत्यागम simple_समुच्चयः_rbuf_str(file_नामानि[rfp->get_file_idx()]);
	पक्ष _vpiDescription:
	    यदि (rfp->get_description()) {
		  प्रत्यागम simple_समुच्चयः_rbuf_str(rfp->get_description());
	    } अन्यथा प्रत्यागम simple_समुच्चयः_rbuf_str("Procedural tracing.");
	स्वभाव:
	    प्रत्यागम 0;
      }
}

अन्तःस्थ __vpiFileLine::__vpiFileLine(ध्रुव वर्ण*desc_, दीर्घ file_idx_,
                                    दीर्घ lineno_)
{
      यदि (desc_) description = vpip_name_string(desc_);
      अन्यथा description = 0;
      file_idx = (अचिह्नित) file_idx_;
      lineno = (अचिह्नित) lineno_;
}

पूर्णाङ्क __vpiFileLine::get_type_संहिता(शून्य) ध्रुव
{ प्रत्यागम vpi_File_पङ्क्तिः; }

पूर्णाङ्क __vpiFileLine::vpi_get(पूर्णाङ्क code)
{ प्रत्यागम file_पङ्क्तिः_get(code, एतत्); }

वर्ण* __vpiFileLine::vpi_get_str(पूर्णाङ्क code)
{ प्रत्यागम file_पङ्क्तिः_get_str(code, एतत्); }


vpiHandle vpip_build_file_पङ्क्तिः(ध्रुव वर्ण*description, दीर्घ file_idx, दीर्घ lineno)
{
      __vpiFileLine*obj = नूतन __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      संहिता_is_instrumented = सत्य;

      प्रत्यागम obj;
}
