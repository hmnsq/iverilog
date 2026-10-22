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
      ಸ್ಪಷ್ಟ __vpiFileLine(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*desc_, ದೀರ್ಘ file_idx_, ದೀರ್ಘ lineno_);
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ code) override;

      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* get_description() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ description; };
      ಚಿಹ್ನೆರಹಿತ get_file_idx() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ file_idx; };
      ಚಿಹ್ನೆರಹಿತ get_lineno() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ lineno; };

    ಖಾಸಗಿ:
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *description;
      ಚಿಹ್ನೆರಹಿತ file_idx;
      ಚಿಹ್ನೆರಹಿತ lineno;
};

ತಾರ್ಕಿಕ show_file_ಸಾಲು = ಸುಳ್ಳು;
ತಾರ್ಕಿಕ ಸಂಕೇತ_is_instrumented = ಸುಳ್ಳು;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ file_ಸಾಲು_get(ಪೂರ್ಣಾಂಕ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ rfp->get_lineno();
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

ಸ್ಥಿರ ಅಕ್ಷರ *file_ಸಾಲು_get_str(ಪೂರ್ಣಾಂಕ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ vpiFile:
	    ಪ್ರತಿಜ್ಞಾ(rfp->get_file_idx() < file_ಹೆಸರುಗಳು.size());
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[rfp->get_file_idx()]);
	ಸ್ಥಿತಿ _vpiDescription:
	    ಆದರೆ (rfp->get_description()) {
		  ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(rfp->get_description());
	    } ಇಲ್ಲದಿದ್ದರೆ ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str("Procedural tracing.");
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಸಾಲೊಳಗೆ __vpiFileLine::__vpiFileLine(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*desc_, ದೀರ್ಘ file_idx_,
                                    ದೀರ್ಘ lineno_)
{
      ಆದರೆ (desc_) description = vpip_name_string(desc_);
      ಇಲ್ಲದಿದ್ದರೆ description = 0;
      file_idx = (ಚಿಹ್ನೆರಹಿತ) file_idx_;
      lineno = (ಚಿಹ್ನೆರಹಿತ) lineno_;
}

ಪೂರ್ಣಾಂಕ __vpiFileLine::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpi_File_ಸಾಲು; }

ಪೂರ್ಣಾಂಕ __vpiFileLine::vpi_get(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ file_ಸಾಲು_get(code, ಇದು); }

ಅಕ್ಷರ* __vpiFileLine::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ file_ಸಾಲು_get_str(code, ಇದು); }


vpiHandle vpip_build_file_ಸಾಲು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*description, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
      __vpiFileLine*obj = ಹೊಸ __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      ಸಂಕೇತ_is_instrumented = ನಿಜ;

      ಹಿಂತಿರುಗಿ obj;
}
