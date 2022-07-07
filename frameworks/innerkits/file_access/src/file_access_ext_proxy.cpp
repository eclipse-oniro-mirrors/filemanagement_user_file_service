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

#include "file_access_ext_proxy.h"

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"

namespace OHOS {
namespace FileAccessFwk {
int FileAccessExtProxy::OpenFile(const Uri &uri, int flags)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OpenFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteInt32(flags)) {
        HILOG_ERROR("fail to WriteString mode");
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_OPEN_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return ERR_IPC_ERROR;
    }

    int fd = reply.ReadFileDescriptor();
    if (fd <= ERR_ERROR) {
        HILOG_ERROR("fail to ReadFileDescriptor fd");
        return ERR_IPC_ERROR;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return fd;
}

int FileAccessExtProxy::CreateFile(const Uri &parent, const std::string &displayName,  Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CreateFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString mode");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("fail to WriteParcelable newFile");
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_CREATE_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return ERR_IPC_ERROR;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return ERR_IPC_ERROR;
    }

    newFile = Uri(*tempUri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtProxy::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Mkdir");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("fail to WriteParcelable newFile");
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MKDIR, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return ERR_IPC_ERROR;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        ret = ERR_IPC_ERROR;
        return ret;
    }

    newFile = Uri(*tempUri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtProxy::Delete(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Delete");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_DELETE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return ERR_IPC_ERROR;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtProxy::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Move");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&targetParent)) {
        HILOG_ERROR("fail to WriteParcelable targetParent");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("fail to WriteParcelable newFile");
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MOVE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return ERR_IPC_ERROR;
    }

    newFile = Uri(*tempUri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtProxy::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("fail to WriteParcelable newFile");
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_RENAME, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return ERR_IPC_ERROR;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return ERR_IPC_ERROR;
    }

    newFile = Uri(*tempUri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

std::vector<FileInfo> FileAccessExtProxy::ListFile(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    std::vector<FileInfo> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return vec;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFileUri");
        return vec;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_LIST_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return vec;
    }

    vec.clear();
    int64_t count = reply.ReadInt64();
    for (int32_t i = 0; i < count; i++) {
        std::unique_ptr<FileInfo> fileInfo(reply.ReadParcelable<FileInfo>());
        if (fileInfo != nullptr) {
            vec.push_back(*fileInfo);
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return vec;
}

std::vector<DeviceInfo> FileAccessExtProxy::GetRoots()
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    std::vector<DeviceInfo> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return vec;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_GET_ROOTS, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return vec;
    }

    vec.clear();
    uint64_t count = reply.ReadUint64();
    for (uint64_t i = 0; i < count; i++) {
        std::unique_ptr<DeviceInfo> deviceInfo(reply.ReadParcelable<DeviceInfo>());
        if (deviceInfo != nullptr) {
            vec.push_back(*deviceInfo);
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return vec;
}
} // namespace FileAccessFwk
} // namespace OHOS
