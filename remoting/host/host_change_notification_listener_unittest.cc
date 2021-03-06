// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "remoting/host/host_change_notification_listener.h"

#include <set>

#include "base/bind.h"
#include "base/memory/ref_counted.h"
#include "base/message_loop.h"
#include "base/string_number_conversions.h"
#include "remoting/base/constants.h"
#include "remoting/jingle_glue/mock_objects.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/libjingle/source/talk/xmllite/xmlelement.h"
#include "third_party/libjingle/source/talk/xmpp/constants.h"

using buzz::QName;
using buzz::XmlElement;

using testing::NotNull;
using testing::Return;

namespace remoting {

namespace {
const char kHostId[] = "0";
const char kTestJid[] = "user@gmail.com/chromoting123";
}  // namespace

ACTION_P(AddListener, list) {
  list->insert(arg0);
}

ACTION_P(RemoveListener, list) {
  EXPECT_TRUE(list->find(arg0) != list->end());
  list->erase(arg0);
}


class HostChangeNotificationListenerTest : public testing::Test {
 protected:
  class MockListener : public HostChangeNotificationListener::Listener {
   public:
    MOCK_METHOD0(OnHostDeleted, void());
  };

  virtual void SetUp() OVERRIDE {
    EXPECT_CALL(signal_strategy_, AddListener(NotNull()))
        .WillRepeatedly(AddListener(&signal_strategy_listeners_));
    EXPECT_CALL(signal_strategy_, RemoveListener(NotNull()))
        .WillRepeatedly(RemoveListener(&signal_strategy_listeners_));
    EXPECT_CALL(signal_strategy_, GetLocalJid())
        .WillRepeatedly(Return(kTestJid));

    host_change_notification_listener_.reset(new HostChangeNotificationListener(
        &mock_listener_, kHostId, &signal_strategy_));
  }

  virtual void TearDown() OVERRIDE {
    host_change_notification_listener_.reset();
    EXPECT_TRUE(signal_strategy_listeners_.empty());
  }

  scoped_ptr<XmlElement> GetNotificationStanza(std::string operation,
                                               std::string hostId,
                                               std::string botJid) {
    scoped_ptr<XmlElement> stanza(new XmlElement(buzz::QN_IQ));
    stanza->AddAttr(QName("", "type"), "set");
    XmlElement* host_changed = new XmlElement(
        QName(kChromotingXmlNamespace, "host-changed"));
    host_changed->AddAttr(QName(kChromotingXmlNamespace, "operation"),
                          operation);
    host_changed->AddAttr(QName(kChromotingXmlNamespace, "hostid"), hostId);
    stanza->AddElement(host_changed);
    stanza->AddAttr(buzz::QN_FROM, botJid);
    stanza->AddAttr(buzz::QN_TO, kTestJid);
    return stanza.Pass();
  }

  MockListener mock_listener_;
  MockSignalStrategy signal_strategy_;
  std::set<SignalStrategy::Listener*> signal_strategy_listeners_;
  scoped_ptr<HostChangeNotificationListener> host_change_notification_listener_;
  MessageLoop message_loop_;
};

TEST_F(HostChangeNotificationListenerTest, ReceiveValidNotification) {
  EXPECT_CALL(mock_listener_, OnHostDeleted())
      .WillOnce(Return());
  scoped_ptr<XmlElement> stanza = GetNotificationStanza(
      "delete", kHostId, kChromotingBotJid);
  host_change_notification_listener_->OnSignalStrategyIncomingStanza(
      stanza.get());
  message_loop_.PostTask(FROM_HERE, base::Bind(MessageLoop::QuitClosure()));
  message_loop_.Run();
}

TEST_F(HostChangeNotificationListenerTest, ReceiveNotificationBeforeDelete) {
  EXPECT_CALL(mock_listener_, OnHostDeleted())
      .Times(0);
  scoped_ptr<XmlElement> stanza = GetNotificationStanza(
      "delete", kHostId, kChromotingBotJid);
  host_change_notification_listener_->OnSignalStrategyIncomingStanza(
      stanza.get());
  host_change_notification_listener_.reset();
  message_loop_.PostTask(FROM_HERE, base::Bind(MessageLoop::QuitClosure()));
  message_loop_.Run();
}


TEST_F(HostChangeNotificationListenerTest, ReceiveInvalidHostIdNotification) {
  EXPECT_CALL(mock_listener_, OnHostDeleted())
      .Times(0);
  scoped_ptr<XmlElement> stanza = GetNotificationStanza(
      "delete", "1", kChromotingBotJid);
  host_change_notification_listener_->OnSignalStrategyIncomingStanza(
      stanza.get());
  message_loop_.PostTask(FROM_HERE, base::Bind(MessageLoop::QuitClosure()));
  message_loop_.Run();
}

TEST_F(HostChangeNotificationListenerTest, ReceiveInvalidBotJidNotification) {
  EXPECT_CALL(mock_listener_, OnHostDeleted())
      .Times(0);
  scoped_ptr<XmlElement> stanza = GetNotificationStanza(
      "delete", kHostId, "notremotingbot@bot.talk.google.com");
  host_change_notification_listener_->OnSignalStrategyIncomingStanza(
      stanza.get());
  message_loop_.PostTask(FROM_HERE, base::Bind(MessageLoop::QuitClosure()));
  message_loop_.Run();
}

TEST_F(HostChangeNotificationListenerTest, ReceiveNonDeleteNotification) {
  EXPECT_CALL(mock_listener_, OnHostDeleted())
      .Times(0);
  scoped_ptr<XmlElement> stanza = GetNotificationStanza(
      "update", kHostId, kChromotingBotJid);
  host_change_notification_listener_->OnSignalStrategyIncomingStanza(
      stanza.get());
  message_loop_.PostTask(FROM_HERE, base::Bind(MessageLoop::QuitClosure()));
  message_loop_.Run();
}

}  // namespace remoting
