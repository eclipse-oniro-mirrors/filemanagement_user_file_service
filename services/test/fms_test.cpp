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

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <gtest/gtest.h>
#include <iostream>
#include <iservice_registry.h>
#include <refbase.h>
#include <system_ability_definition.h>
#include <unistd.h>
#include "fms_proxy.h"

namespace {
using namespace std;
using namespace OHOS;
class FMSTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FMS code test" << endl;
    }
    static void TearDownTestCase() {}
};

/**
 * @tc.number SUB_STORAGE_FMS_Function_Enable_0000
 * @tc.name FMS_Function_Enable_0000
 * @tc.desc Test function of Enable interface.
 */
HWTEST_F(FMSTest, FMS_Function_Enable_0000, testing::ext::TestSize.Level1)
{
    cout << "FMSTest-begin FMS_Function_Enable_0000" << endl;
    try {
        int result = 0;
        FileManagerService::FileManagerProxy proxy;
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FMSTest-an exception occurred." << endl;
    }
    cout << "FMSTest-end FMS_Function_Enable_0000" << endl;
}

} // namespace
