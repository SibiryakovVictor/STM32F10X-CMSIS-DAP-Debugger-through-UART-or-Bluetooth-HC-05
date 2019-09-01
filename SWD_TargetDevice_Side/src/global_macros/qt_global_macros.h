#pragma once

/***************************************************************************************************
 В этом файле лежат все макросы и инлайн-функции для всех проектов

***************************************************************************************************/

/***************************************************************************************************
                            Глобальные дефайны
***************************************************************************************************/

// критическая секция - просто заглушка
#define ENTER_CRITICAL_SECTION() {
#define EXIT_CRITICAL_SECTION() }

// просто заглушка
#define DATA_SYNCHRONIZATION_BARRIER()


/*
====================================================================================================
 Это макросы для ассерта времени исполнения - бесконечный цикл, если условие ложно

 if(0) нужен, чтобы убрать предупреждение "statement unreachable"
====================================================================================================
*/

#define UMBA_ASSERT( statement )     Q_ASSERT(statement)

#define UMBA_ASSERT_FAIL()           UMBA_ASSERT( 0 )

/*
====================================================================================================
 Это макрос для ассерта времени компиляции - UMBA_STATIC_ASSERT
====================================================================================================
*/

#define UMBA_STATIC_ASSERT_MSG(condition, msg) typedef char umba_static_assertion_##msg[(condition)?1:-1]
#define UMBA_STATIC_ASSERT3(X, L) UMBA_STATIC_ASSERT_MSG(X, at_line_##L)
#define UMBA_STATIC_ASSERT2(X, L) UMBA_STATIC_ASSERT3(X, L)


#define UMBA_STATIC_ASSERT(X) UMBA_STATIC_ASSERT2(X, __LINE__)

/*
====================================================================================================
 Это макрос для получения количества элементов в массиве
====================================================================================================
*/

#define NUM_ELEM(x) (sizeof (x) / sizeof (x[0]))


/*
====================================================================================================
 Это прагмы для подавления ворнингов компиляторов, версии для Кейла и gcc.

 Пример использования:
    PRAGMA_SUPPRESS_UNUSED_PARAMETER_BEGIN
        .. код, генерирующий ворнинг про неиспользуемый параметр
    PRAGMA_END

====================================================================================================
*/
#if defined ( __GNUC__ )

  #define PRAGMA_END _Pragma("GCC diagnostic pop")

  #define PRAGMA_SUPPRESS_STATEMENT_UNREACHABLE_BEGIN  _Pragma("GCC diagnostic push")

  #define PRAGMA_SUPPRESS_UNUSED_PARAMETER_BEGIN     _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")

  #define PRAGMA_SUPPRESS_POINTLESS_COMPARISON_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")

  #define PRAGMA_SUPPRESS_ALL_WARNINGS_BEGIN   _Pragma("GCC diagnostic push")\
                                               _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")\
                                               _Pragma("GCC diagnostic ignored \"-Weffc++\"")\
                                               _Pragma("GCC diagnostic ignored \"-Wreturn-type\"")\
                                               _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")\
                                               _Pragma("GCC diagnostic ignored \"-Wall\"")


#endif


/*
====================================================================================================
 Это макрос для получения строкового представления элемента enum, если этот enum находится внутри
 класса, содержащего Q OBJECT

 Пример использования:

 QSerialPort::SerialPortError error = QSerialPort::NoError;
 QString errorString;
 STRINGIFY_ENUM(errorString, error, QSerialPort, SerialPortError);

 теперь errorString содержит "NoError"

====================================================================================================
*/
#define STRINGIFY_ENUM(string, error, parentClass, enumName)   { QMetaObject meta = parentClass::staticMetaObject;       \
                                                               for (int i=0; i < meta.enumeratorCount(); ++i)            \
                                                               {                                                         \
                                                                   QMetaEnum m = meta.enumerator(i);                     \
                                                                   if (m.name() == QLatin1String(#enumName))             \
                                                                   {                                                     \
                                                                       string = QLatin1String(m.valueToKey(error));      \
                                                                       break;                                            \
                                                                   }                                                     \
                                                               } }

// это макрос, делающий регистрацию типов чуть проще
#define Q_REGISTER_META_TYPE( type )   qRegisterMetaType< type >( #type )



/***************************************************************************************************
                            Глобальные типы данных
***************************************************************************************************/

// Перечисление в структуре, чтобы не засорять общее пространство имен

// Использование: STRONG_ENUM( EnumName, ENUM_VALUE_1, ENUM_VALUE_2 );

// Предотвращает неявное преобразование значений перечисления к любым типам, кроме типа type,
// что препятствует сравнению значений перечислений с интегральными типами или со значениями
// других перечислений

// Если же вам нужно привести тип от этого перечисления к int, используйте функцию toInt
// Для обратного приведения используйте явный вызов конструктора - SuperType s = SuperType(5)
// или метод fromInt

// Создать экземпляр и не присваивать ему значение нельзя.

#if defined __GNUC__

    // к сожалению, вне кейла не удается совместить строгость и работу в свитче
    #define STRONG_ENUM( Name, ... )                     \
                                                         \
    struct Name                                          \
    {                                                    \
        enum Type                                        \
        {                                                \
            __VA_ARGS__                                  \
        };                                               \
                                                         \
        Type t_;                                         \
                                                         \
        Name(Type t) : t_(t) {}                          \
        explicit Name(int a) : t_( (Type)a) {}           \
        operator Type () const {return t_;}              \
                                                         \
        int toInt() const { return (int)t_; }            \
        void fromInt(int arg) { t_ = (Type)arg; }        \
    }

#endif


/***************************************************************************************************
                            Глобальные inline функции
***************************************************************************************************/

/***************************************************************************************************
                            Глобальные прототипы функций
***************************************************************************************************/
