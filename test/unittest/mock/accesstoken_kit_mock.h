/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef TEST_UNITTEST_MOCK_FILE_ACCESSTOKEN_KIT_MOCK_H
#define TEST_UNITTEST_MOCK_FILE_ACCESSTOKEN_KIT_MOCK_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "accesstoken_kit.h"


namespace OHOS {
namespace FileAccessFwk {
using namespace OHOS::Security::AccessToken;
class FileAccessAccesstokenMock {
public:
virtual ~FileAccessAccesstokenMock() = default;
virtual int VerifyAccessToken(AccessTokenID tokenID, const std::string &permissionName) = 0;
static inline std::shared_ptr<FileAccessAccesstokenMock> fileAccessAccesstokenMock = nullptr;
};
class AccesstokenMock : public FileAccessAccesstokenMock {
public:
    MOCK_METHOD2(VerifyAccessToken,
                 int(AccessTokenID tokenID, const std::string &permissionName));
};
} // End of namespace FileAccessFwk
} // End of namespace OHOS
#endif