
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
