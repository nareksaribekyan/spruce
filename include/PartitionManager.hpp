//		PartitionManager.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Authors:
//				Ruzanna Karakozova <r.karakozova@gmail.com>
//				Vahram Martirosyan <vmartirosyan@gmail.com>
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

#ifndef PARTITIONMANAGER_HPP
#define PARTITIONMANAGER_HPP

#include <Logger.hpp>
#include <sstream>
#include <stdint.h>
#include <inttypes.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <pwd.h>
#include <UnixCommand.hpp>
#include <map>
#include <set>
#include <sstream>
using namespace std;

enum FileSystems
{
	FS_COMMON,
	EXT4,
	XFS,
	BTRFS,
	JFS,
	FS_UNSUPPORTED
};

const string FSNames[] = {
	"common",
	"ext4",
	"xfs",
	"btrfs",
	"jfs",
	"Unsupported FS"
};

enum PartitionStatus
{
	PS_Success,
	PS_Done,
	PS_Skip,
	PS_Fatal
};

/* Config file format:
 * [FS=ext]
 * mkfs_opt1,mkfs_opt2:mount_opt1,mount_opt2
 * mkfs_opt3:
 * mount_opt3
 * 
 * IMPORTANT:
 * All the mkfs option conbinations must end up with ':' symbol, 
 * even if there are no mount options to use with them!!
 * */

class PartitionManager
{
public:
	PartitionManager():
		_ConfigFile(""),
		_DeviceName(""),
		_MountPoint(""),
		_FileSystem(""),
		_MountOpts(""),		
		_CurrentOptions(),
		//_CurrentMountOptions(""),
		_MkfsOpts(""),
		//_CurrentMkfsOptions(""),
		_Index(0),
		_FSIndex(FS_UNSUPPORTED)		
		{
			
		}		
	PartitionManager(string ConfigFile, string DeviceName, string MountPoint, string FileSystem, string MountOpts):
		_ConfigFile(ConfigFile),
		_DeviceName(DeviceName),
		_MountPoint(MountPoint),
		_FileSystem(FileSystem),
		_MountOpts(MountOpts),
		_CurrentOptions(),
		//_CurrentMountOptions(""),
		_MkfsOpts(""),
		//_CurrentMkfsOptions(""),
		_Index(0),
		_FSIndex(FS_UNSUPPORTED)
		{				
			Initialize(ConfigFile, DeviceName, MountPoint, FileSystem, MountOpts);
		}
	virtual ~PartitionManager() {}
	
	PartitionManager & operator = (const PartitionManager & pm)
	{			
		_ConfigFile = pm._ConfigFile;
		_DeviceName = pm._DeviceName;
		_MountPoint = pm._MountPoint;
		_FileSystem = pm._FileSystem;
		_MountOpts = pm._MountOpts;
		_CurrentOptions = pm._CurrentOptions;
		//_CurrentMountOptions = pm._CurrentMountOptions;
		_Index = pm._Index;
		_FSIndex = pm._FSIndex;			
		return *this;
	}
	
	void Initialize(string ConfigFile, string DeviceName, string MountPoint, string FileSystem, string MountOpts)
	{			
		_ConfigFile = ConfigFile;
		_DeviceName = DeviceName;
		_MountPoint = MountPoint;
		_FileSystem = FileSystem;
		_MountOpts = MountOpts;		
		//_CurrentMountOptions = "";
		_Index = _AdditionalOptions[FS_UNSUPPORTED].end();
		_FSIndex = FS_UNSUPPORTED;
		if ( _MountOpts != "" && _MountOpts[_MountOpts.size() - 1] != ',' )
			_MountOpts += ',';
			
		LoadConfiguration();
		_FSIndex = GetFSNumber(_FileSystem);			
		_Index = _AdditionalOptions[_FSIndex].begin();
	}
	
	PartitionStatus PreparePartition();
	static PartitionStatus RestorePartition(string DeviceName, string MountPoint, string FileSystem, bool RecreateFilesystem = false, bool resizeFlag = false);
	static bool ReleasePartition(string MountPoint, string* output = NULL);
	static bool Mount(string DeviceName,string MountPoint,string FileSystem,string Options, unsigned long Flags = 0);
	string GetCurrentOptions(bool Stripped = true) const;
	void ClearCurrentOptions();
	static bool IsOptionEnabled(string optionName);
	static uint64_t GetDeviceSize(string partition);
	
	struct CurrentOptions
	{
		CurrentOptions(string mkfs_opts = "", unsigned long mount_flags = 0, string mount_data = ""):
			MkfsOptions(mkfs_opts),
			MountFlags(mount_flags),
			MountData(mount_data),
			RawData("")
			{}
		bool operator == (const CurrentOptions & cur) const
		{
			return (
				( MkfsOptions == cur.MkfsOptions ) &&
				( MountFlags == cur.MountFlags ) &&
				( MountData == cur.MountData ) );
		}
		bool operator < (const CurrentOptions & opt) const
		{
			stringstream str_this, str_opt;
			
			str_this << MkfsOptions << MountData << MountFlags;
			str_opt << opt.MkfsOptions << opt.MountData << opt.MountFlags;
			
			return str_this.str() < str_opt.str();			
		}
		CurrentOptions & operator = (const CurrentOptions & opt)
		{
			MkfsOptions = opt.MkfsOptions;
			MountFlags = opt.MountFlags;
			MountData = opt.MountData;
			RawData = opt.RawData;
			return *this;
		}
		string MkfsOptions;
		unsigned long MountFlags;
		string MountData;
		string RawData;
	};
		
private:

		string _ConfigFile;
		string _DeviceName;
		string _MountPoint;
		string _FileSystem;
		string _MountOpts;
		CurrentOptions _CurrentOptions;
		//string _CurrentMountOptions;
		string _MkfsOpts;
		//string _CurrentMkfsOptions;
				
		set<CurrentOptions>::iterator _Index;
		FileSystems _FSIndex;		
		set<CurrentOptions> _AdditionalOptions[FS_UNSUPPORTED];		
		
		static bool IsSpecialOption(const string & opt);
		static bool IsSpecialOptionEnabled(const string & opt);
		static bool IsOptionEnabledInternal(const string & opt, int * position = 0);
		bool LoadConfiguration();
		static bool CreateFilesystem(string fs, string partition, bool resizeFlag = false, string mkfs_opts = "");
		FileSystems GetFSNumber(string FSName);
		
		static bool IsFlag(string optionName);
		static bool IsFlagEnabled(string optionName);
		
		static std::map<string, unsigned long> MountFlagMap;
};

#endif /* PARTITIONMANAGER_HPP */

