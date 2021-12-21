/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "file_manager_proxy.h"

#include "file_manager_service_const.h"
#include "file_info.h"
#include "file_manager_service_stub.h"
#include "log.h"

using namespace std;

namespace OHOS {
namespace FileManagerService {
int GetFileInfo(FileInfo &file, MessageParcel &reply)
{
    string path;
    string name;
    string type;
    int64_t size = 0;
    int64_t at = 0;
    int64_t mt = 0;

    reply.ReadString(path);
    reply.ReadString(type);
    reply.ReadString(name);
    reply.ReadInt64(size);
    reply.ReadInt64(at);
    reply.ReadInt64(mt);
    file = FileInfo(name, path, type, size, at, mt);
    return SUCCESS;
}

FileManagerProxy::FileManagerProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IFileManagerService>(impl) { }

int FileManagerProxy::CreateFile(string name, string path, string &uri)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> remote = Remote();
    data.WriteString(name);
    data.WriteString(path);
    int err = remote->SendRequest(FILE_OPER::CREATE_FILE, data, reply, option);
    if (err != ERR_NONE) {
        ERR_LOG("FileManagerProxy::CreateFile send request fail %{public}d", err);
        return err;
    }
    reply.ReadString(uri);
    reply.ReadInt32(err);
    return err;
}

IFmsClient *IFmsClient::GetFmsInstance()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        ERR_LOG("samgr object is NULL.");
        return nullptr;
    }
    sptr<IRemoteObject> object = samgr->GetSystemAbility(FILE_MANAGER_SERVICE_ID);
    if (object == nullptr) {
        ERR_LOG("FileManager Service object is NULL.");
        return nullptr;
    }
    static FileManagerProxy proxy(object);
    return &proxy;
}

int FileManagerProxy::ListFile(string path, int off, int count, vector<FileInfo> &fileRes)
{
    int err;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> remote = Remote();
    data.WriteString(path);
    data.WriteInt32(off);
    data.WriteInt32(count);
    err = remote->SendRequest(FILE_OPER::LIST_FILE, data, reply, option);
    if (err != ERR_NONE) {
        ERR_LOG("FileManagerProxy::ListFile err %{public}d", err);
        return err;
    }
    int fileInfoNum = 0;
    reply.ReadInt32(fileInfoNum);
    while (fileInfoNum) {
        FileInfo file;
        GetFileInfo(file, reply);
        fileRes.emplace_back(file);
        fileInfoNum--;
    }
    reply.ReadInt32(err);
    return err;
}

int FileManagerProxy::Mkdir(string name, string path)
{
    int err;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> remote = Remote();
    data.WriteString(name);
    data.WriteString(path);
    err = remote->SendRequest(FILE_OPER::MKDIR, data, reply, option);
    if (err != ERR_NONE) {
        ERR_LOG("FileManagerProxy::mkdir err %{public}d", err);
        return err;
    }
    reply.ReadInt32(err);
    return err;
}
} // FileManagerService
} // namespace OHOS