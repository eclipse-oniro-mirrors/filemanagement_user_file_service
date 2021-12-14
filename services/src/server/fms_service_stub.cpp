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
int getEquipmentCode(uint32_t code)
{
    return (code >> EQUIPMENT_SHIFT) & CODE_MASK;
}
int getOperCode(uint32_t code)
{
    return code & CODE_MASK;
}
int FileManagerServiceStub::OperProcess(uint32_t code, MessageParcel &data,
    MessageParcel &reply)
{
    int equipmentId = getEquipmentCode(code);
    int operCode = getOperCode(code);
    OperFactory factory = OperFactory();
    auto *fp = factory.getFileOper(equipmentId);
    if (fp == nullptr) {
        ERR_LOG("OnRemoteRequest inner error %{public}d", code);
        return FAIL;
    }
    int errCode = fp->OperProcess(operCode, data, reply);

    delete fp;
    return errCode;
}

int FileManagerServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    // to do checkpermission()
    // do file process
    int32_t errCode = OperProcess(code, data, reply);
    reply.WriteInt32(errCode);
    return errCode;
}

} // namespace FileManagerService
} // namespace OHOS