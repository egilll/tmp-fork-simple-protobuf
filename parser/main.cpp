// Copyright (c) 2017-2022 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

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

    // if( analyze< proto::grammar::proto_file >( ) != 0 )
    // {
    //     std::cerr << "failed to parse proto3.proto" << std::endl;
    //     return 1;
    // }

    file_input in( "../../test/proto/proto3.proto" );
    try
    {
        auto root =
            tao::pegtl::parse_tree::parse< proto::grammar::proto_file, proto::selector >( in );

        {
            std::ofstream out_file( "graphviz.dot" );
            print_dot_horizontal( out_file, *root );
        }

        /* Convert dot file to svg */
        system( "cat graphviz.dot | sed -e 's/proto::grammar:://g' | dot -Tsvg > graphviz.svg" );
    }
    catch( const parse_error & e )
    {
        const auto p = e.positions( ).front( );
        std::cerr << e.what( ) << '\n'
                  << in.line_at( p ) << '\n'
                  << std::setw( p.column ) << '^' << '\n';
    }

    return 0;
}
