/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef MOTION_SERVICE_H
#define MOTION_SERVICE_H

#include <memory>
#include <iremote_object.h>
#include <system_ability.h>

#include "motion_srv_stub.h"
#include "imotion_callback.h"
#include "motion_data_utils.h"
#include "motion_manager.h"
#include "motion_dump.h"
#include "motion_delayed_sp_singleton.h"

namespace OHOS {
namespace Msdp {
class MotionService final : public SystemAbility, public MotionSrvStub {
    DECLARE_SYSTEM_ABILITY(MotionService)
    MOTION_DECLARE_DELAYED_SP_SINGLETON(MotionService);
public:
    virtual void OnDump() override;
    virtual void OnStart() override;
    virtual void OnStop() override;

    void Subscribe(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback) override;
    void UnSubscribe(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback) override;
    MotionDataUtils::MotionData GetCache(const MotionDataUtils::MotionType& type) override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    bool IsServiceReady();
    std::shared_ptr<MotionManager> GetMotionManager();
private:
    bool Init();
    bool ready_ = false;
    std::shared_ptr<MotionManager> motionManager_;
    std::shared_ptr<MotionMsdpClientImpl> msdpImpl_;
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_SERVICE_H
