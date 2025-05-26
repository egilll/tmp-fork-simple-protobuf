#pragma once

#include "proto_grammar.h"
#include <tao/pegtl/contrib/analyze.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

namespace proto
{

namespace pegtl = TAO_PEGTL_NAMESPACE;

using node_ptr = std::unique_ptr< pegtl::parse_tree::node >;

// clang-format off
template < typename Rule >
using selector = pegtl::parse_tree::selector<
Rule,
pegtl::parse_tree::fold_one::on<
>,
pegtl::parse_tree::remove_content::on<
grammar::proto3::file,
grammar::proto2::file,
grammar::edition::file,
grammar::edition::head,
grammar::message,
grammar::package,
grammar::reserved, 
grammar::extensions,
grammar::import, 
grammar::field,
grammar::field_number, 
grammar::enum_int, 
grammar::enum_def,
grammar::enum_field,
grammar::range,
grammar::max_keyword,
grammar::option,
grammar::message_name,
grammar::rpc, 
grammar::service,
grammar::constant, 
grammar::field_option, 
grammar::option_name, 
grammar::defined_type, 
grammar::type, 
grammar::field_optional,
grammar::field_repeated,
grammar::map_field, 
grammar::key_type, 
grammar::enum_value_option, 
grammar::import_option,
grammar::oneof, 
grammar::oneof_field, 
grammar::bool_type,
grammar::bytes_type,
grammar::double_type,
grammar::float_type,
grammar::string_type,
grammar::int32_type,
grammar::int64_type,
grammar::sint32_type,
grammar::sint64_type,
grammar::uint32_type,
grammar::uint64_type,
grammar::fixed32_type,
grammar::fixed64_type,
grammar::sfixed32_type,
grammar::sfixed64_type,
grammar::message_text_format,
grammar::message_literal_field,
grammar::extend
>,
pegtl::parse_tree::store_content::on<
grammar::comment,
grammar::ident, 
grammar::hex_lit, 
grammar::oct_lit, 
grammar::dec_lit, 
grammar::float_lit, 
grammar::enum_dec, 
grammar::bool_lit, 
grammar::field_name, 
grammar::oneof_name, 
grammar::map_name, 
grammar::field_names, 
grammar::enum_name, 
grammar::rpc_name, 
grammar::rpc_type, 
grammar::rpc_options, 
grammar::service_name,
grammar::str_lit_value< '\'' >,
grammar::str_lit_value< '"' >
> >;
// clang-format on
}// namespace proto
