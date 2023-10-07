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

#ifndef FILE_ACCESS_SERVICE_H
#define FILE_ACCESS_SERVICE_H

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <system_ability.h>
#include <unordered_map>
#include <vector>

#include "bundle_mgr_interface.h"
#include "file_access_service_stub.h"
#include "ifile_access_ext_base.h"
#include "holder_manager.h"
#include "ifile_access_ext_base.h"
#include "iremote_object.h"
#include "timer.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {

class ObserverContext {
    public:
        ObserverContext(sptr<IFileAccessObserver> obs): obs_(obs) {}
        virtual ~ObserverContext() = default;
        sptr<IFileAccessObserver> obs_ = nullptr;
        void Ref()
        {
            if (ref_ == std::numeric_limits<int32_t>::max()) {
                HILOG_ERROR("Ref is over max");
                return;
            }
            ref_++;
        }

        void UnRef()
        {
            if (ref_ == 0) {
                HILOG_ERROR("Ref is already zero");
                return;
            }
            ref_--;
        }

        bool IsValid()
        {
            return ref_ > 0;
        }

        bool EqualTo(std::shared_ptr<ObserverContext> observerContext)
        {
            return obs_->AsObject() == observerContext->obs_->AsObject();
        }

    std::unordered_map<NotifyType, std::vector<std::string>> notifyMap_;
    std::mutex mapMutex_;
    private:
        std::atomic<int32_t> ref_;
};

class ObserverNode {
    public:
        ObserverNode(const bool needChildNote): needChildNote_(needChildNote) {}
        virtual ~ObserverNode() = default;
        std::shared_ptr<ObserverNode> parent_;
        std::vector<std::shared_ptr<ObserverNode>> children_;
        std::vector<uint32_t> obsCodeList_;
        bool needChildNote_;
};

class OnDemandTimer {
public:
    using TimerCallback = std::function<bool()>;
    OnDemandTimer(const TimerCallback &callback, int32_t intervalTime, int32_t maxCount) : timerCallback_(callback),
        intervalTime_(intervalTime), maxCount_(maxCount) {
            if (timerCallback_ == nullptr) {
                HILOG_ERROR("timerCallback_ is required not to be nullptr");
            }
            timer_.Setup();
        }
    virtual ~OnDemandTimer()
    {
        timer_.Shutdown();
    }
    void start()
    {
        if (isTimerStart_) {
            return;
        }
        std::lock_guard<std::mutex> lock(timerMutex_);
        if (isTimerStart_) {
            return;
        }
        timerId_ = timer_.Register([this] {
            if (timerCallback_ == nullptr) {
                HILOG_ERROR("timerCallback_ is nullptr");
                return;
            }
            if (!timerCallback_()) {
                if (++count_ >= maxCount_) {
                    stop();
                    count_ = 0;
                }
            } else {
                count_ = 0;
            }
        }, intervalTime_);
        isTimerStart_ = true;
    }

    void stop()
    {
        timer_.Unregister(timerId_);
        isTimerStart_ = false;
    }
private:
    Utils::Timer timer_{"OnDemandTimer"};
    TimerCallback timerCallback_ = nullptr;
    bool isTimerStart_ = false;
    uint32_t intervalTime_ = 0;
    uint32_t timerId_ = 0;
    uint32_t maxCount_ = 0;
    uint32_t count_ = 0;
    std::mutex timerMutex_;
};

class FileAccessService final : public SystemAbility, public FileAccessServiceStub {
    DECLARE_SYSTEM_ABILITY(FileAccessService)

public:
    static sptr<FileAccessService> GetInstance();
    virtual ~FileAccessService() = default;

    virtual void OnStart() override;
    virtual void OnStop() override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

public:
    int32_t RegisterNotify(Uri uri, bool notifyForDescendants, const sptr<IFileAccessObserver> &observer) override;
    int32_t UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer) override;
    int32_t CleanAllNotify(Uri uri) override;
    int32_t OnChange(Uri uri, NotifyType notifyType) override;
    int32_t GetExensionProxy(const std::shared_ptr<ConnectExtensionInfo> &info,
                             sptr<IFileAccessExtBase> &extensionProxy) override;

private:
    class ExtensionDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        ExtensionDeathRecipient() = default;
        virtual void OnRemoteDied(const wptr<IRemoteObject>& remote);
        virtual ~ExtensionDeathRecipient() = default;
    };

    class ObserverDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        ObserverDeathRecipient() = default;
        virtual void OnRemoteDied(const wptr<IRemoteObject>& remote);
        virtual ~ObserverDeathRecipient() = default;
    };

    void CleanRelativeObserver(const sptr<IFileAccessObserver> &observer);
    void SendListNotify(std::string uri, NotifyType notifyType, const std::vector<uint32_t> &list);
    void RemoveRelations(std::string &uriStr, std::shared_ptr<ObserverNode> obsNode);
    int FindUri(const std::string &uriStr, std::shared_ptr<ObserverNode> &outObsNode);
    int32_t ConnectExtension();
    void ResetProxy();
    FileAccessService();
    bool IsServiceReady() const;
    void InitTimer();
    std::shared_ptr<OnDemandTimer> onDemandTimer_ = nullptr;
    sptr<IFileAccessExtBase> extensionProxy_{nullptr};
    static sptr<FileAccessService> instance_;
    sptr<IRemoteObject::DeathRecipient> extensionDeathRecipient_;
    sptr<IRemoteObject::DeathRecipient> observerDeathRecipient_;
    bool ready_ = false;
    static std::mutex mutex_;
    std::mutex nodeMutex_;
    std::unordered_map<std::string, std::shared_ptr<ObserverNode>> relationshipMap_;
    HolderManager<std::shared_ptr<ObserverContext>> obsManager_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_H