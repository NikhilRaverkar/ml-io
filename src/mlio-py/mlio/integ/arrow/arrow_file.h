/*
 * Copyright 2019-2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not use this file except in compliance with the License. A copy of
 * the License is located at
 *
 *      http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * ANY KIND, either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 */

#pragma once

#include <cstdint>
#include <memory>

#include <arrow/io/interfaces.h>
#include <arrow/type_fwd.h>
#include <mlio/intrusive_ptr.h>
#include <mlio/streams/input_stream.h>

namespace pymlio {

class Arrow_file final : public arrow::io::RandomAccessFile {
public:
    explicit Arrow_file(mlio::Intrusive_ptr<mlio::Input_stream> stream);

    Arrow_file(const Arrow_file &) = delete;

    Arrow_file(Arrow_file &&) = delete;

    ~Arrow_file() final;

public:
    Arrow_file &operator=(const Arrow_file &) = delete;

    Arrow_file &operator=(Arrow_file &&) = delete;

public:
    arrow::Result<std::int64_t> Read(std::int64_t nbytes, void *out) noexcept final;

    arrow::Result<std::shared_ptr<arrow::Buffer>> Read(std::int64_t nbytes) noexcept final;

    arrow::Status Close() noexcept final;

    arrow::Result<std::int64_t> Tell() const noexcept final;

    arrow::Result<std::int64_t> GetSize() noexcept final;

private:
    arrow::Status check_if_closed() const noexcept;

public:
    bool supports_zero_copy() const noexcept final;

    bool closed() const noexcept final;

private:
    mlio::Intrusive_ptr<mlio::Input_stream> stream_;
};

}  // namespace pymlio
