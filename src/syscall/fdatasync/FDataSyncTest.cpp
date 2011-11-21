//      FDataSyncTest.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Suren Grigoryan <suren.grigoryan@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <FDataSyncTest.hpp>
#include <../stat/StatFile.hpp>


int FDataSyncTest::Main(vector<string> args)
{
    
    if ( geteuid() != 0 )
	{
		// The tests should be executed by the root user;
		cerr << "Only root can execute these tests..." << endl;
		return Unres;
	}
    
	if ( _mode == Normal )
	{
		switch (_operation)
		{
            case FDataSyncNormExec:
                return NormExec();
            case FDataSyncErrInvalidFD:
                return ErrInvalidFD();
            case FDataSyncErrNonFsyncFD:
                return ErrNonFsyncFD();
            
         
            default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}

	cerr << "Test was successful";

	return Success;
}


Status FDataSyncTest::NormExec ()
{
    const char * buf = "abcdefg";
    StatFile file (this->_tmpDir + "/fdatasync_test");
    
    if (write (file.GetFileDescriptor(), buf, strlen(buf)) == -1) {
        cerr << "write returned -1";
        return Unres;
    }
    sleep(1);
    
    int  ret = fdatasync (file.GetFileDescriptor());
    
    if (ret != 0) {
        cerr << "fdatasync returned non zero value";
        return Fail;
    } 
    
    struct stat stat_buf;
    
    if(stat (file.GetPathname().c_str(), &stat_buf) == -1) {
		cerr << "stat returned -1";
		return Unres;
	}
	
	if (file.GetATime() != stat_buf.st_atime) {
        cerr << "fdatasync updated last access time of the file";
        return Fail;
    } 
    if (file.GetMTime() != stat_buf.st_mtime) {
        cerr << "fdatasync updated last modification time of the file";
        return Fail;
    } 
    
    return Success;
}


// EBADF the fildes argument is not a valid descriptor.
Status FDataSyncTest::ErrInvalidFD ()
{
    if (fdatasync (-1) != -1) {
        cerr << "fdatasync must return -1 when the fildes argument is not a valid "
            "descriptor";
        return Fail;
    }
    if (errno != EBADF) {
        cerr<<"fdatasync must set EBADF error when the fildes argument is not a "
            "valid descriptor";
        return Fail;    
    }
    
    return Success;
}

// EINVAL the fildes argument does not refer to a file on which this operation 
// is possible.
Status FDataSyncTest::ErrNonFsyncFD ()
{
    int fds[2];
    
    if (pipe (fds) == -1) {
        cerr<<"pipe returned -1";
        return Unres;
    }
    
    int ret = fdatasync (fds[0]);
    
    if (close (fds[0]) == -1 || close (fds[1]) == -1) {
        cerr<<"close returned -1";
        return Unres;
    }
    
    if (ret != -1) {
        cerr << "fdatasync must return -1 when the fildes argument is not a valid "
            "descriptor";
        return Fail;
    }
    if (errno != EINVAL) {
        cerr<<"fsync must set EINVAL error when the fildes argument is not a "
            "valid descriptor";
        return Fail;    
    }
    
    return Success;
}
