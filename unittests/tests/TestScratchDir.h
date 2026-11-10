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

#ifndef AMULE_TEST_SCRATCH_DIR_H
#define AMULE_TEST_SCRATCH_DIR_H

#include <muleunit/test.h>

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/utils.h>

#include <fstream>
#include <string>

#include <stdlib.h>
#include <sys/stat.h>

// A fresh, empty directory under the system temp dir, mode 0700 on POSIX. The destructor removes
// it with its contents, so a failed ASSERT cannot leak it. Failing to create it fails the test.
class TestScratchDir
{
public:
	explicit TestScratchDir(const wxString &prefix)
	{
		const wxString base = wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + prefix;
		wxCharBuffer tpl = (base + "XXXXXX").fn_str();
		if (mkdtemp(tpl.data())) {
			m_path = wxString(tpl.data(), *wxConvFileName);
		}
		if (m_path.empty()) {
			throw muleunit::CTestFailureException(
				"Cannot create a scratch directory at " + base, __FILE__, __LINE__);
		}
	}

	~TestScratchDir() { wxFileName::Rmdir(m_path, wxPATH_RMDIR_RECURSIVE); }

	TestScratchDir(const TestScratchDir &) = delete;
	TestScratchDir &operator=(const TestScratchDir &) = delete;

	std::string Path() const { return std::string(m_path.mb_str(wxConvFile)); }
	const wxString &WxPath() const { return m_path; }

private:
	wxString m_path;
};

inline bool MkSubdir(const std::string &path)
{
	return mkdir(path.c_str(), 0755) == 0;
}

inline bool WriteFile(const std::string &path, const std::string &body)
{
	std::ofstream f(path.c_str(), std::ios::binary);
	if (!f.is_open())
		return false;
	f << body;
	return f.good();
}

#endif // AMULE_TEST_SCRATCH_DIR_H
