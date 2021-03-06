// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/command_line.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop.h"
#include "chrome/browser/printing/cloud_print/cloud_print_proxy_service.h"
#include "chrome/browser/printing/cloud_print/cloud_print_proxy_service_factory.h"
#include "chrome/browser/service/service_process_control.h"
#include "chrome/browser/ui/startup/startup_browser_creator.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/cloud_print/cloud_print_proxy_info.h"
#include "chrome/common/pref_names.h"
#include "chrome/common/service_messages.h"
#include "chrome/test/base/testing_browser_process.h"
#include "chrome/test/base/testing_pref_service.h"
#include "chrome/test/base/testing_profile.h"
#include "chrome/test/base/testing_profile_manager.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/test/test_browser_thread.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::Assign;
using ::testing::AtMost;
using ::testing::DeleteArg;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Property;
using ::testing::Return;
using ::testing::ReturnPointee;
using ::testing::WithArgs;
using ::testing::WithoutArgs;
using ::testing::_;

class MockServiceProcessControl : public ServiceProcessControl {
 public:
  static std::string EnabledUserId();

  MockServiceProcessControl() : connected_(false) { }

  MOCK_CONST_METHOD0(IsConnected, bool());

  MOCK_METHOD2(Launch, void(const base::Closure&, const base::Closure&));
  MOCK_METHOD0(Disconnect, void());

  MOCK_METHOD1(OnMessageReceived, bool(const IPC::Message&));
  MOCK_METHOD1(OnChannelConnected, void(int32 peer_pid));
  MOCK_METHOD0(OnChannelError, void());

  MOCK_METHOD1(Send, bool(IPC::Message*));

  typedef enum {
    kServiceStateDisabled,
    kServiceStateEnabled,
    kServiceStateNone
  } ServiceState;

  void SetConnectSuccessMockExpectations(ServiceState state, bool post_task);

  void SetServiceEnabledExpectations();
  void SetServiceDisabledExpectations();
  void SetWillBeEnabledExpectations();
  void SetWillBeDisabledExpectations();

  bool SendEnabledInfo();
  bool SendDisabledInfo();

 private:
  bool connected_;
  cloud_print::CloudPrintProxyInfo info_;
};

// static
std::string MockServiceProcessControl::EnabledUserId() {
  return std::string("dorothy@somewhere.otr");
}

void CallTask(const base::Closure& task) {
  if (!task.is_null())
    task.Run();
}

void PostTask(const base::Closure& task) {
  if (!task.is_null())
    MessageLoop::current()->PostTask(FROM_HERE, task);
}

void MockServiceProcessControl::SetConnectSuccessMockExpectations(
    ServiceState service_state,
    bool post_task) {
  EXPECT_CALL(*this, IsConnected()).WillRepeatedly(ReturnPointee(&connected_));

  EXPECT_CALL(*this, Launch(_, _))
      .WillRepeatedly(
          DoAll(Assign(&connected_, true),
                WithArgs<0>(Invoke(post_task ? PostTask : CallTask))));

  EXPECT_CALL(*this, Disconnect()).Times(AtMost(1))
      .WillRepeatedly(Assign(&connected_, false));

  EXPECT_CALL(*this, Send(_)).Times(0);

  if (service_state == kServiceStateEnabled)
    SetServiceEnabledExpectations();
  else if (service_state == kServiceStateDisabled)
    SetServiceDisabledExpectations();
}

void MockServiceProcessControl::SetServiceEnabledExpectations() {
  EXPECT_CALL(
      *this,
      Send(Property(&IPC::Message::type,
                    static_cast<int32>(ServiceMsg_GetCloudPrintProxyInfo::ID))))
      .Times(1).WillOnce(
          DoAll(
              DeleteArg<0>(),
              WithoutArgs(
                  Invoke(this, &MockServiceProcessControl::SendEnabledInfo))));
}

void MockServiceProcessControl::SetServiceDisabledExpectations() {
  EXPECT_CALL(
      *this,
      Send(Property(&IPC::Message::type,
                    static_cast<int32>(ServiceMsg_GetCloudPrintProxyInfo::ID))))
      .Times(1).WillOnce(
          DoAll(
              DeleteArg<0>(),
              WithoutArgs(
                  Invoke(this, &MockServiceProcessControl::SendDisabledInfo))));
}

void MockServiceProcessControl::SetWillBeEnabledExpectations() {
  EXPECT_CALL(
      *this,
      Send(Property(&IPC::Message::type,
                    static_cast<int32>(ServiceMsg_EnableCloudPrintProxy::ID))))
      .Times(1).WillOnce(DoAll(DeleteArg<0>(), Return(true)));
}

void MockServiceProcessControl::SetWillBeDisabledExpectations() {
  EXPECT_CALL(
      *this,
      Send(Property(&IPC::Message::type,
                    static_cast<int32>(ServiceMsg_DisableCloudPrintProxy::ID))))
      .Times(1).WillOnce(DoAll(DeleteArg<0>(), Return(true)));
}

bool MockServiceProcessControl::SendEnabledInfo() {
  info_.enabled = true;
  info_.email = EnabledUserId();
  OnCloudPrintProxyInfo(info_);
  return true;
}

bool MockServiceProcessControl::SendDisabledInfo() {
  info_.enabled = false;
  info_.email = std::string();
  OnCloudPrintProxyInfo(info_);
  return true;
}

class TestCloudPrintProxyService : public CloudPrintProxyService {
 public:
  explicit TestCloudPrintProxyService(Profile* profile)
      : CloudPrintProxyService(profile) { }

  virtual ServiceProcessControl* GetServiceProcessControl() {
    return &process_control_;
  }
  MockServiceProcessControl* GetMockServiceProcessControl() {
    return &process_control_;
  }

 private:
  MockServiceProcessControl process_control_;
};

class CloudPrintProxyPolicyTest : public ::testing::Test {
 public:
  CloudPrintProxyPolicyTest()
      : ui_thread_(content::BrowserThread::UI, &message_loop_) {
  }

  bool LaunchBrowser(const CommandLine& command_line, Profile* profile) {
    int return_code = 0;
    StartupBrowserCreator browser_creator;
    return StartupBrowserCreator::ProcessCmdLineImpl(
        command_line, FilePath(), false, profile,
        StartupBrowserCreator::Profiles(), &return_code, &browser_creator);
  }

 protected:
  MessageLoopForUI message_loop_;
  content::TestBrowserThread ui_thread_;
  TestingProfile profile_;
};

TEST_F(CloudPrintProxyPolicyTest, VerifyExpectations) {
  MockServiceProcessControl mock_control;
  mock_control.SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateNone, false);

  EXPECT_FALSE(mock_control.IsConnected());
  mock_control.Launch(base::Closure(), base::Closure());
  EXPECT_TRUE(mock_control.IsConnected());
  mock_control.Launch(base::Closure(), base::Closure());
  EXPECT_TRUE(mock_control.IsConnected());
  mock_control.Disconnect();
  EXPECT_FALSE(mock_control.IsConnected());
}

TEST_F(CloudPrintProxyPolicyTest, StartWithNoPolicyProxyDisabled) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateDisabled, false);

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(
                         MockServiceProcessControl::EnabledUserId()));

  service.Initialize();

  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
}

TEST_F(CloudPrintProxyPolicyTest, StartWithNoPolicyProxyEnabled) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateEnabled, false);

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(std::string()));

  service.Initialize();
  service.RefreshStatusFromService();

  EXPECT_EQ(MockServiceProcessControl::EnabledUserId(),
            prefs->GetString(prefs::kCloudPrintEmail));
}

TEST_F(CloudPrintProxyPolicyTest, StartWithPolicySetProxyDisabled) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateDisabled, false);

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(std::string()));
  prefs->SetManagedPref(prefs::kCloudPrintProxyEnabled,
                        Value::CreateBooleanValue(false));

  service.Initialize();

  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
}

TEST_F(CloudPrintProxyPolicyTest, StartWithPolicySetProxyEnabled) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateEnabled, false);
  service.GetMockServiceProcessControl()->SetWillBeDisabledExpectations();

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(std::string()));
  prefs->SetManagedPref(prefs::kCloudPrintProxyEnabled,
                        Value::CreateBooleanValue(false));

  service.Initialize();

  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
}

TEST_F(CloudPrintProxyPolicyTest, StartWithNoPolicyProxyDisabledThenSetPolicy) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateDisabled, false);

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(
                         MockServiceProcessControl::EnabledUserId()));

  service.Initialize();

  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));

  prefs->SetManagedPref(prefs::kCloudPrintProxyEnabled,
                        Value::CreateBooleanValue(false));

  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
}

TEST_F(CloudPrintProxyPolicyTest, StartWithNoPolicyProxyEnabledThenSetPolicy) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateEnabled, false);

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(std::string()));

  service.Initialize();
  service.RefreshStatusFromService();

  EXPECT_EQ(MockServiceProcessControl::EnabledUserId(),
            prefs->GetString(prefs::kCloudPrintEmail));

  service.GetMockServiceProcessControl()->SetWillBeDisabledExpectations();
  prefs->SetManagedPref(prefs::kCloudPrintProxyEnabled,
                        Value::CreateBooleanValue(false));

  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
}

TEST_F(CloudPrintProxyPolicyTest,
       StartWithPolicySetProxyDisabledThenClearPolicy) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateDisabled, false);

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(std::string()));
  prefs->SetManagedPref(prefs::kCloudPrintProxyEnabled,
                        Value::CreateBooleanValue(false));

  service.Initialize();

  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
  prefs->RemoveManagedPref(prefs::kCloudPrintProxyEnabled);
  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
}

TEST_F(CloudPrintProxyPolicyTest,
       StartWithPolicySetProxyEnabledThenClearPolicy) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateEnabled, false);
  service.GetMockServiceProcessControl()->SetWillBeDisabledExpectations();

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(std::string()));
  prefs->SetManagedPref(prefs::kCloudPrintProxyEnabled,
                        Value::CreateBooleanValue(false));

  service.Initialize();

  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
  prefs->RemoveManagedPref(prefs::kCloudPrintProxyEnabled);
  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
}

TEST_F(CloudPrintProxyPolicyTest, StartWithNoPolicyProxyDisabledThenEnable) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateDisabled, false);

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(
                         MockServiceProcessControl::EnabledUserId()));

  service.Initialize();
  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));

  service.GetMockServiceProcessControl()->SetWillBeEnabledExpectations();
  service.EnableForUser(std::string(),
                        MockServiceProcessControl::EnabledUserId());

  EXPECT_EQ(MockServiceProcessControl::EnabledUserId(),
            prefs->GetString(prefs::kCloudPrintEmail));
}

TEST_F(CloudPrintProxyPolicyTest,
       StartWithPolicySetProxyEnabledThenClearPolicyAndEnable) {
  TestCloudPrintProxyService service(&profile_);

  service.GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateEnabled, false);
  service.GetMockServiceProcessControl()->SetWillBeDisabledExpectations();

  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(std::string()));
  prefs->SetManagedPref(prefs::kCloudPrintProxyEnabled,
                        Value::CreateBooleanValue(false));

  service.Initialize();

  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));
  service.EnableForUser(std::string(),
                        MockServiceProcessControl::EnabledUserId());
  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));

  prefs->RemoveManagedPref(prefs::kCloudPrintProxyEnabled);
  EXPECT_EQ(std::string(), prefs->GetString(prefs::kCloudPrintEmail));

  service.GetMockServiceProcessControl()->SetWillBeEnabledExpectations();
  service.EnableForUser(std::string(),
                        MockServiceProcessControl::EnabledUserId());

  EXPECT_EQ(MockServiceProcessControl::EnabledUserId(),
            prefs->GetString(prefs::kCloudPrintEmail));
}

ProfileKeyedService* TestCloudPrintProxyServiceFactory(Profile* profile) {
  TestCloudPrintProxyService* service = new TestCloudPrintProxyService(profile);

  service->GetMockServiceProcessControl()->SetConnectSuccessMockExpectations(
      MockServiceProcessControl::kServiceStateEnabled, true);
  service->GetMockServiceProcessControl()->SetWillBeDisabledExpectations();

  service->Initialize();
  MessageLoop::current()->RunUntilIdle();
  return service;
}

TEST_F(CloudPrintProxyPolicyTest, StartupBrowserCreatorWithCommandLine) {
  TestingPrefServiceSyncable* prefs = profile_.GetTestingPrefService();
  prefs->SetUserPref(prefs::kCloudPrintEmail,
                     Value::CreateStringValue(std::string()));
  prefs->SetManagedPref(prefs::kCloudPrintProxyEnabled,
                        Value::CreateBooleanValue(false));

  CloudPrintProxyServiceFactory::GetInstance()->
      SetTestingFactory(&profile_, TestCloudPrintProxyServiceFactory);

  CommandLine command_line(CommandLine::NO_PROGRAM);
  command_line.AppendSwitch(switches::kCheckCloudPrintConnectorPolicy);

  EXPECT_FALSE(LaunchBrowser(command_line, &profile_));
  MessageLoop::current()->RunUntilIdle();
}
