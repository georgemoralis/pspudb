#include "WorkerThread.h"
#include "ext\zlib\include\zlib.h"
#include "ext\liblzma\include\lzma.h"
#include "umdloader\types.h"
#include "umdloader\umdimageloader.h"
#include "umdloader\isodrv.h"
#include "umdloader\isolib.h"
#include "umdloader\iso.h"
#include "umdloader\psf.h"


#include <QTime>
#include <QWidget>
#include <QDir>
#include <QMetaType>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <qsettings.h>

extern QSqlDatabase dbcollect;
extern QSqlDatabase dbadv;


#include "databasetools\databaseChecks.h"

extern int umdloader_imagetype;  //from imageloader

WorkerThread::WorkerThread( QObject* parent )
	: QThread( parent )
{
}

WorkerThread::~WorkerThread()
{
	if ( isRunning() ) {
		qWarning() << "Waiting thread to finish...";
		stop();
		wait();
	}
}

WorkerThread::Task WorkerThread::task() const
{
	QMutexLocker locker( &const_cast<WorkerThread*>( this )->mMutex );
	return mWork.task;
}

bool WorkerThread::setWork( const WorkerThread::Work& work )
{
	if ( isRunning() ) {
		Q_ASSERT( 0 );
		return false;
	}
	
	mWork = work;
	mWork.window->setWindowTitle( taskToWindowTitle( mWork.task ) );
	
	start();
	
	return true;
}

QString WorkerThread::taskToWindowTitle( WorkerThread::Task task)
{
	return taskToLabel( task).append( "..." );
}

QString WorkerThread::taskToLabel( WorkerThread::Task task)
{
	switch ( task ) {
		case WorkerThread::ConvertToCSO:
			return tr( "Converting to CSO" );
		case WorkerThread::ConvertCsoToIso:
			return tr( "Converting CSO to ISO" );
		case WorkerThread::ConvertIsoToCs7:
			return tr( "Converting ISO to 7SO" );
		case WorkerThread::ConvertCs7ToIso:
			return tr( "Converting 7SO to ISO" );
		case WorkerThread::ScanUMD:
			return tr( "Scanning UMDs");
		case WorkerThread::FixupStatus:
			return tr( "Fixing up e/boot.bin database status");
		case WorkerThread::FixupImages:
			return tr( "Fixing up/adding image in database");
		case WorkerThread::FixupFW:
			return tr( "Fixing up FW database entries");
	}
	
	return QString::null;
}

void WorkerThread::stop()
{
	QMutexLocker locker( &mMutex );
	mStop = true;
	emit canceled();
}

void WorkerThread::run()
{
	WorkerThread::Work work;
	int count = 0;
	int id = 0;
	
	{
		QMutexLocker locker( &mMutex );
		mStop = false;
		work = mWork;
	}
	
	emit globalProgressChanged( id, count );
	
	count = work.files.size();//get the number of files
	
	emit globalProgressChanged( id, count );

	foreach (QString file, work.files)
	{
		switch ( work.task ) 
		{
			case WorkerThread::ConvertToCSO:
				isoToCSO(work.task, file,work.compressLevel, work.target);
			   break;
			case WorkerThread::ConvertCsoToIso:
				csoToISO(work.task, file,work.target);
				break;
			case WorkerThread::ConvertIsoToCs7:
				isoToCS7(work.task,file,work.compressLevel,work.target);
				break;
			case WorkerThread::ConvertCs7ToIso:
				cs7ToISO(work.task, file,work.target);
				break;
			case WorkerThread::ScanUMD:
				scanUMD(work.task,file);
				break;
			case WorkerThread::FixupStatus:
				fixupStatus(work.task,file);
				break;
			case WorkerThread::FixupImages:
				fixupImages(work.task,file);
				break;
			case WorkerThread::FixupFW:
				fixupFW(work.task,file);
				break;

		}
		emit globalProgressChanged( ++id, count );
		emit jobFinished( file );
		
		{
			QMutexLocker locker( &mMutex );
			
			if ( mStop ) {
				break;
			}
		}
	}
	emit globalProgressChanged( count, count );
}
void WorkerThread::isoToCSO(WorkerThread::Task task, QString filename,int compressLevel, const QString& target)
{
	FILE *fin,*fout;
    if ((fin = fopen(filename.toUtf8(), "rb")) == NULL)
    {
        return;
    }
	QFileInfo f(filename);
	QString outname = f.completeBaseName();
	QString fileout = target +"/"+ outname + ".cso";
    if ((fout = fopen(fileout.toUtf8(), "wb")) == NULL) 
    {
        return;
    }
	QFileInfo info(filename);
	emit message( QString( "%1 '%2'..." ).arg( taskToLabel( task ) ).arg( info.completeBaseName() + "." + info.suffix() ) );

    z_stream z;

    unsigned int *index_buf = NULL;
    unsigned int *crc_buf = NULL;
    unsigned char *block_buf1 = NULL;
    unsigned char *block_buf2 = NULL;

    long long file_size;
    unsigned long long write_pos;
    int index_size;
    int block;
    unsigned char buf4[64];
    int cmp_size;
    int status;
    int percent_period;
    int percent_cnt;
    int align,align_b,align_m;
    int ciso_total_block;
    CISO_H ciso;
    file_size = check_file_size(fin, ciso_total_block, ciso);
    if(file_size<0)
    {
        // free memory
        if(index_buf) free(index_buf);
        if(crc_buf) free(crc_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }

    // allocate index block
    index_size = (ciso_total_block + 1 ) * sizeof(unsigned long);
    index_buf  = (unsigned int *)malloc(index_size);
    crc_buf    = (unsigned int *)malloc(index_size);
    block_buf1 = (unsigned char *)malloc(ciso.block_size);
    block_buf2 = (unsigned char *)malloc(ciso.block_size*2);

    if( !index_buf || !crc_buf || !block_buf1 || !block_buf2 )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(crc_buf) free(crc_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);
        
        return;
    }
    memset(index_buf,0,index_size);
    memset(crc_buf,0,index_size);
    memset(buf4,0,sizeof(buf4));

    // init zlib
    z.zalloc = Z_NULL;
    z.zfree  = Z_NULL;
    z.opaque = Z_NULL;

    // write header block
    CISO_H beCiso = ciso;
    fwrite(&beCiso,1,sizeof(beCiso),fout);

    // dummy write index block
    fwrite(index_buf,1,index_size,fout);

    write_pos = sizeof(ciso) + index_size;

    // compress data
    percent_period = ciso_total_block/100;
    percent_cnt    = ciso_total_block/100;

	emit currentProgressChanged( 0, 100);
    align_b = 1<<(ciso.align);
    align_m = align_b -1;

    for(block = 0;block < ciso_total_block ; block++)
    {
        if(--percent_cnt<=0)
        {
            percent_cnt = percent_period;
			emit currentProgressChanged(block / percent_period, 100);

        }

        if (deflateInit2(&z, compressLevel, Z_DEFLATED, -15,8,Z_DEFAULT_STRATEGY) != Z_OK)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(crc_buf) free(crc_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);
            
            return;
        }

        // write align
        align = (int)write_pos & align_m;
        if(align)
        {
            align = align_b - align;
            if((int)fwrite(buf4,1,align, fout) != align)
            {
                 // free memory
                 if(index_buf) free(index_buf);
                 if(crc_buf) free(crc_buf);
                 if(block_buf1) free(block_buf1);
                 if(block_buf2) free(block_buf2);

                 return;
            }
            write_pos += align;
        }

        // mark offset index
        index_buf[block] = write_pos>>(ciso.align);

        // read buffer
        z.next_out  = block_buf2;
        z.avail_out = ciso.block_size*2;
        z.next_in   = block_buf1;
        z.avail_in  = fread(block_buf1, 1, ciso.block_size , fin);
        if(z.avail_in != ciso.block_size)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(crc_buf) free(crc_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }

        // compress block
        status = deflate(&z, Z_FINISH);
        if (status != Z_STREAM_END)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(crc_buf) free(crc_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }

        cmp_size = ciso.block_size*2 - z.avail_out;

        // choise plain / compress
        if(cmp_size >= (long)ciso.block_size)
        {
            cmp_size = ciso.block_size;
            memcpy(block_buf2,block_buf1,cmp_size);
            // plain block mark
            index_buf[block] |= 0x80000000;
        }

        // write compressed block
        if((int)fwrite(block_buf2, 1,cmp_size , fout) != cmp_size)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(crc_buf) free(crc_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }

        // mark next index
        write_pos += cmp_size;

        // term zlib
        if (deflateEnd(&z) != Z_OK)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(crc_buf) free(crc_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }
        //Flush! For testing
        fflush(fout);
    }
	emit currentProgressChanged( 100 , 100);
    // last position (total size)
    index_buf[block] = write_pos>>(ciso.align);

    // write header & index block
    fseek(fout,sizeof(ciso),SEEK_SET);
    fwrite(index_buf,1,index_size,fout);

    // free memory
    if(index_buf) free(index_buf);
    if(crc_buf) free(crc_buf);
    if(block_buf1) free(block_buf1);
    if(block_buf2) free(block_buf2);

    // close files
    fclose(fin);
    fclose(fout);
}
long long WorkerThread::check_file_size(FILE *fp, int &ciso_total_block, CISO_H &ciso)
{
  long long pos;

  if( fseek(fp,0,SEEK_END) < 0)
    return -1;
  pos = ftell(fp);
  if(pos==-1) return pos;

  // init ciso header
  memset(&ciso,0,sizeof(ciso));

  ciso.magic[0] = 'C';
  ciso.magic[1] = 'I';
  ciso.magic[2] = 'S';
  ciso.magic[3] = 'O';
  ciso.ver      = 0x01;

  ciso.block_size  = 0x800; /* ISO9660 one of sector */
  ciso.total_bytes = pos;
  ciso_total_block = pos / ciso.block_size ;

  fseek(fp,0,SEEK_SET);

  return pos;
}
void WorkerThread::csoToISO(WorkerThread::Task task, QString filename,const QString& target)
{
   	FILE *fin,*fout;
    if ((fin = fopen(filename.toUtf8(), "rb")) == NULL)
    {
        return;
    }
	QFileInfo f(filename);
	QString outname = f.completeBaseName();
	QString fileout = target +"/"+ outname + ".iso";
    if ((fout = fopen(fileout.toUtf8(), "wb")) == NULL) 
    {
        return;
    }
	QFileInfo info(filename);
	emit message( QString( "%1 '%2'..." ).arg( taskToLabel( task ) ).arg( info.completeBaseName() + "." + info.suffix() ) );

    z_stream z;

    unsigned int *index_buf = NULL;
    unsigned char *block_buf1 = NULL;
    unsigned char *block_buf2 = NULL;
    unsigned int index , index2;
    unsigned long long read_pos , read_size;
    int index_size;
    int block;
    int cmp_size;
    int status;
    int percent_period;
    int percent_cnt;
    int plain;

    // read header
    CISO_H ciso;
    if( fread(&ciso, 1, sizeof(ciso), fin) != sizeof(ciso) )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }
    // check header
    if(
        ciso.magic[0] != 'C' ||
        ciso.magic[1] != 'I' ||
        ciso.magic[2] != 'S' ||
        ciso.magic[3] != 'O' ||
        ciso.block_size ==0  ||
        ciso.total_bytes == 0
        )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }
  
    int ciso_total_block = ciso.total_bytes / ciso.block_size;

    // allocate index block
    index_size = (ciso_total_block + 1 ) * sizeof(unsigned long);
    index_buf  = (unsigned int *)malloc(index_size);
    block_buf1 = (unsigned char *)malloc(ciso.block_size);
    block_buf2 = (unsigned char *)malloc(ciso.block_size*2);

    if( !index_buf || !block_buf1 || !block_buf2 )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }
    memset(index_buf,0,index_size);

    // read index block
    if( (int)fread(index_buf, 1, index_size, fin) != index_size )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }

    // init zlib
    z.zalloc = Z_NULL;
    z.zfree  = Z_NULL;
    z.opaque = Z_NULL;

    // decompress data
    percent_period = ciso_total_block/100;
    percent_cnt = 0;
    emit currentProgressChanged( 0, 100);

    for(block = 0;block < ciso_total_block ; block++)
    {
        if(--percent_cnt<=0)
        {
            percent_cnt = percent_period;
            emit currentProgressChanged(block / percent_period, 100);
        }

        if (inflateInit2(&z,-15) != Z_OK)
        {
            //free memory
            if(index_buf) free(index_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }

        // check index
        index  = index_buf[block];
        plain  = index & 0x80000000;
        index  &= 0x7fffffff;
        read_pos = index << (ciso.align);
        if(plain)
        {
            read_size = ciso.block_size;
        }
        else
        {
            index2 = index_buf[block+1] & 0x7fffffff;
            read_size = (index2-index) << (ciso.align);
        }
        fseek(fin,read_pos,SEEK_SET);

        z.avail_in  = fread(block_buf2, 1, read_size , fin);
        if(z.avail_in != read_size)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }

        if(plain)
        {
            memcpy(block_buf1,block_buf2,read_size);
            cmp_size = read_size;
        }
        else
        {
            z.next_out  = block_buf1;
            z.avail_out = ciso.block_size;
            z.next_in   = block_buf2;
            status = inflate(&z, Z_FULL_FLUSH);
            if (status != Z_STREAM_END)
            {
                // free memory
                if(index_buf) free(index_buf);
                if(block_buf1) free(block_buf1);
                if(block_buf2) free(block_buf2);

                return;
            }
            cmp_size = ciso.block_size - z.avail_out;
            if(cmp_size != (long)ciso.block_size)
            {
                // free memory
                if(index_buf) free(index_buf);
                if(block_buf1) free(block_buf1);
                if(block_buf2) free(block_buf2);

                return;
            }
        }
        // write decompressed block
        if((int)fwrite(block_buf1, 1,cmp_size , fout) != cmp_size)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }

        // term zlib
        if (inflateEnd(&z) != Z_OK)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }
        //Flush! For testing
        fflush(fout);
    }
    emit currentProgressChanged( 100, 100);

    // free memory
    if(index_buf) free(index_buf);
    if(block_buf1) free(block_buf1);
    if(block_buf2) free(block_buf2);

    // close files
    fclose(fin);
    fclose(fout);
}
void WorkerThread::isoToCS7(WorkerThread::Task task, QString filename,int compressLevel, const QString& target)
{
	FILE *fin,*fout;
    if ((fin = fopen(filename.toUtf8(), "rb")) == NULL)
    {
        return;
    }
	QFileInfo f(filename);
	QString outname = f.completeBaseName();
	QString fileout = target +"/"+ outname + ".7so";
    if ((fout = fopen(fileout.toUtf8(), "wb")) == NULL) 
    {
        return;
    }
	QFileInfo info(filename);
	emit message( QString( "%1 '%2'..." ).arg( taskToLabel( task ) ).arg( info.completeBaseName() + "." + info.suffix() ) );

	lzma_stream strm = LZMA_STREAM_INIT;  //intialaze lzma stream

    unsigned int *index_buf = NULL;
    unsigned int *crc_buf = NULL;
    unsigned char *block_buf1 = NULL;
    unsigned char *block_buf2 = NULL;

    long long file_size;
    unsigned long long write_pos;
    int index_size;
    int block;
    unsigned char buf4[64];
    int cmp_size;
    int status;
    int percent_period;
    int percent_cnt;
    int align,align_b,align_m;
    int cs7_total_block;  //total blocks

    CS7_H cs7;
    file_size = check_cs7_file_size(fin, cs7_total_block, cs7);
    if(file_size<0)
    {
        // free memory
        if(index_buf) free(index_buf);
        if(crc_buf) free(crc_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }
	// allocate index block
    index_size = (cs7_total_block + 1 ) * sizeof(unsigned long);
    index_buf  = (unsigned int *)malloc(index_size);
    crc_buf    = (unsigned int *)malloc(index_size);
    block_buf1 = (unsigned char *)malloc(cs7.block_size);
    block_buf2 = (unsigned char *)malloc(cs7.block_size*2);

    if( !index_buf || !crc_buf || !block_buf1 || !block_buf2 )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(crc_buf) free(crc_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);
        
        return;
    }
    memset(index_buf,0,index_size);
    memset(crc_buf,0,index_size);
    memset(buf4,0,sizeof(buf4));

	// write header block
    CS7_H beCS7 = cs7;
    fwrite(&beCS7,1,sizeof(beCS7),fout);

    // dummy write index block
    fwrite(index_buf,1,index_size,fout);

    write_pos = sizeof(cs7) + index_size;

    // compress data
    percent_period = cs7_total_block/100;
    percent_cnt    = cs7_total_block/100;

    emit currentProgressChanged(0,100);    

    align_b = 1<<(cs7.align);
    align_m = align_b -1;

	for(block = 0;block < cs7_total_block ; block++)
    {
		if(--percent_cnt<=0)
        {
            percent_cnt = percent_period;
			emit currentProgressChanged(block / percent_period, 100);

        }
		if (lzma_easy_encoder(&strm,compressLevel, LZMA_CHECK_CRC64) != LZMA_OK)
		{
				// free memory
				if(index_buf) free(index_buf);
				if(crc_buf) free(crc_buf);
				if(block_buf1) free(block_buf1);
				if(block_buf2) free(block_buf2);
            
				return;
		}
		// write align
        align = (int)write_pos & align_m;
        if(align)
        {
            align = align_b - align;
            if((int)fwrite(buf4,1,align, fout) != align)
            {
                 // free memory
                 if(index_buf) free(index_buf);
                 if(crc_buf) free(crc_buf);
                 if(block_buf1) free(block_buf1);
                 if(block_buf2) free(block_buf2);

                 return;
            }
            write_pos += align;
        }
		// mark offset index
        index_buf[block] = write_pos>>(cs7.align);

		// read buffer
        strm.next_out  = block_buf2;
        strm.avail_out = cs7.block_size*2;
        strm.next_in   = block_buf1;
        strm.avail_in  = fread(block_buf1, 1, cs7.block_size , fin);

		// compress block
        status = lzma_code(&strm, LZMA_FINISH);
        if (status != LZMA_STREAM_END)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(crc_buf) free(crc_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }
		cmp_size = strm.total_out;

        // choise plain / compress
        if(cmp_size >= (long)cs7.block_size)
        {
            cmp_size = cs7.block_size;
            memcpy(block_buf2,block_buf1,cmp_size);
            // plain block mark
            index_buf[block] |= 0x80000000;
        }

        // write compressed block
        if((int)fwrite(block_buf2, 1,cmp_size , fout) != cmp_size)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(crc_buf) free(crc_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }

        // mark next index
        write_pos += cmp_size;

        // term zlib
		lzma_end(&strm);

        //Flush! For testing
        fflush(fout);
	}
	emit currentProgressChanged(100,100);
    // last position (total size)
    index_buf[block] = write_pos>>(cs7.align);

    // write header & index block
    fseek(fout,sizeof(cs7),SEEK_SET);
    fwrite(index_buf,1,index_size,fout);

    // free memory
    if(index_buf) free(index_buf);
    if(crc_buf) free(crc_buf);
    if(block_buf1) free(block_buf1);
    if(block_buf2) free(block_buf2);

    // close files
    fclose(fin);
    fclose(fout);

}
long long WorkerThread::check_cs7_file_size(FILE *fp, int &cs7_total_block, CS7_H &cs7)
{
  long long pos;

  if( fseek(fp,0,SEEK_END) < 0)
    return -1;
  pos = ftell(fp);
  if(pos==-1) return pos;

  // init cs7 header
  memset(&cs7,0,sizeof(cs7));

  cs7.magic[0] = 'C';
  cs7.magic[1] = '7';
  cs7.magic[2] = 'S';
  cs7.magic[3] = 'O';
  cs7.ver      = 0x01;

  cs7.block_size  = 0x2000;/* 8192bytes per sector*/     //0x800; /* ISO9660 one of sector */
  cs7.total_bytes = pos;


  cs7_total_block = (pos + cs7.block_size - 1) / cs7.block_size ;

  fseek(fp,0,SEEK_SET);

  return pos;
}
void WorkerThread::cs7ToISO(WorkerThread::Task task, QString filename,const QString& target)
{
   	FILE *fin,*fout;
    if ((fin = fopen(filename.toUtf8(), "rb")) == NULL)
    {
        return;
    }
	QFileInfo f(filename);
	QString outname = f.completeBaseName();
	QString fileout = target +"/"+ outname + ".iso";
    if ((fout = fopen(fileout.toUtf8(), "wb")) == NULL) 
    {
        return;
    }
	QFileInfo info(filename);
	emit message( QString( "%1 '%2'..." ).arg( taskToLabel( task ) ).arg( info.completeBaseName() + "." + info.suffix() ) );

    lzma_stream strm = LZMA_STREAM_INIT;

    unsigned int *index_buf = NULL;
    unsigned char *block_buf1 = NULL;
    unsigned char *block_buf2 = NULL;
    unsigned int index , index2;
    unsigned long long read_pos , read_size;
    int index_size;
    int block;
    int cmp_size;
    int status;
    int percent_period;
    int percent_cnt;
    int plain;

    // read header
    CS7_H cs7;
    if( fread(&cs7, 1, sizeof(cs7), fin) != sizeof(cs7) )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }
    // check header
    if(
        cs7.magic[0] != 'C' ||
        cs7.magic[1] != '7' ||
        cs7.magic[2] != 'S' ||
        cs7.magic[3] != 'O' ||
        cs7.block_size ==0  ||
        cs7.total_bytes == 0
        )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }
  
    int cs7_total_block = (cs7.total_bytes + cs7.block_size - 1) / cs7.block_size;

    // allocate index block
    index_size = (cs7_total_block + 1 ) * sizeof(unsigned long);
    index_buf  = (unsigned int *)malloc(index_size);
    block_buf1 = (unsigned char *)malloc(cs7.block_size);
    block_buf2 = (unsigned char *)malloc(cs7.block_size*2);

    if( !index_buf || !block_buf1 || !block_buf2 )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }
    memset(index_buf,0,index_size);

    // read index block
    if( (int)fread(index_buf, 1, index_size, fin) != index_size )
    {
        // free memory
        if(index_buf) free(index_buf);
        if(block_buf1) free(block_buf1);
        if(block_buf2) free(block_buf2);

        return;
    }


    // decompress data
    percent_period = cs7_total_block/100;
    percent_cnt = 0;
    emit currentProgressChanged( 0, 100);

    for(block = 0;block < cs7_total_block ; block++)
    {
        if(--percent_cnt<=0)
        {
            percent_cnt = percent_period;
            emit currentProgressChanged(block / percent_period, 100);
        }

        if (lzma_stream_decoder(&strm, UINT64_MAX, 0) != LZMA_OK)
        {
            //free memory
            if(index_buf) free(index_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }

        // check index
        index  = index_buf[block];
        plain  = index & 0x80000000;
        index  &= 0x7fffffff;
        read_pos = index << (cs7.align);
        if(plain)
        {
            read_size = cs7.block_size;
        }
        else
        {
            index2 = index_buf[block+1] & 0x7fffffff;
            read_size = (index2-index) << (cs7.align);
        }
        fseek(fin,read_pos,SEEK_SET);

        strm.avail_in = fread(block_buf2, 1, read_size , fin);

        if(plain)
        {
            memcpy(block_buf1,block_buf2,strm.avail_in);
            cmp_size = strm.avail_in;
        }
        else
        {
            strm.next_out  = block_buf1;
            strm.avail_out = cs7.block_size;
            strm.next_in   = block_buf2;
            status = lzma_code(&strm, LZMA_FINISH);
            if (status != LZMA_STREAM_END)
            {
                // free memory
                if(index_buf) free(index_buf);
                if(block_buf1) free(block_buf1);
                if(block_buf2) free(block_buf2);

                return;
            }
            cmp_size = strm.total_out;
        }
        // write decompressed block
        if((int)fwrite(block_buf1, 1,cmp_size , fout) != cmp_size)
        {
            // free memory
            if(index_buf) free(index_buf);
            if(block_buf1) free(block_buf1);
            if(block_buf2) free(block_buf2);

            return;
        }

		lzma_end(&strm);

        //Flush! For testing
        fflush(fout);
    }
    emit currentProgressChanged( 100, 100);

    // free memory
    if(index_buf) free(index_buf);
    if(block_buf1) free(block_buf1);
    if(block_buf2) free(block_buf2);

    // close files
    fclose(fin);
    fclose(fout);
}
void WorkerThread::scanUMD(WorkerThread::Task task, QString filename)
{
	QFileInfo fi(filename);
	QString path              = fi.absoluteFilePath();
	QDateTime lastModified    = fi.lastModified();
    unsigned int lastModinSec = lastModified.toTime_t();
    qint64 filesize           = fi.size();
	
	emit message( QString( "%1 '%2'..." ).arg( taskToLabel( task ) ).arg( fi.completeBaseName() + "." + fi.suffix() ) );
	emit currentProgressChanged( 0, 100);
	//check if entry is already in cache , don't calcuate crc for speed
	QSqlQuery query2(dbcollect);
    query2.prepare("SELECT * FROM cache where path =? AND lastmodified=? AND filesize=?"); 
    query2.addBindValue(path); 
	query2.addBindValue(lastModinSec);
    query2.addBindValue(filesize);
	query2.exec();
	if(query2.first())//entry already exists make it available
	{
		QSqlQuery query3(dbcollect);
		query3.prepare("UPDATE cache SET available = 1 where path =? AND lastmodified=? AND filesize=?"); 
		query3.addBindValue(path); 
		query3.addBindValue(lastModinSec);
		query3.addBindValue(filesize);
		query3.exec();

	}
	else  //create a new entry
	{
		 emit currentProgressChanged( 50, 100);
		QString crc32s(getCRC32(fi.absoluteFilePath()));

		//create a query
		QSqlQuery query(dbcollect);
		query.prepare("INSERT INTO cache (path,lastmodified,filesize,crc32,available) "
				  "VALUES (?,?,?,?,?)");
				  query.addBindValue(path);
				  query.addBindValue(lastModinSec);
				  query.addBindValue(filesize);
				  query.addBindValue(crc32s.toUpper());
				  query.addBindValue(1);//since we add a record it is added as available
				  query.exec();  
	}
	emit currentProgressChanged(100, 100);
}
void WorkerThread::fixupStatus(WorkerThread::Task task, QString filename)
{
	umdimageloader::reboot(filename.toLatin1().data());
	QFileInfo fi(filename);
	emit message( QString( "%1 '%2'..." ).arg( taskToLabel( task ) ).arg( fi.completeBaseName() + "." + fi.suffix() ) );
	emit currentProgressChanged( 0, 100);
	if(!ISOFS_GetVolumeDescriptor())
	{
		emit message("Image didn't recongnized as valid ignore");
		return;
	}
	QString bootbinstatus;
	bootbinstatus.clear();
	int size = ISOFS_getfilesize("PSP_GAME/SYSDIR/BOOT.BIN");
	int     f;
    if (size)
    {
         int header;
         f = ISOFS_open("PSP_GAME/SYSDIR/BOOT.BIN", 1);
         ISOFS_read(f, (char *)&header, sizeof(int));
         ISOFS_close(f);

         switch (header)
         {
            case 0x464C457F: bootbinstatus = "Plain BOOT.BIN"; break;
            case 0x5053507E: bootbinstatus = "Encrypted BOOT.BIN (PSP~)"; break;
            case 0x4543537E: bootbinstatus = "Encrypted BOOT.BIN (~SCE)"; break;
            case 0x00000000: bootbinstatus = "Zero Length BOOT.BIN"; break;
            default:         bootbinstatus = "Unsupported BOOT.BIN"; break;
          }
    }
	else
	{
		bootbinstatus = "Zero Length BOOT.BIN";
	}
	QString ebootbinstatus;
	ebootbinstatus.clear();
    size = ISOFS_getfilesize("PSP_GAME/SYSDIR/EBOOT.BIN");
	if (size)
    {
		int header;
		f = ISOFS_open("PSP_GAME/SYSDIR/EBOOT.BIN", 1);
		ISOFS_read(f, (char *)&header, sizeof(int));
		ISOFS_close(f);

        switch (header)
        {
            case 0x464C457F: ebootbinstatus = "Plain EBOOT.BIN"; break;
            case 0x5053507E: ebootbinstatus = "Encrypted EBOOT.BIN (PSP~)"; break;
            case 0x4543537E: ebootbinstatus = "Encrypted EBOOT.BIN (~SCE)"; break;
			case 0x00000000: ebootbinstatus = "Zero Length EBOOT.BIN"; break;
            default:         ebootbinstatus = "Unsupported EBOOT.BIN"; break;
         }
                    
	}
	else
	{
		ebootbinstatus = "Zero Length EBOOT.BIN";
	}
	umdimageloader::shutdown();

	QSqlQuery query(dbadv);
	query.prepare("SELECT cache.crc32,cache.path,images.id,bootbinstatus,ebootbinstatus FROM cache,images WHERE path=? AND cache.crc32=images.crc AND available=1");
	query.addBindValue(filename);
	query.exec();
	if(query.first())
	{
		QString crc32 = query.value(0).toString();
		QString currentbootbin = query.value(3).toString();
		QString currentebootbin = query.value(4).toString();

		QSettings udb1("fixupstatus.ini", QSettings::IniFormat);
		QString scrc(crc32);
		QString udbinigroup = scrc.toUpper();
		udb1.beginGroup(udbinigroup);
		QString crc32exists = udb1.value("crc32","").toString();
		if(crc32exists.isEmpty())
        {
			udb1.setValue("crc32",         scrc.toUpper());
			udb1.setValue("currentbootbin",        currentbootbin);
			udb1.setValue("currentebootbin",        currentebootbin);
			udb1.setValue("newbootbin",        bootbinstatus);
			udb1.setValue("newebootbin",        ebootbinstatus);
		}

		udb1.endGroup();
        //udb1.remove(udbinigroup);

		//update the database with the new values
		QSqlQuery query3(dbadv);
		query3.exec("UPDATE images SET bootbinstatus='" + bootbinstatus + "'" + ", ebootbinstatus='" + ebootbinstatus + "' where crc = '" + crc32 + "'");
	}
	emit currentProgressChanged(100, 100);


}
void WorkerThread::fixupImages(WorkerThread::Task task, QString filename)
{
	umdimageloader::reboot(filename.toLatin1().data());
	QFileInfo fi(filename);
	emit message( QString( "%1 '%2'..." ).arg( taskToLabel( task ) ).arg( fi.completeBaseName() + "." + fi.suffix() ) );
	emit currentProgressChanged( 0, 100);

	if(!ISOFS_GetVolumeDescriptor())
	{
		emit message("Image didn't recongnized as valid ignore");
		return;
	}
	//neccesary values to fill
	QString crc32="";
	QString type="";
	int gameid=0;
	QString firmware="";
	QString discid="";
	QString genre="";
	int filesize=0;
	QString bootbinstatus="";
	QString ebootbinstatus="";


	crc32.clear();
	type.clear();
	firmware.clear();
	discid.clear();
	genre.clear();
	QSqlQuery query(dbadv);
	query.prepare("SELECT crc32,filesize,path FROM cache WHERE path=? AND available=1");
	query.addBindValue(filename);
	query.exec();
	if(query.first())
	{
		crc32 = query.value(0).toString();
		//check if we have already added the crc with previous commit in our thread
		QSqlQuery query2(dbadv);
		query2.prepare("SELECT crc FROM images WHERE crc=?");
		query2.addBindValue(filename);
		query2.exec();
		if(query2.first())
		{
			return;
		}
		filesize = query.value(1).toUInt();
		if(umdloader_imagetype==1) //cso;
			type="cso";
		else if(umdloader_imagetype==0) //iso
			type="iso";
		else
			type="<UNKN>";

		//get discid and firmware from param.sfo file
		u32 size = ISOFS_getfilesize("PSP_GAME/PARAM.SFO");
        u8     *data = new u8[size];
        int f = ISOFS_open("PSP_GAME/PARAM.SFO", 1);
        ISOFS_read(f, (char *)data, size);
        ISOFS_close(f);
		if (size)
        {
              psf::load_psf(data);
			  discid  = QString::fromUtf8(psfinfo.disc_id).trimmed();
			  firmware = QString::fromUtf8(psfinfo.psp_system_version);
		}
		 QSqlQuery query4(dbadv);
		 //check first if there is already an entry with the same discid from advanscene so we can update gameid
		 query4.prepare("SELECT gameid FROM games,images WHERE games.id = images.gameid AND discid=?");
		 query4.addBindValue(discid);
		 query4.exec();
		 if(query4.first())
		 {
			  gameid=query4.value(0).toUInt();
		 }
			bootbinstatus.clear();
			int size2 = ISOFS_getfilesize("PSP_GAME/SYSDIR/BOOT.BIN");
			int     f2;
			if (size2)
			{
				 int header;
				 f2 = ISOFS_open("PSP_GAME/SYSDIR/BOOT.BIN", 1);
				 ISOFS_read(f2, (char *)&header, sizeof(int));
				 ISOFS_close(f2);

				 switch (header)
				 {
					case 0x464C457F: bootbinstatus = "Plain BOOT.BIN"; break;
					case 0x5053507E: bootbinstatus = "Encrypted BOOT.BIN (PSP~)"; break;
					case 0x4543537E: bootbinstatus = "Encrypted BOOT.BIN (~SCE)"; break;
					case 0x00000000: bootbinstatus = "Zero Length BOOT.BIN"; break;
					default:         bootbinstatus = "Unsupported BOOT.BIN"; break;
				  }
			}
			else
			{
				bootbinstatus = "Zero Length BOOT.BIN";
			}
			ebootbinstatus.clear();
			size2 = ISOFS_getfilesize("PSP_GAME/SYSDIR/EBOOT.BIN");
			if (size2)
			{
				int header;
				f = ISOFS_open("PSP_GAME/SYSDIR/EBOOT.BIN", 1);
				ISOFS_read(f2, (char *)&header, sizeof(int));
				ISOFS_close(f2);

				switch (header)
				{
					case 0x464C457F: ebootbinstatus = "Plain EBOOT.BIN"; break;
					case 0x5053507E: ebootbinstatus = "Encrypted EBOOT.BIN (PSP~)"; break;
					case 0x4543537E: ebootbinstatus = "Encrypted EBOOT.BIN (~SCE)"; break;
					case 0x00000000: ebootbinstatus = "Zero Length EBOOT.BIN"; break;
					default:         ebootbinstatus = "Unsupported EBOOT.BIN"; break;
				 }
                    
			}
			else
			{
				ebootbinstatus = "Zero Length EBOOT.BIN";
			}

	QSqlQuery query5(dbadv);
	query5.prepare("INSERT INTO images (crc,isotype,grouprelease,gameid,firmware,discid,umdversion,filesize,bootbinstatus,ebootbinstatus,renrelease)"
              "VALUES (?, ?, ?,?,?,?,?,?,?,?,?)");
			 query5.addBindValue(crc32);
             query5.addBindValue(type);
			 query5.addBindValue("<UNKN>");
			 query5.addBindValue(gameid);
			 query5.addBindValue(firmware);
			 query5.addBindValue(discid);
			 query5.addBindValue("<UNKN>");
			 query5.addBindValue(filesize);
			 query5.addBindValue(bootbinstatus);
			 query5.addBindValue(ebootbinstatus);
			 query5.addBindValue(0);//always renascene release
		     query5.exec();

			 fixupdata();//fixupnumbering

			QSettings udb1("fixupImages.ini", QSettings::IniFormat);
			QString scrc(crc32);
			QString udbinigroup = scrc.toUpper();
			udb1.beginGroup(udbinigroup);
			QString crc32exists = udb1.value("crc32","").toString();
			if(crc32exists.isEmpty())
			{
				udb1.setValue("crc32",         scrc.toUpper());
				udb1.setValue("discid",        discid);
				udb1.setValue("firmware",        firmware);
				udb1.setValue("gamesize",      filesize);
				udb1.setValue("type",        type);
				udb1.setValue("gameid",       gameid);
				udb1.setValue("bootbinstatus",        bootbinstatus);
				udb1.setValue("ebootbinstatus",        ebootbinstatus);

			}

			udb1.endGroup();
	}
	emit currentProgressChanged(100, 100);
}
void WorkerThread::fixupFW(WorkerThread::Task task, QString filename)
{
	umdimageloader::reboot(filename.toLatin1().data());
	QFileInfo fi(filename);
	emit message( QString( "%1 '%2'..." ).arg( taskToLabel( task ) ).arg( fi.completeBaseName() + "." + fi.suffix() ) );
	emit currentProgressChanged( 0, 100);
	if(!ISOFS_GetVolumeDescriptor())
	{
		emit message("Image didn't recongnized as valid ignore");
		return;
	}
	QString firmware;
	firmware.clear();

	u32 size = ISOFS_getfilesize("PSP_GAME/PARAM.SFO");
    u8     *data = new u8[size];
    int f = ISOFS_open("PSP_GAME/PARAM.SFO", 1);
    ISOFS_read(f, (char *)data, size);
    ISOFS_close(f);
	if (size)
    {
       psf::load_psf(data);
	   firmware = QString::fromUtf8(psfinfo.psp_system_version);
	}
	umdimageloader::shutdown();

	QSqlQuery query(dbadv);
	query.prepare("SELECT cache.crc32,cache.path,images.id,firmware,cache.filesize FROM cache,images WHERE path=? AND cache.crc32=images.crc AND available=1");
	query.addBindValue(filename);
	query.exec();
	if(query.first())
	{
		QString crc32 = query.value(0).toString();
		QString currentfirmware = query.value(3).toString();
		QString filesize = query.value(4).toString();

		QSettings udb1("fixupFW.ini", QSettings::IniFormat);
		QString scrc(crc32);
		QString udbinigroup = scrc.toUpper();
		udb1.beginGroup(udbinigroup);
		QString crc32exists = udb1.value("crc32","").toString();
		if(crc32exists.isEmpty())
        {
			udb1.setValue("crc32",         scrc.toUpper());
			udb1.setValue("currentFW",        currentfirmware);
			udb1.setValue("updatedFW",        firmware);
		}

		udb1.endGroup();
        //udb1.remove(udbinigroup);

		//update the database with the new values
		QSqlQuery query3(dbadv);
		query3.exec("UPDATE images SET firmware='" + firmware  + "' where crc = '" + crc32 + "'");

		//update filesize
		QSqlQuery query4(dbadv);
		query4.exec("UPDATE images SET filesize='" + filesize  + "' where crc = '" + crc32 + "'");
	}
	emit currentProgressChanged(100, 100);
}