

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
