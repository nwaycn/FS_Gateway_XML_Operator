// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: head.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "head.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace nway_fs_opterator {

namespace {

const ::google::protobuf::Descriptor* MsgHead_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  MsgHead_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_head_2eproto() {
  protobuf_AddDesc_head_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "head.proto");
  GOOGLE_CHECK(file != NULL);
  MsgHead_descriptor_ = file->message_type(0);
  static const int MsgHead_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgHead, command_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgHead, nway_length_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgHead, cmd_flag_),
  };
  MsgHead_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      MsgHead_descriptor_,
      MsgHead::default_instance_,
      MsgHead_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgHead, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MsgHead, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(MsgHead));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_head_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    MsgHead_descriptor_, &MsgHead::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_head_2eproto() {
  delete MsgHead::default_instance_;
  delete MsgHead_reflection_;
}

void protobuf_AddDesc_head_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\nhead.proto\022\021nway_fs_opterator\"A\n\007MsgHe"
    "ad\022\017\n\007command\030\001 \002(\005\022\023\n\013nway_length\030\002 \002(\005"
    "\022\020\n\010cmd_flag\030\003 \002(\005", 98);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "head.proto", &protobuf_RegisterTypes);
  MsgHead::default_instance_ = new MsgHead();
  MsgHead::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_head_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_head_2eproto {
  StaticDescriptorInitializer_head_2eproto() {
    protobuf_AddDesc_head_2eproto();
  }
} static_descriptor_initializer_head_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int MsgHead::kCommandFieldNumber;
const int MsgHead::kNwayLengthFieldNumber;
const int MsgHead::kCmdFlagFieldNumber;
#endif  // !_MSC_VER

MsgHead::MsgHead()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:nway_fs_opterator.MsgHead)
}

void MsgHead::InitAsDefaultInstance() {
}

MsgHead::MsgHead(const MsgHead& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:nway_fs_opterator.MsgHead)
}

void MsgHead::SharedCtor() {
  _cached_size_ = 0;
  command_ = 0;
  nway_length_ = 0;
  cmd_flag_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

MsgHead::~MsgHead() {
  // @@protoc_insertion_point(destructor:nway_fs_opterator.MsgHead)
  SharedDtor();
}

void MsgHead::SharedDtor() {
  if (this != default_instance_) {
  }
}

void MsgHead::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* MsgHead::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return MsgHead_descriptor_;
}

const MsgHead& MsgHead::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_head_2eproto();
  return *default_instance_;
}

MsgHead* MsgHead::default_instance_ = NULL;

MsgHead* MsgHead::New() const {
  return new MsgHead;
}

void MsgHead::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<MsgHead*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  ZR_(command_, cmd_flag_);

#undef OFFSET_OF_FIELD_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool MsgHead::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:nway_fs_opterator.MsgHead)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 command = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &command_)));
          set_has_command();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_nway_length;
        break;
      }

      // required int32 nway_length = 2;
      case 2: {
        if (tag == 16) {
         parse_nway_length:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &nway_length_)));
          set_has_nway_length();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(24)) goto parse_cmd_flag;
        break;
      }

      // required int32 cmd_flag = 3;
      case 3: {
        if (tag == 24) {
         parse_cmd_flag:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &cmd_flag_)));
          set_has_cmd_flag();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:nway_fs_opterator.MsgHead)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:nway_fs_opterator.MsgHead)
  return false;
#undef DO_
}

void MsgHead::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:nway_fs_opterator.MsgHead)
  // required int32 command = 1;
  if (has_command()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->command(), output);
  }

  // required int32 nway_length = 2;
  if (has_nway_length()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->nway_length(), output);
  }

  // required int32 cmd_flag = 3;
  if (has_cmd_flag()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->cmd_flag(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:nway_fs_opterator.MsgHead)
}

::google::protobuf::uint8* MsgHead::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:nway_fs_opterator.MsgHead)
  // required int32 command = 1;
  if (has_command()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->command(), target);
  }

  // required int32 nway_length = 2;
  if (has_nway_length()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->nway_length(), target);
  }

  // required int32 cmd_flag = 3;
  if (has_cmd_flag()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->cmd_flag(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:nway_fs_opterator.MsgHead)
  return target;
}

int MsgHead::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 command = 1;
    if (has_command()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->command());
    }

    // required int32 nway_length = 2;
    if (has_nway_length()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->nway_length());
    }

    // required int32 cmd_flag = 3;
    if (has_cmd_flag()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->cmd_flag());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void MsgHead::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const MsgHead* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const MsgHead*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void MsgHead::MergeFrom(const MsgHead& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_command()) {
      set_command(from.command());
    }
    if (from.has_nway_length()) {
      set_nway_length(from.nway_length());
    }
    if (from.has_cmd_flag()) {
      set_cmd_flag(from.cmd_flag());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void MsgHead::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void MsgHead::CopyFrom(const MsgHead& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool MsgHead::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000007) != 0x00000007) return false;

  return true;
}

void MsgHead::Swap(MsgHead* other) {
  if (other != this) {
    std::swap(command_, other->command_);
    std::swap(nway_length_, other->nway_length_);
    std::swap(cmd_flag_, other->cmd_flag_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata MsgHead::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = MsgHead_descriptor_;
  metadata.reflection = MsgHead_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace nway_fs_opterator

// @@protoc_insertion_point(global_scope)
