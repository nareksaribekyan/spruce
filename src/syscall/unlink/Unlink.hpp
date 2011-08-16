//      Unlink.hpp
//      
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author: Ani Tumanyan <ani.tumanyan92@gmail.com>
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

#ifndef UNLINK_Hkk
#define UNLINK_Hkk

#include "SyscallTest.hpp"

// Operations
enum UnlinkSyscalls
{
	UnlinkTestTooLongPath,
	UnlinkTestNormalFile,
	UnlinkTestIsNotDirectory,
	UnlinkTestNoSuchFile,
	UnlinkIsDirectory,
	UnlinkTestEmptyPath,
	UnlinkTestNegativeAdress,
	UnlinkTestPermissionDenied
};

class UnlinkTest : public SyscallTest
{			
public:	
	UnlinkTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "Unlink")
	{			
	}
protected:
	virtual ~UnlinkTest() {}	
	Status UnlinkTestTooLongPathFunction();	
	Status UnlinkTestNormalFileFunction();
	Status UnlinkTestIsNotDirectoryFunction();
	Status UnlinkTestNoSuchFileFunction();
	Status UnlinkIsDirectoryFunction();
	Status UnlinkTestEmptyPathFunction();
	Status UnlinkTestNegativeAdressFunction();
	Status UnlinkTestPermissionDeniedFunction();
	virtual int Main(vector<string> args);	
};
#endif
