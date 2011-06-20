/**
//      Deleter.cpp
//      
//      Copyright 2011 Tigran Piloyan <tigran.piloyan@gmail.com>
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
*/

#include <stdlib.h>
#include <memory>
#include "UnixCommand.hpp"
#include "Deleter.hpp"

int DeleterTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case MultipleFilesDeletion:
				return MultipleFilesDeletionFunc();
			default:
				cerr << "Unsupported operation.";	
				return Unres;		
		}
	}
	
	cerr << "Test was successful";	
	return Success;		
}

Status DeleterTest::MultipleFilesDeletionFunc()
{
	UnixCommand command("${CMAKE_INSTALL_PREFIX}/bin/MultipleFilesDeletion.sh");
    std::auto_ptr<ProcessResult> result(command.Execute(CreateArguments()));
    cerr << result->GetOutput() << " ";
    
    return (Status)result->GetStatus();
}
vector<string> DeleterTest::CreateArguments()
{
	vector<string> command;
	command.push_back(dirName);
	
	return command;
}
