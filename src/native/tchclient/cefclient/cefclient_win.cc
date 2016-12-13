// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "cefclient.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <locale>
#include <cstring>
#include "../../tch_include/TchVersion.h"

// Program entry point function.
mbstate_t in_cvt_state;
mbstate_t out_cvt_state;

std::wstring s2ws(const std::string& s)
{
	std::locale sys_loc("");

	const char* src_str = s.c_str();
	const size_t BUFFER_SIZE = s.size() + 1;

	wchar_t* intern_buffer = new wchar_t[BUFFER_SIZE];
	std::wmemset(intern_buffer, 0, BUFFER_SIZE);

	const char* extern_from = src_str;
	const char* extern_from_end = extern_from + s.size();
	const char* extern_from_next = 0;
	wchar_t* intern_to = intern_buffer;
	wchar_t* intern_to_end = intern_to + BUFFER_SIZE;
	wchar_t* intern_to_next = 0;

	typedef std::codecvt<wchar_t, char, mbstate_t> CodecvtFacet;

	const std::codecvt<wchar_t, char, mbstate_t>& ct = std::use_facet<CodecvtFacet>(sys_loc);
	auto cvt_rst = ct.in(
		in_cvt_state,
		extern_from, extern_from_end, extern_from_next,
		intern_to, intern_to_end, intern_to_next);

	if (cvt_rst != CodecvtFacet::ok) {
		switch (cvt_rst) {
		case CodecvtFacet::partial:
			std::cerr << "partial";
			break;
		case CodecvtFacet::error:
			std::cerr << "error";
			break;
		case CodecvtFacet::noconv:
			std::cerr << "noconv";
			break;
		default:
			std::cerr << "unknown";
		}
		std::cerr << ", please check in_cvt_state."
			<< std::endl;
	}
	std::wstring result = intern_buffer;

	delete[]intern_buffer;

	return result;
}

std::string ws2s(const std::wstring& ws)
{
	std::locale sys_loc("");

	const wchar_t* src_wstr = ws.c_str();
	const size_t MAX_UNICODE_BYTES = 4;
	const size_t BUFFER_SIZE =
		ws.size() * MAX_UNICODE_BYTES + 1;

	char* extern_buffer = new char[BUFFER_SIZE];
	std::memset(extern_buffer, 0, BUFFER_SIZE);

	const wchar_t* intern_from = src_wstr;
	const wchar_t* intern_from_end = intern_from + ws.size();
	const wchar_t* intern_from_next = 0;
	char* extern_to = extern_buffer;
	char* extern_to_end = extern_to + BUFFER_SIZE;
	char* extern_to_next = 0;

	typedef std::codecvt<wchar_t, char, mbstate_t> CodecvtFacet;

	const std::codecvt<wchar_t, char, mbstate_t>& ct = std::use_facet<CodecvtFacet>(sys_loc);
	auto cvt_rst = ct.out(
		out_cvt_state,
		intern_from, intern_from_end, intern_from_next,
		extern_to, extern_to_end, extern_to_next);
	if (cvt_rst != CodecvtFacet::ok) {
		switch (cvt_rst) {
		case CodecvtFacet::partial:
			std::cerr << "partial";
			break;
		case CodecvtFacet::error:
			std::cerr << "error";
			break;
		case CodecvtFacet::noconv:
			std::cerr << "noconv";
			break;
		default:
			std::cerr << "unknown";
		}
		std::cerr << ", please check out_cvt_state."
			<< std::endl;
	}
	std::string result = extern_buffer;

	delete[]extern_buffer;

	return result;
}
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	int exit_code = 0;
	
	auto tnelab_tch_path_var = getenv("TNELAB_TCH_PATH");
	if (tnelab_tch_path_var != 0) {
		std::wstring tch_dir = s2ws(tnelab_tch_path_var);
		tch_dir.append(L"\\bin\\")
			.append(TCH_VERSION)
			.append(L"-").append(CEF_VERSION)
			.append(L"-").append(DOTNETCORE_VERSION)
			.append(L"-").append(TCH_VERSION_STATUS)
			.append(L"\\x64");
		::SetDllDirectory(tch_dir.c_str());
	}
	//TchStart("http://so.com");

	auto tchmain_hmodule = ::LoadLibrary(L"tchmain.dll");
	if (tchmain_hmodule == 0) {
		TchErrorHandler::OnTchError(-1, "Failed to load tchmain.dll.");
		return -1;
	}
	auto tchmain_ptr = (TchMain)::GetProcAddress(tchmain_hmodule, "TchMain");
	if (tchmain_ptr == 0) {
		TchErrorHandler::OnTchError(-1, "Find TchMain function failed in tchmain.");
		return -1;
	}
	int argc = 0;
	auto argv_w = CommandLineToArgvW(GetCommandLine(), &argc);
	char **argv = new char *[argc];
	std::string tmp_str;
	for (int i = 0; i < argc; ++i) {
		tmp_str = ::ws2s(std::wstring(argv_w[i]));
		argv[i] = const_cast<char*>(tmp_str.c_str());
	}

	exit_code = tchmain_ptr(argc, argv);
	delete(argv);
	return exit_code;
}

//zmg end