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
