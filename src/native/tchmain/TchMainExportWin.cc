#include "TchMainExport.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <locale>
#include <cstring>
#include <experimental/filesystem>
#include "../tch_include/TchVersion.h"
#include <io.h>

namespace gnu_fs = std::experimental::filesystem;

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
	
	const std::codecvt<wchar_t,char,mbstate_t>& ct=std::use_facet<CodecvtFacet>(sys_loc);
		auto cvt_rst=ct.in(
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
	auto cvt_rst =ct.out(
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

void AddFilesFromDirectoryToTpaList(std::wstring directory, std::wstring& tpa_list_out) {

	for (auto& dirent : gnu_fs::directory_iterator(directory)) {
		std::wstring path = s2ws(dirent.path().string());

		if (!path.compare(path.length() - 4, 4, L".dll")) {
			tpa_list_out.append(path + L";");
		}
	}
	tpa_list_out.erase(tpa_list_out.size() - 1, 1);
}

int TchMain(int argc,char* argv[]){	
	wchar_t exe_path[MAX_PATH];
	GetModuleFileName(GetModuleHandle(0), exe_path, MAX_PATH);
	
	std::wstring cmd_line_str(exe_path);
	int scan_i = cmd_line_str.rfind('\\');
	std::wstring app_name = cmd_line_str.substr(scan_i + 1, cmd_line_str.size() - scan_i);
	std::wstring app_path(exe_path);
	std::wstring app_dir = app_path.substr(0, app_path.length() - app_name.length() - 1);

	std::wstring cli_entry_path = app_path.substr(0, app_path.size() - 4).append(L".dll");

	std::wstring clr_dir;
	std::wstring clr_path;
	std::string clr_path_c(ws2s(app_dir));
	clr_path_c.append("\\clr\\coreclr.dll");
	bool is_use_local_clr = access(clr_path_c.c_str(), 0)==0;//检查是否存在本地clr
	if (!is_use_local_clr) {
		auto clr_path_var = ::getenv("ProgramFiles");
		clr_dir = s2ws(clr_path_var).append(L"\\dotnet").append(L"\\shared\\Microsoft.NETCore.App\\").append(DOTNETCORE_VERSION);
	}
	else {
		clr_dir=app_dir;
		clr_dir.append(L"\\clr");
	}		
	clr_path = clr_dir;
	clr_path.append(L"\\coreclr.dll");

	std::wstring tpa_list;
	AddFilesFromDirectoryToTpaList(clr_dir, tpa_list);

	std::wstring nativeDllSearchDirs(app_dir);
	nativeDllSearchDirs.append(L";").append(clr_dir);

	const char* serverGcVar = "CORECLR_SERVER_GC";
	const char* useServerGc = std::getenv(serverGcVar);
	if (useServerGc == nullptr)
	{
		useServerGc = "0";
	}
	useServerGc = std::strcmp(useServerGc, "1") == 0 ? "true" : "false";

	const char *propertyKeys[] = {
		"TRUSTED_PLATFORM_ASSEMBLIES",
		"APP_PATHS",
		"APP_NI_PATHS",
		"NATIVE_DLL_SEARCH_DIRECTORIES",
		"System.GC.Server",
	};
	std::string tpa_list_c = ws2s(tpa_list);
	std::string app_dir_c = ws2s(app_dir);
	std::string nativeDllSearchDirs_c = ws2s(nativeDllSearchDirs);
	const char *propertyValues[] = {
		// TRUSTED_PLATFORM_ASSEMBLIES
		tpa_list_c.c_str(),
		// APP_PATHS
		app_dir_c.c_str(),
		// APP_NI_PATHS
		app_dir_c.c_str(),
		// NATIVE_DLL_SEARCH_DIRECTORIES
		nativeDllSearchDirs_c.c_str(),
		// System.GC.Server
		useServerGc,
	};
	auto coreclr_hmodule = ::LoadLibrary(clr_path.c_str());
	auto coreclr_initialize = (coreclr_initialize_ptr)::GetProcAddress(coreclr_hmodule, "coreclr_initialize");
	auto coreclr_shutdown = (coreclr_shutdown_ptr)::GetProcAddress(coreclr_hmodule, "coreclr_shutdown");
	//auto coreclr_create_delegate = (coreclr_create_delegate_ptr)::GetProcAddress(coreclr_hmodule, "coreclr_create_delegate");
	auto coreclr_execute_assembly = (coreclr_execute_assembly_ptr)::GetProcAddress(coreclr_hmodule, "coreclr_execute_assembly");

	void* hostHandle;
	unsigned int domainId;
	int status = 0;
	status = coreclr_initialize(ws2s(app_path).c_str(), "TchApp", sizeof(propertyKeys) / sizeof(propertyKeys[0]), propertyKeys, propertyValues, &hostHandle, &domainId);
	LPWSTR str_status;
	printf("coreclr_initialize status:%x\r\n", status);
	unsigned int exit_code = 0;
	std::string cli_entry_path_c = ws2s(cli_entry_path);
	const char** argv_c = const_cast<const char**>(argv);
	status = coreclr_execute_assembly(hostHandle, domainId, argc, argv_c, cli_entry_path_c.c_str(), &exit_code);
	printf("coreclr_execute_assembly status:%x\r\n", status);
	//Run run = 0;
	//status = coreclr_create_delegate(hostHandle, domainId, "TchApp.CLIEntrance", "TchApp.CLIEntrance.Program", "EnterCLI", reinterpret_cast<void**>(&run));	
	//printf("coreclr_create_delegate status:%x\r\n", status);	

	coreclr_shutdown(hostHandle, domainId);
	return exit_code;
}