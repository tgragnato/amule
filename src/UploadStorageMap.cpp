//
// This file is part of the aMule Project.
//
// Copyright (c) 2003-2026 aMule Team ( https://amule-org.github.io )
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

#include "config.h"
#include "UploadStorageMap.h"
#include <cstdio>
#include <cstdlib>
#include <cwchar>

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <sys/param.h>
#include <sys/mount.h>
#endif

bool UploadStorageMap::Refresh([[maybe_unused]] bool includeNetwork)
{
	UploadStorageMap next;
#if defined(__linux__)
	// mountinfo contains device IDs and mount points without probing any volume.
	if (FILE *file = std::fopen("/proc/self/mountinfo", "r")) {
		char *line = nullptr;
		size_t size = 0;
		while (::getline(&line, &size, file) >= 0) {
			next.AddLinuxMount(line);
		}
		std::free(line);
		std::fclose(file);
	}
#elif defined(__APPLE__) || defined(__FreeBSD__)
	struct statfs *mounts = nullptr;
	const int count = getmntinfo(&mounts, MNT_NOWAIT);
	for (int i = 0; i < count; ++i) {
		next.Add(mounts[i].f_mntonname,
			"device:" + std::to_string(mounts[i].f_fsid.val[0]) + ":" +
				std::to_string(mounts[i].f_fsid.val[1]));
	}
#ifdef __APPLE__
	next.MergeMacDataVolume();
#endif
#endif
	// Keep the previous snapshot on a transient discovery failure. Unsupported OSes
	// use the conservative unknown group rather than inventing per-directory limits.
	if (!next.m_mounts.empty()) {
		m_mounts = std::move(next.m_mounts);
		return true;
	}
	return false;
}

void UploadStorageMap::ResolveDirectories([[maybe_unused]] const std::vector<std::string> &directories)
{
	for (const auto &directory : directories) {
		if (char *target = ::realpath(directory.c_str(), nullptr)) {
			if (directory != target) {
				AddResolvedDirectory(directory, target);
			}
			std::free(target);
		}
	}
}
