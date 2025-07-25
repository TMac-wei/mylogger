// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: effective_msg.proto

#include "effective_msg.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

PROTOBUF_CONSTEXPR EffectiveMsg::EffectiveMsg(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.file_name_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.func_name_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.log_info_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.timestamp_)*/int64_t{0}
  , /*decltype(_impl_.level_)*/0
  , /*decltype(_impl_.pid_)*/0
  , /*decltype(_impl_.tid_)*/0
  , /*decltype(_impl_.line_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct EffectiveMsgDefaultTypeInternal {
  PROTOBUF_CONSTEXPR EffectiveMsgDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~EffectiveMsgDefaultTypeInternal() {}
  union {
    EffectiveMsg _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 EffectiveMsgDefaultTypeInternal _EffectiveMsg_default_instance_;

// ===================================================================

class EffectiveMsg::_Internal {
 public:
};

EffectiveMsg::EffectiveMsg(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:EffectiveMsg)
}
EffectiveMsg::EffectiveMsg(const EffectiveMsg& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite() {
  EffectiveMsg* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.file_name_){}
    , decltype(_impl_.func_name_){}
    , decltype(_impl_.log_info_){}
    , decltype(_impl_.timestamp_){}
    , decltype(_impl_.level_){}
    , decltype(_impl_.pid_){}
    , decltype(_impl_.tid_){}
    , decltype(_impl_.line_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  _impl_.file_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.file_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_file_name().empty()) {
    _this->_impl_.file_name_.Set(from._internal_file_name(), 
      _this->GetArenaForAllocation());
  }
  _impl_.func_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.func_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_func_name().empty()) {
    _this->_impl_.func_name_.Set(from._internal_func_name(), 
      _this->GetArenaForAllocation());
  }
  _impl_.log_info_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.log_info_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_log_info().empty()) {
    _this->_impl_.log_info_.Set(from._internal_log_info(), 
      _this->GetArenaForAllocation());
  }
  ::memcpy(&_impl_.timestamp_, &from._impl_.timestamp_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.line_) -
    reinterpret_cast<char*>(&_impl_.timestamp_)) + sizeof(_impl_.line_));
  // @@protoc_insertion_point(copy_constructor:EffectiveMsg)
}

inline void EffectiveMsg::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.file_name_){}
    , decltype(_impl_.func_name_){}
    , decltype(_impl_.log_info_){}
    , decltype(_impl_.timestamp_){int64_t{0}}
    , decltype(_impl_.level_){0}
    , decltype(_impl_.pid_){0}
    , decltype(_impl_.tid_){0}
    , decltype(_impl_.line_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.file_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.file_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.func_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.func_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.log_info_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.log_info_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

EffectiveMsg::~EffectiveMsg() {
  // @@protoc_insertion_point(destructor:EffectiveMsg)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<std::string>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void EffectiveMsg::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.file_name_.Destroy();
  _impl_.func_name_.Destroy();
  _impl_.log_info_.Destroy();
}

void EffectiveMsg::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void EffectiveMsg::Clear() {
// @@protoc_insertion_point(message_clear_start:EffectiveMsg)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.file_name_.ClearToEmpty();
  _impl_.func_name_.ClearToEmpty();
  _impl_.log_info_.ClearToEmpty();
  ::memset(&_impl_.timestamp_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.line_) -
      reinterpret_cast<char*>(&_impl_.timestamp_)) + sizeof(_impl_.line_));
  _internal_metadata_.Clear<std::string>();
}

const char* EffectiveMsg::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // int32 level = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          _impl_.level_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int64 timestamp = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          _impl_.timestamp_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int32 pid = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 24)) {
          _impl_.pid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int32 tid = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 32)) {
          _impl_.tid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int32 line = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 40)) {
          _impl_.line_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // string file_name = 6;
      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 50)) {
          auto str = _internal_mutable_file_name();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, nullptr));
        } else
          goto handle_unusual;
        continue;
      // string func_name = 7;
      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 58)) {
          auto str = _internal_mutable_func_name();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, nullptr));
        } else
          goto handle_unusual;
        continue;
      // string log_info = 8;
      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 66)) {
          auto str = _internal_mutable_log_info();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, nullptr));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<std::string>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* EffectiveMsg::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:EffectiveMsg)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 level = 1;
  if (this->_internal_level() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt32ToArray(1, this->_internal_level(), target);
  }

  // int64 timestamp = 2;
  if (this->_internal_timestamp() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt64ToArray(2, this->_internal_timestamp(), target);
  }

  // int32 pid = 3;
  if (this->_internal_pid() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt32ToArray(3, this->_internal_pid(), target);
  }

  // int32 tid = 4;
  if (this->_internal_tid() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt32ToArray(4, this->_internal_tid(), target);
  }

  // int32 line = 5;
  if (this->_internal_line() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt32ToArray(5, this->_internal_line(), target);
  }

  // string file_name = 6;
  if (!this->_internal_file_name().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_file_name().data(), static_cast<int>(this->_internal_file_name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "EffectiveMsg.file_name");
    target = stream->WriteStringMaybeAliased(
        6, this->_internal_file_name(), target);
  }

  // string func_name = 7;
  if (!this->_internal_func_name().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_func_name().data(), static_cast<int>(this->_internal_func_name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "EffectiveMsg.func_name");
    target = stream->WriteStringMaybeAliased(
        7, this->_internal_func_name(), target);
  }

  // string log_info = 8;
  if (!this->_internal_log_info().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_log_info().data(), static_cast<int>(this->_internal_log_info().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "EffectiveMsg.log_info");
    target = stream->WriteStringMaybeAliased(
        8, this->_internal_log_info(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:EffectiveMsg)
  return target;
}

size_t EffectiveMsg::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:EffectiveMsg)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string file_name = 6;
  if (!this->_internal_file_name().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_file_name());
  }

  // string func_name = 7;
  if (!this->_internal_func_name().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_func_name());
  }

  // string log_info = 8;
  if (!this->_internal_log_info().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_log_info());
  }

  // int64 timestamp = 2;
  if (this->_internal_timestamp() != 0) {
    total_size += ::_pbi::WireFormatLite::Int64SizePlusOne(this->_internal_timestamp());
  }

  // int32 level = 1;
  if (this->_internal_level() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(this->_internal_level());
  }

  // int32 pid = 3;
  if (this->_internal_pid() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(this->_internal_pid());
  }

  // int32 tid = 4;
  if (this->_internal_tid() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(this->_internal_tid());
  }

  // int32 line = 5;
  if (this->_internal_line() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(this->_internal_line());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::_pbi::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void EffectiveMsg::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::_pbi::DownCast<const EffectiveMsg*>(
      &from));
}

void EffectiveMsg::MergeFrom(const EffectiveMsg& from) {
  EffectiveMsg* const _this = this;
  // @@protoc_insertion_point(class_specific_merge_from_start:EffectiveMsg)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_file_name().empty()) {
    _this->_internal_set_file_name(from._internal_file_name());
  }
  if (!from._internal_func_name().empty()) {
    _this->_internal_set_func_name(from._internal_func_name());
  }
  if (!from._internal_log_info().empty()) {
    _this->_internal_set_log_info(from._internal_log_info());
  }
  if (from._internal_timestamp() != 0) {
    _this->_internal_set_timestamp(from._internal_timestamp());
  }
  if (from._internal_level() != 0) {
    _this->_internal_set_level(from._internal_level());
  }
  if (from._internal_pid() != 0) {
    _this->_internal_set_pid(from._internal_pid());
  }
  if (from._internal_tid() != 0) {
    _this->_internal_set_tid(from._internal_tid());
  }
  if (from._internal_line() != 0) {
    _this->_internal_set_line(from._internal_line());
  }
  _this->_internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
}

void EffectiveMsg::CopyFrom(const EffectiveMsg& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:EffectiveMsg)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool EffectiveMsg::IsInitialized() const {
  return true;
}

void EffectiveMsg::InternalSwap(EffectiveMsg* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.file_name_, lhs_arena,
      &other->_impl_.file_name_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.func_name_, lhs_arena,
      &other->_impl_.func_name_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.log_info_, lhs_arena,
      &other->_impl_.log_info_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(EffectiveMsg, _impl_.line_)
      + sizeof(EffectiveMsg::_impl_.line_)
      - PROTOBUF_FIELD_OFFSET(EffectiveMsg, _impl_.timestamp_)>(
          reinterpret_cast<char*>(&_impl_.timestamp_),
          reinterpret_cast<char*>(&other->_impl_.timestamp_));
}

std::string EffectiveMsg::GetTypeName() const {
  return "EffectiveMsg";
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::EffectiveMsg*
Arena::CreateMaybeMessage< ::EffectiveMsg >(Arena* arena) {
  return Arena::CreateMessageInternal< ::EffectiveMsg >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
