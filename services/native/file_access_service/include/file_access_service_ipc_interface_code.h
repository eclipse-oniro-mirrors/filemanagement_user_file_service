/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_SERVICE_IPC_INTERFACE_CODE_H
#define FILE_ACCESS_SERVICE_IPC_INTERFACE_CODE_H

namespace OHOS {
namespace FileAccessFwk {
    enum class FileAccessServiceInterfaceCode {
        CMD_REGISTER_NOTIFY = 0,
        CMD_UNREGISTER_NOTIFY,
        CMD_ONCHANGE,
        CMD_GET_EXTENSION_PROXY,
        CMD_CONNECT_FILE_EXT_ABILITY,
        CMD_DISCONNECT_FILE_EXT_ABILITY
    };
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_IPC_INTERFACE_CODE_H