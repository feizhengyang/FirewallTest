#include "stdafx.h"
#include "commoninclude.h"

#define DRV_SERVICE_NAME_FIREWALL	"Firewall"
#define DRV_FILE_NAME_FIREWALL		"SimpleFirewall.sys"		

#define dbvprint					//dbvprinta

CFirewallDriver::CFirewallDriver()
{
	char drv_path[MAX_PATH];
	char tmp_path[MAX_PATH];

	dbvprint("CFirewallDriver() enter.");

	get_cur_folder_full_path_a(tmp_path, _countof(tmp_path));

	m_scm.install_service(DRV_SERVICE_NAME_FIREWALL, drv_path, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START);
	sprintf_s(drv_path, MAX_PATH, "%s%s", tmp_path, DRV_FILE_NAME_FIREWALL);
	dbvprint("install service(%s) success, path : %s.", DRV_SERVICE_NAME_FIREWALL, drv_path);

	// try to start when fail, try to recover
	try
	{
		m_scm.start_service(DRV_SERVICE_NAME_FIREWALL);
		dbvprint("start service %s success.", DRV_SERVICE_NAME_FIREWALL);
	}
	catch (CExp& e)
	{
		dbvprint("%s", e.get_err_string());
		try
		{
			m_scm.stop_service(DRV_SERVICE_NAME_FIREWALL);
			dbvprint("stop service %s success.", DRV_SERVICE_NAME_FIREWALL);
		}
		catch (CExp &e)
		{
			dbvprint("%s", e.get_err_string());
		}
		m_scm.delete_service(DRV_SERVICE_NAME_FIREWALL);
		dbvprint("delete service %s success.", DRV_SERVICE_NAME_FIREWALL);

		// try a 2nd time to load service
		m_scm.install_service(DRV_SERVICE_NAME_FIREWALL, drv_path, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START);
		dbvprint("install service %s success(2nd time).", DRV_SERVICE_NAME_FIREWALL);
		m_scm.start_service(DRV_SERVICE_NAME_FIREWALL);
		dbvprint("start service %s success(2nd time).", DRV_SERVICE_NAME_FIREWALL);
	}
#if 0
	m_driverhandle = CreateFileA(DRV_KERNEL_NAME_SMB, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_driverhandle)
		raise_exp("Fail to open smbus driver, code(%d).", GetLastError());
#endif // 0
}

CFirewallDriver::~CFirewallDriver()
{
	dbvprint("~CFirewallDriver() enter.");

#if 0
	/* if this handle is not deleted, or the service is not
	stopped before deleting a service, the service will be
	marked as TO BE DELETED and there is no way you can
	start this service ever again before you restart the
	computer
	*/
	CloseHandle(m_driverhandle);
#endif // 0

	try
	{
		m_scm.stop_service(DRV_SERVICE_NAME_FIREWALL);
		dbvprint("stop service %s success.", DRV_SERVICE_NAME_FIREWALL);
	}
	catch (CExp &e)
	{
		UNREFERENCED_PARAMETER(e);
		dbvprint("%s", e.get_err_string());
	}

	try
	{
		m_scm.delete_service(DRV_SERVICE_NAME_FIREWALL);
		dbvprint("delete service %s success.", DRV_SERVICE_NAME_FIREWALL);
	}
	catch (CExp &e)
	{
		UNREFERENCED_PARAMETER(e);
		dbvprint("%s", e.get_err_string());
	}
}