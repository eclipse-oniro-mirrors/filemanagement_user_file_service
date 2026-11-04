/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "ipc_skeleton.h"
#include "ufs_access_token_helper.h"

namespace {
pid_t g_mockCallingAppUid = 0;
} // namespace

namespace OHOS {
pid_t IPCSkeleton::GetCallingAppUid()
{
    return g_mockCallingAppUid;
}
} // namespace OHOS

namespace OHOS::FileAccessFwk {
namespace {
constexpr int32_t BASE_USER_RANGE = 200000;
}

TEST(FileAccessHelperUidTest, GetUserIdFromAppUid)
{
    g_mockCallingAppUid = 100 * BASE_USER_RANGE + 10000;
    EXPECT_EQ(UfsAccessTokenHelper::GetUserId(), 100);
}

TEST(FileAccessHelperUidTest, GetUserIdFromAppUidDefaultUser)
{
    g_mockCallingAppUid = 10000;
    EXPECT_EQ(UfsAccessTokenHelper::GetUserId(), 0);
}

TEST(FileAccessHelperUidTest, GetUserIdFromAppUidLegacyApp)
{
    g_mockCallingAppUid = 101 * BASE_USER_RANGE + 20000;
    EXPECT_EQ(UfsAccessTokenHelper::GetUserId(), 101);
}
} // namespace OHOS::FileAccessFwk
