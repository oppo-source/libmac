#ifdef IO_USE_STD_LIB_FILE_IO

#ifndef APE_STDLIBFILEIO_H
#define APE_STDLIBFILEIO_H

#include "IO.h"

class CStdLibFileIO : public CIO
{
public:

    // construction / destruction
    CStdLibFileIO();
    ~CStdLibFileIO();

    // open / close
    int Open(const LPCTSTR pName);
    int Close();

    // read / write
    int Read(void * pBuffer, unsigned int nBytesToRead, unsigned int * pBytesRead);
    int Write(const void * pBuffer, unsigned int nBytesToWrite, unsigned int * pBytesWritten);

    // seek
    int Seek(int nDistance, unsigned int nMoveMode);

    // other functions
    int SetEOF();

    // creation / destruction
    int Create(const LPCTSTR pName);
    int Delete();

    // attributes
    int GetPosition();
    int GetSize();
    int GetName(LPCTSTR pBuffer);
    int GetHandle();

    private:

    char m_cFileName[MAX_PATH];
    BOOL m_bReadOnly;
    FILE * m_pFile;
};

#endif // #ifndef APE_STDLIBFILEIO_H

#endif // #ifdef IO_USE_STD_LIB_FILE_IO

