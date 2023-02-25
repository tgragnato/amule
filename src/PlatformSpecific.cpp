//
// This file is part of the aMule Project.
//
// Copyright (c) 2008-2011 aMule Team ( admin@amule.org / http://www.amule.org )
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

#include "PlatformSpecific.h"
#include "config.h"
#include "CFile.h"

bool PlatformSpecific::CreateSparseFile(const CPath& name, uint64_t WXUNUSED(size))
{
	CFile f;
	return f.Create(name.GetRaw(), true) && f.Close();
}


#if defined(HAVE_GETMNTENT) && defined(HAVE_MNTENT_H)
#include <stdio.h>
#include <string.h>
#include <mntent.h>
#ifndef _PATH_MOUNTED
#	define _PATH_MOUNTED	"/etc/mtab"
#endif
#include <common/StringFunctions.h>

static PlatformSpecific::EFSType doGetFilesystemType(const CPath& path)
{
	struct mntent *entry = NULL;
	PlatformSpecific::EFSType retval = PlatformSpecific::fsOther;
	FILE *mnttab = fopen(_PATH_MOUNTED, "r");
	unsigned bestPrefixLen = 0;

	if (mnttab == NULL) {
		return PlatformSpecific::fsOther;
	}

	while ((entry = getmntent(mnttab)) != NULL) {
		if (entry->mnt_dir) {
			wxString dir = char2unicode(entry->mnt_dir);
			if (dir == path.GetRaw().Mid(0, dir.Length())) {
				if (dir.Length() >= bestPrefixLen) {
					if (entry->mnt_type == NULL) {
						break;
					} else if (!strcmp(entry->mnt_type, "ntfs")) {
						retval = PlatformSpecific::fsNTFS;
					} else if (!strcmp(entry->mnt_type, "msdos") ||
						   !strcmp(entry->mnt_type, "umsdos") ||
						   !strcmp(entry->mnt_type, "vfat") ||
						   !strncmp(entry->mnt_type, "fat", 3)) {
						retval = PlatformSpecific::fsFAT;
					} else if (!strcmp(entry->mnt_type, "hfs")) {
						retval = PlatformSpecific::fsHFS;
					} else if (!strcmp(entry->mnt_type, "hpfs")) {
						retval = PlatformSpecific::fsHPFS;
					} else if (!strcmp(entry->mnt_type, "minix")) {
						retval = PlatformSpecific::fsMINIX;
					} /* Add more filesystem types here */
					else if (dir.Length() > bestPrefixLen) {
						retval = PlatformSpecific::fsOther;
					}
					bestPrefixLen = dir.Length();
				}
			}
		}
	}
	fclose(mnttab);
	return retval;
}

#elif defined(HAVE_GETMNTENT) && defined(HAVE_SYS_MNTENT_H) && defined(HAVE_SYS_MNTTAB_H)
#include <stdio.h>
#include <string.h>
#include <sys/mntent.h>
#include <sys/mnttab.h>
#ifndef MNTTAB
#	define MNTTAB	"/etc/mnttab"
#endif
#include <common/StringFunctions.h>

static PlatformSpecific::EFSType doGetFilesystemType(const CPath& path)
{
	struct mnttab entryStatic;
	struct mnttab *entry = &entryStatic;
	PlatformSpecific::EFSType retval = PlatformSpecific::fsOther;
	FILE *fmnttab = fopen(MNTTAB, "r");
	unsigned bestPrefixLen = 0;

	if (fmnttab == NULL) {
		return PlatformSpecific::fsOther;
	}

	while (getmntent(fmnttab, entry) == 0) {
		if (entry->mnt_mountp) {
			wxString dir = char2unicode(entry->mnt_mountp);
			if (dir == path.GetRaw().Mid(0, dir.Length())) {
				if (dir.Length() >= bestPrefixLen) {
					if (entry->mnt_fstype == NULL) {
						break;
					} else if (!strcmp(entry->mnt_fstype, MNTTYPE_PCFS)) {
						retval = PlatformSpecific::fsFAT;
					} else if (hasmntopt(entry, MNTOPT_NOLARGEFILES)) {
						// MINIX is a file system that can handle special chars but has no large files.
						retval = PlatformSpecific::fsMINIX;
					} else if (dir.Length() > bestPrefixLen) {
						retval = PlatformSpecific::fsOther;
					}
					bestPrefixLen = dir.Length();
				}
			}
		}
	}
	fclose(fmnttab);
	return retval;
}

#else

// No way to determine filesystem type, no restrictions apply.
static inline PlatformSpecific::EFSType doGetFilesystemType(const CPath& WXUNUSED(path))
{
	return PlatformSpecific::fsOther;
}

#endif

#include <map>
#include <wx/thread.h>

PlatformSpecific::EFSType PlatformSpecific::GetFilesystemType(const CPath& path)
{
	typedef std::map<wxString, EFSType>	FSMap;
	// Caching previous results, to speed up further checks.
	static FSMap	s_fscache;
	// Lock used to ensure the integrity of the cache.
	static wxMutex	s_lock;

	wxCHECK_MSG(path.IsOk(), fsOther, wxT("Invalid path in GetFilesystemType()"));

	wxMutexLocker locker(s_lock);

	FSMap::iterator it = s_fscache.find(path.GetRaw());
	if (it != s_fscache.end()) {
		return it->second;
	}

	return s_fscache[path.GetRaw()] = doGetFilesystemType(path);
}


// Power event vetoing

static bool m_preventingSleepMode = false;

#if defined(__WXMAC__) && __MAC_OS_X_VERSION_MAX_ALLOWED >= 1050	// 10.5 only
	#include <IOKit/pwr_mgt/IOPMLib.h>
	static IOPMAssertionID assertionID;
#endif

void PlatformSpecific::PreventSleepMode()
{
	if (!m_preventingSleepMode) {
		#if defined(__WXMAC__) && __MAC_OS_X_VERSION_MAX_ALLOWED >= 1060	// 10.6 only
			CFStringRef reasonForActivity= CFSTR("Prevent Display Sleep");
			IOReturn success = IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
												kIOPMAssertionLevelOn, reasonForActivity, &assertionID);
			if (success == kIOReturnSuccess) {
				// Correctly vetoed, flag so we don't do it again.
				m_preventingSleepMode = true;
			} else {
				// May be should be better to trace in log?
			}

		#elif defined(__WXMAC__) && __MAC_OS_X_VERSION_MAX_ALLOWED >= 1050	// 10.5 only
			IOReturn success = IOPMAssertionCreate(kIOPMAssertionTypeNoDisplaySleep,
												kIOPMAssertionLevelOn, &assertionID);
			if (success == kIOReturnSuccess) {
				// Correctly vetoed, flag so we don't do it again.
				m_preventingSleepMode = true;
			} else {
				// ??
			}
		#else
			//#warning Power event vetoing not implemented.
			// Not implemented
		#endif
	}
}

void PlatformSpecific::AllowSleepMode()
{
	if (m_preventingSleepMode) {
		#if defined(__WXMAC__) && __MAC_OS_X_VERSION_MAX_ALLOWED >= 1050	// 10.5 only
			IOReturn success = IOPMAssertionRelease(assertionID);
			if (success == kIOReturnSuccess) {
				// Correctly restored, flag so we don't do it again.
				m_preventingSleepMode = false;
			} else {
				// ??
			}
		#else
			// Not implemented
		#endif
	}
}
