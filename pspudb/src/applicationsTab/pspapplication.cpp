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
 
#include "pspapplication.h"
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


//WARNING: if we use dinamic memory in the future be aware that
//this constructor throws an exception
 PSPApplication::PSPApplication(QString path)
 {
     //Initial checks. We have to make sure the file is a PBP file
     if (!isPBPFile(path))
     {
         //throw QPSPManagerPBPException();
     }
     
     //Now we have to know if the PBP is a single file or it's a multiple one
     if (isSinglePBP(path))
     {
         m_type = Single;
         m_path = path;
		 //Now we get the rest of the info: icon and name
         getAppName(m_path);
         m_icon = getAppIcon(m_path);
     }
     else
     {
         m_type = Multiple;
         m_path = getMultiplePath(path);
		 getAppName(path);
         m_icon = getAppIcon(path);
     }
     QStringList dir= m_path.split("/");
	 applicationdirectory = dir[dir.size()-2];

	 gamedirectory = dir[dir.size()-3];

     
     //qDebug() << "Application created:" << m_path << m_name;
     //qDebug() << m_icon.width() << m_icon.height();
 }
 
 PSPApplication::~PSPApplication()
 {}
 
PSPApplication::ApplicationType PSPApplication::getType() const
{
    return m_type;
}
QString PSPApplication::getApplicationdirectory() const
{
	return applicationdirectory;
}
QString PSPApplication::getGamedirectory() const
{
	return gamedirectory;
}

QString PSPApplication::getPath() const
{
    return m_path;
}


QString PSPApplication::getAppTitle() const
{
    return appTitle;
}


QPixmap PSPApplication::getIcon() const
{
    return m_icon;
}
QString PSPApplication::getAppID() const
{
	return appID;
}
QString PSPApplication::getAppFW() const 
{
	return appFW;
}

//Returns true if the file is a valid PBP file
bool PSPApplication::isPBPFile(QString file)
{
    //First we check this is a file
    QFileInfo fi(file);
    if (!fi.isFile())
    {
        return false;
    }
    
    //We have a file, we check the signature to know if it's a PBP file or not
    if (!isPBPSignature(file))
    {
        return false;
    }
    
    return true;
}

//Return true if the signature of the file is the one from a PBP file
bool PSPApplication::isPBPSignature(QString file)
{
    bool isPBPFile = true;
    QFile inFile(file);
    
    if (!(inFile.open(QIODevice::ReadOnly)))
    {
        isPBPFile = false; //Cannot open file for reading
    }
    else
    {
        QDataStream inStream(&inFile);
      //  static const u32 m_pbpMagic = 0x50425000;
    //static const u32 m_elfMagic = 0x464C457F;
        //Signature
        char signature[4];
        inStream.readRawData((char *)(signature), sizeof(signature));

        if ((signature[0] != 0x00) || (signature[1] != 0x50) || (signature[2] != 0x42) || (signature[3] != 0x50))
        {
			if ((signature[0] != 0x7F) || (signature[1] != 0x45) || (signature[2] != 0x4C) || (signature[3] != 0x46))//check if it an elf
			{
				isPBPFile = false; //Not a PBP file or elf file
			}
        }
    
        inFile.close();
    }
    
    return isPBPFile;
}


//Returns true if the file is from a single PBP
//Returns false if it's from a multiple PBP
bool PSPApplication::isSinglePBP(QString file)
{
    //We simply do a check using file structure. If the structure is fucked up
    //this will not work. We try to find both the SCE and old plain % directory
    //structure. If another kind of structure is used, bad luck! Feel free to
    //program it yourself ;)
    QDir dir(file);
    
    if (!dir.cdUp()) //the file has not a "parent" so it has to be a single pbp
    {
        return true;
    }
    
    if (isSCEMultiple(dir) || isPercentMultiple(dir))
    {
        return false;
    }
    
    //We could do some more checking here like finding if both files
    //have the same application name. Maybe in the future
    return true;
}


bool PSPApplication::isSCEMultiple(QDir dir)
{
    //Get the dir name of the pbp container
	QString dirName = dir.dirName();//dir.absolutePath();
    
    //Set the name of the other dir
    QString otherName;
    if (dirName.startsWith("%__SCE__"))
    {
        otherName = dirName.right(dirName.length() -1);
    }
    else if (dirName.startsWith("__SCE__"))
    {
        otherName = dirName;
        otherName.append("%");
    }
    else
    {
        return false;
    }
    
    //Now we have to check that both the otherName dir exists
    //and it has a valid PBP file
	dir.cdUp();
	QDir otherDir(dir.absolutePath() +"/"+ otherName);
    if (!otherDir.exists())
    {
        return false;
    }
    
    //Find the eboot.pbp file inside the dir
    QFileInfoList files;
    files = otherDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    for (int i = 0; i < files.size(); ++i) 
    {
        if (files.at(i).fileName().toLower() == "eboot.pbp")
        {
            if (isPBPSignature(files.at(i).filePath()))
            {
                return true; //We have the correct directory and contains a correct pbp file
            }
        }
    }
    
    return false;
}


bool PSPApplication::isPercentMultiple(QDir dir)
{
    //Get the dir name of the pbp container
    QString dirName = dir.dirName();//dir.absolutePath();
    
    //Set the name of the other dir
    QString otherName = dirName;
    if (dirName.endsWith("%"))
    {
        otherName.resize(otherName.length() -  1);
    }
    else
    {
        otherName.append("%");
    }
    
    
    //Now we have to check that both the otherName dir exists
    //and it has a valid PBP file
	dir.cdUp();
    QDir otherDir(dir.absolutePath() +"/"+ otherName);
    if (!otherDir.exists())
    {
        return false;
    }
    
    //Find the eboot.pbp file inside the dir
    QFileInfoList files;
    files = otherDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    for (int i = 0; i < files.size(); ++i) 
    {
        if (files.at(i).fileName().toLower() == "eboot.pbp")
        {
            if (isPBPSignature(files.at(i).filePath()))
            {
                return true; //We have the correct directory and contains a correct pbp file
            }
        }
    }
    
    return false;
}


//Given a file path, gets the pbp path if this one is not the one we want
QString PSPApplication::getMultiplePath(QString path)
{
    //We want the non stripped directory
    QDir dir(path);
    
    if (!dir.cdUp())
    {
//        throw QPSPManagerPBPException();
    }
    
    QString destDirectory = dir.dirName();//dir.absolutePath();//dir.absolutePath();
    dir.cdUp();
    if (destDirectory.startsWith("%__SCE__")) //We need to remove the initial %
    {
        destDirectory = /*dir.absolutePath() +"/"+ */destDirectory.right(destDirectory.length() - 1);//dir.absolutePath().right(dir.absolutePath().length() - 1);
    }
    
    if (destDirectory.endsWith("%")) //We need to remove the final %
    {
        destDirectory = /*dir.absolutePath() +"/"+*/ destDirectory.left(destDirectory.length() - 1);//dir.absolutePath().left(dir.absolutePath().length() - 1);
	}
    
    //We have the same dir as entered because all other possibilities
    //under the precondition that this is a multiple PBP file, are pointing
    //to the directory we want.
    //We need to add the PBP
    //Find the eboot.pbp file inside the dir
    QFileInfoList files;
    QDir targetDir(dir.absolutePath() +"/"+destDirectory);
    files = targetDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    for (int i = 0; i < files.size(); ++i) 
    {
        if (files.at(i).fileName().toLower() == "eboot.pbp")
        {
            destDirectory = dir.absolutePath() +"/"+ destDirectory + "/" + files.at(i).fileName();
            return destDirectory;
        }
    }
    
    //We should never reach here, if we do, we're in trouble :)
    return "";
}


void PSPApplication::getAppName(QString filename)
{
    QFile file(filename);
    
    if(!(file.open(QIODevice::ReadOnly)))
    {
//        throw QPSPManagerPBPException();
    }
    
    QDataStream stream(&file);

    quint8 signature[4];
    //Signature
    for (int i = 0; i < 4; i++)
    {
        stream >> signature[i];
    }

    //Version
    quint32 version;
    stream >> version;

    //Files offset
    unsigned long offset[8];
    stream.readRawData((char *)(offset), sizeof(offset));


    //PARAM.SFO
    int paramSFOSize = offset[1] - offset[0];
    char *buffer=new char[paramSFOSize];
    
    if (stream.readRawData(buffer, paramSFOSize) != paramSFOSize)
    {
 //       throw QPSPManagerPBPException();
    }

    //We have the SFO file in the buffer, now we have to
    //seek the title field on it
    //TODO: document the SFO file format ffs!
    SFOHeader *header = (SFOHeader *)buffer;
    SFODir *entries = (SFODir *)(buffer+sizeof(SFOHeader));
    
    
    for (int i = 0; i < header->nitems; i++)
    {
        if (strcmp(buffer+header->fields_table_offs+entries[i].field_offs, "TITLE") == 0)
        {
            char *title=new char[entries[i].size];
            strncpy(title, buffer + header->values_table_offs+entries[i].val_offs, entries[i].size);
            appTitle = QString::fromUtf8(title);
        }
	    if (strcmp(buffer+header->fields_table_offs+entries[i].field_offs, "DISC_ID") == 0)
        {
            char *title=new char[entries[i].size];
            strncpy(title, buffer + header->values_table_offs+entries[i].val_offs, entries[i].size);
            appID = QString::fromUtf8(title);
        }
		 if (strcmp(buffer+header->fields_table_offs+entries[i].field_offs, "PSP_SYSTEM_VER") == 0)
        {
            char *title=new char[entries[i].size];
            strncpy(title, buffer + header->values_table_offs+entries[i].val_offs, entries[i].size);
            appFW = QString::fromUtf8(title);
        }
    }

    file.close();
}


QPixmap PSPApplication::getAppIcon(QString file)
{
    QFile inFile(file);
    
    if (!(inFile.open(QIODevice::ReadOnly)))
    {
//        throw QPSPManagerPBPException();
    }
    
    QDataStream inStream( &inFile );
    
    //Signature
    char signature[4];
    inStream.readRawData((char *)(signature), sizeof(signature));

    if ((signature[0] != 0x00) || (signature[1] != 0x50) || (signature[2] != 0x42) || (signature[3] != 0x50))
    {
 //       throw QPSPManagerPBPException();
    }

    //Version
    unsigned int version;
    inStream >> version;

    //Files offset
    unsigned long offset[8];
    inStream.readRawData((char *)(offset), sizeof(offset));

    //We ned to know where the icon starts and its size
    int iconSize = offset[2] - offset[1];
    int iconOffset = offset[1] - offset[0];
    char *buffer=new char[iconSize];
    if (inStream.skipRawData(iconOffset) != iconOffset)
    {
  //      throw QPSPManagerPBPException();
    }
    
    if (inStream.readRawData(buffer, iconSize) != iconSize)
    {
  //      throw QPSPManagerPBPException();
    }

    
    //Now we create the icon
    QPixmap result;
    //TODO: why the fuck this needs uchar and not char?
    QByteArray pngArray(buffer, iconSize);
    if (!result.loadFromData(pngArray))
    {
        //if not able to load an icon then load a default one
		inFile.close();
		return QPixmap(":/pspudb/images/icon0.png");
    }
    
    //Clean up and return
    inFile.close();

    return result;
}

