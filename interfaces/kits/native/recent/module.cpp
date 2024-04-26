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

#include <memory>
#include <vector>

#include "filemgmt_libn.h"
#include "hilog_wrapper.h"
#include "recent_n_exporter.h"

namespace OHOS::FileManagement::Recent {
using namespace std;
using namespace LibN;

static napi_value Export(napi_env env, napi_value exports)
{
    std::vector<unique_ptr<NExporter>> products;
    products.emplace_back(make_unique<RecentNExporter>(env, exports));
    for (auto &&product : products) {
        if (!product->Export()) {
            HILOG_ERROR("INNER BUG. Failed to export class %{public}s for module recent",
                product->GetClassName().c_str());
            return nullptr;
        } else {
            HILOG_DEBUG("Class %{public}s for module fileio has been exported", product->GetClassName().c_str());
        }
    }

    return exports;
}

static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Export,
    .nm_modname = "file.recent",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
} // namespace OHOS::AppFileService::ModuleFileUri