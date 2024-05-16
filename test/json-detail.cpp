#include <cstdint>
#include <memory>
#include <name.pb.h>
#include <person.pb.h>
#include <sds/json/deserialize.hpp>
#include <sds/json/serialize.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

namespace Test
{
auto operator==( const Test::Name & lhs, const Test::Name & rhs ) noexcept -> bool
{
    return lhs.name == rhs.name;
}
}// namespace Test

using namespace std::literals;

TEST_CASE( "json" )
{
    SUBCASE( "dj2hash" )
    {
        const auto hash           = sds::json::detail::djb2_hash( "hello" );
        const auto collision      = sds::json::detail::djb2_hash( "narpjy" );
        const auto hash2          = sds::json::detail::djb2_hash( "world" );
        const auto name_hash      = sds::json::detail::djb2_hash( "name" );
        const auto name_collision = sds::json::detail::djb2_hash( "bkfvdzz" );
        const auto hash3          = sds::json::detail::djb2_hash( { } );
        CHECK( hash == collision );
        CHECK( name_hash == name_collision );
        CHECK( hash3 != 0 );
        CHECK( hash != 0 );
        CHECK( hash2 != 0 );
        CHECK( hash != hash2 );
        CHECK( hash3 != hash2 );
        CHECK( hash3 != hash );
    }
    SUBCASE( "deserialize" )
    {
        SUBCASE( "ignore" )
        {
            SUBCASE( "empty" )
            {
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({})" ) == Test::Name{ } );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( "" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"(})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({)" ) );
            }
            SUBCASE( "string" )
            {
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"string":"string"})" ) == Test::Name{ } );
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"bkfvdzz":"string"})" ) == Test::Name{ } );
            }
            SUBCASE( "int" )
            {
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"integer":42})" ) == Test::Name{ } );
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"bkfvdzz":42})" ) == Test::Name{ } );
            }
            SUBCASE( "float" )
            {
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"fl":42.0, "fl2":0})" ) == Test::Name{ } );
            }
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"array":[42,"hello"]})" ) == Test::Name{ } );
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"array":[]})" ) == Test::Name{ } );
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"bkfvdzz":[]})" ) == Test::Name{ } );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"array":[})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"array":[)" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"array":[42)" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"array":[42,)" ) );
            }
            SUBCASE( "bool" )
            {
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"value":true, "value2":false})" ) == Test::Name{ } );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"value":tru, "value2":false})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"value":true, "value2":fals})" ) );
            }
            SUBCASE( "null" )
            {
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"value":null})" ) == Test::Name{ } );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"value":nul})" ) );
            }
            SUBCASE( "object" )
            {
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"value":{}})" ) == Test::Name{ } );
                CHECK( sds::json::detail::deserialize< Test::Name >( R"({"value":{"key":"value", "key2":[42]}})" ) == Test::Name{ } );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"value"})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"value":{"key":"value", "key2":[42]})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"value":{"key":}})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"value":{"key"}})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"value":{"key":"value")" ) );
                CHECK_THROWS( sds::json::detail::deserialize< Test::Name >( R"({"value":{"key":"value",)" ) );
            }
        }
        SUBCASE( "person" )
        {
            constexpr std::string_view jsons[] = {
                R"({"name": "John Doe","id": 123,"email": "QXUeh@example.com", "phones": [ {"number": "555-4321","type": "HOME"}]})",
                R"({"name": "John Doe","id": 123,"email": "QXUeh@example.com", "phones": [ {"number": "555-4321","type": 1}]})",
                R"({"name2": "Jack","name": "John Doe","id": 123,"email": "QXUeh@example.com", "phones": [ {"number": "555-4321","type": "HOME"}]})",
            };
            for( auto json : jsons )
            {
                const auto person = sds::json::deserialize< PhoneBook::Person >( json );
                CHECK( person.name == "John Doe" );
                CHECK( person.id == 123 );
                CHECK( person.email == "QXUeh@example.com" );
                CHECK( person.phones.size( ) == 1 );
                CHECK( person.phones[ 0 ].number == "555-4321" );
                CHECK( person.phones[ 0 ].type == PhoneBook::Person::PhoneType::HOME );
            }
        }
        SUBCASE( "bool" )
        {
            CHECK( sds::json::detail::deserialize< bool >( "true" ) == true );
            CHECK( sds::json::detail::deserialize< bool >( "false" ) == false );
            CHECK_THROWS( sds::json::detail::deserialize< bool >( "hello" ) );
            auto value = false;
            CHECK_NOTHROW( sds::json::detail::deserialize( "true", value ) );
            CHECK( value );
            CHECK_NOTHROW( sds::json::detail::deserialize( "false", value ) );
            CHECK( value == false );
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::deserialize< std::vector< bool > >( R"([true,false])" ) == std::vector< bool >{ true, false } );
                CHECK( sds::json::detail::deserialize< std::vector< bool > >( R"([])" ) == std::vector< bool >{ } );
                CHECK( sds::json::detail::deserialize< std::vector< bool > >( R"(null)" ) == std::vector< bool >{ } );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< bool > >( R"()" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< bool > >( R"(true)" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< bool > >( R"([null])" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< bool > >( R"("hello")" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< bool > >( R"([)" ) );
            }
            SUBCASE( "optional" )
            {
                CHECK( sds::json::detail::deserialize< std::optional< bool > >( "true" ) == std::optional< bool >( true ) );
                CHECK( sds::json::detail::deserialize< std::optional< bool > >( "false" ) == std::optional< bool >( false ) );
                CHECK( sds::json::detail::deserialize< std::optional< bool > >( "null" ) == std::optional< bool >( ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< bool > >( "hello" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< bool > >( R"()" ) );

                auto value = std::optional< bool >( );
                CHECK_NOTHROW( sds::json::detail::deserialize( "true", value ) );
                CHECK( value == true );
                CHECK_NOTHROW( sds::json::detail::deserialize( "false", value ) );
                CHECK( value == false );
            }
            SUBCASE( "ptr" )
            {
                CHECK( *sds::json::detail::deserialize< std::unique_ptr< bool > >( "true" ) == true );
                CHECK( *sds::json::detail::deserialize< std::unique_ptr< bool > >( "false" ) == false );
                CHECK( sds::json::detail::deserialize< std::unique_ptr< bool > >( "null" ) == std::unique_ptr< bool >( ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::unique_ptr< bool > >( "hello" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::unique_ptr< bool > >( R"()" ) );

                auto value = std::unique_ptr< bool >( );
                CHECK_NOTHROW( sds::json::detail::deserialize( "true", value ) );
                CHECK( *value == true );
                CHECK_NOTHROW( sds::json::detail::deserialize( "false", value ) );
                CHECK( *value == false );
            }
        }
        SUBCASE( "float" )
        {
            CHECK( sds::json::detail::deserialize< float >( "0" ) == 0 );
            CHECK( sds::json::detail::deserialize< float >( "42" ) == 42 );
            CHECK( sds::json::detail::deserialize< float >( "3.14" ) == 3.14f );
            CHECK( sds::json::detail::deserialize< float >( "0.0" ) == 0.0f );
            CHECK( sds::json::detail::deserialize< float >( "-0.0" ) == -0.0f );
            CHECK( sds::json::detail::deserialize< float >( "-3.14" ) == -3.14f );
            CHECK( sds::json::detail::deserialize< float >( "3.14e+10" ) == 3.14e+10f );
            CHECK( sds::json::detail::deserialize< float >( "3.14e-10" ) == 3.14e-10f );
            CHECK( sds::json::detail::deserialize< float >( "3.14E+10" ) == 3.14E+10f );
            CHECK( sds::json::detail::deserialize< float >( "3.14E-10" ) == 3.14E-10f );
            CHECK_THROWS( sds::json::detail::deserialize< float >( "hello" ) );
            CHECK_THROWS( sds::json::detail::deserialize< float >( "" ) );
            CHECK_THROWS( sds::json::detail::deserialize< float >( "true" ) );
            CHECK_THROWS( sds::json::detail::deserialize< float >( R"("hello")" ) );
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::deserialize< std::vector< float > >( "[]" ) == std::vector< float >{ } );
                CHECK( sds::json::detail::deserialize< std::vector< float > >( "null" ) == std::vector< float >{ } );
                CHECK( sds::json::detail::deserialize< std::vector< float > >( "[42]" ) == std::vector< float >{ 42 } );
                CHECK( sds::json::detail::deserialize< std::vector< float > >( "[42,3.14]" ) == std::vector< float >{ 42, 3.14f } );
                CHECK( sds::json::detail::deserialize< std::vector< float > >( "[42,3.14,0.0]" ) == std::vector< float >{ 42, 3.14f, 0.0f } );
                CHECK( sds::json::detail::deserialize< std::vector< float > >( "[42,3.14,-0.0]" ) == std::vector< float >{ 42, 3.14f, -0.0f } );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< float > >( "42" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< float > >( "true" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< float > >( "hello" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< float > >( R"([)" ) );
            }
            SUBCASE( "optional" )
            {
                auto value = std::optional< float >( );
                CHECK_NOTHROW( sds::json::detail::deserialize( "42", value ) );
                CHECK( *value == 42.0f );
                CHECK_NOTHROW( sds::json::detail::deserialize( "3.14", value ) );
                CHECK( *value == 3.14f );
                CHECK_NOTHROW( sds::json::detail::deserialize( "0.0", value ) );
                CHECK( *value == 0.0f );
                CHECK_NOTHROW( sds::json::detail::deserialize( "-3.14", value ) );
                CHECK( *value == -3.14f );
                CHECK( sds::json::detail::deserialize< std::optional< float > >( "42" ) == 42 );
                CHECK( sds::json::detail::deserialize< std::optional< float > >( "null" ) == std::nullopt );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< float > >( "hello" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< float > >( "" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< float > >( "true" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< float > >( R"("hello")" ) );
            }
        }
        SUBCASE( "int32" )
        {
            CHECK( sds::json::detail::deserialize< int32_t >( "42" ) == 42 );
            CHECK( sds::json::detail::deserialize< int32_t >( "-42" ) == -42 );
            CHECK( sds::json::detail::deserialize< int32_t >( "0" ) == 0 );
            CHECK( sds::json::detail::deserialize< int32_t >( "2147483647" ) == 2147483647 );
            CHECK( sds::json::detail::deserialize< int32_t >( "-2147483648" ) == -2147483648 );
            CHECK_THROWS( sds::json::detail::deserialize< int32_t >( "hello" ) );
            CHECK_THROWS( sds::json::detail::deserialize< int32_t >( "" ) );
            CHECK_THROWS( sds::json::detail::deserialize< int32_t >( "true" ) );
            CHECK_THROWS( sds::json::detail::deserialize< int32_t >( R"("hello")" ) );
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::deserialize< std::vector< int32_t > >( R"([42,-42,0])" ) == std::vector< int32_t >{ 42, -42, 0 } );
                CHECK( sds::json::detail::deserialize< std::vector< int32_t > >( R"([-2147483648,2147483647])" ) == std::vector< int32_t >{ -2147483648, 2147483647 } );
                CHECK( sds::json::detail::deserialize< std::vector< int32_t > >( R"([])" ) == std::vector< int32_t >( ) );
                CHECK( sds::json::detail::deserialize< std::vector< int32_t > >( R"(null)" ) == std::vector< int32_t >( ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< int32_t > >( R"([)" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< int32_t > >( R"(])" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::vector< int32_t > >( R"([42,)" ) );
            }
            SUBCASE( "optional" )
            {
                CHECK( sds::json::detail::deserialize< std::optional< int32_t > >( "42" ) == std::optional< int32_t >( 42 ) );
                CHECK( sds::json::detail::deserialize< std::optional< int32_t > >( "-42" ) == std::optional< int32_t >( -42 ) );
                CHECK( sds::json::detail::deserialize< std::optional< int32_t > >( "0" ) == std::optional< int32_t >( 0 ) );
                CHECK( sds::json::detail::deserialize< std::optional< int32_t > >( "2147483647" ) == std::optional< int32_t >( 2147483647 ) );
                CHECK( sds::json::detail::deserialize< std::optional< int32_t > >( "-2147483648" ) == std::optional< int32_t >( -2147483648 ) );
                CHECK( sds::json::detail::deserialize< std::optional< int32_t > >( "null" ) == std::optional< int32_t >( ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< int32_t > >( "hello" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< int32_t > >( R"()" ) );
            }
            SUBCASE( "ptr" )
            {
                auto value = std::unique_ptr< int32_t >( );
                CHECK_NOTHROW( sds::json::detail::deserialize( "42", value ) );
                CHECK( *value == 42 );
                CHECK_NOTHROW( sds::json::detail::deserialize( "-42", value ) );
                CHECK( *value == -42 );
                CHECK_NOTHROW( sds::json::detail::deserialize( "0", value ) );
                CHECK( *value == 0 );
                CHECK_NOTHROW( sds::json::detail::deserialize( "2147483647", value ) );
                CHECK( *value == 2147483647 );
                CHECK_NOTHROW( sds::json::detail::deserialize( "-2147483648", value ) );
                CHECK( *value == -2147483648 );
                CHECK_NOTHROW( sds::json::detail::deserialize( "null", value ) );
                CHECK( value == nullptr );
            }
        }

        SUBCASE( "array" )
        {
            CHECK( sds::json::detail::deserialize< std::vector< std::string > >( R"(["hello","world"])" ) == std::vector< std::string >{ "hello", "world" } );
            CHECK( sds::json::detail::deserialize< std::vector< std::string > >( R"([])" ) == std::vector< std::string >{ } );
            CHECK( sds::json::detail::deserialize< std::vector< std::string > >( R"(null)" ) == std::vector< std::string >{ } );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::string > >( R"()" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::string > >( R"(true)" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::string > >( R"("hello"])" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::string > >( R"([)" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::string > >( R"(["hello")" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::string > >( R"(["hello",)" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::string > >( R"(["hello",])" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::string > >( R"(["hello","world")" ) );
        }
        SUBCASE( "bytes" )
        {
            CHECK( sds::json::detail::deserialize< std::vector< std::byte > >( R"("AAECAwQ=")" ) == std::vector< std::byte >{ std::byte( 0 ), std::byte( 1 ), std::byte( 2 ), std::byte( 3 ), std::byte( 4 ) } );
            CHECK( sds::json::detail::deserialize< std::vector< std::byte > >( R"("aGVsbG8=")" ) == std::vector< std::byte >{ std::byte( 'h' ), std::byte( 'e' ), std::byte( 'l' ), std::byte( 'l' ), std::byte( 'o' ) } );
            CHECK( sds::json::detail::deserialize< std::vector< std::byte > >( R"(null)" ) == std::vector< std::byte >{ } );
            CHECK( sds::json::detail::deserialize< std::vector< std::byte > >( R"("")" ) == std::vector< std::byte >{ } );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::byte > >( R"(true)" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::byte > >( R"("AAECAwQ")" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::byte > >( R"([])" ) );
            CHECK_THROWS( sds::json::detail::deserialize< std::vector< std::byte > >( R"()" ) );
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::deserialize< std::vector< std::vector< std::byte > > >( R"(["AAECAwQ="])" ) == std::vector< std::vector< std::byte > >{ std::vector< std::byte >{ std::byte( 0 ), std::byte( 1 ), std::byte( 2 ), std::byte( 3 ), std::byte( 4 ) } } );
                CHECK( sds::json::detail::deserialize< std::vector< std::vector< std::byte > > >( R"(["AAECAwQ=","aGVsbG8="])" ) == std::vector< std::vector< std::byte > >{ std::vector< std::byte >{ std::byte( 0 ), std::byte( 1 ), std::byte( 2 ), std::byte( 3 ), std::byte( 4 ) }, std::vector< std::byte >{ std::byte( 'h' ), std::byte( 'e' ), std::byte( 'l' ), std::byte( 'l' ), std::byte( 'o' ) } } );
                CHECK( sds::json::detail::deserialize< std::vector< std::vector< std::byte > > >( R"([])" ) == std::vector< std::vector< std::byte > >{ } );
                CHECK( sds::json::detail::deserialize< std::vector< std::vector< std::byte > > >( R"(null)" ) == std::vector< std::vector< std::byte > >{ } );
                CHECK( sds::json::detail::deserialize< std::vector< std::vector< std::byte > > >( R"([""])" ) == std::vector< std::vector< std::byte > >{ std::vector< std::byte >{} } );
            }
            SUBCASE( "optional" )
            {
                CHECK( sds::json::detail::deserialize< std::optional< std::vector< std::byte > > >( R"(null)" ) == std::nullopt );
                CHECK( sds::json::detail::deserialize< std::optional< std::vector< std::byte > > >( R"("AAECAwQ=")" ) == std::vector< std::byte >{ std::byte( 0 ), std::byte( 1 ), std::byte( 2 ), std::byte( 3 ), std::byte( 4 ) } );
                CHECK( sds::json::detail::deserialize< std::optional< std::vector< std::byte > > >( R"("aGVsbG8=")" ) == std::vector< std::byte >{ std::byte( 'h' ), std::byte( 'e' ), std::byte( 'l' ), std::byte( 'l' ), std::byte( 'o' ) } );
                CHECK( sds::json::detail::deserialize< std::optional< std::vector< std::byte > > >( R"("")" ) == std::vector< std::byte >{ } );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< std::vector< std::byte > > >( R"(true)" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< std::vector< std::byte > > >( R"("AAECAwQ")" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< std::vector< std::byte > > >( R"([])" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::optional< std::vector< std::byte > > >( R"()" ) );
            }
        }
        SUBCASE( "map" )
        {
            SUBCASE( "int32/int32" )
            {
                CHECK( sds::json::detail::deserialize< std::map< int32_t, int32_t > >( R"({"1":2})" ) == std::map< int32_t, int32_t >{ { 1, 2 } } );
                CHECK( sds::json::detail::deserialize< std::map< int32_t, int32_t > >( R"({"1":2,"2":3})" ) == std::map< int32_t, int32_t >{ { 1, 2 }, { 2, 3 } } );
                CHECK( sds::json::detail::deserialize< std::map< int32_t, int32_t > >( R"({})" ) == std::map< int32_t, int32_t >{ } );
                CHECK( sds::json::detail::deserialize< std::map< int32_t, int32_t > >( R"(null)" ) == std::map< int32_t, int32_t >{ } );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< int32_t, int32_t > >( R"()" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< int32_t, int32_t > >( R"("hello")" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< int32_t, int32_t > >( R"({"hello":2})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< int32_t, int32_t > >( R"({"1":"hello"})" ) );
            }
            SUBCASE( "string/string" )
            {
                CHECK( sds::json::detail::deserialize< std::map< std::string, std::string > >( R"({"hello":"world"})" ) == std::map< std::string, std::string >{ { "hello", "world" } } );
                CHECK( sds::json::detail::deserialize< std::map< std::string, std::string > >( R"({})" ) == std::map< std::string, std::string >{ } );
                CHECK( sds::json::detail::deserialize< std::map< std::string, std::string > >( R"(null)" ) == std::map< std::string, std::string >{ } );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< std::string, std::string > >( R"()" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< std::string, std::string > >( R"({"1":["hello"]})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< std::string, std::string > >( R"({"hello":{"hello":"world"}]})" ) );
            }
            SUBCASE( "int32/string" )
            {
                CHECK( sds::json::detail::deserialize< std::map< int32_t, std::string > >( R"({"1":"hello"})" ) == std::map< int32_t, std::string >{ { 1, "hello" } } );
                CHECK( sds::json::detail::deserialize< std::map< int32_t, std::string > >( R"({})" ) == std::map< int32_t, std::string >{ } );
                CHECK( sds::json::detail::deserialize< std::map< int32_t, std::string > >( R"(null)" ) == std::map< int32_t, std::string >{ } );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< int32_t, std::string > >( R"()" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< int32_t, std::string > >( R"({"hello":"world"})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< int32_t, std::string > >( R"({"1":1})" ) );
            }
            SUBCASE( "string/int32" )
            {
                CHECK( sds::json::detail::deserialize< std::map< std::string, int32_t > >( R"({"hello":2})" ) == std::map< std::string, int32_t >{ { "hello", 2 } } );
                CHECK( sds::json::detail::deserialize< std::map< std::string, int32_t > >( R"({})" ) == std::map< std::string, int32_t >{ } );
                CHECK( sds::json::detail::deserialize< std::map< std::string, int32_t > >( R"(null)" ) == std::map< std::string, int32_t >{ } );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< std::string, int32_t > >( R"()" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< std::string, int32_t > >( R"({"2","hello"})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< std::string, int32_t > >( R"({"1":})" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::map< std::string, int32_t > >( R"({"hello":2)" ) );
            }
            SUBCASE( "string/name" )
            {
                CHECK( sds::json::detail::deserialize< std::map< std::string, Test::Name > >( R"({"hello":{"name":"john"}})" ) == std::map< std::string, Test::Name >{ { "hello", { .name = "john" } } } );
            }
        }
        SUBCASE( "string" )
        {
            SUBCASE( "escape" )
            {
                for( int c = 0; c <= 0xff; c++ )
                {
                    const auto esc = c == '"' || c == '\\' || c == '/' || c == 'b' || c == 'f' || c == 'n' || c == 'r' || c == 't';

                    CHECK( sds::json::detail::is_escape( c ) == esc );
                    char buffer[] = { '"', '\\', char( c ), '"', 0 };
                    if( !esc )
                    {
                        CHECK_THROWS( sds::json::detail::deserialize< std::string >( buffer ) );
                    }
                }

                CHECK( sds::json::detail::deserialize< std::string >( R"("hello")" ) == "hello" );
                CHECK( sds::json::detail::deserialize< std::string_view >( R"("hello")" ) == "hello" );
                CHECK_THROWS( sds::json::detail::deserialize< std::string >( R"(hello")" ) );
                CHECK_THROWS( sds::json::detail::deserialize< std::string >( R"("hello)" ) );
                SUBCASE( "escaped" )
                {
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\"")" ) == "\"" );
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\\")" ) == "\\" );
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\/")" ) == "/" );
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\b")" ) == "\b" );
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\f")" ) == "\f" );
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\n")" ) == "\n" );
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\r")" ) == "\r" );
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\t")" ) == "\t" );
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\"\b\f\n\r\t\"")" ) == "\"\b\f\n\r\t\"" );
                    CHECK( sds::json::detail::deserialize< std::string >( R"("\nhell\to")" ) == "\nhell\to" );
                }
            }
        }
        SUBCASE( "variant" )
        {
            SUBCASE( "int" )
            {
                auto variant = sds::json::deserialize< Test::Variant >( R"({"var_int":42})" );
                CHECK( variant.oneof_field.index( ) == 0 );
                CHECK( std::get< 0 >( variant.oneof_field ) == 42 );
            }
            SUBCASE( "string" )
            {
                auto variant = sds::json::deserialize< Test::Variant >( R"({"var_string":"hello"})" );
                CHECK( variant.oneof_field.index( ) == 1 );
                CHECK( std::get< 1 >( variant.oneof_field ) == std::string( "hello" ) );
            }
            SUBCASE( "bytes" )
            {
                auto variant = sds::json::deserialize< Test::Variant >( R"({"var_bytes":"aGVsbG8="})" );
                CHECK( variant.oneof_field.index( ) == 2 );
                CHECK( std::get< 2 >( variant.oneof_field ) == std::vector< std::byte >{ std::byte( 'h' ), std::byte( 'e' ), std::byte( 'l' ), std::byte( 'l' ), std::byte( 'o' ) } );
            }
            SUBCASE( "name" )
            {
                auto variant = sds::json::deserialize< Test::Variant >( R"({"name":{"name":"John"}})" );
                CHECK( variant.oneof_field.index( ) == 3 );
                CHECK( std::get< 3 >( variant.oneof_field ) == Test::Name{ .name = "John" } );
            }
            SUBCASE( "collision" )
            {
                auto variant = sds::json::deserialize< Test::Variant >( R"({"bkfvdzz":{"name":"John"}})" );
                CHECK( variant.oneof_field.index( ) == 0 );
            }
        }
    }
    SUBCASE( "serialize" )
    {
        SUBCASE( "string" )
        {
            CHECK( sds::json::detail::serialize< std::string >( "john" ) == R"("john")" );
            SUBCASE( "escaped" )
            {
                CHECK( sds::json::detail::serialize< std::string >( "\"" ) == R"("\"")" );
                CHECK( sds::json::detail::serialize< std::string >( "\\" ) == R"("\\")" );
                CHECK( sds::json::detail::serialize< std::string >( "/" ) == R"("\/")" );
                CHECK( sds::json::detail::serialize< std::string >( "\b" ) == R"("\b")" );
                CHECK( sds::json::detail::serialize< std::string >( "\f" ) == R"("\f")" );
                CHECK( sds::json::detail::serialize< std::string >( "\n" ) == R"("\n")" );
                CHECK( sds::json::detail::serialize< std::string >( "\r" ) == R"("\r")" );
                CHECK( sds::json::detail::serialize< std::string >( "\t" ) == R"("\t")" );
                CHECK( sds::json::detail::serialize< std::string >( "\"\\/\b\f\n\r\t" ) == R"("\"\\\/\b\f\n\r\t")" );
            }
            SUBCASE( "optional" )
            {
                CHECK( sds::json::detail::serialize< std::optional< std::string > >( std::nullopt ) == "" );
                CHECK( sds::json::detail::serialize< std::optional< std::string > >( "hello" ) == R"("hello")" );
            }
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::serialize< std::vector< std::string > >( { "hello", "world" } ) == R"(["hello","world"])" );
                CHECK( sds::json::detail::serialize< std::vector< std::string > >( { } ) == "" );
            }
        }
        SUBCASE( "string_view" )
        {
            CHECK( sds::json::detail::serialize( "john"sv ) == R"("john")" );
            SUBCASE( "escaped" )
            {
                CHECK( sds::json::detail::serialize( "\""sv ) == R"("\"")" );
                CHECK( sds::json::detail::serialize( "\\"sv ) == R"("\\")" );
                CHECK( sds::json::detail::serialize( "/"sv ) == R"("\/")" );
                CHECK( sds::json::detail::serialize( "\b"sv ) == R"("\b")" );
                CHECK( sds::json::detail::serialize( "\f"sv ) == R"("\f")" );
                CHECK( sds::json::detail::serialize( "\n"sv ) == R"("\n")" );
                CHECK( sds::json::detail::serialize( "\r"sv ) == R"("\r")" );
                CHECK( sds::json::detail::serialize( "\t"sv ) == R"("\t")" );
                CHECK( sds::json::detail::serialize( "he\"\\/\b\f\n\r\tlo"sv ) == R"("he\"\\\/\b\f\n\r\tlo")" );
            }
            SUBCASE( "optional" )
            {
                CHECK( sds::json::detail::serialize< std::optional< std::string_view > >( std::nullopt ) == "" );
                CHECK( sds::json::detail::serialize< std::optional< std::string_view > >( "hello" ) == R"("hello")" );
            }
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::serialize< std::vector< std::string_view > >( { "hello", "world" } ) == R"(["hello","world"])" );
                CHECK( sds::json::detail::serialize< std::vector< std::string_view > >( { } ) == "" );
            }
        }
        SUBCASE( "bool" )
        {
            CHECK( sds::json::detail::serialize( true ) == "true" );
            CHECK( sds::json::detail::serialize( false ) == "false" );
            SUBCASE( "optional" )
            {
                CHECK( sds::json::detail::serialize< std::optional< bool > >( std::nullopt ) == "" );
                CHECK( sds::json::detail::serialize< std::optional< bool > >( true ) == "true" );
                CHECK( sds::json::detail::serialize< std::optional< bool > >( false ) == "false" );
            }
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::serialize< std::vector< bool > >( { true, false } ) == R"([true,false])" );
                CHECK( sds::json::detail::serialize< std::vector< bool > >( { } ) == "" );
            }
        }
        SUBCASE( "int" )
        {
            CHECK( sds::json::detail::serialize( 42 ) == "42" );
            SUBCASE( "optional" )
            {
                CHECK( sds::json::detail::serialize< std::optional< int > >( std::nullopt ) == "" );
                CHECK( sds::json::detail::serialize< std::optional< int > >( 42 ) == "42" );
            }
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::serialize< std::vector< int > >( { 42 } ) == R"([42])" );
                CHECK( sds::json::detail::serialize< std::vector< int > >( { 42, 3 } ) == R"([42,3])" );
                CHECK( sds::json::detail::serialize< std::vector< int > >( { } ) == "" );
            }
        }
        SUBCASE( "double" )
        {
            CHECK( sds::json::detail::serialize( 42.0 ) == "42" );
            SUBCASE( "optional" )
            {
                CHECK( sds::json::detail::serialize< std::optional< double > >( std::nullopt ) == "" );
                CHECK( sds::json::detail::serialize< std::optional< double > >( 42.3 ) == "42.3" );
            }
            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::serialize< std::vector< double > >( { 42.3 } ) == R"([42.3])" );
                CHECK( sds::json::detail::serialize< std::vector< double > >( { 42.3, 3.0 } ) == R"([42.3,3])" );
                CHECK( sds::json::detail::serialize< std::vector< double > >( { } ) == "" );
            }
        }
        SUBCASE( "bytes" )
        {
            CHECK( sds::json::detail::serialize( std::vector< std::byte >{ std::byte{ 0 }, std::byte{ 1 }, std::byte{ 2 } } ) == R"("AAEC")" );

            CHECK( sds::json::detail::serialize< std::vector< std::byte > >( { std::byte( 0 ), std::byte( 1 ), std::byte( 2 ), std::byte( 3 ), std::byte( 4 ) } ) == R"("AAECAwQ=")" );
            CHECK( sds::json::detail::serialize< std::vector< std::byte > >( { std::byte( 'h' ), std::byte( 'e' ), std::byte( 'l' ), std::byte( 'l' ), std::byte( 'o' ) } ) == R"("aGVsbG8=")" );
            CHECK( sds::json::detail::serialize< std::vector< std::byte > >( { } ) == "" );

            SUBCASE( "array" )
            {
                CHECK( sds::json::detail::serialize< std::vector< std::vector< std::byte > > >( std::vector< std::vector< std::byte > >{ std::vector< std::byte >{ std::byte( 0 ), std::byte( 1 ), std::byte( 2 ), std::byte( 3 ), std::byte( 4 ) } } ) == R"(["AAECAwQ="])" );
                CHECK( sds::json::detail::serialize< std::vector< std::vector< std::byte > > >( std::vector< std::vector< std::byte > >{ std::vector< std::byte >{ std::byte( 0 ), std::byte( 1 ), std::byte( 2 ), std::byte( 3 ), std::byte( 4 ) }, std::vector< std::byte >{ std::byte( 'h' ), std::byte( 'e' ), std::byte( 'l' ), std::byte( 'l' ), std::byte( 'o' ) } } ) == R"(["AAECAwQ=","aGVsbG8="])" );
                CHECK( sds::json::detail::serialize< std::vector< std::vector< std::byte > > >( std::vector< std::vector< std::byte > >{ } ) == "" );
            }
            SUBCASE( "optional" )
            {
                CHECK( sds::json::detail::serialize< std::optional< std::vector< std::byte > > >( std::nullopt ) == "" );
                CHECK( sds::json::detail::serialize< std::optional< std::vector< std::byte > > >( std::vector< std::byte >{ std::byte( 0 ), std::byte( 1 ), std::byte( 2 ), std::byte( 3 ), std::byte( 4 ) } ) == R"("AAECAwQ=")" );
                CHECK( sds::json::detail::serialize< std::optional< std::vector< std::byte > > >( std::vector< std::byte >{ std::byte( 'h' ), std::byte( 'e' ), std::byte( 'l' ), std::byte( 'l' ), std::byte( 'o' ) } ) == R"("aGVsbG8=")" );
                CHECK( sds::json::detail::serialize< std::optional< std::vector< std::byte > > >( std::vector< std::byte >{ } ) == "" );
            }
        }
        SUBCASE( "variant" )
        {
            SUBCASE( "int" )
            {
                CHECK( sds::json::serialize( Test::Variant{ .oneof_field = 42U } ) == R"({"var_int":42})" );
            }
            SUBCASE( "string" )
            {
                CHECK( sds::json::serialize( Test::Variant{ .oneof_field = "hello" } ) == R"({"var_string":"hello"})" );
            }
            SUBCASE( "bytes" )
            {
                CHECK( sds::json::serialize( Test::Variant{ .oneof_field = std::vector< std::byte >{ std::byte( 'h' ), std::byte( 'e' ), std::byte( 'l' ), std::byte( 'l' ), std::byte( 'o' ) } } ) == R"({"var_bytes":"aGVsbG8="})" );
            }
            SUBCASE( "name" )
            {
                CHECK( sds::json::serialize( Test::Variant{ .oneof_field = Test::Name{ .name = "John" } } ) == R"({"name":{"name":"John"}})" );
            }
        }
        SUBCASE( "map" )
        {
            SUBCASE( "int32/int32" )
            {
                CHECK( sds::json::detail::serialize( std::map< int32_t, int32_t >{ { 1, 2 } } ) == R"({"1":2})" );
                CHECK( sds::json::detail::serialize( std::map< int32_t, int32_t >{ { 1, 2 }, { 2, 3 } } ) == R"({"1":2,"2":3})" );
                CHECK( sds::json::detail::serialize( std::map< int32_t, int32_t >{ } ) == "" );
            }
            SUBCASE( "string/string" )
            {
                CHECK( sds::json::detail::serialize( std::map< std::string, std::string >{ { "hello", "world" } } ) == R"({"hello":"world"})" );
                CHECK( sds::json::detail::serialize( std::map< std::string, std::string >{ } ) == "" );
            }
            SUBCASE( "int32/string" )
            {
                CHECK( sds::json::detail::serialize( std::map< int32_t, std::string >{ { 1, "hello" } } ) == ( R"({"1":"hello"})" ) );
                CHECK( sds::json::detail::serialize( std::map< int32_t, std::string >{ } ) == "" );
            }
            SUBCASE( "string/int32" )
            {
                CHECK( sds::json::detail::serialize( std::map< std::string, int32_t >{ { "hello", 2 } } ) == R"({"hello":2})" );
                CHECK( sds::json::detail::serialize( std::map< std::string, int32_t >{ } ) == "" );
            }
            SUBCASE( "string/name" )
            {
                CHECK( sds::json::detail::serialize( std::map< std::string, Test::Name >{ { "hello", { .name = "john" } } } ) == R"({"hello":{"name":"john"}})" );
            }
        }
        SUBCASE( "person" )
        {
            CHECK( sds::json::serialize( PhoneBook::Person{
                       .name   = "John Doe",
                       .id     = 123,
                       .email  = "QXUeh@example.com",
                       .phones = {
                           PhoneBook::Person::PhoneNumber{
                               .number = "555-4321",
                               .type   = PhoneBook::Person::PhoneType::HOME,
                           },
                       },
                   } ) == R"({"name":"John Doe","id":123,"email":"QXUeh@example.com","phones":[{"number":"555-4321","type":"HOME"}]})" );
        }
        SUBCASE( "name" )
        {
            CHECK( sds::json::serialize( Test::Name{ } ) == R"({})" );
        }
    }
}
