#pragma once

void dbvprinta(char * fmt, ...);

void dbvprintw(wchar_t * wfmt, ...);

void get_cur_folder_full_path_a(char* file_path, int elem_num);

void get_cur_folder_full_path_w(wchar_t* file_path, int elem_num);

class CExp
{
public:
	static const INT32 EXP_MAX_LEN = MAX_PATH;

private:
	INT32 m_exp_type;
	static const INT32 EXP_TYPE_STR = 1;
	static const INT32 EXP_TYPE_UINT = 2;

private:
	char m_exp_str[EXP_MAX_LEN];
	UINT32 m_exp_val;

public:
	CExp(char *exp_string)
	{
		strcpy_s(m_exp_str, _countof(m_exp_str), exp_string);
		m_exp_type = EXP_TYPE_STR;
	}
	CExp(UINT32 exp_value)
	{
		m_exp_val = exp_value;
		m_exp_type = EXP_TYPE_UINT;
	}
	const char *get_err_string()
	{
		assert(m_exp_type == EXP_TYPE_STR);
		return m_exp_str;
	}
	const UINT32 get_err_code()
	{
		assert(m_exp_type == EXP_TYPE_UINT);
		return m_exp_val;
	}
};

void raise_exp(UINT32 exp_val);
void raise_exp(char *exp_fmt, ...);

class COneInstance
{
public:
	COneInstance(char* instance_name);
	~COneInstance();
private:
	HANDLE	m_handle;
};

class CSCManager
{
public:
	CSCManager(DWORD access = SC_MANAGER_ALL_ACCESS);
	~CSCManager();
	void install_service(char * svc_name, char * svc_path, DWORD svc_type, DWORD start_type);
	void delete_service(char * svc_name);
	void start_service(char * svc_name, int n_arg = 0, const char**p_args = NULL);
	void stop_service(char * svc_name);
	SC_HANDLE open_service(char * svc_name, DWORD access);
private:
	SC_HANDLE m_sc_manger;
};
