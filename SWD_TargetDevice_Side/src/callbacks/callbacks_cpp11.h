/***************************************************************************************************

 Это велосипедная реализация std::function из С++11 - универсальные коллбэки.
 Они позволяют хранить указатель на свободную функцию, на метод класса с указателем на объект или лямбду и
 вызывать их единообразно.

 При создании по-умолчанию инициализируются нулем. Их можно сравнивать друг с другом, сравнивать с нулем.
 Можно явно инициализировать нулем.

 Пример использования:

 callback::Callback<void (int)> c; // создал экземпляр коллбэка, который возвращает void и принимает
                                   // 1 параметр типа int

 c = CALLBACK_BIND( a, A::foo );  // прибиндил к нему метод foo объекта а класса А

 c = CALLBACK_BIND( bar );        // прибиндил к нему свободную функцию bar

 c = CALLBACK_BIND( A::bazz );    // прибиндил к нему статический метод класса А

 c = CALLBACK_BIND( [data](int a){ std::printf("lambda\n");} ); // прибиндил к нему лямбду

 c = CALLBACK_BIND( a );        // прибиндил к нему функтор а ( класс с перегруженным оператором () )


Помимо макросов можно писать так:

callback::Callback<void ( int )> c( bar ); //прибиндил свободную функцию в конструкторе

callback::Callback<void ( int )> c( A::bazz ); //прибиндил статический метод класса А в конструкторе

callback::Callback<void ( int )> c( [data](int a){ std::printf("lambda\n");} ); //прибиндил лямбду в конструкторе

callback::Callback<void ( int )> c( a ); //прибиндил функтор в конструкторе


Или вот так:

c = bar; //прибиндил свободную функцию

c = A::bazz; //прибиндил статический метод класса А

c = [data](int a){ std::printf("lambda\n");}; //прибиндил лямбду

c = a; //прибиндил функтор


Теперь у коллбеков есть конструкторы от callable-объектов, что даёт возможность делать следующее:

void foo( Callback<int (int)> c );

int bar(int);

int main
{
    foo(bar); // передаём в функцию объект Callback, к которому был прибинден bar
}

Это работает с функторами, свободными функциями, статическими методами и лямбдами

 c(5);                            // вызов прибинденного



можно еще вот так биндить, если хочется:

c.bind<bar>(); //прибиндил свободную функцию

c.bind<A::bazz>(); //прибиндил статический метод класса А

c.bind( [data](int a){ std::printf("lambda\n");} ); //прибиндил лямбду

c.bind(a); //прибиндил функтор

c.bind(a, &A::foo2); //прибиндил нестатический метод класса

 Апи перетащено с коллбеков прежних.
***************************************************************************************************/

//#error обнови ретаргет, пжлст, а то тут какой-то заскорузлый и в шланге не собирается

#pragma once

#include <new>
#include <cstddef>

//макросы почти без изменений переехали
// это служебный макрос для перегрузки
#define UNIVERSAL_CALLBACK_GET_BIND_MACRO( _1, _2, NAME, ... )  NAME

// это универсальный макрос для биндинга, он принимает или имя свободной функции или имя объекта+имя метода
// DUMMY нужен, чтобы убрать ворнинг про "ISO C++11 requires at least one argument for the "..." in a variadic macro"
#define CALLBACK_BIND( ... ) UNIVERSAL_CALLBACK_GET_BIND_MACRO( __VA_ARGS__, CALLBACK_BIND_MEMBER, CALLBACK_BIND_FREE, DUMMY ) (__VA_ARGS__)

// это макрос для биндинга метода
#define CALLBACK_BIND_MEMBER( object, member ) ( callback::makeMemberCallback(&member)\
                                                .bind( object, &member ) )

// это макрос для биндинга остального
#define CALLBACK_BIND_FREE( stuff )  ( stuff )


namespace callback
{
    //структура для проверки, является ли метод константным
    template <class T>
    struct IsMethodConst;

    //случай константности
    template <class TObj, class TOut, class ... TIn>
    struct IsMethodConst< TOut (TObj::*)(TIn ...) const >
    {
        static const bool value = true;
    };
    //случай неконстантности
    template <class TObj, class TOut, class ... TIn>
    struct IsMethodConst< TOut (TObj::*)(TIn ...) >
    {
        static const bool value = false;
    };
    
    //енум для возможности использования виртуального оператора сравнения делегатов
    enum class DelegateType{ CONST_MEMBER,
                             MEMBER,
                             FUNCTOR,
                             FUNCTION };

    //В классе Callback лежит указатель на интерфейс функционального объекта IDelegate
    //От этого интерфейса наследуются все реализации делегатов для разного рода функциональных объектов
    //Позволяет биндить Callback с одинаковой сигнатурой к разного рода функциональным объектам
    template <typename TOutput, typename ... TInput>
    class IDelegate
    {

    public:

        virtual TOutput operator() ( TInput ... tInputs ) = 0;
    
        virtual IDelegate * clone( void * ) = 0;

        virtual bool operator==( IDelegate * that ) = 0;

        virtual DelegateType getType( void ) const = 0;
    
        virtual ~IDelegate(){}

    };
    
    //делегат для произвольного метода класса
    template <bool TIsMethodConst, class TObject, typename TMethod, typename TOutput, typename ... TInput>
    class DelegateMethod;
    
    
    //делегат для константного метода
    template <class TObject, typename TMethod, typename TOutput, typename ... TInput>
    class DelegateMethod< true, TObject, TMethod, TOutput, TInput...> : public IDelegate<TOutput, TInput ...>
    {

    public:
    
        DelegateMethod( const TObject & object, TMethod method ) :
            m_object( object ),
            m_method( method )
        {}

        DelegateMethod( const DelegateMethod & that ) :
            m_object( that.m_object ),
            m_method( that.m_method )
        {}

        //нехорошо плодить делегатов просто так, надо делать это клоном
        DelegateMethod & operator=( const DelegateMethod & that ) = delete;

        virtual TOutput operator() ( TInput ... tInputs ) override
        {
            return ( m_object.*m_method )( tInputs ... );
        }
        
        using BaseType = IDelegate<TOutput, TInput ...>;

        virtual BaseType * clone( void * place )
        {
            BaseType * ret = ( BaseType * )new( place ) DelegateMethod( *this );
            return ret;
        }
        
        virtual bool operator==( BaseType * that ) override
        {
            //если тип совпадает, то можно кастовать к нему
            if( that->getType() == getType() )
            {
                return operator==( ( DelegateMethod * )that );
            }
            return false;
        }
        
        bool operator==( DelegateMethod * that )
        {
            return ( ( &m_object == &that->m_object ) && ( m_method == that->m_method ) );
        }

        virtual DelegateType getType( void ) const override
        {
            return DelegateType::CONST_MEMBER;
        }

        virtual ~DelegateMethod() 
        {}

    private:

        const TObject & m_object;
        TMethod m_method;
    };
    
    //делегат для неконстантного метода
    template <class TObject, typename TMethod, typename TOutput, typename ... TInput>
    class DelegateMethod< false, TObject, TMethod, TOutput, TInput...> : public IDelegate<TOutput, TInput ...>
    {

    public:
           
        DelegateMethod( TObject & object, TMethod method ) :
            m_object( object ),
            m_method( method )
        {}

        DelegateMethod( const DelegateMethod & that ) :
            m_object( that.m_object ),
            m_method( that.m_method )
        {}

        //нехорошо плодить делегатов просто так, надо делать это клоном
        DelegateMethod & operator=( const DelegateMethod & that ) = delete;


        virtual TOutput operator() ( TInput ... tInputs ) override
        {
            return ( m_object.*m_method )( tInputs ... );
        }

        
        using BaseType = IDelegate<TOutput, TInput ...>;

        virtual BaseType * clone( void * place ) override
        {
            BaseType * ret = ( BaseType * )new( place ) DelegateMethod( *this );
            return ret;
        }
        
        
        virtual bool operator==( BaseType * that ) override
        {
            //если тип совпадает, то можно кастовать к нему
            if( that->getType() == getType() )
            {
                return operator==( ( DelegateMethod * )that );
            }
            return false;
        }
        
        bool operator==( DelegateMethod * that )
        {
            return ( ( &m_object == &that->m_object) && ( m_method == that->m_method ) );
        }
        virtual DelegateType getType( void ) const override
        {
            return DelegateType::MEMBER;
        }
        
        virtual ~DelegateMethod() 
        {}


    private:

        TObject & m_object;

        TMethod m_method;
    };
    
    
    //делегат для оператора ()
    template <class TObject, typename TOutput, typename ... TInput>
    class DelegateFunctor : public IDelegate<TOutput, TInput ...>
    {

    public:
    
        DelegateFunctor( TObject & object ) : m_object( object )
        {}
        DelegateFunctor( const DelegateFunctor & that ) : m_object( that.m_object ) 
        {}

        //нехорошо плодить делегатов просто так, надо делать это клоном
        DelegateFunctor & operator=( const DelegateFunctor & that ) = delete;

        virtual TOutput operator() ( TInput ... tInputs ) override
        {
            return ( m_object )( tInputs ... );
        }

        using BaseType = IDelegate<TOutput, TInput ...>;

        virtual BaseType * clone( void * place ) override
        {
            BaseType * ret = ( BaseType * )new( place ) DelegateFunctor( *this );
            return ret;
        }
       
        virtual bool operator==( BaseType * that ) override
        {
            //если тип совпадает, то можно кастовать к нему
            if( that->getType() == getType() )
            {
                return operator==( ( DelegateFunctor * )that );
            }
            return false;
        }
        
        bool operator==( DelegateFunctor * that )
        {
            return ( &m_object == &that->m_object );
        }

        virtual DelegateType getType( void ) const override
        {
            return DelegateType::FUNCTOR;
        }

        virtual ~DelegateFunctor() 
        {}

    private:

        TObject & m_object;

    };


    //делегат для указателя на свободную функцию или статического метода
    template <typename TOutput, typename ... TInput>
    class DelegateFunction : public IDelegate<TOutput, TInput ...>
    {

    public:

        DelegateFunction( TOutput ( *function )( TInput ... ) ) : m_function( function )
        {}

        DelegateFunction( const DelegateFunction & that ) : m_function( that.m_function )
        {}

        //нехорошо плодить делегатов просто так, надо делать это клоном
        DelegateFunction & operator=( const DelegateFunction & that ) = delete;

        virtual TOutput operator() ( TInput ... tInputs ) override
        {
            return ( *m_function )( tInputs ... );
        }

        using BaseType = IDelegate<TOutput, TInput ...>;

        virtual BaseType * clone( void * place ) override
        {
            BaseType * ret = ( BaseType * )new( place ) DelegateFunction( *this );
            return ret;
        }
        
        virtual bool operator==( BaseType * that ) override
        {
            //если тип совпадает, то можно кастовать к нему
            if( that->getType() == getType() )
            {
                return operator==( ( DelegateFunction * )that );
            }
            return false;
        }
        
        bool operator==( DelegateFunction * that )
        {
            return ( m_function == that->m_function );
        }
        
        virtual DelegateType getType( void ) const override
        {
            return DelegateType::FUNCTION;
        }
        
        virtual ~DelegateFunction() 
        {}
        
    private:

        TOutput ( *m_function )( TInput ... );
    };


    //пустая структура, можно сунуть в конструктор коллбеку. Осталось для обратной совместимости
    struct NullCallback {};
    
    //класс, экземпляры которого надо использовать для унификации объектов типа callable
    template <class T>
    class Callback;
    
    //функции для создания локального экземпляра коллбека, чтобы CALLBACK_BIND реализовать обратносовместимо
    //создаёт экземпляр коллбека нужного типа
    template<class TObject, typename TOutput, typename ... TInput>
    Callback<TOutput( TInput ... )> makeMemberCallback( TOutput ( TObject::* )( TInput ... ) )
    {
        Callback<TOutput ( TInput ... )> callable;
        return callable;
    }

    //функции для создания локального экземпляра коллбека, чтобы CALLBACK_BIND реализовать обратносовместимо
    template<class TObject, typename TOutput, typename ... TInput>
    Callback<TOutput( TInput ... )> makeMemberCallback( TOutput ( TObject::* )( TInput ... ) const )
    {
        Callback<TOutput ( TInput ... )> callable;
        return callable;
    }
    
    

    //реализация класса универсального коллбека
    template <typename TOutput, typename ... TInput>
    class Callback<TOutput( TInput ... )>
    {

    public:

        Callback( ){}

        explicit Callback( int a )
        {
            ( void )a;
        }

        Callback( decltype( nullptr ) a )
        {
            ( void )a;
        }

        Callback( NullCallback )
        {
        }

        Callback( const Callback& rhs ) : m_delegate( rhs.m_delegate )
        {
            *this = rhs;
        }

        Callback( Callback& rhs ) : m_delegate( rhs.m_delegate )
        {
            *this = rhs;
        }

        //конструктор для лямбды
        template<class TObject>
        Callback( TObject && object )
        {
            bind( object );
        }

        //конструктор для функтора
        template<class TObject>
        Callback( TObject & object )
        {
            bind( object );
        }

        Callback( TOutput ( *fun )( TInput ... ) )
        {
            bind( fun );
        }

        Callback & operator=( NullCallback )
        {
            m_delegate = nullptr;
            return *this;
        }

        Callback & operator=( const Callback & rhs )
        {
            if( rhs.m_delegate != nullptr )
            {
                m_delegate = rhs.m_delegate->clone( m_place );
            }
            return *this;
        }

        Callback & operator=( Callback & rhs )
        {
            if( rhs.m_delegate != nullptr )
            {
                m_delegate = rhs.m_delegate->clone( m_place );
            }
            return *this;
        }


        inline bool operator!() const
        {
            return m_delegate == nullptr;
        }

        operator bool() const
        {
            return !(m_delegate == nullptr);
        }

        TOutput operator() ( TInput ... tInputs )
        {
            return m_delegate->operator()( tInputs ... );
        }
    
        //бинд свободной функции
        Callback & bind( TOutput ( *function )( TInput ... ) )
        {
            m_delegate = new( m_place ) DelegateFunction<TOutput, TInput ...>( function );
            return *this;
        }

        //бинд для произвольного метода
        template<class TObject, typename TMethodPointer>
        Callback & bind( TObject & object, TMethodPointer Tmethod )
        {
            m_delegate = new( m_place ) 
                DelegateMethod<IsMethodConst<TMethodPointer>::value, TObject, TMethodPointer, TOutput, TInput ...> ( object, Tmethod );
      
            return *this;
        }

        //бинд для оператора ()
        template<class TObject>
        Callback & bind( TObject & object )
        {
            m_delegate = new( m_place ) DelegateFunctor<TObject, TOutput, TInput ...>( object );
            
            return *this;
        }

        //бинд для константного оператора ()
        template<class TObject>
        Callback & bind( const TObject & object )
        {
            m_delegate = new( m_place ) DelegateFunctor<const TObject, TOutput, TInput ...>( object );
      
            return *this;
        }

        //бинд для лямбды
        template<typename TObject>
        Callback & bind( TObject && object )
        {
            m_delegate = new( m_place ) DelegateFunctor<TObject, TOutput, TInput ...>( object );
           
            return *this;
        }

        bool operator==( const Callback & that )
        {
            return ( m_delegate->operator==( that.m_delegate ) );
        }

        bool operator!=( const Callback & that )
        {
            return !( operator==( that ) );
        }

        bool operator==( const int num )
        {
            if( num != 0 )
            {
                UMBA_ASSERT_FAIL();
            }
            
            if( m_delegate == nullptr ) 
            {
                return true;
            }
            
            return false;
        }

        bool operator!=( const int num )
        {
            return !( operator==( num ) );
        }


        ~Callback() {}

    private:

        //поиск делегата наибольшего размера для плейсмент нью
        using Method = DelegateMethod<false, Callback, void (Callback::*)(void), void >;
        using Function = DelegateFunction<void>;
        using Functor = DelegateFunctor<Callback, void>;

        static const size_t method_size = sizeof( Method );
        static const size_t function_size = sizeof( Function );
        static const size_t functor_size = sizeof( Functor );

        static const size_t max_size_tmp = ( method_size > function_size ) ? method_size : function_size;
        static const size_t max_size = ( max_size_tmp > functor_size ) ? max_size_tmp : functor_size;

        union
        {
            char m_place[ max_size ];

            //поля, которые нужны для выравнивания в памяти массива байт под плейсмент нью
            Method never_used;
            Function never_used1;
            Functor never_used2;
        };
    
        //указатель на базовый класс, в котором лежит адрес статического функтора
        IDelegate<TOutput, TInput ...> * m_delegate = nullptr;
    };


    // коллбек без параметров
    using VoidCallback =  Callback<void ( void )>;

    
    
}//namespace callback
