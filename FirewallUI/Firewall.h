#pragma once

class CFirewallDriver
{
public:
	CFirewallDriver();
	~CFirewallDriver();
	CSCManager	m_scm;
	HANDLE		m_driverhandle;
};
