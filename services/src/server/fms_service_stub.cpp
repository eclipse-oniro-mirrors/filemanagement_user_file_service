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

#include "fms_service_stub.h"

#include "fms_const.h"
#include "fms_oper_factory.h"
#include "fms_service.h"
#include "log.h"

using namespace std;

namespace OHOS {
namespace FileManagerService {
int FileManagerServiceStub::OperMediaProcess(OperFactory &factory, uint32_t code, MessageParcel &data,
    MessageParcel &reply)
{
    int errCode = SUCCESS;
    if (code < FMS_MEDIA_START || code > FMS_MEDIA_END) {
        return errCode;
    }
    auto *fp = factory.getFileOper("media");
    // media process
    switch(code) {
        case FMS_MEDIA_MKDIR: {
            string name = data.ReadString();
            string path = data.ReadString();
            errCode = fp->mkdir(name, path);
            break;
        }
        case FMS_MEDIA_LISTFILE: {
            string path = data.ReadString();
            int off = data.ReadInt32();
            int count = data.ReadInt32();
            errCode = fp->ListFile(path, off, count, reply);
            // need reply fileInfo
            break;
        }
        case FMS_MEDIA_CREATEFILE: {
            string name = data.ReadString();
            string path = data.ReadString();
            string uri;
            errCode = fp->CreateFile(name, path, uri);
            reply.WriteString(uri);
            break;
        }
        default:
            break;
    }
    delete fp;
    return errCode;
}

int FileManagerServiceStub::OperExtProcess(OperFactory &factory, uint32_t code, MessageParcel &data,
    MessageParcel &reply)
{
    int errCode = SUCCESS;
    if (code < FMS_EXTERNAL_START || code > FMS_EXTERNAL_END) {
        return errCode;
    }
    // do Exteranl storage process;
    return errCode;
}

int FileManagerServiceStub::OperProcess(uint32_t code, MessageParcel &data,
    MessageParcel &reply)
{
    int errCode = SUCCESS;

    switch (code) {
        case FMS_GET_ROOT: {
            // return root base on type
            // return fileInfo
            break;
        }
        default: {
            OperFactory factory = OperFactory();
            // check uri -->Media or --> External
            errCode = OperMediaProcess(factory, code, data, reply);
        }
    }
    return errCode;
}

int FileManagerServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    DEBUG_LOG("OnRemoteRequest %{public}d", code);

    // to do checkpermission()
    // do file process
    int32_t errCode = OperProcess(code, data, reply);
    reply.WriteInt32(errCode);
    return errCode;
}

} // namespace FileManagerService
} // namespace OHOS