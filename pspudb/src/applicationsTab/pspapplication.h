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

#ifndef PSPAPPLICATION_H
 #define PSPAPPLICATION_H

#include <QString>
#include <QPixmap>


 //Forward declarators
class QDir;
 
 class PSPApplication
 {
 public:
     enum ApplicationType
      {
          Single = 0, //Single Folder and PBP File
          Multiple = 1 //Multiple folders and PBP Files
      };
      
     PSPApplication(QString path);
     ~PSPApplication();
     
     ApplicationType getType() const;
     QString getPath() const;
     QString getAppTitle() const;
     QPixmap getIcon() const;
     void getAppName(QString filename);
	 QString getAppID() const;
	 QString getAppFW() const;
	 QString getApplicationdirectory() const;
	 QString getGamedirectory() const;
	 QPixmap getAppIcon(QString filename);
     
 private:
     bool isPBPFile(QString filePath);
     bool isPBPSignature(QString file);
     bool isSinglePBP(QString file);
     QString getMultiplePath(QString path);
     bool isSCEMultiple(QDir dir);
     bool isPercentMultiple(QDir dir);
     
     QString m_path; //Path to the single PBP or to the % folder multiple PBP
     ApplicationType m_type;
     QString appTitle;
	 QString appID;
	 QString appFW; 
     QPixmap m_icon;

	 QString applicationdirectory;
	 QString gamedirectory;
 };
 
 #endif
