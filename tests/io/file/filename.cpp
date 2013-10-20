#include <common.h>
#include <pspiofilemgr.h>

struct TestFile {
	TestFile(const char *fn) : filename(fn) {
		int fd = sceIoOpen(filename, PSP_O_CREAT | PSP_O_WRONLY, 0777);
		valid_ = fd >= 0;
		if (valid_) {
			sceIoClose(fd);
		}
	}

	~TestFile() {
		if (valid_) {
			sceIoRemove(filename);
		}
	}

	bool IsValid() {
		return valid_;
	}

	void TestExists(const char *title) {
		TestExists(title, filename);
	}

	static void TestExists(const char *title, const char *fn) {
		int fd = sceIoOpen(fn, PSP_O_RDONLY, 0777);
		if (fd >= 0) {
			sceIoClose(fd);
			checkpoint("%s: Success", title);
		} else {
			checkpoint("%s: Failed (%08x)", title, fd);
		}
	}

	bool valid_;
	const char *filename;
};

void testCreate(const char *title, const char *fn) {
	TestFile f(fn);
	f.TestExists(title);
}

void checkSlashes() {
	checkpointNext("Slash usage");
	
	testCreate("  Doubled slashes at device", "ms0://PSP/__pspautotests_testfile1.txt");
	testCreate("  Doubled slashes in path", "ms0:/PSP//__pspautotests_testfile1.txt");
	testCreate("  Backslash at device", "ms0:\\PSP/__pspautotests_testfile3.txt");
	testCreate("  Backslash in path", "ms0:/PSP\\__pspautotests_testfile4.txt");
	testCreate("  Backslash after bad dir", "ms0:/PSP/__pspautotests_testdir\\file.txt");
	testCreate("  Double backslashes at device", "ms0:\\\\PSP/__pspautotests_testfile5.txt");
	testCreate("  Double backslashes in path", "ms0:/PSP\\\\__pspautotests_testfile6.txt");
}

void checkTrailingDot() {
	checkpointNext("Trailing dots");

	TestFile f("ms0:/PSP/__pspautotests_testfileA.");
	f.TestExists("  With dot");
	TestFile::TestExists("  Without dot", "ms0:/PSP/__pspautotests_testfileA");

	TestFile f2("ms0:/PSP/__pspautotests_testfileB....");
	f2.TestExists("  Five dots");
	TestFile::TestExists("  Without any", "ms0:/PSP/__pspautotests_testfileB");
}

void checkTrailingSpace() {
	checkpointNext("Trailing spaces");

	testCreate("  Trailing space", "ms0:/PSP/__pspautotests_testfile ");
}

void checkLeadingSpace() {
	checkpointNext("Leading spaces");

	testCreate("  Before device", "   ms0:/PSP/__pspautotests_testfileleading.txt");
	testCreate("  Before filename", "ms0:/PSP/     __pspautotests_testfileleading.txt");
	testCreate("  Before path", "ms0:/     PSP/__pspautotests_testfileleading.txt");
	testCreate("  After colon", "ms0:    /PSP/__pspautotests_testfileleading.txt");
}

void checkDeviceCase() {
	checkpointNext("Device name case");

	testCreate("  Lowercase", "ms0:/PSP/__pspautotests_testfile1.txt");
	testCreate("  Uppercase", "MS0:/PSP/__pspautotests_testfile2.txt");
	testCreate("  Mixed", "mS0:/PSP/__pspautotests_testfile2.txt");
}

void checkPathCase() {
	checkpointNext("Path/filename case");

	TestFile f("ms0:/PSP/__pspautotests_testfile1");
	f.TestExists("  Lowercase");
	TestFile::TestExists("  Uppercase", "ms0:/PSP/__PSPAUTOTESTS_TESTFILE1");
	TestFile::TestExists("  Mixed case", "ms0:/PSP/__PsPaUtOtEsTs_tEsTfIlE1");
}

void checkSpecialChars() {
	checkpointNext("Special characters");

	testCreate("  *", "ms0:/PSP/__pspautotests_testfile*.txt");
	testCreate("  .", "ms0:/PSP/__pspautotests_testfile..txt");
	testCreate("  $", "ms0:/PSP/__pspautotests_testfile$.txt");
	testCreate("  :", "ms0:/PSP/__pspautotests_testfile:.txt");
	testCreate("  [", "ms0:/PSP/__pspautotests_testfile[.txt");
	testCreate("  (", "ms0:/PSP/__pspautotests_testfile(.txt");
	testCreate("  <", "ms0:/PSP/__pspautotests_testfile<.txt");
	testCreate("  |", "ms0:/PSP/__pspautotests_testfile|.txt");
	testCreate("  +", "ms0:/PSP/__pspautotests_testfile+.txt");
	testCreate("  ~", "ms0:/PSP/__pspautotests_testfile~.txt");
	testCreate("  &", "ms0:/PSP/__pspautotests_testfile&.txt");
	testCreate("  %", "ms0:/PSP/__pspautotests_testfile%.txt");
	testCreate("  #", "ms0:/PSP/__pspautotests_testfile#.txt");
	testCreate("  ?", "ms0:/PSP/__pspautotests_testfile?.txt");
	testCreate("  '", "ms0:/PSP/__pspautotests_testfile'.txt");
	testCreate("  \"", "ms0:/PSP/__pspautotests_testfile\".txt");
}

void checkRelative() {
	checkpointNext("Relative paths");

	testCreate("  With ignored components", "ms0:/PSP/blah/../__pspautotests_testfile.txt");

	sceIoChdir("ms0:/PSP");
	testCreate("  Relative", "__pspautotests_testfile.txt");
	testCreate("  Relative inside root", "../PSP/__pspautotests_testfile.txt");
	testCreate("  Relative ouside root", "../../PSP/__pspautotests_testfile.txt");
}

void checkUMD() {
	checkpointNext("disc0:/ paths");

	TestFile::TestExists("  Uppercase", "disc0:/PSP_GAME/PARAM.SFO");
	TestFile::TestExists("  Lowercase", "disc0:/psp_game/param.sfo");
	TestFile::TestExists("  Backslash at device", "disc0:\\PSP_GAME/PARAM.SFO");
	TestFile::TestExists("  Backslash in path", "disc0:/PSP_GAME\\PARAM.SFO");
	TestFile::TestExists("  Trailing dot", "disc0:/PSP_GAME/PARAM.SFO.");
	TestFile::TestExists("  Trailing space", "disc0:/PSP_GAME/PARAM.SFO ");
	TestFile::TestExists("  Special char", "disc0:/PSP_GAME/PARAM.SFO*");
	TestFile::TestExists("  Relative components", "disc0:/blah/../PSP_GAME/PARAM.SFO");

	sceIoChdir("disc0:/PSP_GAME");
	TestFile::TestExists("  Relative", "PARAM.SFO");
	TestFile::TestExists("  Relative inside root", "../PSP_GAME/PARAM.SFO");
	TestFile::TestExists("  Relative outside root", "../../PSP_GAME/PARAM.SFO");
}

extern "C" int main(int argc, char *argv[]) {
	checkSlashes();
	checkTrailingDot();
	checkTrailingSpace();
	checkLeadingSpace();
	checkDeviceCase();
	checkPathCase();
	checkSpecialChars();
	checkRelative();
	checkUMD();

	return 0;
}