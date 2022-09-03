/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "file_access_ext_stub.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_object_stub.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
}
FileAccessExtStub::FileAccessExtStub()
{
    stubFuncMap_[CMD_OPEN_FILE] = &FileAccessExtStub::CmdOpenFile;
    stubFuncMap_[CMD_CREATE_FILE] = &FileAccessExtStub::CmdCreateFile;
    stubFuncMap_[CMD_MKDIR] = &FileAccessExtStub::CmdMkdir;
    stubFuncMap_[CMD_DELETE] = &FileAccessExtStub::CmdDelete;
    stubFuncMap_[CMD_MOVE] = &FileAccessExtStub::CmdMove;
    stubFuncMap_[CMD_RENAME] = &FileAccessExtStub::CmdRename;
    stubFuncMap_[CMD_LIST_FILE] = &FileAccessExtStub::CmdListFile;
    stubFuncMap_[CMD_GET_ROOTS] = &FileAccessExtStub::CmdGetRoots;
    stubFuncMap_[CMD_ACCESS] = &FileAccessExtStub::CmdAccess;
    stubFuncMap_[CMD_REGISTER_NOTIFY] = &FileAccessExtStub::CmdRegisterNotify;
    stubFuncMap_[CMD_UNREGISTER_NOTIFY] = &FileAccessExtStub::CmdUnregisterNotify;
}

FileAccessExtStub::~FileAccessExtStub()
{
    stubFuncMap_.clear();
}

int FileAccessExtStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnRemoteRequest");
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PERMISSION_DENIED;
    }

    std::u16string descriptor = FileAccessExtStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_STATE;
    }

    const auto &itFunc = stubFuncMap_.find(code);
    if (itFunc != stubFuncMap_.end()) {
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return (this->*(itFunc->second))(data, reply);
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode FileAccessExtStub::CmdOpenFile(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdOpenFile");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("Parameter OpenFile fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    int flags = ERR_PARCEL_FAIL;
    if (!data.ReadInt32(flags)) {
        HILOG_ERROR("Parameter OpenFile fail to ReadInt32 flags");
        return ERR_PARCEL_FAIL;
    }

    if (flags < 0) {
        HILOG_ERROR("Parameter OpenFile flags is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_PARAM;
    }

    int fd;
    int ret = OpenFile(*uri, flags, fd);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter OpenFile fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!reply.WriteFileDescriptor(fd)) {
        HILOG_ERROR("Parameter OpenFile fail to WriteFileDescriptor fd");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdCreateFile(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdCreateFile");
    std::shared_ptr<Uri> parent(data.ReadParcelable<Uri>());
    if (parent == nullptr) {
        HILOG_ERROR("Parameter CreateFile fail to ReadParcelable parent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    std::string displayName = "";
    if (!data.ReadString(displayName)) {
        HILOG_ERROR("Parameter CreateFile fail to ReadString displayName");
        return ERR_PARCEL_FAIL;
    }

    if (displayName.empty()) {
        HILOG_ERROR("Parameter CreateFile displayName is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_PARAM;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int ret = CreateFile(*parent, displayName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter CreateFile fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!reply.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("Parameter CreateFile fail to WriteParcelable newFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdMkdir(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdMkdir");
    std::shared_ptr<Uri> parent(data.ReadParcelable<Uri>());
    if (parent == nullptr) {
        HILOG_ERROR("Parameter Mkdir fail to ReadParcelable parent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    std::string displayName = "";
    if (!data.ReadString(displayName)) {
        HILOG_ERROR("Parameter Mkdir fail to ReadString displayName");
        return ERR_PARCEL_FAIL;
    }

    if (displayName.empty()) {
        HILOG_ERROR("Parameter Mkdir displayName is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_PARAM;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int ret = Mkdir(*parent, displayName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Mkdir fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!reply.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("Parameter Mkdir fail to WriteParcelable newFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdDelete(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdDelete");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("Parameter Delete fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    int ret = Delete(*uri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Delete fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdMove(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdMove");
    std::shared_ptr<Uri> sourceFile(data.ReadParcelable<Uri>());
    if (sourceFile == nullptr) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    std::shared_ptr<Uri> targetParent(data.ReadParcelable<Uri>());
    if (targetParent == nullptr) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable targetParent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int ret = Move(*sourceFile, *targetParent, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Move fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!reply.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("Parameter Move fail to WriteParcelable newFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdRename(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdRename");
    std::shared_ptr<Uri> sourceFile(data.ReadParcelable<Uri>());
    if (sourceFile == nullptr) {
        HILOG_ERROR("Parameter Rename fail to ReadParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    std::string displayName = "";
    if (!data.ReadString(displayName)) {
        HILOG_ERROR("Parameter Rename fail to ReadString displayName");
        return ERR_PARCEL_FAIL;
    }

    if (displayName.empty()) {
        HILOG_ERROR("Parameter Rename displayName is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_PARAM;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int ret = Rename(*sourceFile, displayName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Rename fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!reply.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("Parameter Rename fail to WriteParcelable newFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdListFile(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdListFile");
    std::shared_ptr<FileInfo> fileInfo(data.ReadParcelable<FileInfo>());
    if (fileInfo == nullptr) {
        HILOG_ERROR("Parameter ListFile fail to ReadParcelable fileInfo");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    int64_t offset = 0;
    if (!data.ReadInt64(offset)) {
        HILOG_ERROR("parameter ListFile offset is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    int64_t maxCount = 0;
    if (!data.ReadInt64(maxCount)) {
        HILOG_ERROR("parameter ListFile maxCount is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    std::vector<FileInfo> fileInfoVec;
    int ret = ListFile(*fileInfo, offset, maxCount, fileInfoVec);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter ListFile fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    int64_t count {fileInfoVec.size()};
    if (!reply.WriteInt64(count)) {
        HILOG_ERROR("Parameter ListFile fail to WriteInt64 count");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    for (const auto &fileInfo : fileInfoVec) {
        if (!reply.WriteParcelable(&fileInfo)) {
            HILOG_ERROR("parameter ListFile fail to WriteParcelable fileInfoVec");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return ERR_PARCEL_FAIL;
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdGetRoots(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdGetRoots");

    std::vector<RootInfo> rootInfoVec;
    int ret = GetRoots(rootInfoVec);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter GetRoots fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    int64_t count {rootInfoVec.size()};
    if (!reply.WriteInt64(count)) {
        HILOG_ERROR("Parameter GetRoots fail to WriteInt64 count");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    for (const auto &rootInfo : rootInfoVec) {
        if (!reply.WriteParcelable(&rootInfo)) {
            HILOG_ERROR("parameter ListFile fail to WriteParcelable rootInfo");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return ERR_PARCEL_FAIL;
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdAccess(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdAccess");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("Access uri is nullptr");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_URI;
    }

    bool isExist = data.ReadBool();
    int ret = Access(*uri, isExist);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Access fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!reply.WriteBool(isExist)) {
        HILOG_ERROR("Parameter Access fail to WriteBool isExist");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

bool FileAccessExtStub::CheckCallingPermission(const std::string &permission)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdGetRoots");
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    int res = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission);
    if (res != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HILOG_ERROR("FileAccessExtStub::CheckCallingPermission have no fileAccess permission");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return false;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return true;
}

ErrCode FileAccessExtStub::CmdRegisterNotify(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdRegisterNotify");
    auto remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_INFO("get remote obj fail.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    auto notify = iface_cast<IFileAccessNotify>(remote);
    if (notify == nullptr) {
        HILOG_INFO("get notify fail");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    int ret = RegisterNotify(notify);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter RegisterNotify fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdUnregisterNotify(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdUnregisterNotify");
    auto remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_INFO("get remote obj fail.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    auto notify = iface_cast<IFileAccessNotify>(remote);
    if (notify == nullptr) {
        HILOG_INFO("get notify fail");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    int ret = UnregisterNotify(notify);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter UnregisterNotify fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS