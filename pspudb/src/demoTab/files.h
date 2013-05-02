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

#pragma once

#include <QString>
#include <QPixmap>
class files
{
public:
	  enum FileType
      {
          pbp = 0, 
          elf = 1 
      };
	files(QString path);
	~files(void);
	bool isPBPSignature(QString file);
	bool isElfSignature(QString file);
	bool isValidFile(QString file);

	QPixmap getDemoIcon(QString filename);
	void getDemoName(QString filename);

	bool isValid() const;
	QPixmap getIcon() const;
	QString getFileName() const;
	QString getDemosTitle() const;
	QString getDemosID() const;
	QString getDemosFW() const;
	QString getPath() const;

	int m_type;
	bool m_isValid;
	QString m_path;
	QString m_filename;
	QPixmap m_icon;
    QString demosTitle;
	QString demosID;
	QString demosFW; 
};

