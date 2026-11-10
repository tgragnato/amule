//
// This file is part of the aMule Project.
//
// Copyright (c) 2003-2026 aMule Team ( https://amule-org.github.io )
//
// Any parts of this program derived from the xMule, lMule or eMule project,
// or contributed by third-party developers are copyrighted by their
// respective authors.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA
//

#include "StringFunctions.h"
#include "Format.h" // Needed for CFormat

#include "MD5Sum.h" // Interface declarations.

MD5Sum::MD5Sum()
: m_hash()
{
}

MD5Sum::MD5Sum(const wxString &sSource)
{
	Calculate(sSource);
}

MD5Sum::MD5Sum(const uint8 *buffer, size_t len)
{
	Calculate(buffer, len);
}

void MD5Sum::Calculate(const wxString &sSource)
{
	// Nothing we can do against this unicode2char
	Calculate((const uint8 *)(const char *)unicode2char(sSource), sSource.Length());
}

void MD5Sum::Calculate(const uint8 *buffer, size_t len)
{
	CryptoPP::Weak::MD5 md5;
	md5.Restart();
	md5.Update(buffer, len);
	md5.Final(m_hash.b);
	m_sHash.Clear();
}

wxString MD5Sum::GetHash()
{
	if (m_sHash.empty()) {
		// That's still far from optimal, but called much less often.
		for (int i = 0; i < MD5_DIGEST_SIZE; ++i) {
			wxString sT;
			sT = CFormat("%02x") % m_hash.b[i];
			m_sHash += sT;
		}
	}
	return m_sHash;
}

// File_checked_for_headers
