#include "project_config.h"


#ifdef USE_TESTS

#include "umba_cpp_test/umba_cpp_tests.h"
#include "common_functions/common_functions.h"
#include "callbacks_cpp11.h"

using namespace callback;

namespace
{
    /***************************************************************************************************
                                           Тестовые данные
    ***************************************************************************************************/
    UMBA_TEST_GROUP( "callbacks cpp11" )

    /***************************************************************************************************
                                       Объекты для тестов
    ***************************************************************************************************/
    static int functionCalledCode = 0;

    template <class T>
    int caller( T t, int a )
    {
        return t(a);
    }

    class BaseClassWithVirtualMethod
    {
    public:
        virtual int operator()( int a )
        {
            functionCalledCode = 110;
            return a;
        }
        virtual int foo( int a )
        {
            functionCalledCode = 100;
            return a;
        }

    };

    class DerivedClassWithNoFoo : public BaseClassWithVirtualMethod
    {

    };

    int foo1( int a )
    {
        functionCalledCode = 1;
        return a;
    }

    int foo2( int a )
    {
        functionCalledCode = 51;
        return a;
    }

    class SimpleClass
    {
    public:

        static int foo1Static( int a )
        {
            functionCalledCode = 3;
            return a;
        }

        int foo1(int a)
        {
            functionCalledCode = 2;
            return a;
        }
        int foo2(int a) const
        {
            functionCalledCode = 101;
            return a;
        }
    };

    class SimpleFunctor
    {
    public:

        int operator()( int a )
        {
            functionCalledCode = 5;
            return a;
        }

        int operator()( int a ) const
        {
            functionCalledCode = 115;
            return a;
        }
    };

    class SimpleConstFunctor
    {
    public:

        int operator()( int a ) const
        {
            functionCalledCode = 105;
            return a;
        }
    };

    class SomeBaseClass
    {
    public:

        virtual int foo1(int a)
        {
            functionCalledCode = 6;
            return a;
        }
    };

    class DerivedFromBase : public SomeBaseClass
    {
    public:

        virtual int foo1(int a) override
        {
            functionCalledCode = 7;
            return a;
        }
    };

    class ComplexClass : public SomeBaseClass, public SimpleClass
    {
    public:

        virtual int foo1(int a) override
        {
            functionCalledCode = 8;
            return a;
        }
    };

    class VirtualBase1 : public virtual SimpleClass
    {
    public:

        virtual int foo2( int a )
        {
            functionCalledCode = 9;
            return a;
        }
    };


    class VirtualBase2 : public virtual SimpleClass
    {
    public:

        virtual int foo3( int a )
        {
            functionCalledCode = 10;
            return a;
        }
    };

    class DiamondClass : public VirtualBase1, public VirtualBase2
    {

    };

    template <class T>
    class TemplateClass
    {
    public:

        T foo1( T a )
        {
            functionCalledCode = 11;
            return a;
        }

        int foo2( int a )
        {
            functionCalledCode = 12;
            return a;
        }
    };


    class ClassWithTemplateMethod
    {
    public:

        template<class T>
        T foo1( T a )
        {
            functionCalledCode = 13;
            return a;
        }
    };


    template <class T>
    T templateFoo( T a )
    {
        functionCalledCode = 14;
        return a;
    }

    template <class T>
    class TemplateClassWithTemplateMethod
    {
    public:

        template<class U>
        U foo1( U a )
        {
            functionCalledCode = 15;
            return a;
        }

    };

    
    class ClassWithCallback
    {
    public:

        int foo2( int a )
        {
            return m_callback( a );
        }

        int foo1( int a )
        {
            functionCalledCode = 16;
            return a;
        }

    private:

        Callback<int (int)> m_callback = CALLBACK_BIND( *this, ClassWithCallback::foo1 );
    };

    template <class T>
    class TemplateClassWithCallback
    {
    public:
        TemplateClassWithCallback()
        {
            m_callback = CALLBACK_BIND( *this, TemplateClassWithCallback::foo1 );
        }
        TemplateClassWithCallback( int a )
        {
            m_callback = CALLBACK_BIND( *this, TemplateClassWithCallback::foo3<int> );
        }


        int foo2( int a )
        {
            return m_callback( a );
        }

        int foo1( int a )
        {
            functionCalledCode = 17;
            return a;
        }

        template <class U>
        U foo3( U u )
        {
            functionCalledCode = 18;
            return u;
        }

    private:

        Callback<int (int)> m_callback;
    };

    class BaseClassWithCallback
    {
    public:

        void init( Callback<int (int)> in, Callback<int (int)> out )
        {
            m_in = in;
            m_out = out;
        }

        virtual void work( int a )
        {
            int inputData = m_in( a );

            m_out( inputData );

        }

    protected:

        Callback<int (int)> m_in;
        Callback<int (int)> m_out;
    };

    class DerivedClassWithCallback : public BaseClassWithCallback
    {
    public:

        DerivedClassWithCallback( BaseClassWithCallback & base  ) : m_base(base)
        {}

        void init( Callback<int (int)> in, Callback<int (int)> out )
        {
            m_in = in;
            m_out = out;

            m_inFake = CALLBACK_BIND( *this, DerivedClassWithCallback::inMe );
            m_outFake = CALLBACK_BIND( *this, DerivedClassWithCallback::outMe );

            m_base.init( m_inFake, m_outFake );
        }

        virtual void work( int a )
        {
            m_base.work( a );
        }

        int inMe( int a )
        {
            return m_in( a );
        }
        int outMe( int a )
        {
            return m_out( a );
        }

    private:

        BaseClassWithCallback & m_base;

        Callback<int (int)> m_inFake;
        Callback<int (int)> m_outFake;

    };


    UMBA_TEST_SETUP()
    {
        functionCalledCode = 0;
    }

    UMBA_TEST_TEARDOWN()
    {

    }
    
    UMBA_TEST("Call function")
    {
        Callback<int (int)> a;
        a = CALLBACK_BIND( foo1 );
        
        Callback<int (int)> b;
        b = CALLBACK_BIND( foo2 );
        
        UMBA_CHECK( functionCalledCode == 0, "function must not be called" );
        
        b(43);
        a(12);
        
        UMBA_CHECK( functionCalledCode == 1, "function must be called" );

        return 0;
    }

    class SomeClassWithVoidMethod
    {
        public:
            
        void foo( void )
        {}
    };
    
    UMBA_TEST("Call method")
    {
        Callback<int (int)> a;
        SimpleClass b;
        SimpleClass & c = b;

        a = CALLBACK_BIND( c, SimpleClass::foo1 );

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a(12);

        UMBA_CHECK( functionCalledCode == 2, "method must be called" );
        
        
        Callback<void (void)> aa;
        SomeClassWithVoidMethod sss;
        aa = CALLBACK_BIND( sss, SomeClassWithVoidMethod::foo );

        return 0;
    }

    UMBA_TEST("Call static method")
    {
        Callback<int (int)> a;

        a = CALLBACK_BIND( SimpleClass::foo1Static );

        UMBA_CHECK( functionCalledCode == 0, "static method must not be called" );
        a(12);

        UMBA_CHECK( functionCalledCode == 3, "static method must be called" );

        return 0;
    }

    UMBA_TEST("Call lambda")
    {
        Callback<int (int)> a;
        {
            a = CALLBACK_BIND( [](int a)->int{ functionCalledCode = 4; return a;} );
        }
        
        UMBA_CHECK( functionCalledCode == 0, "lambda must not be called" );
        a(12);

        UMBA_CHECK( functionCalledCode == 4, "lambda must be called" );

        return 0;
    }

    UMBA_TEST("Call functor")
    {
        Callback<int (int)> a;
        SimpleFunctor b;

        a = CALLBACK_BIND( b );

        UMBA_CHECK( functionCalledCode == 0, "functor must not be called" );
        a(12);

        UMBA_CHECK( functionCalledCode == 5, "functor must be called" );

        return 0;
    }
    UMBA_TEST( "Call constant methods" )
    {
        Callback<int (int)> a;
        const SimpleClass b;
        SimpleClass c;

        a = CALLBACK_BIND( b, SimpleClass::foo2 );
        UMBA_CHECK( functionCalledCode == 0, "const method must not be called" );
        a(12);
        UMBA_CHECK( functionCalledCode == 101, "const method must be called" );
        functionCalledCode = 0;

        a = CALLBACK_BIND( c, SimpleClass::foo2 );
        UMBA_CHECK( functionCalledCode == 0, "const method must not be called" );
        a(12);
        UMBA_CHECK( functionCalledCode == 101, "const method must be called" );
        functionCalledCode = 0;

        SimpleConstFunctor d;
        const SimpleConstFunctor e;

        a = CALLBACK_BIND( d );
        UMBA_CHECK( functionCalledCode == 0, "const method must not be called" );
        a(12);
        UMBA_CHECK( functionCalledCode == 105, "const method must be called" );
        functionCalledCode = 0;

        a = CALLBACK_BIND( e );
        UMBA_CHECK( functionCalledCode == 0, "const method must not be called" );
        a(12);
        UMBA_CHECK( functionCalledCode == 105, "const method must be called" );
        functionCalledCode = 0;

        return 0;
    }

//    UMBA_TEST("Check nullptr calling")
//    {
//        Callback<int (int)> a;
//
//        UMBA_CHECK( true, "must fall to hardfault" );
//        a(12);
//
//        UMBA_CHECK( false, "must not came here" );
//    }

    UMBA_TEST("Call virtual method")
    {
        Callback<int (int)> a;
        SomeBaseClass c;
        DerivedFromBase d;

        BaseClassWithVirtualMethod e;
        DerivedClassWithNoFoo f;

        printf( "safsfa %d \n", sizeof(&SomeBaseClass::foo1) );
        
        a = CALLBACK_BIND( c, SomeBaseClass::foo1 );
            UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a(12);
        UMBA_CHECK( functionCalledCode == 6, "method must be called" );
        functionCalledCode = 0;

        a = CALLBACK_BIND( d, DerivedFromBase::foo1 );
        a(12);
        UMBA_CHECK( functionCalledCode == 7, "method must be called" );
        functionCalledCode = 0;

        a = CALLBACK_BIND( f, DerivedClassWithNoFoo::foo );
        a(12);
        UMBA_CHECK( functionCalledCode == 100, "method must be called" );
        functionCalledCode = 0;

        a = CALLBACK_BIND( f );
        a(12);
        UMBA_CHECK( functionCalledCode == 110, "method must be called" );

        return 0;
    }

    UMBA_TEST("Call virtual method in strange class hierarchy")
    {
        Callback<int (int)> a;
        ComplexClass e;

        a = CALLBACK_BIND( e, ComplexClass::foo1 );

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a(12);

        UMBA_CHECK( functionCalledCode == 8, "method must be called" );

        return 0;
    }

    UMBA_TEST("Call method of base class")
    {
        Callback<int (int)> a;
        ComplexClass e;

        a = CALLBACK_BIND( e, SimpleClass::foo1 );

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a(12);

        UMBA_CHECK( functionCalledCode == 2, "method must be called" );

        return 0;
    }

    //это тест на одну из предполагаемых реализаций уарта
    UMBA_TEST("Call method of class with virtual inheritance")
    {
        Callback<int (int)> a;
        DiamondClass h;

        a = CALLBACK_BIND( h, DiamondClass::foo1 );

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a(12);

        UMBA_CHECK( functionCalledCode == 2, "method must be called" );

        a = CALLBACK_BIND( h, DiamondClass::foo2 );
        a(12);
        UMBA_CHECK( functionCalledCode == 9, "method must be called" );

        a = CALLBACK_BIND( h, DiamondClass::foo3 );
        a(12);
        UMBA_CHECK( functionCalledCode == 10, "method must be called" );

        return 0;
    }


    UMBA_TEST("Call method of template class")
    {
        Callback<int (int)> a1;
        Callback<float (float)> a2;
        TemplateClass<int> i1;
        TemplateClass<float> i2;

        a1 = CALLBACK_BIND( i1, TemplateClass<int>::foo1 );
        a2 = CALLBACK_BIND( i2, TemplateClass<float>::foo1 );

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a1(12);
        UMBA_CHECK( functionCalledCode == 11, "method must be called" );

        a1 = CALLBACK_BIND( i1, TemplateClass<int>::foo2 );
        a1(12);
        UMBA_CHECK( functionCalledCode == 12, "method must be called" );

        a2(12);
        UMBA_CHECK( functionCalledCode == 11, "method must be called" );


        a1 = CALLBACK_BIND( i2, TemplateClass<float>::foo2 );
        a1(12);
        UMBA_CHECK( functionCalledCode == 12, "method must be called" );

        return 0;
    }


    UMBA_TEST("Call template method of non-template class")
    {
        Callback<int (int)> a1;
        Callback<float (float)> a2;
        ClassWithTemplateMethod b;

        a1 = CALLBACK_BIND( b, ClassWithTemplateMethod::foo1<int> );
        a2 = CALLBACK_BIND( b, ClassWithTemplateMethod::foo1<float> );

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a1(12);
        UMBA_CHECK( functionCalledCode == 13, "method must be called" );

        functionCalledCode = 0;
        a2(12);
        UMBA_CHECK( functionCalledCode == 13, "method must be called" );

        return 0;
    }

    UMBA_TEST("Call template function")
    {
        Callback<int (int)> a1;
        Callback<float (float)> a2;

        a1 = CALLBACK_BIND( templateFoo<int> );
        a2 = CALLBACK_BIND( templateFoo<float> );

        UMBA_CHECK( functionCalledCode == 0, "function must not be called" );
        a1(12);
        UMBA_CHECK( functionCalledCode == 14, "function must be called" );

        functionCalledCode = 0;
        a2(12);
        UMBA_CHECK( functionCalledCode == 14, "function must be called" );

        return 0;
    }


    UMBA_TEST("Call template method of template class")
    {
        Callback<int (int)> a1;
        Callback<float (float)> a2;
        TemplateClassWithTemplateMethod<int> b;
        TemplateClassWithTemplateMethod<float> c;

        a1 = CALLBACK_BIND( b, TemplateClassWithTemplateMethod<int>::foo1<int> );
        a2 = CALLBACK_BIND( b, TemplateClassWithTemplateMethod<int>::foo1<float> );

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a1(12);
        UMBA_CHECK( functionCalledCode == 15, "method must be called" );

        functionCalledCode = 0;
        a2(12);
        UMBA_CHECK( functionCalledCode == 15, "method must be called" );


        a1 = CALLBACK_BIND( c, TemplateClassWithTemplateMethod<float>::foo1<int> );
        a2 = CALLBACK_BIND( c, TemplateClassWithTemplateMethod<float>::foo1<float> );
        functionCalledCode = 0;
        a1(12);
        UMBA_CHECK( functionCalledCode == 15, "method must be called" );
        functionCalledCode = 0;
        a2(12);
        UMBA_CHECK( functionCalledCode == 15, "method must be called" );

        return 0;
    }

    UMBA_TEST("Check class with callback")
    {
        ClassWithCallback a;

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a.foo2(12);
        UMBA_CHECK( functionCalledCode == 16, "method must be called" );


        return 0;
    }

    UMBA_TEST("Check template class with callback")
    {
        TemplateClassWithCallback<float> a;

        TemplateClassWithCallback<int> b;

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a.foo2(12);
        UMBA_CHECK( functionCalledCode == 17, "method must be called" );
        functionCalledCode = 0;

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        b.foo2(12);
        UMBA_CHECK( functionCalledCode == 17, "method must be called" );

        return 0;
    }


    UMBA_TEST("Check template class with template method callback")
    {
        TemplateClassWithCallback<float> a(123);

        TemplateClassWithCallback<int> b(32);

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        a.foo2(12);
        UMBA_CHECK( functionCalledCode == 18, "method must be called" );
        functionCalledCode = 0;

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        b.foo2(12);
        UMBA_CHECK( functionCalledCode == 18, "method must be called" );

        return 0;
    }

    UMBA_TEST("Check simple equality")
    {
        SimpleClass a;
        
        callback::Callback<int (int)> cal1;
        callback::Callback<int (int)> cal2;

        cal1 = CALLBACK_BIND( foo1 );
        cal2 = CALLBACK_BIND( foo1 );

        UMBA_CHECK( ( cal1 == cal2 ) == true, "callbacks must be equal" );
        UMBA_CHECK( ( cal1 != cal2 ) == false, "callbacks must be equal" );

        cal2 = CALLBACK_BIND( a, SimpleClass::foo1 );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        return 0;
    }

    UMBA_TEST("Check equality one class two methods")
    {
        TemplateClass<int> a;
        TemplateClass<int> b;

        callback::Callback<int (int)> cal1;
        callback::Callback<int (int)> cal2;

        cal1 = CALLBACK_BIND( a, TemplateClass<int>::foo1 );
        cal2 = CALLBACK_BIND( a, TemplateClass<int>::foo2  );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        cal2 = CALLBACK_BIND( b, TemplateClass<int>::foo1 );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        Callback<int (int)> c;
        TemplateClassWithCallback<int> d;
        c = CALLBACK_BIND( d, TemplateClassWithCallback<int>::foo3<int> );//.bind< callback::Deref<decltype( d )>::Type, decltype( &TemplateClassWithCallback<int>::foo3<int> ), &TemplateClassWithCallback<int>::foo3<int> >( d );

        return 0;
    }

    UMBA_TEST("Check null comparison")
    {
        TemplateClass<int> a;

        callback::Callback<int (int)> cal1;
        callback::Callback<int (int)> cal2;

        cal1 = CALLBACK_BIND( a, TemplateClass<int>::foo1 );

        UMBA_CHECK( !( cal1 == (int32_t)0 ), "callbacks must not be null" );


        return 0;
    }

    UMBA_TEST("Check equality of lambdas")
    {
        callback::Callback<int (int)> cal1;
        callback::Callback<int (int)> cal2;

        cal1 = CALLBACK_BIND( [](int a){ return a;} );
        cal2 = CALLBACK_BIND( [](int a){ return a;} );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        return 0;
    }

    UMBA_TEST("Check equality of functors")
    {
        SimpleFunctor a;
        SimpleConstFunctor b;
        const SimpleFunctor c;

        callback::Callback<int (int)> cal1;
        callback::Callback<int (int)> cal2;

        cal1 = CALLBACK_BIND( a );
        cal2 = CALLBACK_BIND( b );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        cal2 = CALLBACK_BIND( a );
        UMBA_CHECK( ( cal1 == cal2 ) == true, "callbacks must be equal" );

        cal1 = CALLBACK_BIND( a );
        cal2 = CALLBACK_BIND( c );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        UMBA_CHECK( functionCalledCode == 0, "method must not be called" );
        cal2(12);
        UMBA_CHECK( functionCalledCode == 115, "method must be called" );
        functionCalledCode = 0;

        return 0;
    }

    UMBA_TEST("Check equality of functions")
    {
        callback::Callback<int (int)> cal1;
        callback::Callback<int (int)> cal2;

        cal1 = CALLBACK_BIND( foo1 );
        cal2 = CALLBACK_BIND( foo2 );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        cal2 = CALLBACK_BIND( foo1 );

        UMBA_CHECK( ( cal1 != cal2 ) == false, "callbacks must be equal" );

        return 0;
    }

    UMBA_TEST("Check equality of inherite methods")
    {
        BaseClassWithVirtualMethod a;
        DerivedClassWithNoFoo b;

        callback::Callback<int (int)> cal1;
        callback::Callback<int (int)> cal2;

        cal1 = CALLBACK_BIND( a );
        cal2 = CALLBACK_BIND( b );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        cal1 = CALLBACK_BIND( a, BaseClassWithVirtualMethod::foo );
        cal2 = CALLBACK_BIND( b, DerivedClassWithNoFoo::foo );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        cal1 = CALLBACK_BIND( a, BaseClassWithVirtualMethod::foo );
        cal2 = CALLBACK_BIND( b, BaseClassWithVirtualMethod::foo );

        UMBA_CHECK( ( cal1 == cal2 ) == false, "callbacks must not be equal" );

        return 0;
    }

    UMBA_TEST("Check cast to bool")
    {
        BaseClassWithVirtualMethod a;

        callback::Callback<int (int)> cal1;
        callback::Callback<int (int)> cal2;

        cal1 = CALLBACK_BIND( a );

        UMBA_CHECK( cal1, "callback is not empty" );
        UMBA_CHECK( (bool)cal1 == true, "callback is not empty" );

        UMBA_CHECK( (bool)cal2 == false, "callback is empty" );

        UMBA_CHECK( !cal2, "callback is empty" );

        return 0;
    }

    UMBA_TEST("Check passing callback to function as parameter")
    {
        Callback<int (int)> a;

        functionCalledCode = 0;

        a =CALLBACK_BIND( foo1 );
        caller( a, 12 );

        UMBA_CHECK( functionCalledCode == 1, "function must be called" );
        functionCalledCode = 0;

        a = CALLBACK_BIND( SimpleClass::foo1Static );
        caller( a, 12 );

        UMBA_CHECK( functionCalledCode == 3, "static method must be called" );
        functionCalledCode = 0;

        a = CALLBACK_BIND( [](int a)->int{ functionCalledCode = 4; return a;} );

        caller( a, 12 );

        UMBA_CHECK( functionCalledCode == 4, "lambda must be called" );
        functionCalledCode = 0;

        SimpleFunctor b;

        a = CALLBACK_BIND( b );
        a(12);

        UMBA_CHECK( functionCalledCode == 5, "functor must be called" );

        return 0;
    }


    UMBA_TEST("Check operator =")
    {
        Callback<int (int)> a = CALLBACK_BIND( foo1 );
        Callback<int (int)> b = CALLBACK_BIND( foo2 );

        a(23);
        UMBA_CHECK( functionCalledCode == 1, "function foo1 must be called" );

        b(23);
        UMBA_CHECK( functionCalledCode == 51, "function foo2 must be called" );

        a = b;
        
        a(234);
        
        UMBA_CHECK( functionCalledCode == 51, "function foo2 must be called" );

        a.bind(foo1);
        a = foo1;
        a = [](int m)->int{ return m*3; };

        return 0;
    }

    UMBA_TEST( "Check pass callback as parameter by value" )
    {
        BaseClassWithCallback base;

        Callback<int (int)> in = CALLBACK_BIND( foo1 );
        Callback<int (int)> out = CALLBACK_BIND( foo2 );

        base.init( in, out );

        DerivedClassWithCallback tun( base );
        tun.init( in, out );

        tun.inMe( 23 );
        UMBA_CHECK( functionCalledCode == 1, "must be called foo1" );

        tun.outMe( 23 );
        UMBA_CHECK( functionCalledCode == 51, "must be called foo2" );
        functionCalledCode = 0;

        tun.work( 34 );
        UMBA_CHECK( functionCalledCode == 51, "must be called foo2 at last" );

        return 0;
    }

}

#endif
