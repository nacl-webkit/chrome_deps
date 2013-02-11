// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "webkit/plugins/ppapi/host_array_buffer_var.h"

using ppapi::ArrayBufferVar;
using webkit::WebArrayBuffer;


namespace webkit {

WebArrayBuffer WebArrayBuffer::create(unsigned numElements, unsigned elementByteSize)
{
    RefPtr<ArrayBuffer> buffer = ArrayBuffer::create(numElements, elementByteSize);
    return WebArrayBuffer(buffer);
}

void WebArrayBuffer::reset()
{
    m_private = 0;
}

void WebArrayBuffer::assign(const WebArrayBuffer& other)
{
    m_private = other.m_private;
}

void* WebArrayBuffer::data() const
{
    if (!isNull())
        return const_cast<void*>(m_private->data());
    return 0;
}

unsigned WebArrayBuffer::byteLength() const
{
    if (!isNull())
        return m_private->byteLength();
    return 0;
}

#if WEBKIT_USING_V8
v8::Handle<v8::Value> WebArrayBuffer::toV8Value()
{
    if (!m_private.get())
        return v8::Handle<v8::Value>();
    return toV8(m_private.get());
}

WebArrayBuffer* WebArrayBuffer::createFromV8Value(v8::Handle<v8::Value> value)
{
    if (!V8ArrayBuffer::HasInstance(value))
        return 0;
    WTF::ArrayBuffer* buffer = V8ArrayBuffer::toNative(value->ToObject());
    return new WebArrayBuffer(buffer);
}
#endif

WebArrayBuffer::WebArrayBuffer(const WTF::PassRefPtr<WTF::ArrayBuffer>& blob)
    : m_private(blob)
{
}

WebArrayBuffer& WebArrayBuffer::operator=(const WTF::PassRefPtr<WTF::ArrayBuffer>& blob)
{
    m_private = blob;
    return *this;
}

WebArrayBuffer::operator WTF::PassRefPtr<WTF::ArrayBuffer>() const
{
    return m_private.get();
}

namespace ppapi {

HostArrayBufferVar::HostArrayBufferVar(uint32 size_in_bytes)
    : buffer_(WebArrayBuffer::create(size_in_bytes, 1 /* element_size */)) {
}

HostArrayBufferVar::HostArrayBufferVar(const WebArrayBuffer& buffer)
    : buffer_(buffer) {
}

HostArrayBufferVar::~HostArrayBufferVar() {
}

void* HostArrayBufferVar::Map() {
  return buffer_.data();
}

void HostArrayBufferVar::Unmap() {
  // We do not used shared memory on the host side. Nothing to do.
}

uint32 HostArrayBufferVar::ByteLength() {
  return buffer_.byteLength();
}

}  // namespace ppapi
}  // namespace webkit

