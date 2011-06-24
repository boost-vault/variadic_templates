//Purpose:
//  Prototype of haskell-like list comprehension.
//Requirements:
//  The variadic template mpl library:
//    https://svn.boost.org/trac/boost/browser/sandbox/variadic_templates
//
#include <boost/mpl/package.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/arg.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/unpack_args.hpp>

namespace boost
{
namespace mpl
{

  template
  < typename Left
  , typename Right
  >
struct cat_pkg
;  
  template
  < typename... Left
  , typename... Right
  >
struct cat_pkg
  < package<Left...>
  , package<Right...>
  >
{
        typedef 
      package
      < Left...
      , Right...
      >
    type
    ;
};  
  template
  < typename One
  , typename Many
  >
struct cross_one_many
;
  template
  < typename... Many
  >
struct cross_one_many
  < package<>
  , package<Many...>
  >
{
        typedef
      package
      < Many...
      >
    type
    ;
};
  template
  < typename One
  , typename... Many
  >
struct cross_one_many
  < One
  , package<Many...>
  >
{
        typedef
      package
      < typename push_back<One,Many>::type...
      >
    type
    ;
};
  template
  < typename Package
  , typename Op
  >
struct transform_pkg
;
  template
  < typename... Args
  , typename Op
  >
struct transform_pkg
  < package<Args...>
  , Op
  >
{
        typedef
      package
      < typename apply<Op,Args>::type...
      >
    type
    ;
};
  template
  < typename Left
  , typename Right
  >
struct cross_product
{
        typedef
      typename transform_pkg
      < Left
      , cross_one_many
        < arg<1>
        , Right
        >
      >::type
    packages
    ;
        typedef
      typename fold
      < packages
      , package<>
      , cat_pkg<arg<1>,arg<2> >
      >::type
    type
    ;
};

struct cross_prod_nil
{};
  template
  < typename... Right
  >
struct cross_product
  < cross_prod_nil
  , package<Right...>
  >
{
        typedef
      package
      < package<Right>...
      >
    type
    ;
};
  template
  < typename LambdaExpr
  , typename Args
  >
struct apply_each_pkg
;
  template
  < typename LambdaExpr
  , typename... Args
  >
struct apply_each_pkg
  < LambdaExpr
  , package<Args...>
  >
{
        typedef
      package
      < typename apply
        < LambdaExpr
        , Args
        >::type
        ...
      >
    type
    ;
};  

  template
  < typename LambdaExpr 
  , typename Domains
  >
struct list_comprehension
/**@brief
 *  The equivalent of haskell list comprehension expression
 *  (see http://www.haskell.org/haskellwiki/List_comprehension ):
 *
 *    [ LambdaExpr
 *    | arg<1> <- at_c<Domains,0>::type
 *    , arg<2> <- at_c<Domains,1>::type
 *    ...
 *    , arg<N> <- at_c<Domains,N-1>::type
 *    ]
 *
 *  where N=size<Domains>::type::value
 *
 **@input_conditions:
 *  LambdaExpr:
 *    an mpl Lambda Expression with arity=N.
 *    (http://www.boost.org/doc/libs/1_40_0/libs/mpl/doc/refmanual/lambda-expression.html)
 *  Domains:
 *    an mpl sequence of sequences, one for each argument to LambdaExpr.
 **@output_conditions:
 *  Let:
 *    Result = the list_comprehension<LambdaExpr,Domains>:;type;
 *    for all L in 0...N-1:
 *      S_L = size<at_c<Domains,L> >::type::value;
 *  Then:
 *    M = size<Result>::type::value = S_0*S_1*...*S_(N-1)
 *    for each K in 0...M-1:
 *      at_c<Result,K>::type = apply<LambdaExpr, Args_K>::type
 *      where:
 *        size<Args_K>::type::value = N;
 *        for all L in 0...N-1:
 *          at_c<Args_K,L>::type = some element in L-th domain of Domains.
 *      Furthermore, each Args_K is "unique", i.e. it's complosed
 *        different elements from at least one domain.
 */
{
        typedef
      typename fold
      < Domains
      , cross_prod_nil
      , cross_product<arg<1>, arg<2> >
      >::type
    arg_lists
    ;
        typedef
      unpack_args
      < LambdaExpr
      >
    unary_expr
    ;
        typedef
      typename apply_each_pkg
      < unary_expr
      , arg_lists
      >::type
    type
    ;
};

}//exit mpl namespace
}//exit boost namespace

#include <boost/mpl/package_c.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/for_each.hpp>
#include <iostream>

namespace boost
{
namespace mpl
{
    typedef
  package
  < package_c<int,9000,9001>
  , package_c<int,9010,9011,9012>
  , package_c<int,9020,9021>
  >
domains_c
;
  template
  < typename Pkg_c
  >
struct get_pkg_type
{
    typedef typename Pkg_c::pkg_type type;
};    
    typedef
  transform
  < domains_c
  , get_pkg_type<arg<1> > 
  , back_inserter<package<> >
  >::type
domains_t
;
//#define DEBUG_MPL
#ifdef DEBUG_MPL
BOOST_MPL_ASSERT((is_same<void,domains_t>));
    typedef
  cross_product
  < cross_prod_nil
  , at_c<domains_t,0>::type
  >::type
cross_nil_0
;  
BOOST_MPL_ASSERT((is_same<void,cross_nil_0>));
    typedef
  cross_product
  < cross_nil_0
  , at_c<domains_t,1>::type
  >::type
cross_nil_1_0
;  
BOOST_MPL_ASSERT((is_same<void,cross_nil_1_0>));
    typedef
  cross_product
  < cross_nil_1_0
  , at_c<domains_t,2>::type
  >::type
cross_nil_2_1_0
;  
BOOST_MPL_ASSERT((is_same<void,cross_nil_2_1_0>));
    typedef
  fold
  < domains_t
  , cross_prod_nil
  , cross_product<arg<1>, arg<2> >
  >::type
cross_prod_domains
;
BOOST_MPL_ASSERT((is_same<void,cross_prod_domains>));
#endif
  template
  < typename Arg1
  , typename Arg2
  , typename Arg3
  >
struct test_impl
{
    static void exec(void)
    {
        std::cout
        <<"{ "<<Arg1::value
        <<", "<<Arg2::value
        <<", "<<Arg3::value
        <<"}\n";
    }
};

    typedef
  list_comprehension
  < test_impl<arg<1>,arg<2>,arg<3> >
  , domains_t
  >::type
list_comp_result
;
struct call_exec
{
      template
      < typename Exec
      >
    void operator()(Exec)
    {
        Exec::exec();
    }
};
  
void test(void)
{
    call_exec a_caller;
    for_each<list_comp_result>(a_caller);
}
  
}//exit mpl namespace
}//exit boost namespace

int main(void)
{
    boost::mpl::test();
    return 0;
}    
