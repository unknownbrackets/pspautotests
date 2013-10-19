#include <common.h>
#include <pspiofilemgr.h>

u64 asyncAdapter(int fd, s64 offset, int whence) {
	int result = sceIoLseekAsync(fd, offset, whence);
	if (result < 0) {
		return result;
	}

	SceInt64 res;
	result = sceIoWaitAsync(fd, &res);
	if (result < 0) {
		return result;
	}

	return res;
}

u64 async32Adapter(int fd, int offset, int whence) {
	int result = sceIoLseek32Async(fd, offset, whence);
	if (result < 0) {
		return result;
	}

	SceInt64 res;
	result = sceIoWaitAsync(fd, &res);
	if (result < 0) {
		return result;
	}

	return res;
}

template <typename Func>
void runTests(const char *title, Func seekFunc) {
	int fd = sceIoOpen("Makefile", PSP_O_RDONLY, 0777);
	char fullTitle[256];

	snprintf(fullTitle, sizeof(fullTitle), "%s - files", title);
	checkpointNext(fullTitle);
	checkpoint("  Invalid fd: %016llx", (s64)seekFunc(0xDEADBEEF, 0, SEEK_CUR));
	checkpoint("  0: %016llx", (s64)seekFunc(0, 0, SEEK_CUR));
	checkpoint("  1: %016llx", (s64)seekFunc(1, 0, SEEK_CUR));
	checkpoint("  2: %016llx", (s64)seekFunc(2, 0, SEEK_CUR));
	checkpoint("  Valid fd: %016llx", (s64)seekFunc(fd, 0, SEEK_CUR));

	snprintf(fullTitle, sizeof(fullTitle), "%s - offsets", title);
	checkpointNext(fullTitle);
	checkpoint("  +Negative: %016llx", (s64)seekFunc(fd, -10, SEEK_CUR));
	checkpoint("  +Positive: %016llx", (s64)seekFunc(fd, 10, SEEK_CUR));
	checkpoint("  +Negative again: %016llx", (s64)seekFunc(fd, -10, SEEK_CUR));
	checkpoint("  +Zero: %016llx", (s64)seekFunc(fd, 0, SEEK_CUR));
	checkpoint("  End +1: %016llx", (s64)seekFunc(fd, 1, SEEK_END));
	checkpoint("  End +0: %016llx", (s64)seekFunc(fd, 0, SEEK_END));
	checkpoint("  End +-0x1000: %016llx", (s64)seekFunc(fd, -0x1000, SEEK_END));

	snprintf(fullTitle, sizeof(fullTitle), "%s - whence", title);
	checkpointNext(fullTitle);
	checkpoint("  END: %016llx", (s64)seekFunc(fd, 0, SEEK_END));
	checkpoint("  CUR: %016llx", (s64)seekFunc(fd, 0, SEEK_CUR));
	checkpoint("  SET: %016llx", (s64)seekFunc(fd, 0, SEEK_SET));
	checkpoint("  4: %016llx", (s64)seekFunc(fd, 0, 4));
	checkpoint("  -1: %016llx", (s64)seekFunc(fd, 0, -1));

	sceIoClose(fd);
}

extern "C" int main(int argc, char *argv[]) {
	int fd = sceIoOpen("seek.cpp", PSP_O_RDONLY, 0777);

	runTests("sceIoLseek", sceIoLseek);
	runTests("sceIoLseek32", sceIoLseek32);
	runTests("sceIoLseekAsync", asyncAdapter);
	runTests("sceIoLseek32Async", async32Adapter);

	sceIoClose(fd);
	return 0;
}