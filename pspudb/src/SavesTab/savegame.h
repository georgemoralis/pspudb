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

 #ifndef SAVEGAME_H
 #define SAVEGAME_H

#include <QString>
#include <QPixmap>

 //Forward declarators
class QDir;
 
 class Savegame
 {
 public:
     Savegame(QString path);
     ~Savegame();
     
     QString getPath() const;
     QString getName() const;
     QPixmap getIcon() const;
	 QString getSaveDataTitle() const;
	 QString getSaveDirectory() const;
	 QString getSaveDataDetail() const;
     
     void getSavegameName(QString path);
     QPixmap getSavegameIcon(QString path);
     
 private:     
     QString m_path;
     QPixmap m_icon;

	 QString savedataTitle;
	 QString appTitle;
	 QString savedirectory;
	 QString savedataDetail;
 };
 
#endif
