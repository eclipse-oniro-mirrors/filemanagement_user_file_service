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
#include <cstdint>
#include "ipc_types.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace FileManagerService {
class FileInfo {
public:
    FileInfo() {};
    FileInfo(const std::string &name, const std::string &path, const std::string &type, int64_t size, int64_t added_time,
        int64_t modified_time) : path_(path), name_(name), type_(type), size_(size),
        added_time_(added_time), modified_time_(modified_time) {}
    std::string GetName()
    {
        return name_;
    }
    std::string GetPath()
    {
        return path_;
    }
    std::string GetType()
    {
        return type_;
    }
    int64_t GetSize()
    {
        return size_;
    }
    int64_t GetAdded_Time()
    {
        return added_time_;
    }
    int64_t GetModified_time()
    {
        return modified_time_;
    }
private:
    std::string path_;
    std::string name_;
    std::string type_;
    int64_t size_;
    int64_t added_time_;
    int64_t modified_time_;
};
} // OHOS
} // FileManager