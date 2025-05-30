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

#ifndef FILE_ACCESS_EXT_ABILITY_MODULE_LOADER_H
#define FILE_ACCESS_EXT_ABILITY_MODULE_LOADER_H

#include <map>
#include <memory>

#include "extension.h"
#include "extension_module_loader.h"
#include "runtime.h"
#include "singleton.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace AbilityRuntime;
class FileAccessExtAbilityModuleLoader : public ExtensionModuleLoader,
    public Singleton<FileAccessExtAbilityModuleLoader> {
    DECLARE_SINGLETON(FileAccessExtAbilityModuleLoader);

public:
    virtual Extension *Create(const std::unique_ptr<Runtime>& runtime) const override;
    virtual std::map<std::string, std::string> GetParams() override;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXT_ABILITY_MODULE_LOADER_H