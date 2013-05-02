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

#ifndef DATABASECHECKS_H
#define DATABASECHECKS_H

#include <qhash.h>
#include <QtSql>

static void fixupdata()
{
	 QList<QString> discidlist;
	  //find all entries that has tha same id
	  QSqlQuery query5(dbadv);
      query5.prepare("SELECT discid FROM images WHERE renrelease!=1");
	  query5.exec();
	  while(query5.next())
	  {
		  QString discid = query5.value(0).toString();
		  if(!discidlist.contains(discid))
			  discidlist.append(discid); 
	  }
	  for (int i = 0; i < discidlist.size(); ++i) 
	  {
		  QSqlQuery query6(dbadv);
		  query6.prepare("SELECT id,isonumber FROM images WHERE discid=? AND renrelease!=1");
		  query6.addBindValue(discidlist.at(i));
		  query6.exec();
		  int num=2;//first number is 2 in non advancescene
		  while(query6.next())
		  {
			  int id = query6.value(0).toUInt();
			  int isonumber= query6.value(1).toUInt();
			  if(isonumber!=0) // there is already a number skip it
			  {
				  num++;
			  }
			  else
			  {
                  QSqlQuery query7(dbadv);
				  query7.prepare("UPDATE images set isonumber=? WHERE id=?");
				  query7.addBindValue(num);
				  query7.addBindValue(id);
				  query7.exec();
				  num++;
			  }
		  }
		  
	  }
}
static void loadudbdata()//load data from udb.ini the hacked version of the old pcsp-udb
{
      QSettings loadfromdatabase("udb.ini", QSettings::IniFormat);
      QStringList keys = loadfromdatabase.childGroups();
      QSqlQuery query(dbadv);
	  QSqlQuery query2(dbadv);
	  QSqlQuery query3(dbadv);
	  QSqlQuery query8(dbadv);
	  QFile file("importresults.txt");
      file.open(QIODevice::WriteOnly | QIODevice::Text);
      QTextStream out(&file);
	  int newgames=0;
	  int orphangames=0;
	  int alreadyexist=0;
	  int updatedgames=0;
      for (int i = 0; i < keys.size(); ++i)
	  {
		
       QString currectkey=keys.at(i);
	   loadfromdatabase.beginGroup(currectkey);
	   QString crc32      = loadfromdatabase.value("crc32").toString();
	   QString discid     = loadfromdatabase.value("discid").toString();
      // QString title      = loadfromdatabase.value("title").toString();
       QString firmware   = loadfromdatabase.value("firmware").toString();
       QString genre      = loadfromdatabase.value("genre").toString();
	   int filesize       = loadfromdatabase.value("gamesize").toUInt();
	   QString type       = loadfromdatabase.value("type").toString();
       QString status     = loadfromdatabase.value("status").toString();
	   QString status2     = loadfromdatabase.value("status2").toString();
	   loadfromdatabase.endGroup();
	   out<<"--------------------------------------------------------------\n";
	   query.exec("SELECT * FROM isospsp where crc = '" + crc32 + "'"); //there is already an entry so we update
	   if(query.first())//entry exists
	   {
		   out<< "crc32: " + crc32 + " for discid: " + discid + " found\n";
		   query2.exec("SELECT * FROM isospsp where discid= '<UNKN>' AND genre= '<UNKN>' AND bootbinstatus= '<UNKN>' AND ebootbinstatus= '<UNKN>' AND crc = '" + crc32 + "'");
		   if(query2.first())
		   {
			   out<< "crc32: " + crc32 + " entry requires full update\n";
			   query3.exec("UPDATE isospsp SET discid='" + discid + "', firmware='" + firmware + "', genre ='" + genre + "' ,bootbinstatus='" + status + "' ,ebootbinstatus='" + status2 + "' where crc = '" + crc32 + "'");
			   out<<"--------------------------------------------------------------\n";
			   updatedgames++;
			   query8.exec("UPDATE isospsp SET isonumber=1 where crc = '" + crc32 + "'");

		   }
		   else
		   {
			   out<<"No full update for crc32: " + crc32+"\n";
			   out<<"--------------------------------------------------------------\n";
			   alreadyexist++;
		   }
	   }
	   else //no entry , requires a full update
	   {
	      out<<"New entry for crc32: " + crc32+"\n";
          QSqlQuery query4(dbadv);
		  int gameid=0;
		  //check first if there is already an entry with the same discid from advanscene so we can update gameid
		  query4.prepare("SELECT gameid FROM advpsp,isospsp WHERE advpsp.id = isospsp.gameid AND discid=?");
		  query4.addBindValue(discid);
		  query4.exec();
		  while(query4.next())
		  {
			  gameid=query4.value(0).toUInt();
			  out<<"gameid found: " + QString::number(gameid) + " for discid " + discid +"\n";
			  newgames++;
		  }
		  if(gameid==0)//we haven't found an entry
		  {
              out<<"no gameid found for "+discid+"\n";
			  orphangames++;
		  }
          QSqlQuery query5(dbadv);
		  query5.prepare("INSERT INTO isospsp (crc,isotype,grouprelease,gameid,firmware,discid,genre,filesize,bootbinstatus,ebootbinstatus,advrelease )"
              "VALUES (?, ?, ?,?,?,?,?,?,?,?,?)");
			 query5.addBindValue(crc32);
             query5.addBindValue(type);
			 query5.addBindValue("<UNKN>");
			 query5.addBindValue(gameid);
			 query5.addBindValue(firmware);
			 query5.addBindValue(discid);
			 query5.addBindValue(genre);
			 query5.addBindValue(filesize);
			 query5.addBindValue(status);
			 query5.addBindValue(status2);
			 query5.addBindValue(0);
		     query5.exec();
			 out<<"Entry successfully added\n";
			 out<<"--------------------------------------------------------------\n";


	   }
	  }
	  out<<"New games succesfully added        : "+QString::number(newgames)+"\n";
	  out<<"New orphan games succesfully added : "+QString::number(orphangames)+"\n";
	  out<<"Updated games                      : "+QString::number(updatedgames)+"\n";
	  out<<"Non-updated games(already exist)   : "+QString::number(alreadyexist)+"\n";
	  out<<"--------------------------------------------------------------\n";
	  out<<"Step2: Try to fix some orphan games\n";
	  QList<QString> discidlist;
	  //find all entries that has gameid=0
	  QSqlQuery query5(dbadv);
      query5.prepare("SELECT discid FROM isospsp WHERE gameid=0");
	  query5.exec();
	  int orphgames=0;
	  while(query5.next())
	  {
		  discidlist.append(query5.value(0).toString());
		  out<<"Added discid = " + query5.value(0).toString() + " to list\n";
		  orphgames++;
	  }
	  out<<"--------------------------------------------------------------\n";	  
	  out<<"Added " + QString::number(orphgames) + "\n";
      int fixupgames=0;
	  for (int i = 0; i < discidlist.size(); ++i) 
	  {
         QSqlQuery query6(dbadv);
		 query6.prepare("SELECT * FROM isospsp WHERE gameid!=0 AND advrelease=1 AND discid=?");
		 query6.addBindValue(discidlist.at(i));
		 query6.exec();
		 if(query6.first())
		 {
            out<<"Trying to fixup entry for discid " + discidlist.at(i) + "\n";
			int gameid = query6.value(4).toUInt();
			QSqlQuery query7(dbadv);
		    query7.prepare("UPDATE isospsp SET gameid = ? where discid =? AND advrelease!=1");
			query7.addBindValue(gameid);
		    query7.addBindValue(discidlist.at(i));
			query7.exec();
            fixupgames++;
		 }
	  }
	  out<<"Fixup " + QString::number(fixupgames) + " games\n";
      fixupdata();//to fix isonumbering

	  
}

static void checkduplicateCRC()
{
	  QFile file("logs/duplicate.txt");
      file.open(QIODevice::WriteOnly | QIODevice::Text);
      QTextStream out(&file);
	  QList<QString> crclist;
	  //find all entries that has tha same id
	  QSqlQuery query5(dbadv);
      query5.prepare("SELECT crc FROM isospsp");
	  query5.exec();
	  while(query5.next())
	  {
		  QString crc = query5.value(0).toString();
		  if(crclist.contains(crc))
		  {
			  out<<"crc already on list " << crc << "\n";
		  }
		  else
		  {
			  crclist.append(crc);
		  }
	  }
}
//fix FW . if one discid has fw entry then find all others and add the firmware there as well
static void fixupFW()
{
	  QFile file("logs/fixupfw.txt");
      file.open(QIODevice::WriteOnly | QIODevice::Text);
      QTextStream out(&file);

	  QHash<QString, QString> hash;

	  //find all entries that has a discid and FW as well
	  QSqlQuery query5(dbadv);
      query5.prepare("SELECT discid,firmware FROM isospsp WHERE firmware !=? AND discid!=?");
	  query5.addBindValue("<UNKN>"); 
	  query5.addBindValue("<UNKN>");
	  query5.exec();
	  while(query5.next())
	  {
		  QString discid = query5.value(0).toString();
		  QString fw	 = query5.value(1).toString();
		  if(hash.contains(discid))
		  {
			  out<<"discid already on list " << discid << " with fw = " << fw << "\n";
		  }
		  else
		  {
			  hash[discid] = fw;
		  }
	  }
	   out<<"--------------------------------------------------------------------------" << "\n";

	  //find now all entries that has a discid but not firmware
	  QSqlQuery query6(dbadv);
      query6.prepare("SELECT discid,firmware FROM isospsp WHERE firmware =? AND discid!=?");
	  query6.addBindValue("<UNKN>"); 
	  query6.addBindValue("<UNKN>");
	  query6.exec();
	  while(query6.next())
	  {
		  QString discid = query6.value(0).toString();
		  QString fw	 = query6.value(1).toString();
		  if(hash.contains(discid)) // there is an entry we can fixup it
		  {
			  //double check 
			  if(!fw.contains("<UNKN>"))
			  {
				   out<<"FW is not unkn!! ERROR" << "\n";
			  }
			  else
			  {
				  QString newfw = hash.value(discid);
				  QSqlQuery query7(dbadv);
				  query7.prepare("UPDATE isospsp set firmware=? WHERE discid=? AND firmware = ? ");
				  query7.addBindValue(newfw);
				  query7.addBindValue(discid);
				  query7.addBindValue("<unkn>"); 
				  query7.exec();
				  out<<"FW  = " << newfw << " added to discid = " << discid << "\n";
			  }
		  }
		  else
		  {
			  out<<"There isn't known FW for the current discid = " << discid << "\n";
		  }
	  }

}
static void fixupbootbinstatus(QString inifile)
{
	  QSettings loadfromdatabase(inifile, QSettings::IniFormat);
      QStringList keys = loadfromdatabase.childGroups();
	  QFile file("logs/fixupbootbin.txt");
      file.open(QIODevice::WriteOnly | QIODevice::Text);
      QTextStream out(&file);
	  QSqlQuery query(dbadv);
	  QSqlQuery query2(dbadv);
	  QSqlQuery query3(dbadv);
	  QSqlQuery query4(dbadv);
	  for (int i = 0; i < keys.size(); ++i)
	  {
		   QString currectkey=keys.at(i);
		   loadfromdatabase.beginGroup(currectkey);
		   QString crc32       = loadfromdatabase.value("crc32").toString();
		   QString status      = loadfromdatabase.value("status").toString();
		   QString status2     = loadfromdatabase.value("status2").toString();
		   loadfromdatabase.endGroup();
		   query.exec("SELECT * FROM isospsp where crc = '" + crc32 + "' AND bootbinstatus= '<UNKN>'"); //there is already an entry so we update
		   if(query.first())//entry exists
		   {
			   query2.exec("UPDATE isospsp SET bootbinstatus='" + status + "' where crc = '" + crc32 + "'");
			   out << "bootbinstatus updated for CRC32 " << crc32 << "\n";
		   }
		   query3.exec("SELECT * FROM isospsp where crc = '" + crc32 + "' AND ebootbinstatus= '<UNKN>'"); //there is already an entry so we update
		   if(query.first())//entry exists
		   {
			   query4.exec("UPDATE isospsp SET ebootbinstatus='" + status2 + "' where crc = '" + crc32 + "'");
			   out << "ebootbinstatus updated for CRC32 " << crc32 << "\n";
		   }
		}

}
static void fixupgenre(QString inifile)
{
	  QSettings loadfromdatabase(inifile, QSettings::IniFormat);
      QStringList keys = loadfromdatabase.childGroups();
	  QFile file("logs/fixupgenre.txt");
      file.open(QIODevice::WriteOnly | QIODevice::Text);
      QTextStream out(&file);
	  QSqlQuery query(dbadv);
	  QSqlQuery query2(dbadv);
	  QSqlQuery query3(dbadv);
	  QSqlQuery query4(dbadv);
      for (int i = 0; i < keys.size(); ++i)
	  {
		   QString currectkey=keys.at(i);
		   loadfromdatabase.beginGroup(currectkey);
		   QString crc32      = loadfromdatabase.value("crc32").toString();
		   QString genre      = loadfromdatabase.value("genre").toString();
		   QString discid     = loadfromdatabase.value("discid").toString();
		   loadfromdatabase.endGroup();
		   query.exec("SELECT * FROM isospsp where crc = '" + crc32 + "' AND genre= '<UNKN>'"); //there is already an entry so we update
		   if(query.first())//entry exists
		   {
		       if(genre.contains("'")) // mysql requires a double quote if a quote found. Else it fails
			     genre.replace("'","''");
			   query2.exec("UPDATE isospsp SET genre='" + genre + "' where crc = '" + crc32 + "'");
			   out << "genre updated for CRC32 " << crc32 << "\n";
		   }
		   else // try with discid and genre compination
		   {
			   query3.exec("SELECT * FROM isospsp where discid = '" + discid + "' AND genre= '<UNKN>'"); //there is already an entry so we update
			   if(query3.first())
			   {
				   if(genre.contains("'")) // mysql requires a double quote if a quote found. Else it fails
					 genre.replace("'","''");
				   query3.exec("UPDATE isospsp SET genre='" + genre + "' where discid = '" + discid + "'");
				   out << "genre updated for discid " << discid << "\n";
			   }
		   }
	  }
}
static void fixup_decription_multi(QString inifile)
{
	  QSettings loadfromdatabase(inifile, QSettings::IniFormat);
      QStringList keys = loadfromdatabase.childGroups();
	  QFile file("logs/fixupdescriptionmulti.txt");
      file.open(QIODevice::WriteOnly | QIODevice::Text);
      QTextStream out(&file);
	  QSqlQuery query(dbadv);
	  QSqlQuery query2(dbadv);
	  QSqlQuery query3(dbadv);
	  QSqlQuery query4(dbadv);
      for (int i = 0; i < keys.size(); ++i)
	  {
		   QString currectkey=keys.at(i);
		   loadfromdatabase.beginGroup(currectkey);
		   QString crc32      = loadfromdatabase.value("crc32").toString();
		   QString discid     = loadfromdatabase.value("discid").toString();
		   QString multiplayer = loadfromdatabase.value("multiplayer").toString();
		   QString description = loadfromdatabase.value("description").toString();
		   loadfromdatabase.endGroup();
		   query.exec("SELECT multiplayer,description,advpsp.id,discid,crc FROM advpsp,isospsp WHERE gameid = advpsp.id AND multiplayer='<unkn>' AND description='<unkn>' AND crc= '" + crc32 + "'");
		   if(query.first())//entry exists
		   {
		      int id = query.value(2).toUInt();
			  query2.prepare("UPDATE advpsp SET multiplayer=? where id = ?");
			  query2.addBindValue(multiplayer);
			  query2.addBindValue(id);
			  if(!query2.exec())
			  {
			    out <<" ERROR executing query multiplater for id " << id << "\n";
			  }
			  query3.prepare("UPDATE advpsp SET description=? where id = ?");
			  query3.addBindValue(description);
			  query3.addBindValue(id);
			  if(!query3.exec())
			  {
			    out <<" ERROR executing query description for id " << id << "\n";
			  }
		   }
	 }
}

#endif
