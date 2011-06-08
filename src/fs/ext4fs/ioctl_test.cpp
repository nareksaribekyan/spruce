#include "ioctl_test.hpp"
#include <sys/capability.h>

int IoctlTest::_file = -1;	
int IoctlTest::_file_donor = -1;	


Status IoctlTest::Main()
{
	if ( _mode == Normal )
	{
		switch (_operation)
		{
			case SetFlagsGetFlags:
				return TestSetFlagsGetFlags();
			case ClearExtentsFlags:
				return TestClearExtentsFlags();
			case SetFlagsNotOwner:
				return TestSetFlagsNotOwner();
			case SetVersionGetVersion:
				return TestSetVersionGetVersion();
			case WaitForReadonly:
				return TestWaitForReadonly();
			case GroupExtend:
				return TestGroupExtend();
			case MoveExtent:
				return TestMoveExtent();
			case GroupAdd:
				return TestGroupAdd();
			case Migrate:
				return TestMigrate();
			case AllocDABlocks:
				return TestAllocDABlocks();
			case Fitrim:
				return TestFitrim();
			case Unsupported:
				return TestUnsupported();
			default:
				cerr << "Unsupported operation.";	
				return Unres;
		}
	}
	cerr << "Test was successful";	
	return Success;
}

Status IoctlTest::TestSetFlagsGetFlags()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	int set_flags = EXT4_EXTENTS_FL | 1; // We may NOT clear the extents flag...
	int get_flags = 0;
	
	// Backup the old values just in case
	int old_flags;
	if ( ioctl(_file, EXT4_IOC_GETFLAGS, &old_flags) == -1 )
	{
		cerr << "Error backing up old values. " << strerror(errno);
		return Unres;
	}
	
	// Set our testing flag values
	if ( ioctl(_file, EXT4_IOC_SETFLAGS, &set_flags) == -1 )
	{
		cerr << "Error setting new flag values. " << strerror(errno);
		return Unres;
	}
	
	// Get the flags back
	if ( ioctl(_file, EXT4_IOC_GETFLAGS, &get_flags) == -1 )
	{
		cerr << "Error getting flag values back. " << strerror(errno);
		return Unres;
	}
	
	// Restore the original flags
	if ( ioctl(_file, EXT4_IOC_SETFLAGS, &old_flags) == -1 )
	{
		cerr << "Error restoring old flag values. " << strerror(errno);
		return Unres;
	}
	
	// Compare them
	if ( get_flags != set_flags )
	{
		cerr << "Set and Get flags mismatch";
		return Fail;
	}
	else
	{
		cerr << "Set and Get flags match";
		return Success;
	}
}

Status IoctlTest::TestClearExtentsFlags()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	int non_permitted_flags = 1; // We may NOT clear the extents flag... but we shall try!
	int result = 0;
	
	// Try to set the non-permitted flag
	if ( ( result = ioctl(_file, EXT4_IOC_SETFLAGS, &non_permitted_flags ) ) != -EOPNOTSUPP )
	{
		cerr << "It was permitted to set non-permitted flag!. " << strerror(errno);
		return Fail;
	}
	else 
	{
		cerr << "It was NOT permitted to set non-permitted flag!. " << strerror(errno);
		return Success;
	}
}

Status IoctlTest::TestSetFlagsNotOwner()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	int flags = 1;
	int result = 0;
	
	cap_user_header_t header = new __user_cap_header_struct;
	cap_user_data_t data = new __user_cap_data_struct;
	
	header->version = _LINUX_CAPABILITY_VERSION_3;
	header->pid = getpid();
	
	data->permitted |= CAP_CHOWN;
	data->inheritable |= CAP_CHOWN;
	data->effective |= CAP_CHOWN;
	
	if ( capset( header, data ) )
	{
		cerr << "Cannot set file capabilities: " <<  strerror(errno);
		return Unres;
	}
	
	if ( fchown(_file, 0, 0) )
	{
		cerr << "Cannot change the file owner: " <<  strerror(errno);
		return Unres;
	}
	
	// Try to set the non-permitted flag
	if ( ( result = ioctl(_file, EXT4_IOC_SETFLAGS, &flags ) ) != -EACCES )
	{
		cerr << "It was permitted to set the flag not being file owner!. " << strerror(errno);
		return Fail;
	}
	else 
	{
		cerr << "It was NOT permitted to set flag not being file owner!. " << strerror(errno);
		return Success;
	}
}

Status IoctlTest::TestSetVersionGetVersion()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	int set_version = 10; 
	int get_version = 0;
	
	// Backup the old version just in case
	int old_version;
	if ( ioctl(_file, EXT4_IOC_GETVERSION, &old_version) == -1 )
	{
		cerr << "Error backing up old version. " << strerror(errno);
		return Unres;
	}
	
	// Set our testing version value
	if ( ioctl(_file, EXT4_IOC_SETVERSION, &set_version) == -1 )
	{
		cerr << "Error setting new version values. " << strerror(errno);
		return Unres;
	}
	
	// Get the version back
	if ( ioctl(_file, EXT4_IOC_GETVERSION, &get_version) == -1 )
	{
		cerr << "Error getting version value back. " << strerror(errno);
		return Unres;
	}
	
	// Restore the original version
	if ( ioctl(_file, EXT4_IOC_SETVERSION, &old_version) == -1 )
	{
		cerr << "Error restoring old version value. " << strerror(errno);
		return Unres;
	}
	
	// Compare them
	if ( get_version != set_version )
	{
		cerr << "Set and Get version mismatch";
		return Fail;
	}
	else
	{
		cerr << "Set and Get versions match";
		return Success;
	}
}

Status IoctlTest::TestWaitForReadonly()
{

	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	if ( ioctl(_file, EXT4_IOC_WAIT_FOR_READONLY, NULL) == -1 )
	{
		cerr << "Error waiting for readonly. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Wait for readonly was successful. " << strerror(errno);
		return Success;
	}
}


Status IoctlTest::TestGroupExtend()
{

	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	unsigned int BlockCount = 1;
	
	if ( ioctl(_file, EXT4_IOC_GROUP_EXTEND, &BlockCount) == -1 )
	{
		cerr << "Error extending group. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Group extention was successful. " << strerror(errno);
		return Success;
	}
}

Status IoctlTest::TestMoveExtent()
{

	if ( _file == -1 || _file_donor == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	unsigned int BlockCount = 1;
	
	struct move_extent me;
	memset(&me, 0, sizeof(me));
	me.donor_fd = _file_donor;
	me.len = 1;
	
	char TestData = 'A';
	
	write(_file, &TestData, 1);
	write(_file_donor, &TestData, 1);
	
	if ( ioctl(_file, EXT4_IOC_MOVE_EXT, &me) == -1 )
	{
		cerr << "Error moving extent. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Move extent was successful. " << strerror(errno);
		return Success;
	}
}

Status IoctlTest::TestGroupAdd()
{
	cerr << "Adding group";
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	struct ext4_new_group_data input;
	
	if ( ioctl(_file, EXT4_IOC_GROUP_ADD, &input) == -1 )
	{
		cerr << "Error adding group. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Group was added successful. " << strerror(errno);
		return Success;
	}
	return Unknown;
}

Status IoctlTest::TestMigrate()
{
	cerr << "Migrating";
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	if ( ioctl(_file, EXT4_IOC_MIGRATE, 0) == -1 )
	{
		cerr << "Error migrating. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Migration was successful. " << strerror(errno);
		return Success;
	}
	return Unknown;
}

Status IoctlTest::TestAllocDABlocks()
{
	cerr << "AllocDABlocks";
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	if ( ioctl(_file, EXT4_IOC_ALLOC_DA_BLKS, 0) == -1 )
	{
		cerr << "Error allocating DA blocks. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "DA blocks allocation was successful. " << strerror(errno);
		return Success;
	}
	return Unknown;
}

Status IoctlTest::TestFitrim()
{
	cerr << "Fitrim???";
#ifdef FITRIM	
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	if ( ioctl(_file, FITRIM, 0) == -1 )
	{
		cerr << "Error FITRIM. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "FITRIM was successful. " << strerror(errno);
		return Success;
	}
#else
	return Unres;
#endif
}
Status IoctlTest::TestUnsupported()
{
	cerr << "Unsupported...";
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	if ( ioctl(_file, -100, 0) != -ENOTTY )
	{
		cerr << "Error on unsupported command. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Unsupported command was processed successfully. " << strerror(errno);
		return Success;
	}
	return Unknown;
}