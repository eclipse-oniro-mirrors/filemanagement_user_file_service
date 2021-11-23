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

#include "fms_napi.h"

#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "common/napi/n_func_arg.h"
#include "common/uni_error.h"

#include "common/napi/n_async/n_async_work_promise.h"
#include "common/napi/n_async/n_async_work_callback.h"
#include "sys/syscall.h"
#define gettid() syscall(__NR_gettid)

#ifdef FILE_SUBSYSTEM_DEV_ON_PC
#define DEBUG_LOG(fmt, ...)
#endif
#ifndef FILE_SUBSYSTEM_DEV_ON_PC
#include "log.h"
#include "fms_proxy.h"
#include "ifms_client.h"
#endif
namespace OHOS {
namespace DistributedFS {
namespace FMS_NAPI {
using namespace std;
#ifndef FILE_SUBSYSTEM_DEV_ON_PC
using namespace FileManagerService;
#endif
struct AsyncFileInfoArg {
    NRef _ref;
#ifndef FILE_SUBSYSTEM_DEV_ON_PC
    vector<OHOS::FileManagerService::FileInfo> fileRes;
#else
    vector<string> res;
#endif
    explicit AsyncFileInfoArg(NVal ref) : _ref(ref) {};
    ~AsyncFileInfoArg() = default;
};

napi_value FMS_NAPI::CreateFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    unique_ptr<char[]> name;
    unique_ptr<char[]> path;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid name");
        return nullptr;
    }
    tie(succ, path, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }
    string uri = "";
#ifndef FILE_SUBSYSTEM_DEV_ON_PC
    FileManagerService::IFmsClient::GetFmsInstance()->CreateFile(name.get(), path.get(), uri);
#endif
    return NVal::CreateUTF8String(env, uri).val_;
}

napi_value FMS_NAPI::ListFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::THREE)) {
        UniError(EINVAL).ThrowErr(env, "Number of argments unmatched");
        return nullptr;
    }

    bool succ = false;
    unique_ptr<char[]> path;
    tie(succ, path, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }
    int64_t offset = 0;
    int64_t count = 0;
    bool hasOp = false;
    // need to check SECOND valid ?
    NVal op(env, NVal(env, funcArg[NARG_POS::SECOND]).val_);
    if (op.HasProp("offset")) {
        tie(succ, offset) = op.GetProp("offset").ToInt64();
        if (!succ) {
            UniError(EINVAL).ThrowErr(env, "Invalid option");
            return nullptr;
        }
        hasOp = true;
    }

    if (op.HasProp("count")) {
        tie(succ, count) = op.GetProp("count").ToInt64();
        if (!succ) {
            UniError(EINVAL).ThrowErr(env, "Invalid option");
            return nullptr;
        }
        hasOp = true;
    }
    napi_value fileArr;
    napi_create_array(env, &fileArr);
    auto arg = make_shared<AsyncFileInfoArg>(NVal(env, fileArr));
    auto cbExec = [path = string(path.get()), offset, count, arg, fileArr] (napi_env env) -> UniError {
#ifndef FILE_SUBSYSTEM_DEV_ON_PC
        int err = 0;
        vector<OHOS::FileManagerService::FileInfo> fileRes;
        err = FileManagerService::IFmsClient::GetFmsInstance()->ListFile(path, offset, count, fileRes);
        arg->fileRes = fileRes;
        if (err) {
            // need to add specific error
            return UniError(EIO);
        }
#else
        vector<string> resArr = {"name", "path", "type", "size", "added_time", "modified_time"};
        arg->res = resArr;
#endif

        return UniError(ERRNO_NOERR);
    };

    auto cbComplete = [arg](napi_env env, UniError err) -> NVal {
#ifndef FILE_SUBSYSTEM_DEV_ON_PC
        for (unsigned int i = 0; i < arg->fileRes.size(); i++) {
            NVal obj = NVal::CreateObject(env);
            FileManagerService::FileInfo res = arg->fileRes[i];
            obj.AddProp("name", NVal::CreateUTF8String(env, res.GetName()).val_);
            obj.AddProp("path", NVal::CreateUTF8String(env, res.GetPath()).val_);
            obj.AddProp("type", NVal::CreateUTF8String(env, res.GetType()).val_);
            obj.AddProp("size", NVal::CreateInt64(env, res.GetSize()).val_);
            obj.AddProp("added_time", NVal::CreateInt64(env, res.GetAdded_Time()).val_);
            obj.AddProp("modified_time", NVal::CreateInt64(env, res.GetModified_time()).val_);
            napi_set_property(env, arg->_ref.Deref(env).val_, NVal::CreateInt32(env, i).val_, obj.val_);
        }
#else
        vector<string> resArr = {"name", "path", "type", "size", "added_time", "modified_time"};
        for (unsigned int  i = 0; i < resArr.size(); i++) {
            NVal obj = NVal::CreateObject(env);
            for (auto s : arg->res) {
                obj.AddProp(s, NVal::CreateUTF8String(env, s).val_);
            }
            napi_set_property(env, arg->_ref.Deref(env).val_, NVal::CreateInt32(env, i).val_, obj.val_);
        }
#endif
        if (err) {
            return { env, err.GetNapiErr(env) };
        } else {
            return NVal(env, arg->_ref.Deref(env).val_);
        }
    };
    string procedureName = "ListFile";
    int argc = funcArg.GetArgc();
    NVal thisVar(env, funcArg.GetThisVar());
    if (argc == NARG_CNT::ONE || (argc == NARG_CNT::TWO && hasOp)) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    } else {
        int cbIdx = ((hasOp == false) ? NARG_POS::SECOND : NARG_POS::THIRD);
        NVal cb(env, funcArg[cbIdx]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
    }
}

napi_value FMS_NAPI::mkdir(napi_env env, napi_callback_info info)
{
    return NVal::CreateUndefined(env).val_;
}

bool FMS_NAPI::Export()
{
    return exports_.AddProp({
        NVal::DeclareNapiFunction("listFile", ListFile),
        NVal::DeclareNapiFunction("CreateFile", CreateFile),
        NVal::DeclareNapiFunction("mkdir", mkdir),
    });
}

string FMS_NAPI::GetClassName()
{
    return FMS_NAPI::className_;
}

FMS_NAPI::FMS_NAPI(napi_env env, napi_value exports) : NExporter(env, exports) {}

FMS_NAPI::~FMS_NAPI() {}
} // namespace FMS_NAPI
} // namespace DistributedFS
} // namespace OHOS
