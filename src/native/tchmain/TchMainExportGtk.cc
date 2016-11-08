#include "TchMainExport.h"
#include <dlfcn.h>
#include <iostream>
#include <string>
#include <locale>
#include <cstring>
#include <experimental/filesystem>
#include <unistd.h>
#include <linux/limits.h>
#include <dirent.h>

namespace gnu_fs = std::experimental::filesystem;


void AddFilesFromDirectoryToTpaList(std::string directory, std::string& tpa_list_out) {
	DIR  *dir_ptr = opendir(directory.c_str());
	struct dirent *entry= readdir(dir_ptr);
	std::string path;
	while (entry) {
		path = entry->d_name;
		if (path.size()>4&&path.compare(path.size() - 4, 4, ".dll")==0) {
			tpa_list_out.append(directory).append("/").append(path + ":");
		}
		entry = readdir(dir_ptr);
	}
	closedir(dir_ptr);
	tpa_list_out.erase(tpa_list_out.size() - 1, 1);
}

std::string exe_name;
int TchMain(int argc, char* argv[]) {	
	const char* exe_path = realpath(argv[0], 0);
	std::string cmd_line_str(exe_path);
	int scan_i = cmd_line_str.rfind('/');
	std::string app_name = cmd_line_str.substr(scan_i + 1, cmd_line_str.size() - scan_i);
	exe_name = app_name;
	std::string app_path(exe_path);
	std::string app_dir = app_path.substr(0, app_path.length() - app_name.length() - 1);
	
	std::string cli_entry_path = app_path+".dll";

	std::string clr_dir;
	std::string clr_path;
	clr_dir = app_dir;
	clr_dir.append("/clr");
	clr_path=clr_dir+"/libcoreclr.so";
	
	std::string tpa_list;
	AddFilesFromDirectoryToTpaList(clr_dir, tpa_list);
	
	std::string nativeDllSearchDirs(app_dir);
	nativeDllSearchDirs.append(":").append(clr_dir);

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

	const char *propertyValues[] = {
		// TRUSTED_PLATFORM_ASSEMBLIES
		tpa_list.c_str(),
		// APP_PATHS
		app_dir.c_str(),
		// APP_NI_PATHS
		app_dir.c_str(),
		// NATIVE_DLL_SEARCH_DIRECTORIES
		nativeDllSearchDirs.c_str(),
		// System.GC.Server
		useServerGc,
	};

	auto coreclr_hmodule = ::dlopen(clr_path.c_str(), RTLD_NOW);
	
	auto coreclr_initialize = (coreclr_initialize_ptr)::dlsym(coreclr_hmodule, "coreclr_initialize");
	auto coreclr_shutdown = (coreclr_shutdown_ptr)::dlsym(coreclr_hmodule, "coreclr_shutdown");
	//auto coreclr_create_delegate = (coreclr_create_delegate_ptr)::dlsym(coreclr_hmodule, "coreclr_create_delegate");
	auto coreclr_execute_assembly = (coreclr_execute_assembly_ptr)::dlsym(coreclr_hmodule, "coreclr_execute_assembly");

	void* hostHandle;
	unsigned int domainId;
	int status = 0;
	printf("app_path->[%s]\r\n", app_path.c_str());
	status = coreclr_initialize(app_path.c_str(), "TchApp", sizeof(propertyKeys) / sizeof(propertyKeys[0]), propertyKeys, propertyValues, &hostHandle, &domainId);
	printf("coreclr_initialize status:%x\r\n", status);

	unsigned int exit_code = 0;
	const char** argv_c = const_cast<const char**>(argv);
	status = coreclr_execute_assembly(hostHandle, domainId, argc, argv_c, cli_entry_path.c_str(), &exit_code);
	printf("coreclr_execute_assembly status:%x\r\n", status);

	//Run run = 0;
	//status = coreclr_create_delegate(hostHandle, domainId, "TchApp.CLIEntrance", "TchApp.CLIEntrance.Program", "EnterCLI", reinterpret_cast<void**>(&run));	
	//printf("coreclr_create_delegate status:%x\r\n", status);	

	coreclr_shutdown(hostHandle, domainId);
	printf("exit_code->%d\r\n", exit_code);
	return exit_code;
}