/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#pragma once

#include "common/napi/n_exporter.h"

namespace OHOS {
namespace DistributedFS {
namespace FMS_NAPI {
class FMS_NAPI final : public NExporter {
public:
    inline static const std::string className_ = "__properities__";

    static napi_value CreateFile(napi_env env, napi_callback_info info);
    static napi_value ListFile(napi_env env, napi_callback_info info);
    static napi_value mkdir(napi_env env, napi_callback_info info);
    bool Export() override;
    std::string GetClassName() override;

    FMS_NAPI(napi_env env, napi_value exports);
    ~FMS_NAPI() override;
};
} // namespace FMS_NAPI
} // namespace DistributedFS
} // namespace OHOS