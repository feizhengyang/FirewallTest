#include "stdafx.h"
#include "commoninclude.h"

void dbvprinta(char * fmt, ...)
{
	static char buf[1000];
	va_list  arg_list;

	va_start(arg_list, fmt);
	vsprintf(buf, fmt, arg_list);
	va_end(arg_list);

	OutputDebugStringA(buf);
}

void dbvprintw(wchar_t * wfmt, ...)
{
	static wchar_t buf[1000];
	va_list arg_list;

	va_start(arg_list, wfmt);
	wvsprintfW(buf, wfmt, arg_list);
	va_end(arg_list);

	OutputDebugStringW(buf);
}

void get_cur_folder_full_path_a(char* file_path, int elem_num)
{
	DWORD ret_val;
	DWORD err_code;

	file_path[0] = 0;

	ret_val = GetModuleFileNameA(NULL, file_path, elem_num);
	err_code = GetLastError();

	if (!ret_val || ERROR_INSUFFICIENT_BUFFER == err_code)
		raise_exp("GetModuleFileName failed with code(%d).", err_code);

	char* file_name_pos = strrchr(file_path, '\\');
	if (!file_name_pos)
		raise_exp("GetFullPath fail, strrchr can't find '\'.");

	*(file_name_pos + 1) = 0;
}

void get_cur_folder_full_path_w(wchar_t* file_path, int elem_num)
{
	DWORD ret_val;
	DWORD err_code;

	file_path[0] = 0;

	ret_val = GetModuleFileNameW(NULL, file_path, elem_num);
	err_code = GetLastError();

	if (!ret_val || ERROR_INSUFFICIENT_BUFFER == err_code)
		raise_exp("GetModuleFileName failed with code(%d).", err_code);

	wchar_t* file_name_pos = wcsrchr(file_path, '\\');
	if (!file_name_pos)
		raise_exp("GetFullPath fail, wcsrchr can't find '\'.");

	*(file_name_pos + 1) = 0;
}

void raise_exp(UINT32 exp_val)
{
	throw CExp(exp_val);
}

void raise_exp(char *exp_fmt, ...)
{
	va_list args;
	char exp_str[CExp::EXP_MAX_LEN];

	va_start(args, exp_fmt);
	vsprintf_s(exp_str, CExp::EXP_MAX_LEN, exp_fmt, args);
	va_end(args);

	throw CExp(exp_str);
}

COneInstance::COneInstance(char* instance_name)
{
	DWORD ret_val;

	m_handle = CreateMutexA(NULL, FALSE, instance_name);
	ret_val = GetLastError();

	if (NULL == m_handle)
		raise_exp("CreateMutexA fail, code(%d).", ret_val);
	else if (ERROR_ALREADY_EXISTS == ret_val)
		raise_exp("Instance already exist.");
}

COneInstance::~COneInstance()
{
	CloseHandle(m_handle);
}

CSCManager::CSCManager(DWORD access /*= SC_MANAGER_ALL_ACCESS*/)
{
	m_sc_manger = OpenSCManager(0, 0, access);
	if (!m_sc_manger)
		raise_exp("Fail to open SCManager with access(0x%08x) : %d.", access, GetLastError());
}

CSCManager::~CSCManager()
{
	CloseServiceHandle(m_sc_manger);
}

void CSCManager::install_service(char * svc_name, char * svc_path, DWORD svc_type, DWORD start_type)
{
	DWORD err_code;

	SC_HANDLE h_svc = CreateServiceA(
		m_sc_manger, svc_name, svc_name,
		SERVICE_ALL_ACCESS, svc_type,
		start_type, SERVICE_ERROR_NORMAL,
		svc_path, 0, 0, 0, 0, 0);

	err_code = GetLastError();

	if (!h_svc && 1073 != err_code)
		raise_exp("Fail to create service name('%s') path('%s') : %d.",
			svc_name, svc_path, GetLastError());

	CloseServiceHandle(h_svc);
}

void CSCManager::delete_service(char * svc_name)
{
	SC_HANDLE h_svc = open_service(svc_name, SC_MANAGER_ALL_ACCESS);

	if (!DeleteService(h_svc))
	{
		CloseServiceHandle(h_svc);
		raise_exp("Fail to delete service : %d.", GetLastError());
	}

	CloseServiceHandle(h_svc);
}

void CSCManager::start_service(char * svc_name, int n_arg /*= 0*/, const char**p_args /*= NULL*/)
{
	SC_HANDLE h_svc;
	h_svc = open_service(svc_name, SC_MANAGER_ALL_ACCESS);
	if (!h_svc)
		raise_exp("Fail to open service name('%s') : %d.", svc_name, GetLastError());

	if (!StartServiceA(h_svc, n_arg, p_args))
	{
		CloseServiceHandle(h_svc);
		raise_exp("Fail to start service name('%s') : %d.", svc_name, GetLastError());
	}

	CloseServiceHandle(h_svc);
}

void CSCManager::stop_service(char * svc_name)
{
	SC_HANDLE h_svc = open_service(svc_name, SC_MANAGER_ALL_ACCESS);
	SERVICE_STATUS svc_sts;

	if (!ControlService(h_svc, SERVICE_CONTROL_STOP, &svc_sts) &&
		ERROR_SERVICE_NOT_ACTIVE != GetLastError())
	{
		CloseServiceHandle(h_svc);
		raise_exp("Fail to stop service : %d.", GetLastError());
	}
	CloseServiceHandle(h_svc);
}

SC_HANDLE CSCManager::open_service(char * svc_name, DWORD access)
{
	return OpenServiceA(m_sc_manger, svc_name, access);
}

