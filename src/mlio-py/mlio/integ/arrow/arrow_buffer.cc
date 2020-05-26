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

#include "arrow_buffer.h"

#include <cstdint>
#include <utility>

using namespace mlio;

namespace pymlio {

Arrow_buffer::Arrow_buffer(Memory_slice s) noexcept
    : arrow::Buffer{nullptr, 0}, slice_{std::move(s)}
{
    data_ = reinterpret_cast<const std::uint8_t *>(slice_.data());

    size_ = static_cast<std::int64_t>(slice_.size());

    capacity_ = size_;
}

Arrow_buffer::~Arrow_buffer() = default;

}  // namespace pymlio
