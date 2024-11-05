/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "iremote_broker.h"
#include "message_parcel_mock.h"

namespace OHOS {
using namespace OHOS::FileAccessFwk;

Parcelable::Parcelable() : Parcelable(false)
{}

Parcelable::Parcelable(bool asRemote)
{
    asRemote_ = asRemote;
    behavior_ = 0;
}

bool MessageParcel::WriteInterfaceToken(std::u16string name)
{
    return UMessageParcel::messageParcel->WriteInterfaceToken(name);
}

std::u16string MessageParcel::ReadInterfaceToken()
{
    return UMessageParcel::messageParcel->ReadInterfaceToken();
}

bool Parcel::WriteParcelable(const Parcelable *object)
{
    return UMessageParcel::messageParcel->WriteParcelable(object);
}

bool Parcel::WriteInt32(int32_t value)
{
    return UMessageParcel::messageParcel->WriteInt32(value);
}

int32_t Parcel::ReadInt32()
{
    return UMessageParcel::messageParcel->ReadInt32();
}

bool Parcel::ReadInt32(int32_t &value)
{
    return UMessageParcel::messageParcel->ReadInt32(value);
}

bool Parcel::WriteRemoteObject(const Parcelable *object)
{
    return UMessageParcel::messageParcel->WriteRemoteObject(object);
}

bool MessageParcel::WriteRemoteObject(const sptr<IRemoteObject> &object)
{
    return UMessageParcel::messageParcel->WriteRemoteObject(object);
}

sptr<IRemoteObject> MessageParcel::ReadRemoteObject()
{
    return UMessageParcel::messageParcel->ReadRemoteObject();
}

bool Parcel::ReadBool()
{
    return UMessageParcel::messageParcel->ReadBool();
}

bool Parcel::ReadBool(bool &value)
{
    return UMessageParcel::messageParcel->ReadBool(value);
}

bool Parcel::WriteBool(bool value)
{
    return UMessageParcel::messageParcel->WriteBool(value);
}

bool Parcel::WriteString(const std::string &value)
{
    return UMessageParcel::messageParcel->WriteString(value);
}

bool Parcel::ReadString(std::string &value)
{
    return UMessageParcel::messageParcel->ReadString(value);
}

bool Parcel::ReadStringVector(std::vector<std::string> *value)
{
    return UMessageParcel::messageParcel->ReadStringVector(value);
}

bool MessageParcel::WriteFileDescriptor(int fd)
{
    return UMessageParcel::messageParcel->WriteFileDescriptor(fd);
}

int MessageParcel::ReadFileDescriptor()
{
    return UMessageParcel::messageParcel->ReadFileDescriptor();
}

bool Parcel::ReadUint32(uint32_t &value)
{
    return UMessageParcel::messageParcel->ReadUint32(value);
}
} // namespace OHOS