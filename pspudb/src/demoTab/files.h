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

