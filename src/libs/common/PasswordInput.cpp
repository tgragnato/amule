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

#include "PasswordInput.h"

#include <wx/string.h>

#include <cerrno>
#include <cstdio>
#include <iostream>

#include <csignal>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

namespace
{
volatile std::sig_atomic_t interrupted = 0;
void InterruptInput(int signal)
{
	interrupted = signal;
}

// Restore echo before delivering a signal to the original handler. In particular, Ctrl-C
// must not leave the user's shell with echo disabled. poll bounds the signal-before-read race.
class TerminalInput
{
public:
	bool Open()
	{
		if (tcgetattr(STDIN_FILENO, &m_mode) != 0) {
			return false;
		}
		interrupted = 0;
		struct sigaction action = {};
		action.sa_handler = InterruptInput;
		sigemptyset(&action.sa_mask);
		for (int signal : m_signals) {
			if (sigaction(signal, &action, &m_previous[m_count]) != 0) {
				return false;
			}
			++m_count;
		}
		auto hidden = m_mode;
		hidden.c_lflag &= ~(ECHO | ECHONL);
		m_changed = tcsetattr(STDIN_FILENO, TCSANOW, &hidden) == 0;
		return m_changed;
	}

	~TerminalInput()
	{
		if (m_changed) {
			tcsetattr(STDIN_FILENO, TCSANOW, &m_mode);
		}
		for (unsigned i = 0; i < m_count; ++i) {
			sigaction(m_signals[i], &m_previous[i], nullptr);
		}
		if (interrupted != 0) {
			raise(interrupted);
		}
	}

private:
	const int m_signals[5] = { SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGTSTP };
	struct sigaction m_previous[5] = {};
	unsigned m_count = 0;
	struct termios m_mode = {};
	bool m_changed = false;
};

bool ReadTerminalLine(std::string &line)
{
	while (interrupted == 0) {
		struct pollfd fd = { STDIN_FILENO, POLLIN, 0 };
		const int ready = poll(&fd, 1, 100);
		if (ready < 0) {
			if (errno == EINTR) {
				continue;
			}
			return false;
		}
		if (ready == 0) {
			continue;
		}
		char c = 0;
		if (read(STDIN_FILENO, &c, 1) != 1) {
			return false;
		}
		if (c == '\n') {
			return true;
		}
		line += c;
	}
	return false;
}
} // namespace

bool mulecommon::ReadPassword(const char *prompt, bool confirm, std::string &password, std::string &error)
{
	error.clear();
	std::string input;
	const bool terminal = isatty(STDIN_FILENO) != 0;
	if (terminal) {
		TerminalInput terminalInput;
		if (!terminalInput.Open()) {
			error = "cannot disable terminal echo";
			return false;
		}
		std::cerr << prompt << std::flush;
		const bool read = ReadTerminalLine(input);
		std::cerr << '\n';
		if (!read) {
			error = "password input cancelled or failed";
			return false;
		}
		if (confirm) {
			std::string repeated;
			std::cerr << "Confirm password: " << std::flush;
			const bool reread = ReadTerminalLine(repeated);
			std::cerr << '\n';
			if (!reread || input != repeated) {
				error = reread ? "passwords do not match"
					       : "password confirmation cancelled or failed";
				return false;
			}
		}
	} else {
		if (!std::getline(std::cin, input) || std::cin.bad()) {
			error = "cannot read password from stdin (EOF or input error)";
			return false;
		}
		// Accept CRLF from scripts on either platform; preserve all other whitespace.
		if (!input.empty() && input.back() == '\r') {
			input.pop_back();
		}
	}
	if (input.find('\0') != std::string::npos ||
		(!input.empty() && wxString::FromUTF8(input.data(), input.size()).empty())) {
		error = "password must be valid UTF-8 without NUL bytes";
		return false;
	}
	password = std::move(input);
	return true;
}
