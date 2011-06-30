#include "SyscallTest.hpp"
#include "DupFileDescriptor.hpp"
#include "ReadWriteFile.hpp"
#include "Close.hpp"
#include "Link.hpp"
#include "Chmod.hpp"
#include "fcntlFD.hpp"
#include "getdents.hpp"
#include "IoctlTest.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;

	tests.AddTest(new DupFileDescriptorTest(Normal, Dup));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadBadFileDescriptor1, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadBadFileDescriptor2, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadEinvalError, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadIsdirError, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadEfaultError, ""));

	tests.AddTest(new Close(Normal, BadFileDescriptor, ""));
 	tests.AddTest(new Close(Normal, CorrectDescriptor, ""));
	tests.AddTest(new Close(Normal, CorrectDescriptorUnlink, ""));

	tests.AddTest(new Chmod(Normal, CHMOD_S_IRWXU, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IRUSR, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IWUSR, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IXUSR, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IRWXG, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IRGRP, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IWGRP, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IXGRP, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IRWXO, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IROTH, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IWOTH, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IXOTH, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_ISUID, ""));
    tests.AddTest(new Chmod(Normal,  CHMOD_ERR_ENAMETOOLONG, ""));
    tests.AddTest(new Chmod(Normal,  CHMOD_ERR_ENOENT, ""));
    tests.AddTest(new Chmod(Normal,  CHMOD_ERR_ENOTDIR, ""));

	tests.AddTest(new LinkTest(Normal, LinkTestTooLongOldPath, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestTooLongNewPath, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestNewPathAleadyExist, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestOldPathIsDirectory, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestNormalFile, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestIsNotDirectory, ""));

	tests.AddTest(new fcntlFD(Normal, fcntlFDGetSetFileDescriptorFlags, ""));
	tests.AddTest(new fcntlFD(Normal, fcntlFDGetSetFileStatusFlags, ""));
	tests.AddTest(new fcntlFD(Normal, fcntlFDGetSetFileStatusFlagsIgnore, ""));
	tests.AddTest(new fcntlFD(Normal, fcntlFDGetSetFileStatusFlagsIgnoreRDONLY, ""));
	
	tests.AddTest(new IoctlTest(Normal, IOCTL_FS_SECRM_FL, ""));
	tests.AddTest(new IoctlTest(Normal, IOCTL_FS_NOATIME_FL, ""));
	tests.AddTest(new IoctlTest(Normal, IOCTL_FS_NODUMP_FL, ""));
	tests.AddTest(new IoctlTest(Normal, IOCTL_FS_APPEND_FL, ""));
	tests.AddTest(new IoctlTest(Normal, IOCTL_FS_IMMUTABLE_FL, ""));
	tests.AddTest(new IoctlTest(Normal, IOCTL_FS_SYNC_FL, ""));
	tests.AddTest(new IoctlTest(Normal, IOCTL_FS_COMPR_FL, ""));
	tests.AddTest(new IoctlTest(Normal, IOCTL_INVALID_FD, ""));
   	tests.AddTest(new IoctlTest(Normal, IOCTL_INVALID_ARGP, ""));

	tests.AddTest(new GetDentsTest(Normal, GetDentsEbadfError, ""));
 	tests.AddTest(new GetDentsTest(Normal, GetDentsEfaultError, ""));
	tests.AddTest(new GetDentsTest(Normal, GetDentsEinvalError, ""));
	tests.AddTest(new GetDentsTest(Normal, GetDentsEnotdirError, ""));
	tests.AddTest(new GetDentsTest(Normal, GetDentsGeneral, ""));

	TestResultCollection res = tests.Run();

	cout << res.ToXML() << endl;
}
