// Copyright (c) 2013- PSPUDB Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/georgemoralis/pspudb and http://www.pspudb.com/

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


