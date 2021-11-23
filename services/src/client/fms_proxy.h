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
#pragma once

#include "fms_service_stub.h"
#include "ifms_client.h"
#include "iremote_proxy.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace FileManagerService {
class FileManagerProxy : public IRemoteProxy<IFileManagerService>, public IFmsClient {
public:
    explicit FileManagerProxy(const sptr<IRemoteObject> &impl);
    virtual ~FileManagerProxy() = default;
    static IFmsClient* GetFmsInstance();
    int mkdir(std::string name, std::string path);
    int ListFile(std::string path, int off, int count, std::vector<FileInfo> &fileRes);
    int CreateFile(std::string name, std::string path, std::string &uri);

private:
    static inline BrokerDelegator<FileManagerProxy> delegator_;
};
} // namespace FileManagerService
} // namespace OHOS