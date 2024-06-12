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
import cooperate from '@ohos.cooperate'
import { describe, it, expect } from '@ohos/hypium'

export default function CooperationTest() {
  describe('CooperationTest', function () {
    console.log("*************Cooperation API Test Begin*************");

    const errCode = {
      COMMON_PARAMETER_CODE: 401
    }

    /**
     * @tc.number DeviceStatus_001
     * @tc.name DeviveStatus_Cooperation_Prepare_Exception_Test_001
     * @tc.desc cooperate interface prepare exception test
     */
    it('msdp_test_devices_status_001', 0, async function (done) {
      console.info(`msdp_test_devices_status_001 enter`);
      try {
        await cooperate.prepare((err, data) => {
          if (err) {
            console.info(`Cooperation_Prepare_Exception_Test_001 failed`);
            expect(false).assertTrue();
            done();
          } else {
            console.info(`Cooperation_Prepare_Exception_Test_001 success`);
            expect(true).assertTrue();
            done();
          }
        });
      } catch (error) {
        console.info(`Cooperation_Prepare_Exception_Test_001: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_002
     * @tc.name DeviveStatus_Cooperation_Prepare_Exception_Test_002
     * @tc.desc cooperate interface prepare exception test
     */
    it('msdp_test_devices_status_002', 0, async function (done) {
      console.info(`msdp_test_devices_status_002 enter`);
      try {
        await cooperate.prepare('10');
      } catch (error) {
        console.info(`Cooperation_Prepare_Exception_Test_002: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_003
     * @tc.name DeviveStatus_Cooperation_Prepare_Exception_Test_003
     * @tc.desc cooperate interface prepare exception test
     */
    it('msdp_test_devices_status_003', 0, async function (done) {
      console.info(`msdp_test_devices_status_003 enter`);
      try {
        await cooperate.prepare(10);
      } catch (error) {
        console.info(`Cooperation_Prepare_Exception_Test_003: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_004
     * @tc.name DeviveStatus_Cooperation_Prepare_Exception_Test_004
     * @tc.desc cooperate interface prepare exception test
     */
    it('msdp_test_devices_status_004', 0, async function (done) {
      console.info(`msdp_test_devices_status_004 enter`);
      try {
        await cooperate.prepare().then((data) => {
          console.info(`Cooperation_Prepare_Exception_Test_004 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_Prepare_Exception_Test_004 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
      } catch (error) {
        console.info(`Cooperation_Prepare_Exception_Test_004: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_005
     * @tc.name DeviveStatus_Cooperation_Unprepare_Exception_Test_001
     * @tc.desc cooperate interface unprepare exception test
     */
    it('msdp_test_devices_status_005', 0, async function (done) {
      console.info(`msdp_test_devices_status_005 enter`);
      try {
        await cooperate.unprepare((err, data) => {
          if (err) {
            console.info(`Cooperation_Unprepare_Exception_Test_001 failed`);
            expect(false).assertTrue();
            done();
          } else {
            console.info(`Cooperation_Unprepare_Exception_Test_001 success`);
            expect(true).assertTrue();
            done();
          }
        });
      } catch (error) {
        console.info(`Cooperation_Unprepare_Exception_Test_001: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_006
     * @tc.name DeviveStatus_Cooperation_Unprepare_Exception_Test_002
     * @tc.desc cooperate interface unprepare exception test
     */
    it('msdp_test_devices_status_006', 0, async function (done) {
      console.info(`msdp_test_devices_status_006 enter`);
      try {
        await cooperate.unprepare('10');
      } catch (error) {
        console.info(`Cooperation_Unprepare_Exception_Test_002: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_007
     * @tc.name DeviveStatus_Cooperation_Unprepare_Exception_Test_003
     * @tc.desc cooperate interface unprepare exception test
     */
    it('msdp_test_devices_status_007', 0, async function (done) {
      console.info(`msdp_test_devices_status_007 enter`);
      try {
        await cooperate.unprepare(10);
      } catch (error) {
        console.info(`Cooperation_Unprepare_Exception_Test_003: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_008
     * @tc.name DeviveStatus_Cooperation_Unprepare_Exception_Test_004
     * @tc.desc cooperate interface unprepare exception test
     */
    it('msdp_test_devices_status_008', 0, async function (done) {
      console.info(`msdp_test_devices_status_008 enter`);
      try {
        await cooperate.prepare().then((data) => {
          console.info(`Cooperation_Prepare_Exception_Test_004 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_Prepare_Exception_Test_004 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
        await cooperate.unprepare().then((data) => {
          console.info(`Cooperation_Unprepare_Exception_Test_004 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_Unprepare_Exception_Test_004 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
      } catch (error) {
        console.info(`Cooperation_Unprepare_Exception_Test_004: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_009
     * @tc.name DeviveStatus_Cooperation_Activate_Exception_Test_001
     * @tc.desc cooperate interface activate exception test
     */
    it('msdp_test_devices_status_009', 0, async function (done) {
      console.info(`msdp_test_devices_status_009 enter`);
      try {
        await cooperate.activate('10', true, (err, data) => {
          if (err) {
            console.info(`Cooperation_Activate_Exception_Test_001 failed`);
            expect(false).assertTrue();
            done();
          } else {
            console.info(`Cooperation_Activate_Exception_Test_001 success`);
            expect(true).assertTrue();
            done();
          }
        });
      } catch (error) {
        console.info(`Cooperation_Activate_Exception_Test_001: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_010
     * @tc.name DeviveStatus_Cooperation_Activate_Exception_Test_002
     * @tc.desc cooperate interface activate exception test
     */
    it('msdp_test_devices_status_010', 0, async function (done) {
      console.info(`msdp_test_devices_status_010 enter`);
      try {
        await cooperate.activate(10, 10, null);
      } catch (error) {
        console.info(`Cooperation_Activate_Exception_Test_002: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_011
     * @tc.name DeviveStatus_Cooperation_Activate_Exception_Test_003
     * @tc.desc cooperate interface activate exception test
     */
    it('msdp_test_devices_status_011', 0, async function (done) {
      console.info(`msdp_test_devices_status_011 enter`);
      try {
        await cooperate.activate('10', '10').then((data) => {
          console.info(`Cooperation_Activate_Exception_Test_003 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_Activate_Exception_Test_003 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
      } catch (error) {
        console.info(`Cooperation_Activate_Exception_Test_003: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_012
     * @tc.name DeviveStatus_Cooperation_Activate_Exception_Test_004
     * @tc.desc cooperate interface activate exception test
     */
    it('msdp_test_devices_status_012', 0, async function (done) {
      console.info(`msdp_test_devices_status_012 enter`);
      try {
        await cooperate.activate().then((data) => {
          console.info(`Cooperation_Activate_Exception_Test_004 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_Activate_Exception_Test_004 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
      } catch (error) {
        console.info(`Cooperation_Activate_Exception_Test_004: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
    * @tc.number DeviceStatus_013
    * @tc.name DeviveStatus_Cooperation_Deactivate_Exception_Test_001
    * @tc.desc cooperate interface deactivate exception test
    */
    it('msdp_test_devices_status_013', 0, function () {
      console.info(`msdp_test_devices_status_013 enter`);
      try {
        var promise = cooperate.deactivate(true);
        if (promise != undefined && promise != null) {
          promise.then((data) => {
            console.info(`Cooperation_Deactivate_Exception_Test_001 success`);
            expect(true).assertTrue();
          }, (error) => {
            console.info(`Cooperation_Deactivate_Exception_Test_001 failed, err=${JSON.stringify(error)}`);
            expect(false).assertTrue();
          });
        }
      } catch (error) {
        console.info(`Cooperation_Deactivate_Exception_Test_001: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
      }
    })

    /**
    * @tc.number DeviceStatus_014
    * @tc.name DeviveStatus_Cooperation_Deactivate_Exception_Test_002
    * @tc.desc cooperate interface deactivate exception test
    */
    it('msdp_test_devices_status_014', 0, function () {
      console.info(`msdp_test_devices_status_014 enter`);
      try {
        var promise = cooperate.deactivate(false);
        if (promise != undefined && promise != null) {
          promise.then((data) => {
            console.info(`Cooperation_Deactivate_Exception_Test_002 success`);
            expect(true).assertTrue();
          }, (error) => {
            console.info(`Cooperation_Deactivate_Exception_Test_002 failed, err=${JSON.stringify(error)}`);
            expect(false).assertTrue();
          });
        }
      } catch (error) {
        console.info(`Cooperation_Deactivate_Exception_Test_002: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);;
      }
    })

    /**
     * @tc.number DeviceStatus_015
     * @tc.name DeviveStatus_Cooperation_Deactivate_Exception_Test_003
     * @tc.desc cooperate interface deactivate exception test
     */
    it('msdp_test_devices_status_015', 0, async function (done) {
      console.info(`msdp_test_devices_status_015 enter`);
      try {
        await cooperate.deactivate(10);
      } catch (error) {
        console.info(`Cooperation_Deactivate_Exception_Test_003: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_016
     * @tc.name DeviveStatus_Cooperation_GetCrossingSwitchState_Exception_Test_001
     * @tc.desc cooperate interface getCrossingSwitchState exception test
     */
    it('msdp_test_devices_status_016', 0, async function (done) {
      console.info(`msdp_test_devices_status_016 enter`);
      try {
        await cooperate.prepare().then((data) => {
          console.info(`Cooperation_Prepare_Exception_Test_004 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_Prepare_Exception_Test_004 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
        await cooperate.getCrossingSwitchState("10", (err, data) => {
          if (err) {
            console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_001 failed`);
            expect(false).assertTrue();
            done();
          } else {
            console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_001 success`);
            expect(true).assertTrue();
            done();
          }
        });
      } catch (error) {
        console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_001: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_017
     * @tc.name DeviveStatus_Cooperation_GetCrossingSwitchState_Exception_Test_002
     * @tc.desc cooperate interface getCrossingSwitchState exception test
     */
    it('msdp_test_devices_status_017', 0, async function (done) {
      console.info(`msdp_test_devices_status_017 enter`);
      try {
        await cooperate.prepare().then((data) => {
          console.info(`Cooperation_Prepare_Exception_Test_004 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_Prepare_Exception_Test_004 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
        await cooperate.getCrossingSwitchState("", (err, data) => {
          if (err) {
            console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_002 failed`);
            expect(false).assertTrue();
            done();
          } else {
            console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_002 success`);
            expect(true).assertTrue();
            done();
          }
        });
      } catch (error) {
        console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_002: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_018
     * @tc.name DeviveStatus_Cooperation_GetCrossingSwitchState_Exception_Test_003
     * @tc.desc cooperate interface getCrossingSwitchState exception test
     */
    it('msdp_test_devices_status_018', 0, async function (done) {
      console.info(`msdp_test_devices_status_018 enter`);
      try {
        await cooperate.getCrossingSwitchState(10, null);
      } catch (error) {
        console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_003: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_019
     * @tc.name DeviveStatus_Cooperation_GetCrossingSwitchState_Exception_Test_004
     * @tc.desc cooperate interface getCrossingSwitchState exception test
     */
    it('msdp_test_devices_status_019', 0, async function (done) {
      console.info(`msdp_test_devices_status_019 enter`);
      try {
        await cooperate.getCrossingSwitchState().then((data) => {
          console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_004 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_004 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
      } catch (error) {
        console.info(`Cooperation_GetCrossingSwitchState_Exception_Test_004: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_020
     * @tc.name DeviveStatus_Cooperation_On_Exception_Test_001
     * @tc.desc cooperate interface on exception test
     */
    it('msdp_test_devices_status_020', 0, async function (done) {
      console.info(`msdp_test_devices_status_020 enter`);
      try {
        await cooperate.on('coordination', (data) => {
          console.info(`Cooperation_On_Exception_Test_001 success` + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        });
        await cooperate.off('coordination', (data) => {
          console.info(`Cooperation_Off_Exception_Test_001 success` + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        });
      } catch (error) {
        console.info(`Cooperation_On_Exception_Test_001 error`);
        expect(true).assertTrue();
        done();
      }
      done();
    })

    /**
     * @tc.number DeviceStatus_021
     * @tc.name DeviveStatus_Cooperation_On_Exception_Test_002
     * @tc.desc cooperate interface on exception test
     */
    it('msdp_test_devices_status_021', 0, async function (done) {
      console.info(`msdp_test_devices_status_021 enter`);
      try {
        await cooperate.on(10, null);
      } catch (error) {
        console.info(`Cooperation_On_Exception_Test_002: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_022
     * @tc.name DeviveStatus_Cooperation_On_Exception_Test_003
     * @tc.desc cooperate interface on exception test
     */
    it('msdp_test_devices_status_022', 0, async function (done) {
      console.info(`msdp_test_devices_status_022 enter`);
      try {
        await cooperate.on().then((data) => {
          console.info(`Cooperation_On_Exception_Test_003 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_On_Exception_Test_003 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
      } catch (error) {
        console.info(`Cooperation_On_Exception_Test_003: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_023
     * @tc.name DeviveStatus_Cooperation_Off_Exception_Test_001
     * @tc.desc cooperate interface off exception test
     */
    it('msdp_test_devices_status_023', 0, async function (done) {
      console.info(`msdp_test_devices_status_023 enter`);
      try {
        await cooperate.off(10, null);
      } catch (error) {
        console.info(`Cooperation_Off_Exception_Test_001: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })

    /**
     * @tc.number DeviceStatus_024
     * @tc.name DeviveStatus_Cooperation_Off_Exception_Test_002
     * @tc.desc cooperate interface off exception test
     */
    it('msdp_test_devices_status_024', 0, async function (done) {
      console.info(`msdp_test_devices_status_024 enter`);
      try {
        await cooperate.off().then((data) => {
          console.info(`Cooperation_Off_Exception_Test_002 success`);
          expect(true).assertTrue();
          done();
        }, (error) => {
          console.info(`Cooperation_Off_Exception_Test_002 failed, err=${JSON.stringify(error)}`);
          expect(false).assertTrue();
          done();
        });
      } catch (error) {
        console.info(`Cooperation_Off_Exception_Test_002: ${JSON.stringify(error, [`code`, `message`])}`);
        expect(error.code).assertEqual(errCode.COMMON_PARAMETER_CODE);
        done();
      }
    })
  })
}
