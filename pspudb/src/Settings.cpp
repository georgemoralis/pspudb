#include <QtGui/QMainWindow>
#include <QSettings>
#include "Settings.h"

Settings *theSettings = 0;

Settings::Settings()
:   m_ini("pspudb.ini", QSettings::IniFormat)
{

}

Settings::~Settings()
{
    
}

void Settings::load()
{
	//emulator paths
    s_jpcspPath = m_ini.value("/emupath/jpcsppath").toString();
	s_pcspPath	= m_ini.value("/emupath/pcsppath").toString();
	s_pspemuPath= m_ini.value("/emupath/pspemupath").toString();
	s_cspspemuPath= m_ini.value("/emupath/cspspemupath").toString();
	s_pspePath= m_ini.value("/emupath/pspepath").toString();
	s_ppssppPath= m_ini.value("/emupath/ppsspppath").toString();

	s_pspdevicePath=m_ini.value("/device/pspdevpath").toString();
	s_localdirpath=m_ini.value("/local/localdir").toString();

	s_umdpaths=m_ini.value("/settings/umdpaths").toString();
}
void Settings::save()
{

}


