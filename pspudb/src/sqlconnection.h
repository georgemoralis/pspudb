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

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

QSqlDatabase dbadv;
QSqlDatabase dbcollect;
static bool createConnection()
{
    dbadv = QSqlDatabase::addDatabase("QSQLITE","advscene");
    dbadv.setDatabaseName("data/database.db");
    if (!dbadv.open()) {
        ///QMessageBox::critical(0, qApp->tr("Cannot open database"),
        //    qApp->tr("Unable to establish a database connection.\n"), QMessageBox::Cancel);
        return false;
    }
	//QSqlQuery query(dbadv);
	/*query.exec("create table if not exists advpsp (id INTEGER PRIMARY KEY, "
               "imageNumber int,"
			   "releaseNumber int,"
			   "title varchar(100),"
			   "type varchar(10),"
			   "publisher varchar(30),"
			   "region varchar(15),"
			   "language varchar(256),"
			   "im1CRC varchar(20),"
			   "im2CRC varchar(20),"
			   "duplicateID int)");

	query.exec("create table if not exists isospsp (id INTEGER PRIMARY KEY, "
		"crc varchar(20),"
		"isotype varchar(5),"
		"grouprelease varchar(50),"
		"gameid int,"
		"firmware varchar(5),"
		"discid varchar(10),"
		"genre varchar(40),"
		"filesize int,"
		"bootbinstatus varchar(100), "
		"ebootbinstatus varchar(100), "
		"advrelease bool DEFAULT 0,"
		"sameID int DEFAULT 0,"
		"isonumber int DEFAULT 0)");*/

	dbcollect = QSqlDatabase::addDatabase("QSQLITE","collection");
    dbcollect.setDatabaseName("userdata.db");
    if (!dbcollect.open()) {
        ///QMessageBox::critical(0, qApp->tr("Cannot open database"),
        //    qApp->tr("Unable to establish a database connection.\n"), QMessageBox::Cancel);
        return false;
    }

		//create cache table
	QSqlQuery query2(dbcollect);
	query2.exec("create table if not exists cache (id INTEGER PRIMARY KEY, "
		       "path varchar(256),"
			   "lastmodified int ,"
			   "filesize int,"
			   "crc32 varchar(40),"
			   "available bool DEFAULT 0)");

	return true;
}

