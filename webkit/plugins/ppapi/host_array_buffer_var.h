// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PPAPI_WEBKIT_PLUGINS_PPAPI_HOST_ARRAY_BUFFER_VAR_H_
#define PPAPI_WEBKIT_PLUGINS_PPAPI_HOST_ARRAY_BUFFER_VAR_H_

#include "ppapi/shared_impl/var.h"
#include <wtf/ArrayBuffer.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>

namespace WTF { class ArrayBuffer; }

namespace webkit {

class WebArrayBuffer {
public:
    ~WebArrayBuffer() { reset(); }

    WebArrayBuffer() { }
    WebArrayBuffer(const WebArrayBuffer& b) { assign(b); }
    WebArrayBuffer& operator=(const WebArrayBuffer& b)
    {
        assign(b);
        return *this;
    }

    static WebArrayBuffer create(unsigned numElements, unsigned elementByteSize);

    void reset();
    void assign(const WebArrayBuffer&);

    bool isNull() const { return m_private; }
    void* data() const;
    unsigned byteLength() const;

#if WEBKIT_USING_V8
    v8::Handle<v8::Value> toV8Value();
    static WebArrayBuffer* createFromV8Value(v8::Handle<v8::Value>);
#endif

#if WEBKIT_IMPLEMENTATION
    WebArrayBuffer(const WTF::PassRefPtr<WTF::ArrayBuffer>&);
    WebArrayBuffer& operator=(const PassRefPtr<WTF::ArrayBuffer>&);
    operator WTF::PassRefPtr<WTF::ArrayBuffer>() const;
#endif

protected:
    RefPtr<WTF::ArrayBuffer> m_private;
};

namespace ppapi {

// Represents a host-side ArrayBufferVar.
class HostArrayBufferVar : public ::ppapi::ArrayBufferVar {
 public:
  explicit HostArrayBufferVar(uint32 size_in_bytes);
  explicit HostArrayBufferVar(const webkit::WebArrayBuffer& buffer);
  virtual ~HostArrayBufferVar();

  // ArrayBufferVar implementation.
  virtual void* Map() OVERRIDE;
  virtual void Unmap() OVERRIDE;
  virtual uint32 ByteLength() OVERRIDE;

  webkit::WebArrayBuffer& webkit_buffer() { return buffer_; }

 private:
  webkit::WebArrayBuffer buffer_;

  DISALLOW_COPY_AND_ASSIGN(HostArrayBufferVar);
};

}  // namespace ppapi
}  // namespace webkit

#endif  // PPAPI_WEBKIT_PLUGINS_PPAPI_HOST_ARRAY_BUFFER_VAR_H_
