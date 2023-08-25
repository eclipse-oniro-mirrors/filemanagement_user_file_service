/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INTERFACES_KITS_NATIVE_SRC_TRASH_FILE_TRASH_N_EXPORTER_H
#define INTERFACES_KITS_NATIVE_SRC_TRASH_FILE_TRASH_N_EXPORTER_H

#include "uv.h"

#include "filemgmt_libn.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Trash {
using namespace FileManagement::LibN;
using namespace std;
// 暂时默认用户100
const std::string TRASH_PATH = "/storage/.Trash/Users/100";
const std::string URI_PATH_PREFIX = "file://docs";
const std::string TRASH_SUB_DIR = "oh_trash_content";

class FileTrashNExporter final : public NExporter {
public:
    inline static const std::string className_ = "trash";
    
    static napi_value ListFile(napi_env env, napi_callback_info info);
    static napi_value Recover(napi_env env, napi_callback_info info);
    static napi_value CompletelyDelete(napi_env env, napi_callback_info info);

    bool Export() override;
    std::string GetClassName() override;
    FileTrashNExporter(napi_env env, napi_value exports);
    ~FileTrashNExporter() override;
};

} // namespace Trash
} // namespace OHOS
#endif // INTERFACES_KITS_NATIVE_SRC_TRASH_FILE_TRASH_N_EXPORTER_H