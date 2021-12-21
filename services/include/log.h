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

#include <stdio.h>
#ifndef FILE_SUBSYSTEM_DEV_ON_PC
#include "hilog/log.h"
#endif

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0xD002B00
#define LOG_TAG "Storage:FMS"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#ifndef FILE_SUBSYSTEM_DEV_ON_PC
#ifndef OHOS_DEBUG
#define DECORATOR_HILOG(op, fmt, args...) \
    do {                                  \
        op(LOG_CORE, fmt, ##args);        \
    } while (0)
#else
#define DECORATOR_HILOG(op, fmt, args...)                                                \
    do {                                                                                 \
        op(LOG_CORE, "{%s()-%s:%d} " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##args); \
    } while (0)
#endif

#define DEBUG_LOG(fmt, args...) HILOG_DEBUG(LOG_CORE, "{%{public}s()-%{public}s:%{public}d} " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##args);
#define ERR_LOG(fmt, args...) HILOG_ERROR(LOG_CORE, "{%{public}s()-%{public}s:%{public}d} " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##args);
#define WARNING_LOG(fmt, args...) HILOG_WARN(LOG_CORE, "{%{public}s()-%{public}s:%{public}d} " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##args);
#define INFO_LOG(fmt, args...) HILOG_INFO(LOG_CORE, "{%{public}s()-%{public}s:%{public}d} " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##args);
#define FATAL_LOG(fmt, args...) HILOG_FATAL(LOG_CORE, "{%{public}s()-%{public}s:%{public}d} " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##args);

#else
#define PCLOG(fmt, ...)                                                  \
    do {                                                                 \
        const std::vector<std::string> filter = {                        \
            "{public}",                                                  \
            "{private}",                                                 \
        };                                                               \
        std::string str____(fmt);                                        \
        for (auto &&pattern : filter) {                                  \
            size_t pos = 0;                                              \
            while (std::string::npos != (pos = str____.find(pattern))) { \
                str____.erase(pos, pattern.length());                    \
            }                                                            \
        }                                                                \
        str____ += "\n";                                                 \
        printf(str____.c_str(), ##__VA_ARGS__);                          \
    } while (0);

#define DEBUG_LOG(fmt, ...) PCLOG("%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define INFO_LOG(fmt, ...) PCLOG("%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define WARNING_LOG(fmt, ...) PCLOG("%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define ERR_LOG(fmt, ...) PCLOG("%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define FATAL_LOG(fmt, ...) PCLOG("%{public}s: " fmt, __func__, ##__VA_ARGS__)
#endif

#define OK 0
#define INVALID_PARAM (-1)
#define INIT_FAIL (-2)
#define ERR (-3)
#define PERMISSION_DENIED (-4)
