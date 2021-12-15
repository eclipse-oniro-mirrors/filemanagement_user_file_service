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
#include "file_manager_proxy.h"

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
 * @tc.number: SUB_STORAGE_FMS_Proxy_GetFmsInstance_0000
 * @tc.name: fms_Proxy_GetFmsInstance_0000
 * @tc.desc: Test function of GetFmsInstance interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FMSTest, FMS_Proxy_GetFmsInstance_0000, testing::ext::TestSize.Level1)
{
    cout << "FMSTest-begin FMS_Proxy_GetFmsInstance_0000" << endl;
    try {
        FileManagerService::FileManagerProxy proxy;
        IFmsClient result = proxy.GetFmsInstance();
        EXPECT_NE(result, nullptr);
    } catch (...) {
        cout << "FMSTest-an exception occurred." << endl;
    }
    cout << "FMSTest-end FMS_Proxy_GetFmsInstance_0000" << endl;
}

/**
 * @tc.number: SUB_STORAGE_FMS_Proxy_getFileInfo_0000
 * @tc.name: fms_Proxy_getFileInfo_0000
 * @tc.desc: Test function of getFileInfo interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FMSTest, FMS_Proxy_getFileInfo_0000, testing::ext::TestSize.Level1)
{
    cout << "FMSTest-begin FMS_Proxy_getFileInfo_0000" << endl;
    try {
        FileManagerService::FileManagerProxy proxy;
        FileInfo fileInfo;
        MessageParcel messageParcel;
        messageParcel.WriteString("FMS_Proxy_getFileInfo_0000");
        messageParcel.WriteString("./");
        messageParcel.WriteString("file");
        messageParcel.WriteString(0);
        messageParcel.WriteString(0);
        messageParcel.WriteString(0);
        int result = proxy.getFileInfo(fileInfo, messageParcel);
        EXPECT_EQ(result, 0);
        EXPECT_NE(fileInfo, nullptr);
    } catch (...) {
        cout << "FMSTest-an exception occurred." << endl;
    }
    cout << "FMSTest-end FMS_Proxy_getFileInfo_0000" << endl;
}

/**
 * @tc.number: SUB_STORAGE_FMS_Proxy_ListFile_0000
 * @tc.name: fms_Proxy_ListFile_0000
 * @tc.desc: Test function of ListFile interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FMSTest, FMS_Proxy_ListFile_0000, testing::ext::TestSize.Level1)
{
    cout << "FMSTest-begin FMS_Proxy_ListFile_0000" << endl;
    try {
        FileManagerService::FileManagerProxy proxy;
        vector<FileInfo> fileList;
        int result = proxy.ListFile("./", 0, 1, fileList);
        EXPECT_EQ(result, 0);
        EXPECT_NE(fileList, nullptr);
    } catch (...) {
        cout << "FMSTest-an exception occurred." << endl;
    }
    cout << "FMSTest-end FMS_Proxy_ListFile_0000" << endl;
}

/**
 * @tc.number: SUB_STORAGE_FMS_Mediafile_Insert_0000
 * @tc.name: fms_Mediafile_Insert_0000
 * @tc.desc: Test function of Insert interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FMSTest, FMS_Mediafile_Insert_0000, testing::ext::TestSize.Level1)
{
    cout << "FMSTest-begin FMS_Mediafile_Insert_0000" << endl;
    try {
        int result = MediaFileOper::Insert("FMS_Mediafile_Insert_0000", "./", "file");
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FMSTest-an exception occurred." << endl;
    }
    cout << "FMSTest-end FMS_Mediafile_Insert_0000" << endl;
}

/**
 * @tc.number: SUB_STORAGE_FMS_Mediafile_CreateFile_0000
 * @tc.name: fms_Mediafile_CreateFile_0000
 * @tc.desc: Test function of CreateFile interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FMSTest, FMS_Mediafile_CreateFile_0000, testing::ext::TestSize.Level1)
{
    cout << "FMSTest-begin FMS_Mediafile_CreateFile_0000" << endl;
    try {
        string path = "./";
        string url;
        int result = MediaFileOper::CreateFile("FMS_Mediafile_CreateFile_0000", path, url);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FMSTest-an exception occurred." << endl;
    }
    cout << "FMSTest-end FMS_Mediafile_CreateFile_0000" << endl;
}

/**
 * @tc.number: SUB_STORAGE_FMS_Mediafile_pushFileInfo_0000
 * @tc.name: fms_Mediafile_pushFileInfo_0000
 * @tc.desc: Test function of pushFileInfo interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FMSTest, FMS_Mediafile_pushFileInfo_0000, testing::ext::TestSize.Level1)
{
    cout << "FMSTest-begin FMS_Mediafile_pushFileInfo_0000" << endl;
    try {
        FileInfo fileInfo = FileInfo("FMS_Mediafile_pushFileInfo_0000", "./", "file", 0, 0, 0);
        MessageParcel messageParcel;
        int result = MediaFileOper::pushFileInfo(fileInfo, messageParcel);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FMSTest-an exception occurred." << endl;
    }
    cout << "FMSTest-end FMS_Mediafile_pushFileInfo_0000" << endl;
}

/**
 * @tc.number: SUB_STORAGE_FMS_Mediafile_ListFile_0000
 * @tc.name: fms_Mediafile_ListFile_0000
 * @tc.desc: Test function of ListFile interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FMSTest, FMS_Mediafile_ListFile_0000, testing::ext::TestSize.Level1)
{
    cout << "FMSTest-begin FMS_Mediafile_ListFile_0000" << endl;
    try {
        MessageParcel messageParcel;
        int result = MediaFileOper::ListFile("./", 0, 0, messageParcel);
        EXPECT_EQ(result, 0);
    } catch (...) {
        cout << "FMSTest-an exception occurred." << endl;
    }
    cout << "FMSTest-end FMS_Mediafile_ListFile_0000" << endl;
}

/**
 * @tc.number: SUB_STORAGE_FMS_Mediafile_mkdir_0000
 * @tc.name: fms_Mediafile_mkdir_0000
 * @tc.desc: Test function of mkdir interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FMSTest, FMS_Mediafile_mkdir_0000, testing::ext::TestSize.Level1)
{
    cout << "FMSTest-begin FMS_Mediafile_mkdir_0000" << endl;
    try {
        int result = MediaFileOper::mkdir("FMS_Mediafile_mkdir_0000", "./");
        EXPECT_EQ(result, 1);
    } catch (...) {
        cout << "FMSTest-an exception occurred." << endl;
    }
    cout << "FMSTest-end FMS_Mediafile_mkdir_0000" << endl;
}
} // namespace