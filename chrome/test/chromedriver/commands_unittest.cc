// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/bind.h"
#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/file_path.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#include "base/values.h"
#include "chrome/test/chromedriver/chrome.h"
#include "chrome/test/chromedriver/chrome_launcher.h"
#include "chrome/test/chromedriver/command_executor_impl.h"
#include "chrome/test/chromedriver/commands.h"
#include "chrome/test/chromedriver/fake_session_accessor.h"
#include "chrome/test/chromedriver/status.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/webdriver/atoms.h"

TEST(CommandsTest, GetStatus) {
  base::DictionaryValue params;
  scoped_ptr<base::Value> value;
  std::string session_id;
  ASSERT_EQ(kOk, ExecuteGetStatus(params, "", &value, &session_id).code());
  base::DictionaryValue* dict;
  ASSERT_TRUE(value->GetAsDictionary(&dict));
  base::Value* unused;
  ASSERT_TRUE(dict->Get("os.name", &unused));
  ASSERT_TRUE(dict->Get("os.version", &unused));
  ASSERT_TRUE(dict->Get("os.arch", &unused));
  ASSERT_TRUE(dict->Get("build.version", &unused));
}

namespace {

class StubChrome : public Chrome {
 public:
  StubChrome() {}
  virtual ~StubChrome() {}

  // Overridden from Chrome:
  virtual Status Load(const std::string& url) OVERRIDE {
    return Status(kOk);
  }
  virtual Status Reload() OVERRIDE {
    return Status(kOk);
  }
  virtual Status EvaluateScript(const std::string& frame,
                                const std::string& function,
                                scoped_ptr<base::Value>* result) OVERRIDE {
    return Status(kOk);
  }
  virtual Status CallFunction(const std::string& frame,
                              const std::string& function,
                              const base::ListValue& args,
                              scoped_ptr<base::Value>* result) OVERRIDE {
    return Status(kOk);
  }
  virtual Status GetFrameByFunction(const std::string& frame,
                                    const std::string& function,
                                    const base::ListValue& args,
                                    std::string* out_frame) OVERRIDE {
    return Status(kOk);
  }
  virtual Status DispatchMouseEvents(
      const std::list<MouseEvent>& events) OVERRIDE {
    return Status(kOk);
  }
  virtual Status DispatchKeyEvents(const std::list<KeyEvent>& events) OVERRIDE {
    return Status(kOk);
  }
  virtual Status Quit() OVERRIDE {
    return Status(kOk);
  }
  virtual Status WaitForPendingNavigations(
      const std::string& frame_id) OVERRIDE {
    return Status(kOk);
  }
  virtual Status GetMainFrame(
      std::string* frame_id) OVERRIDE {
    return Status(kOk);
  }
};

class OkChrome : public StubChrome {
 public:
  OkChrome() {}
  virtual ~OkChrome() {}

  // Overridden from StubChrome:
  virtual Status EvaluateScript(const std::string& frame,
                                const std::string& function,
                                scoped_ptr<base::Value>* result) OVERRIDE {
    result->reset(base::Value::CreateStringValue("99.0.99999.0"));
    return Status(kOk);
  }
};

class OkLauncher : public ChromeLauncher {
 public:
  OkLauncher() {}
  virtual ~OkLauncher() {}

  // Overridden from ChromeLauncher:
  virtual Status Launch(const FilePath& chrome_exe,
                        scoped_ptr<Chrome>* chrome) OVERRIDE {
    chrome->reset(new OkChrome());
    return Status(kOk);
  }
};

}  // namespace

TEST(CommandsTest, NewSession) {
  OkLauncher launcher;
  SessionMap map;
  base::DictionaryValue params;
  scoped_ptr<base::Value> value;
  std::string session_id;
  Status status =
      ExecuteNewSession(&map, &launcher, params, "", &value, &session_id);
  ASSERT_EQ(kOk, status.code());
  ASSERT_TRUE(value);
  base::DictionaryValue* dict;
  ASSERT_TRUE(value->GetAsDictionary(&dict));
  std::string browserName;
  ASSERT_TRUE(dict->GetString("browserName", &browserName));
  ASSERT_STREQ("chrome", browserName.c_str());

  scoped_refptr<SessionAccessor> accessor;
  ASSERT_TRUE(map.Get(session_id, &accessor));
  scoped_ptr<base::AutoLock> lock;
  Session* session = accessor->Access(&lock);
  ASSERT_TRUE(session);
  ASSERT_STREQ(session_id.c_str(), session->id.c_str());
  ASSERT_TRUE(session->chrome);
}

namespace {

class FailLauncher : public ChromeLauncher {
 public:
  FailLauncher() {}
  virtual ~FailLauncher() {}

  // Overridden from ChromeLauncher:
  virtual Status Launch(const FilePath& chrome_exe,
                        scoped_ptr<Chrome>* chrome) OVERRIDE {
    return Status(kUnknownError);
  }
};

}  // namespace

TEST(CommandsTest, NewSessionLauncherFails) {
  FailLauncher launcher;
  SessionMap map;
  base::DictionaryValue params;
  scoped_ptr<base::Value> value;
  std::string session_id;
  Status status =
      ExecuteNewSession(&map, &launcher, params, "", &value, &session_id);
  ASSERT_EQ(kSessionNotCreatedException, status.code());
  ASSERT_FALSE(value);
}

namespace {

Status ExecuteStubQuit(
    int* count,
    const base::DictionaryValue& params,
    const std::string& session_id,
    scoped_ptr<base::Value>* value,
    std::string* out_session_id) {
  if (*count == 0) {
    EXPECT_STREQ("id", session_id.c_str());
  } else {
    EXPECT_STREQ("id2", session_id.c_str());
  }
  (*count)++;
  return Status(kOk);
}

}  // namespace

TEST(CommandsTest, QuitAll) {
  SessionMap map;
  Session session("id");
  Session session2("id2");
  map.Set(session.id,
          scoped_refptr<SessionAccessor>(new FakeSessionAccessor(&session)));
  map.Set(session2.id,
          scoped_refptr<SessionAccessor>(new FakeSessionAccessor(&session2)));

  int count = 0;
  Command cmd = base::Bind(&ExecuteStubQuit, &count);
  base::DictionaryValue params;
  scoped_ptr<base::Value> value;
  std::string session_id;
  Status status =
      ExecuteQuitAll(cmd, &map, params, "", &value, &session_id);
  ASSERT_EQ(kOk, status.code());
  ASSERT_FALSE(value.get());
  ASSERT_EQ(2, count);
}

TEST(CommandsTest, Quit) {
  SessionMap map;
  Session session("id", scoped_ptr<Chrome>(new StubChrome()));
  map.Set(session.id,
          scoped_refptr<SessionAccessor>(new FakeSessionAccessor(&session)));
  base::DictionaryValue params;
  scoped_ptr<base::Value> value;
  ASSERT_EQ(kOk, ExecuteQuit(&map, &session, params, &value).code());
  ASSERT_FALSE(map.Has(session.id));
  ASSERT_FALSE(value.get());
}

namespace {

class FailsToQuitChrome : public StubChrome {
 public:
  FailsToQuitChrome() {}
  virtual ~FailsToQuitChrome() {}

  // Overridden from Chrome:
  virtual Status Quit() OVERRIDE {
    return Status(kUnknownError);
  }
};

}  // namespace

TEST(CommandsTest, QuitFails) {
  SessionMap map;
  Session session("id", scoped_ptr<Chrome>(new FailsToQuitChrome()));
  map.Set(session.id,
          scoped_refptr<SessionAccessor>(new FakeSessionAccessor(&session)));
  base::DictionaryValue params;
  scoped_ptr<base::Value> value;
  ASSERT_EQ(kUnknownError, ExecuteQuit(&map, &session, params, &value).code());
  ASSERT_FALSE(map.Has(session.id));
  ASSERT_FALSE(value.get());
}

namespace {

enum TestScenario {
  kElementExistsQueryOnce = 0,
  kElementExistsQueryTwice,
  kElementNotExistsQueryOnce,
  kElementExistsTimeout
};

class FindElementChrome : public StubChrome {
 public:
  FindElementChrome(bool only_one, TestScenario scenario)
      : only_one_(only_one), scenario_(scenario), current_count_(0) {
    switch (scenario_) {
      case kElementExistsQueryOnce:
      case kElementExistsQueryTwice:
      case kElementExistsTimeout: {
        if (only_one_) {
          base::DictionaryValue element;
          element.SetString("ELEMENT", "1");
          result_.reset(element.DeepCopy());
        } else {
          base::DictionaryValue element1;
          element1.SetString("ELEMENT", "1");
          base::DictionaryValue element2;
          element2.SetString("ELEMENT", "2");
          base::ListValue list;
          list.Append(element1.DeepCopy());
          list.Append(element2.DeepCopy());
          result_.reset(list.DeepCopy());
        }
        break;
      }
      case kElementNotExistsQueryOnce: {
        if (only_one_)
          result_.reset(base::Value::CreateNullValue());
        else
          result_.reset(new base::ListValue());
        break;
      }
    }
  }
  virtual ~FindElementChrome() {}

  void Verify(const std::string& expected_frame,
              const base::ListValue* expected_args,
              const base::Value* actrual_result) {
    EXPECT_EQ(expected_frame, frame_);
    std::string function;
    if (only_one_)
      function = webdriver::atoms::asString(webdriver::atoms::FIND_ELEMENT);
    else
      function = webdriver::atoms::asString(webdriver::atoms::FIND_ELEMENTS);
    EXPECT_EQ(function, function_);
    ASSERT_TRUE(args_.get());
    EXPECT_TRUE(expected_args->Equals(args_.get()));
    ASSERT_TRUE(actrual_result);
    EXPECT_TRUE(result_->Equals(actrual_result));
  }

  // Overridden from Chrome:
  virtual Status CallFunction(const std::string& frame,
                              const std::string& function,
                              const base::ListValue& args,
                              scoped_ptr<base::Value>* result) OVERRIDE {
    ++current_count_;
    if (scenario_ == kElementExistsTimeout ||
        (scenario_ == kElementExistsQueryTwice && current_count_ == 1)) {
        // Always return empty result when testing timeout.
        if (only_one_)
          result->reset(base::Value::CreateNullValue());
        else
          result->reset(new base::ListValue());
    } else {
      switch (scenario_) {
        case kElementExistsQueryOnce:
        case kElementNotExistsQueryOnce: {
          EXPECT_EQ(1, current_count_);
          break;
        }
        case kElementExistsQueryTwice: {
          EXPECT_EQ(2, current_count_);
          break;
        }
        default: {
          break;
        }
      }

      result->reset(result_->DeepCopy());
      frame_ = frame;
      function_ = function;
      args_.reset(args.DeepCopy());
    }
    return Status(kOk);
  }

 private:
  bool only_one_;
  TestScenario scenario_;
  int current_count_;
  std::string frame_;
  std::string function_;
  scoped_ptr<base::ListValue> args_;
  scoped_ptr<base::Value> result_;
};

}  // namespace

TEST(CommandsTest, SuccessfulFindElement) {
  FindElementChrome* chrome =
      new FindElementChrome(true, kElementExistsQueryTwice);
  Session session("id", scoped_ptr<Chrome>(chrome));
  session.implicit_wait = 1000;
  session.frame = "frame_id1";
  base::DictionaryValue params;
  params.SetString("using", "id");
  params.SetString("value", "a");
  scoped_ptr<base::Value> result;
  ASSERT_EQ(kOk, ExecuteFindElement(1, &session, params, &result).code());
  base::DictionaryValue param;
  param.SetString("id", "a");
  base::ListValue expected_args;
  expected_args.Append(param.DeepCopy());
  chrome->Verify("frame_id1", &expected_args, result.get());
}

TEST(CommandsTest, FailedFindElement) {
  Session session("id",
      scoped_ptr<Chrome>(
          new FindElementChrome(true, kElementNotExistsQueryOnce)));
  base::DictionaryValue params;
  params.SetString("using", "id");
  params.SetString("value", "a");
  scoped_ptr<base::Value> result;
  ASSERT_EQ(kNoSuchElement,
            ExecuteFindElement(1, &session, params, &result).code());
}

TEST(CommandsTest, SuccessfulFindElements) {
  FindElementChrome* chrome =
      new FindElementChrome(false, kElementExistsQueryTwice);
  Session session("id", scoped_ptr<Chrome>(chrome));
  session.implicit_wait = 1000;
  session.frame = "frame_id2";
  base::DictionaryValue params;
  params.SetString("using", "name");
  params.SetString("value", "b");
  scoped_ptr<base::Value> result;
  ASSERT_EQ(kOk, ExecuteFindElements(1, &session, params, &result).code());
  base::DictionaryValue param;
  param.SetString("name", "b");
  base::ListValue expected_args;
  expected_args.Append(param.DeepCopy());
  chrome->Verify("frame_id2", &expected_args, result.get());
}

TEST(CommandsTest, FailedFindElements) {
  Session session("id",
      scoped_ptr<Chrome>(
          new FindElementChrome(false, kElementNotExistsQueryOnce)));
  base::DictionaryValue params;
  params.SetString("using", "id");
  params.SetString("value", "a");
  scoped_ptr<base::Value> result;
  ASSERT_EQ(kOk, ExecuteFindElements(1, &session, params, &result).code());
  base::ListValue* list;
  ASSERT_TRUE(result->GetAsList(&list));
  ASSERT_EQ(0U, list->GetSize());
}

TEST(CommandsTest, SuccessfulFindChildElement) {
  FindElementChrome* chrome =
      new FindElementChrome(true, kElementExistsQueryTwice);
  Session session("id", scoped_ptr<Chrome>(chrome));
  session.implicit_wait = 1000;
  session.frame = "frame_id3";
  base::DictionaryValue params;
  params.SetString("using", "tag name");
  params.SetString("value", "div");
  std::string element_id = "1";
  scoped_ptr<base::Value> result;
  ASSERT_EQ(
      kOk,
      ExecuteFindChildElement(1, &session, element_id, params, &result).code());
  base::DictionaryValue locator_param;
  locator_param.SetString("tag name", "div");
  base::DictionaryValue root_element_param;
  root_element_param.SetString("ELEMENT", element_id);
  base::ListValue expected_args;
  expected_args.Append(locator_param.DeepCopy());
  expected_args.Append(root_element_param.DeepCopy());
  chrome->Verify("frame_id3", &expected_args, result.get());
}

TEST(CommandsTest, FailedFindChildElement) {
  Session session("id",
      scoped_ptr<Chrome>(
          new FindElementChrome(true, kElementNotExistsQueryOnce)));
  base::DictionaryValue params;
  params.SetString("using", "id");
  params.SetString("value", "a");
  std::string element_id = "1";
  scoped_ptr<base::Value> result;
  ASSERT_EQ(
      kNoSuchElement,
      ExecuteFindChildElement(1, &session, element_id, params, &result).code());
}

TEST(CommandsTest, SuccessfulFindChildElements) {
  FindElementChrome* chrome =
      new FindElementChrome(false, kElementExistsQueryTwice);
  Session session("id", scoped_ptr<Chrome>(chrome));
  session.implicit_wait = 1000;
  session.frame = "frame_id4";
  base::DictionaryValue params;
  params.SetString("using", "class name");
  params.SetString("value", "c");
  std::string element_id = "1";
  scoped_ptr<base::Value> result;
  ASSERT_EQ(
      kOk,
      ExecuteFindChildElements(
          1, &session, element_id, params, &result).code());
  base::DictionaryValue locator_param;
  locator_param.SetString("class name", "c");
  base::DictionaryValue root_element_param;
  root_element_param.SetString("ELEMENT", element_id);
  base::ListValue expected_args;
  expected_args.Append(locator_param.DeepCopy());
  expected_args.Append(root_element_param.DeepCopy());
  chrome->Verify("frame_id4", &expected_args, result.get());
}

TEST(CommandsTest, FailedFindChildElements) {
  Session session("id",
      scoped_ptr<Chrome>(
          new FindElementChrome(false, kElementNotExistsQueryOnce)));
  base::DictionaryValue params;
  params.SetString("using", "id");
  params.SetString("value", "a");
  std::string element_id = "1";
  scoped_ptr<base::Value> result;
  ASSERT_EQ(
      kOk,
      ExecuteFindChildElements(
          1, &session, element_id, params, &result).code());
  base::ListValue* list;
  ASSERT_TRUE(result->GetAsList(&list));
  ASSERT_EQ(0U, list->GetSize());
}

TEST(CommandsTest, TimeoutInFindElement) {
  Session session("id",
      scoped_ptr<Chrome>(
          new FindElementChrome(true, kElementExistsTimeout)));
  session.implicit_wait = 2;
  base::DictionaryValue params;
  params.SetString("using", "id");
  params.SetString("value", "a");
  params.SetString("id", "1");
  scoped_ptr<base::Value> result;
  ASSERT_EQ(kNoSuchElement,
            ExecuteFindElement(1, &session, params, &result).code());
}

namespace {

class ErrorCallFunctionChrome : public StubChrome {
 public:
  explicit ErrorCallFunctionChrome(StatusCode code) : code_(code) {}
  virtual ~ErrorCallFunctionChrome() {}

  // Overridden from Chrome:
  virtual Status CallFunction(const std::string& frame,
                              const std::string& function,
                              const base::ListValue& args,
                              scoped_ptr<base::Value>* result) OVERRIDE {
    return Status(code_);
  }

 private:
  StatusCode code_;
};

}  // namespace

TEST(CommandsTest, ErrorFindElement) {
  Session session("id",
      scoped_ptr<Chrome>(new ErrorCallFunctionChrome(kUnknownError)));
  base::DictionaryValue params;
  params.SetString("using", "id");
  params.SetString("value", "a");
  scoped_ptr<base::Value> value;
  ASSERT_EQ(kUnknownError,
            ExecuteFindElement(1, &session, params, &value).code());
  ASSERT_EQ(kUnknownError,
            ExecuteFindElements(1, &session, params, &value).code());
}

TEST(CommandsTest, ErrorFindChildElement) {
  Session session("id",
      scoped_ptr<Chrome>(new ErrorCallFunctionChrome(kStaleElementReference)));
  base::DictionaryValue params;
  params.SetString("using", "id");
  params.SetString("value", "a");
  std::string element_id = "1";
  scoped_ptr<base::Value> result;
  ASSERT_EQ(
      kStaleElementReference,
      ExecuteFindChildElement(1, &session, element_id, params, &result).code());
  ASSERT_EQ(
      kStaleElementReference,
      ExecuteFindChildElements(
          1, &session, element_id, params, &result).code());
}
