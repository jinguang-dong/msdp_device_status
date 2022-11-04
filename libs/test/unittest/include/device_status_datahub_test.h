#ifndef OHOS_MSDP_DEVICESTATUS_DATAHUB_TEST_H
#define OHOS_MSDP_DEVICESTATUS_DATAHUB_TEST_H

#include <list>
#include <mutex>
#include <thread>
#include <map>
#include <errors.h>
#include <string>
#include <memory>
#include <vector>
#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "sensor_agent.h"
#include "sensor_agent_type.h"
#include "sensor_data_callback.h"
#include "rdb_store.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_store_config.h"
#include "values_bucket.h"
#include "result_set.h"
#include "devicestatus_common.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_data_define.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_msdp_interface.h"
#include "devicestatus_msdp_mock.h"
#include "devicestatus_msdp_client_impl.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {

class DeviceStatusDatahubTest : public testing::Test{

public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};


    } // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif //OHOS_MSDP_DEVICESTATUS_DATAHUB_TEST_H
