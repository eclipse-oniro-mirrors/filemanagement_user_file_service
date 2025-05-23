/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef NAPI_OBSERVER_CALLBACK_H
#define NAPI_OBSERVER_CALLBACK_H

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "file_access_observer_stub.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi/native_api.h"
#include "uri.h"
#include "uv.h"

namespace OHOS {
namespace FileAccessFwk {
class NapiObserver {
public:
    NapiObserver(napi_env env, napi_value callback);
    virtual ~NapiObserver();
    void OnChange(const NotifyMessage &notifyMessage);
    napi_ref cbOnRef_ = nullptr;
private:
    struct CallbackParam {
        NapiObserver *napiObserver;
        NotifyMessage iNotifyMessage;
        CallbackParam(NapiObserver *napiObserver, NotifyMessage notifyMessage) : napiObserver(napiObserver),
            iNotifyMessage(notifyMessage) {}
    };
    napi_env env_;
    std::unique_ptr<uv_work_t> work_ = nullptr;
    static void NapiWorkScope(uv_work_t *work, int status);
};

class NapiObserverCallback : public FileAccessObserverStub {
public:
    explicit NapiObserverCallback(std::shared_ptr<NapiObserver> observer): FileAccessObserverStub(),
        observer_(observer) {}
    virtual ~NapiObserverCallback() = default;
    int OnChange(const NotifyMessage &notifyMessage) override
    {
        observer_->OnChange(notifyMessage);
        return 0;
    }
    std::shared_ptr<NapiObserver> observer_ = nullptr;
};

} // namespace FileAccessFwk
} // namespace OHOS
#endif // NAPI_OBSERVER_CALLBACK_H
