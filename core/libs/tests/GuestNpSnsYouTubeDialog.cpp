#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>

extern "C" {
int APS5_VABI sceNpSnsYouTubeDialogInitialize(void);
int APS5_VABI sceNpSnsYouTubeDialogOpen(const void* param);
int APS5_VABI sceNpSnsYouTubeDialogUpdateStatus(void);
int APS5_VABI sceNpSnsYouTubeDialogGetStatus(void);
int APS5_VABI sceNpSnsYouTubeDialogGetResult(void* result);
int APS5_VABI sceNpSnsYouTubeDialogClose(void);
int APS5_VABI sceNpSnsYouTubeDialogTerminate(void);
}

namespace {

void Require(bool condition, const char* message) {
    if (!condition) {
        std::fprintf(stderr, "NpSnsYouTubeDialog: %s\n", message);
        std::abort();
    }
}

constexpr int kStatusNone = 0;
constexpr int kStatusInitialized = 1;
constexpr int kStatusFinished = 3;
constexpr int kErrNotInitialized = static_cast<int>(0x80B80003);
constexpr int kErrAlreadyInitialized = static_cast<int>(0x80B80004);
constexpr int kErrNotFinished = static_cast<int>(0x80B80005);
constexpr int kErrNotRunning = static_cast<int>(0x80B8000B);
constexpr int kErrArgNull = static_cast<int>(0x80B8000D);
constexpr int kResultUserCanceled = 1;

}

int main() {
    std::int64_t param = 0;
    std::int32_t result[2] = {-1, -1};

    Require(sceNpSnsYouTubeDialogTerminate() == kErrNotInitialized, "terminate before init");
    Require(sceNpSnsYouTubeDialogOpen(&param) == kErrNotInitialized, "open before init");
    Require(sceNpSnsYouTubeDialogClose() == kErrNotInitialized, "close before init");
    Require(sceNpSnsYouTubeDialogGetResult(result) == kErrNotInitialized, "get result before init");
    Require(sceNpSnsYouTubeDialogGetStatus() == kStatusNone, "status before init");
    Require(sceNpSnsYouTubeDialogUpdateStatus() == kStatusNone, "update status before init");

    Require(sceNpSnsYouTubeDialogInitialize() == 0, "initialize");
    Require(sceNpSnsYouTubeDialogInitialize() == kErrAlreadyInitialized, "double initialize");
    Require(sceNpSnsYouTubeDialogGetStatus() == kStatusInitialized, "status after initialize");
    Require(sceNpSnsYouTubeDialogUpdateStatus() == kStatusInitialized, "update status after initialize");

    Require(sceNpSnsYouTubeDialogClose() == kErrNotRunning, "close while initialized");
    Require(sceNpSnsYouTubeDialogOpen(nullptr) == kErrArgNull, "open with null param");
    Require(sceNpSnsYouTubeDialogGetResult(result) == kErrNotFinished, "get result before open");

    Require(sceNpSnsYouTubeDialogTerminate() == 0, "terminate from initialized");
    Require(sceNpSnsYouTubeDialogTerminate() == kErrNotInitialized, "terminate after terminate");
    Require(sceNpSnsYouTubeDialogGetStatus() == kStatusNone, "status after terminate");

    Require(sceNpSnsYouTubeDialogInitialize() == 0, "re-initialize");
    Require(sceNpSnsYouTubeDialogOpen(&param) == 0, "open");
    Require(sceNpSnsYouTubeDialogGetStatus() == kStatusFinished, "status after open");
    Require(sceNpSnsYouTubeDialogUpdateStatus() == kStatusFinished, "update status after open");

    Require(sceNpSnsYouTubeDialogGetResult(nullptr) == kErrArgNull, "get result null pointer");
    Require(sceNpSnsYouTubeDialogGetResult(result) == 0, "get result");
    Require(result[0] == kResultUserCanceled, "result canceled code");
    Require(result[1] == -1, "result untouched buffer");

    Require(sceNpSnsYouTubeDialogClose() == kErrNotRunning, "close when finished");
    Require(sceNpSnsYouTubeDialogTerminate() == 0, "terminate from finished");
    Require(sceNpSnsYouTubeDialogTerminate() == kErrNotInitialized, "terminate after terminate 2");
    Require(sceNpSnsYouTubeDialogGetStatus() == kStatusNone, "status after terminate 2");
    Require(sceNpSnsYouTubeDialogUpdateStatus() == kStatusNone, "update status after terminate 2");
}
