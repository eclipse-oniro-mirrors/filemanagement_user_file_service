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

#include "fms_mediafile_oper.h"

#include <vector>

#include "fms_const.h"
#include "fms_fileinfo.h"
#include "ipc_types.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "log.h"
#include "media_data_ability_const.h"
#include "medialibrary_data_ability.h"

using namespace std;

namespace OHOS {
namespace FileManagerService {
const std::vector<std::string> mediaData = { Media::MEDIA_DATA_DB_ID,
                             Media::MEDIA_DATA_DB_URI,
                             Media::MEDIA_DATA_DB_NAME };
const std::vector<std::pair<int, std::string>> mediaMetaMap = { {0, "string"}, // 0 id
                                            {1, "string"},  // 1 fileuri
                                            {4, "string"},  // 4 type
                                            {6, "string"},  // 6 name
                                            {7, "int"},     // 7 size
                                            {8, "int"},     // 8 at
                                            {9, "int"} };   // 9 mt
int Insert(const string &name, const string &path, const string &type)
{
    Media::ValuesBucket values;
    Media::MediaLibraryDataAbility abi;
    string abilityUri = Media::MEDIALIBRARY_DATA_URI;
    string oper;
    if (type == "album") {
        oper = Media::MEDIA_ALBUMOPRN + "/" + Media::MEDIA_ALBUMOPRN_CREATEALBUM;
    } else if(type == "file") {
        oper = Media::MEDIA_FILEOPRN + "/" + Media::MEDIA_FILEOPRN_CREATEASSET;
    }
    Uri createUri(abilityUri + "/" + oper);
    struct stat statInfo {};
    // need getfileStat info
    // need MEDIA_DATA_DB_RELATIVE_PATH
    // need MEDIA_DATA_DB_NAME
    values.PutLong(Media::MEDIA_DATA_DB_DATE_ADDED, statInfo.st_ctime);
    values.PutLong(Media::MEDIA_DATA_DB_DATE_MODIFIED, statInfo.st_mtime);
    abi.InitMediaLibraryRdbStore();
    return abi.Insert(createUri, values);
}

int MediaFileOper::CreateFile(const std::string &name, const std::string &path, std::string &uri)
{
    string type = "file";

    int index = Insert(name, path, type);

    // media type need to check the path
    if (index < 0) {
         ERR_LOG("MediaFileOper:: Fail to create fail %{public}s %{public}s", name.c_str(), path.c_str());
         return index;
    }
    uri = Media::MEDIALIBRARY_FILE_URI;
    uri += "/" + to_string(index);
    return SUCCESS;
}

bool pushFileInfo(shared_ptr<Media::AbsSharedResultSet> result, MessageParcel &reply)
{
    string id;
    string uri;
    result->GetString(mediaMetaMap[0].first, id);
    result->GetString(mediaMetaMap[1].first, uri);
    reply.WriteString(uri + "/" + id);
    for (int i = 2; i < mediaMetaMap.size(); i++) {
        if(mediaMetaMap[i].second == "string")
        {
            string value;
            result->GetString(mediaMetaMap[i].first, value);
            reply.WriteString(value);
        } else {
            int64_t value;
            result->GetLong(mediaMetaMap[i].first, value);
            reply.WriteInt64(value);
        }
    }
    return true;
}

int GetFileInfoFromResult(shared_ptr<Media::AbsSharedResultSet> result, MessageParcel &reply)
{
    int count = 0;
    result->GetRowCount(count);
    if (count == 0) {
        ERR_LOG("GetFileInfoFromResult::AbsSharedResultSet null");
        return FAIL;
    }
    result->GoToFirstRow();
    reply.WriteInt32(count);
    for (int i = 0; i < count; i++) {
        pushFileInfo(result, reply);
        result->GoToNextRow();
    }
    return SUCCESS;
}

bool GetRelativePath(const string &path, string &relativePath)
{
    NativeRdb::DataAbilityPredicates predicates;
    string selection = Media::MEDIA_DATA_DB_ID + " LIKE ? ";
    vector<string> selectionArgs = { path };
    predicates.SetWhereClause(selection);
    predicates.SetWhereArgs(selectionArgs);
    Media::MediaLibraryDataAbility abi;
    abi.InitMediaLibraryRdbStore();
    Uri uri(Media::MEDIALIBRARY_DATA_URI);
    vector<string> columns;
    shared_ptr<Media::AbsSharedResultSet> result = abi.Query(uri, columns, predicates);
    if (result == nullptr) {
        return false;
    }
    int count = 0;
    result->GetRowCount(count);

    if (count != 1) {
        ERR_LOG("GetRelativePath::AbsSharedResultSet more than one uri");
    }
    int32_t columnIndex;
    int ret = result->GetColumnIndex(Media::MEDIA_DATA_DB_FILE_PATH, columnIndex);
    if(ret != NativeRdb::E_OK) {
        return false;
    }
    result->GoToFirstRow();
    ret = result->GetString(columnIndex, relativePath);
    if(ret != NativeRdb::E_OK) {
        return false;
    }
    DEBUG_LOG("GetRelativePath %{public}s", relativePath.c_str());
    return true;
}

int MediaFileOper::ListFile(const string &path, int offset, int count, MessageParcel &reply)
{
    //get the relative path from the path uri
    string relativePath;

    DEBUG_LOG("ListFile %{public}s", path.c_str());
    if(!GetRelativePath(path, relativePath)) {
        ERR_LOG("MediaFileOper::path not exsit");
        return E_NOEXIST;
    }
    NativeRdb::DataAbilityPredicates predicates;
    string selection = Media::MEDIA_DATA_DB_RELATIVE_PATH + " LIKE ? ";
    vector<string> selectionArgs = { relativePath };
    predicates.SetWhereClause(selection);
    predicates.SetWhereArgs(selectionArgs);
    Media::MediaLibraryDataAbility abi;
    abi.InitMediaLibraryRdbStore();
    Uri uri(Media::MEDIALIBRARY_DATA_URI);
    vector<string> columns;
    shared_ptr<Media::AbsSharedResultSet> result = abi.Query(uri, columns, predicates);
    if (result == nullptr) {
        ERR_LOG("MediaFileOper::ListFile folder is empty");
        return E_EMPTYFOLDER;
    }
    GetFileInfoFromResult(result, reply);
    return SUCCESS;
}

int MediaFileOper::mkdir(const string &name, const string &path)
{
    string type = "album";
    Insert(name, path, type);
    DEBUG_LOG("MediaFileOper::mkdir path %{public}s.", path.c_str());
    return 1;
}

} // namespace FileManagerService
} // namespace OHOS