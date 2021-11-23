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

#include <vector>
namespace OHOS {
namespace FileManagerService {
enum {
    FMS_GET_ROOT,
    FMS_MEDIA_START,
    FMS_MEDIA_MKDIR,
    FMS_MEDIA_LISTFILE,
    FMS_MEDIA_CREATEFILE,
    FMS_MEDIA_END,
    FMS_EXTERNAL_START,
    FMS_EXTERNAL_END,
};
const int32_t SUCCESS = 0;
const int32_t FAIL = -1;
const int32_t E_NOEXIST = -2;
const int32_t E_EMPTYFOLDER = -3;

} // namespace FileManagerService
} // namespace OHOS
