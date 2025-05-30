/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "file_access_ext_connection.h"

#include <errors.h>

#include "ability_connect_callback_interface.h"
#include "ability_manager_client.h"
#include "file_access_ext_base_proxy.h"
#include "hilog_wrapper.h"
#include "iremote_broker.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace FileAccessFwk {

std::mutex FileAccessExtConnection::mutex_;

const int32_t DEFAULT_USER_ID = -1;

void FileAccessExtConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    HILOG_INFO("OnAbilityConnectDone start");
    if (remoteObject == nullptr) {
        HILOG_ERROR("remote is nullptr");
        return;
    }

    fileExtProxy_ = iface_cast<FileAccessExtBaseProxy>(remoteObject);
    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("FileAccessExtConnection fileExtProxy_ is nullptr");
        return;
    }
    HILOG_INFO("OnAbilityConnectDone set connected info");
    isConnected_.store(true);
    std::lock_guard<std::mutex> lock(connectLockInfo_.mutex);
    connectLockInfo_.isReady = true;
    connectLockInfo_.condition.notify_all();
}

void FileAccessExtConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    fileExtProxy_ = nullptr;
    isConnected_.store(false);
}

void FileAccessExtConnection::ConnectFileExtAbility(const AAFwk::Want &want, const sptr<IRemoteObject> &token)
{
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, DEFAULT_USER_ID);
    HILOG_INFO("ConnectFileExtAbility ret: %{public}d ", ret);
    if (ret != ERR_OK) {
        HILOG_INFO("ConnectAbility ret=%{public}d", ret);
        return;
    }
    std::unique_lock<std::mutex> lock(connectLockInfo_.mutex);
    const int WAIT_TIME = 3;  // second
    if (!connectLockInfo_.condition.wait_for(lock, std::chrono::seconds(WAIT_TIME),
        [this] { return fileExtProxy_ != nullptr && connectLockInfo_.isReady; })) {
        HILOG_INFO("Wait connect timeout.");
    }
}

void FileAccessExtConnection::DisconnectFileExtAbility()
{
    fileExtProxy_ = nullptr;
    isConnected_.store(false);
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
    HILOG_INFO("DisconnectFileExtAbility called end, ret=%{public}d", ret);
}

bool FileAccessExtConnection::IsExtAbilityConnected()
{
    return isConnected_.load();
}

sptr<IFileAccessExtBase> FileAccessExtConnection::GetFileExtProxy()
{
    return fileExtProxy_;
}

void AgentFileAccessExtConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    HILOG_INFO("start OnAbilityConnectDone");
    if (connection_ == nullptr) {
        return ;
    }
    connection_->OnAbilityConnectDone(element, remoteObject, resultCode);
    HILOG_INFO("end OnAbilityConnectDone");
}

void AgentFileAccessExtConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOG_INFO("start OnAbilityDisconnectDone");
    if (connection_ == nullptr) {
        HILOG_WARN("OnAbilityDisconnectDone connection_ is nullptr");
        return;
    }
    connection_->OnAbilityDisconnectDone(element, resultCode);
    FileAccessService::GetInstance()->DisconnectAppProxy(connection_);
    FileAccessService::GetInstance()->RemoveAppProxy(connection_);
    HILOG_INFO("OnAbilityDisconnectDone resultCode=%{public}d", resultCode);
}

void AgentFileAccessExtConnection::ConnectFileExtAbility(const AAFwk::Want &want)
{
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, DEFAULT_USER_ID);
    if (ret != ERR_OK) {
        HILOG_ERROR("AgentFileAccessExtConnection ConnectAbility failed, ret=%{public}d", ret);
        return;
    }
    HILOG_INFO("AgentFileAccessExtConnection ConnectFileExtAbility success");
}

void AgentFileAccessExtConnection::DisconnectFileExtAbility()
{
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
    if (ret != ERR_OK) {
        HILOG_ERROR("DisconnectAbility failed, ret=%{public}d", ret);
        return;
    }
    HILOG_INFO("DisconnectFileExtAbility done");
}

AgentFileAccessExtConnection::~AgentFileAccessExtConnection()
{
}

} // namespace FileAccessFwk
} // namespace OHOS
