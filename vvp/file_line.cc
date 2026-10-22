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

ವರ್ಗ __vpiFileLine : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiFileLine(ಧ್ರುವ ವರ್ಣ*desc_, long file_idx_, long lineno_);
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;

      ಧ್ರುವ ವರ್ಣ* get_description() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ description; };
      unsigned get_file_idx() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ file_idx; };
      unsigned get_lineno() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ lineno; };

    ಗುಪ್ತ:
      ಧ್ರುವ ವರ್ಣ *description;
      unsigned file_idx;
      unsigned lineno;
};

ತಾರ್ಕಿಕ show_file_ಸಾಲು = ಅಸತ್ಯ;
ತಾರ್ಕಿಕ code_is_instrumented = ಅಸತ್ಯ;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ file_ಸಾಲು_get(ಪೂರ್ಣಾಙ್ಕ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ rfp->get_lineno();
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

ಸ್ಥಿರ ವರ್ಣ *file_ಸಾಲು_get_str(ಪೂರ್ಣಾಙ್ಕ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ vpiFile:
	    ಪ್ರತಿಜ್ಞಾ(rfp->get_file_idx() < file_names.size());
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	ಪಕ್ಷ _vpiDescription:
	    ಯದಿ (rfp->get_description()) {
		  ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(rfp->get_description());
	    } ಅನ್ಯಥಾ ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str("Procedural tracing.");
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಅನ್ತಃಸ್ಥ __vpiFileLine::__vpiFileLine(ಧ್ರುವ ವರ್ಣ*desc_, long file_idx_,
                                    long lineno_)
{
      ಯದಿ (desc_) description = vpip_name_string(desc_);
      ಅನ್ಯಥಾ description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

ಪೂರ್ಣಾಙ್ಕ __vpiFileLine::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpi_File_ಸಾಲು; }

ಪೂರ್ಣಾಙ್ಕ __vpiFileLine::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ file_ಸಾಲು_get(code, ಏತತ್); }

ವರ್ಣ* __vpiFileLine::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ file_ಸಾಲು_get_str(code, ಏತತ್); }


vpiHandle vpip_build_file_ಸಾಲು(ಧ್ರುವ ವರ್ಣ*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = ನೂತನ __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = ಸತ್ಯ;

      ಪ್ರತ್ಯಾಗಮ obj;
}
