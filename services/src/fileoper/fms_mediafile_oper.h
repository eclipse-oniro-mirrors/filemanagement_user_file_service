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

#include <string>
#include "fms_file_oper.h"

namespace OHOS {
namespace FileManagerService {
class MediaFileOper : public FileOper {
public:
    virtual ~MediaFileOper() = default;
    int mkdir(const std::string &name, const std::string &path) override;
    int ListFile(const std::string &path, int offset, int count, MessageParcel &data) override;
    int CreateFile(const std::string &name, const std::string &path, std::string &uri) override;
};
} // OHOS
} // FileManager