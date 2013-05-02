#include "files.h"
#include <QFileInfo>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QDebug>

files::files(QString path)
{
	m_isValid=false;
	if (!isValidFile(path))
	{
		return;
	}
	m_isValid=true;
	m_path=path;
    QStringList dir= m_path.split("/");
	m_filename=dir[dir.size()-1];
	if(m_type==pbp)
	{
		getDemoName(m_path);
        m_icon = getDemoIcon(m_path);
	}
	else //type is elf
	{
		m_icon = QPixmap(":/pspudb/images/icon0.png");
	}
}


files::~files(void)
{
}
//Returns true if the file is a valid  file
bool files::isValidFile(QString file)
{
    //First we check this is a file
    QFileInfo fi(file);
    if (!fi.isFile())
    {
        return false;
    }
    
    //We have a file, we check the signature to know if it's a PBP file or not
    if (isPBPSignature(file))
    {
		m_type=pbp;
        return true;
    }
	if(isElfSignature(file))
	{
		m_type=elf;
		return true;
	}
    
    return false;
}
//Return true if the signature of the file is the one from a PBP file
bool files::isPBPSignature(QString file)
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
        //Signature
        char signature[4];
        inStream.readRawData((char *)(signature), sizeof(signature));

        if ((signature[0] != 0x00) || (signature[1] != 0x50) || (signature[2] != 0x42) || (signature[3] != 0x50))
        {
				isPBPFile = false; //Not a PBP file or elf file
			
        }
    
        inFile.close();
    }
    
    return isPBPFile;
}
//Return true if the signature of the file is the one from a elf file
bool files::isElfSignature(QString file)
{
    bool isElfFile = true;
    QFile inFile(file);
    
    if (!(inFile.open(QIODevice::ReadOnly)))
    {
        isElfFile = false; //Cannot open file for reading
    }
    else
    {
        QDataStream inStream(&inFile);
        //Signature
        char signature[4];
        inStream.readRawData((char *)(signature), sizeof(signature));

			if ((signature[0] != 0x7F) || (signature[1] != 0x45) || (signature[2] != 0x4C) || (signature[3] != 0x46))//check if it an elf
			{
				isElfFile = false; //Not a  elf file
			}
    
        inFile.close();
    }
    
    return isElfFile;
}
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

QPixmap files::getDemoIcon(QString filename)
{
    QFile inFile(filename);
    
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
void files::getDemoName(QString filename)
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
            demosTitle = QString::fromUtf8(title);
        }
	    if (strcmp(buffer+header->fields_table_offs+entries[i].field_offs, "DISC_ID") == 0)
        {
            char *title=new char[entries[i].size];
            strncpy(title, buffer + header->values_table_offs+entries[i].val_offs, entries[i].size);
            demosID = QString::fromUtf8(title);
        }
		 if (strcmp(buffer+header->fields_table_offs+entries[i].field_offs, "PSP_SYSTEM_VER") == 0)
        {
            char *title=new char[entries[i].size];
            strncpy(title, buffer + header->values_table_offs+entries[i].val_offs, entries[i].size);
            demosFW = QString::fromUtf8(title);
        }
    }

    file.close();
}

bool files::isValid() const
{
	return m_isValid;
}
QPixmap files::getIcon() const
{
	return m_icon;
}
QString files::getFileName() const
{
	return m_filename;
}
QString files::getDemosTitle() const
{
	return demosTitle;
}
QString files::getDemosID() const
{
	return demosID;
}
QString files::getDemosFW() const
{
	return demosFW; 
}
QString files::getPath() const
{
	return m_path;
}