// Copyright (c) 2017-2022 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

#include <filesystem>
#include <fstream>

#include <tao/pegtl.hpp>
// #include <tao/pegtl/contrib/analyze.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

#include "proto_parser.h"

template < typename Node >
void print_dot_horizontal( std::ostream & os, const Node & n )
{
    os << "digraph parse_tree\n{\n";
    os << "  rankdir=LR;\n";
    tao::pegtl::parse_tree::internal::print_dot_node( os, n, n.is_root( ) ? "ROOT" : n.type );
    os << "}\n";
}

int main( int argc, char ** argv )
{
    using namespace TAO_PEGTL_NAMESPACE;

    if( analyze< proto::grammar::proto_file >( ) != 0 )
    {
        std::cerr << "Analysis of grammar returned an error" << std::endl;
        return 1;
    }

    auto files = {
        "../test.proto",
        "../nanopb.proto",
        "../descriptor.proto",
    };

    for( const auto & path : files )
    {
        auto file_path = std::filesystem::path( path );
        auto base      = file_path.filename( ).string( );
        auto dot_file  = "graphviz_" + base + ".dot";
        auto svg_file  = "graphviz_" + base + ".svg";

        file_input in( path );

        try
        {
            auto root =
                tao::pegtl::parse_tree::parse< proto::grammar::proto_file, proto::selector >( in );

            if( !root )
            {
                std::cout << "Failed to parse";
                exit( 1 );
            }

            {
                std::ofstream out_file( dot_file );
                print_dot_horizontal( out_file, *root );
            }

            std::string cmd =
                "cat " + dot_file + " | sed -e 's/proto::grammar:://g' | dot -Tsvg > " + svg_file;
            system( cmd.c_str( ) );
        }
        catch( const parse_error & e )
        {
            const auto p = e.positions( ).front( );
            std::cerr << e.what( ) << '\n'
                      << in.line_at( p ) << '\n'
                      << std::setw( p.column ) << '^' << '\n';
        }
    }

    return 0;
}
