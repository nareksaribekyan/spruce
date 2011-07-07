//      ReadWriteFile.hpp
//      
//      Copyright 2011 Gurgen Suren
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

#ifndef LINK_H
#define LINK_H

#include "SyscallTest.hpp"

// Operations
enum LinkSyscalls
{
	LinkTestTooLongOldPath,
	LinkTestTooLongNewPath,
	LinkTestNewPathAleadyExist,
	LinkTestOldPathIsDirectory,
	LinkTestNormalFile,
	LinkTestIsNotDirectory
};

class LinkTest : public SyscallTest
{			
public:	
	LinkTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "Link")
	{			
	}
	virtual ~LinkTest() {}	
	Status LinkTooLongOldPath();	
	Status LinkTooLongNewPath();
	Status LinkNewPathAlreadyExist();
	Status LinkOldPathIsDirectory();
	Status LinkNormalLink();
	Status LinkIsNotDirectory();

protected:
	virtual int Main(vector<string> args);	
};
#endif