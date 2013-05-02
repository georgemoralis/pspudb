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

 #include "savegame.h"
 
 #include <QFileInfo>
 #include <QFile>
 #include <QDataStream>
 #include <QDir>
 #include <QDebug>

typedef struct  //__attribute__((packed))
{
    unsigned long signature;
    unsigned long version;
    unsigned long fields_table_offs;
    unsigned long values_table_offs;
    signed long nitems;
} SFOHeader;

typedef struct //__attribute__((packed))
{
    unsigned short field_offs;
    unsigned char  unk;
    unsigned char  type; // 0x2 -> string, 0x4 -> number
    unsigned long length;
    unsigned long size;
    unsigned short val_offs;
    unsigned short unk4;
} SFODir;


Savegame::Savegame(QString path)
:m_path(path)
{
    //qDebug() << "Attempting to create a savegame from" << path;
    if (!QFileInfo(m_path).isDir())
    {
        //throw QPSPManagerPBPException();
    }
     
    getSavegameName(m_path);
    m_icon = getSavegameIcon(m_path);

	QStringList dir= m_path.split("/");
	savedirectory = dir[dir.size()-1];

}
 
Savegame::~Savegame()
{}


QString Savegame::getPath() const
{
    return m_path;
}
QString Savegame::getSaveDataTitle() const
{
    return savedataTitle;
}
QString Savegame::getSaveDirectory() const
{
	return savedirectory;
}
QString Savegame::getSaveDataDetail() const
{
	return savedataDetail;
}
QString Savegame::getName() const
{
    return appTitle;
}


QPixmap Savegame::getIcon() const
{
    return m_icon;
}


void Savegame::getSavegameName(QString path)
{
    QDir dir(path);
    QFileInfoList files;
    files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    for (int i = 0; i < files.size(); ++i) 
    {
        if (files.at(i).fileName().toLower() == "param.sfo")
        {
            QFile file(files.at(i).filePath());
            if(!(file.open(QIODevice::ReadOnly)))
            {
                //throw QPSPManagerPBPException();
            }

            QDataStream stream(&file);
            char *read_buffer = new char[file.size()];
            if (stream.readRawData(read_buffer, file.size()) != file.size())
            {
                //throw QPSPManagerPBPException();
            }
            SFOHeader *header = (SFOHeader *)read_buffer;
            SFODir *entries = (SFODir *)(read_buffer+sizeof(SFOHeader));
  

            //qDebug() << "File size:" << file.size();
    		//header->fields_table_offs = Endian::le32(header->fields_table_offs);
			//header->values_table_offs = Endian::le32(header->values_table_offs);
            //header->nitems = Endian::le32(header->nitems);

            for (int i = 0; i < header->nitems; i++)
            {
    			//entries[i].field_offs = Endian::le16(entries[i].field_offs);
                if (strcmp(read_buffer+header->fields_table_offs+entries[i].field_offs, "TITLE") == 0)
                {
                	//entries[i].size = Endian::le32(entries[i].size);
        	  		//entries[i].val_offs = Endian::le16(entries[i].val_offs);
                    char *title=new char[entries[i].size];
                    strncpy(title, read_buffer + header->values_table_offs+entries[i].val_offs, entries[i].size);
                    appTitle = QString::fromUtf8(title);
                }
				if (strcmp(read_buffer+header->fields_table_offs+entries[i].field_offs, "SAVEDATA_TITLE") == 0)
				{
                    char *savetitle=new char[entries[i].size];
                    strncpy(savetitle, read_buffer + header->values_table_offs+entries[i].val_offs, entries[i].size);
                    savedataTitle = QString::fromUtf8(savetitle);
                }
				if (strcmp(read_buffer+header->fields_table_offs+entries[i].field_offs, "SAVEDATA_DETAIL") == 0)
				{
                    char *savetitle=new char[entries[i].size];
                    strncpy(savetitle, read_buffer + header->values_table_offs+entries[i].val_offs, entries[i].size);
                    savedataDetail = QString::fromUtf8(savetitle);
                }
            }

            file.close();
        }
    }
    
//    throw QPSPManagerPBPException();
}


QPixmap Savegame::getSavegameIcon(QString path)
{
    QDir dir(path);
    QFileInfoList files;
    files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    for (int i = 0; i < files.size(); ++i) 
    {
        if (files.at(i).fileName().toLower() == "icon0.png")
        {
            QPixmap icon;
            if (icon.load(files.at(i).filePath()))
            {
                return icon;
            }
            else
            {
				return QPixmap(":/pspudb/images/icon0.png");
            }
        }
    }
	return QPixmap(":/pspudb/images/icon0.png");
    
}
