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

#ifndef Settings_h__
#define Settings_h__

class Settings
{    
public:
    Settings();
    ~Settings();
	void load();
	void save();
	 
     QSettings              m_ini;
     //variables
	 QString s_jpcspPath;
     QString s_pcspPath;
	 QString s_pspemuPath;
	 QString s_cspspemuPath;
	 QString s_pspePath;
	 QString s_ppssppPath;
	 QString s_pspdevicePath;
	 QString s_localdirpath;

	 //umd dirs
	 QString s_umdpaths;
};

extern Settings *theSettings;

#endif // Settings_h__
