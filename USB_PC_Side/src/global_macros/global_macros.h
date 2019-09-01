/***************************************************************************************************
 В этом файле лежат все макросы и инлайн-функции для всех проектов
 
***************************************************************************************************/

#pragma once

/***************************************************************************************************
                            Глобальные дефайны
***************************************************************************************************/

/*
====================================================================================================
                            Приоритеты прерываний

 При использовании FreeRTOS если в прерывании вам нужен системный вызов - приоритет прерывания должен
 быть не выше IRQ_PRIORITY_LEVEL_SYSCALL (численно - не меньше).
====================================================================================================
*/


// самый низкий возможный приоритет - зависит от процессора
#define IRQ_PRIORITY_LEVEL_LOWEST    ((1 << __NVIC_PRIO_BITS) - 1)

// самый высокий возможный приоритет
#define IRQ_PRIORITY_LEVEL_HIGHEST   0

#ifdef configMAX_SYSCALL_INTERRUPT_PRIORITY

    // приоритет системного прерывания FreeRTOS, все прерывания, которые используют системные вызовы
    // не должны иметь приоритет выше (численно - меньше)!
    #define IRQ_PRIORITY_LEVEL_MAX_SYSCALL  ( configMAX_SYSCALL_INTERRUPT_PRIORITY >> (8 - __NVIC_PRIO_BITS) )

    // это приоритет между минимальным и системным
    #define IRQ_PRIORITY_LEVEL_NORMAL       ( IRQ_PRIORITY_LEVEL_MAX_SYSCALL + (IRQ_PRIORITY_LEVEL_LOWEST - IRQ_PRIORITY_LEVEL_MAX_SYSCALL)/2 )

    // самый высокий приоритет, допустимый для прерываний, использующих системные вызовы
    #define IRQ_PRIORITY_LEVEL_HIGH         IRQ_PRIORITY_LEVEL_MAX_SYSCALL

#else
    // это приоритет между минимальным и максимальным
    #define IRQ_PRIORITY_LEVEL_NORMAL       ( IRQ_PRIORITY_LEVEL_LOWEST/2 )

    #define IRQ_PRIORITY_LEVEL_HIGH         ( IRQ_PRIORITY_LEVEL_LOWEST/4 )

#endif

/*
====================================================================================================
 Это более красивые имена для барьеров синхронизации
====================================================================================================
*/

// все команды, написанные после барьера, будут выполнены после барьера
#define DATA_SYNCHRONIZATION_BARRIER()    __DSB()

// все обращения к памяти, написанные до барьера будут выполнены ДО обращений, написанных после барьера
#define DATA_MEMORY_BARRIER()             __DMB()

// это сброс конвейера, все инструкции будут считаны из памяти еще раз
#define INSTRUCTION_SYNCHRONIZATION_BARRIER()    __ISB()

/*
====================================================================================================
 Это макрос для проверки, находимся мы в обработчике прерывания или нет
====================================================================================================
*/

#define IS_IN_ISR() (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)


/*
====================================================================================================
 Более читаемый макрос для определения, разрешены прерывания или нет
====================================================================================================
*/

#define ARE_INTERRUPTS_DISABLED()   __get_PRIMASK()


/*
====================================================================================================
 Это макрос для объявления критической секции, вариант для Кейла и gcc
====================================================================================================
*/

// его можно смешивать с taskENTER_CRITICAL, потому что они используют разные способы запрета прерываний
// создает свою область видимости!

// барьер нужен, чтобы процессор не менял порядок инструкций
#define ENTER_CRITICAL_SECTION()    { uint32_t wasMasked = ARE_INTERRUPTS_DISABLED();  DATA_SYNCHRONIZATION_BARRIER();  __disable_irq()


#define LEAVE_CRITICAL_SECTION()    if ( !wasMasked ) {__enable_irq();} }

#define EXIT_CRITICAL_SECTION()     LEAVE_CRITICAL_SECTION()


#if defined USE_FREERTOS

    // это псевдоним, который, вроде бы, лучше отражает суть
    // т.к. макросы FreeRTOS на самом деле запрещают только прерывания, у которых приоритет ниже 
    // configMAX_SYSCALL_INTERRUPT_PRIORITY ( это должны быть только прерывания от диспетчера и т.п.)
    
    // однако, если ваше прерывание имеет приоритет ниже - оно тоже будет запрещено!
    
    // помните, что меньший номер приоритета соответствует большей важности (0 - самое важное)
    
    // если диспетчер еще не запущен - совершенно логичным образом ничего не происходит
    
    #define ENTER_UNSCHEDULED_SECTION()   if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED ) { taskENTER_CRITICAL(); }
    #define LEAVE_UNSCHEDULED_SECTION()   if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED ) { taskEXIT_CRITICAL();  }
    
    #define EXIT_UNSCHEDULED_SECTION()    LEAVE_UNSCHEDULED_SECTION()

#endif
        
        
/*
====================================================================================================
 Это макросы для ассерта времени исполнения - бесконечный цикл, если условие ложно
 
 if(0) нужен, чтобы убрать предупреждение "statement unreachable"
====================================================================================================
*/

#define UMBA_ASSERT( statement )     do { if(! (statement) ) { __disable_irq(); while(1){ __BKPT(0xAB); if(0) break;} }  } while(0)

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
 Это макросы для тестирования
====================================================================================================
*/

#if defined USE_TESTS
    namespace umba { uint32_t decrementTaskCycleCounter(void); }
    #define OS_IS_RUNNING (::umba::decrementTaskCycleCounter())
#else
    #define OS_IS_RUNNING 1
#endif


/*
====================================================================================================
 Это прагмы для подавления ворнингов компиляторов, версии для Кейла и gcc.
 
 Пример использования:
    PRAGMA_SUPPRESS_UNUSED_PARAMETER_BEGIN
        .. код, генерирующий ворнинг про неиспользуемый параметр
    PRAGMA_END
 
====================================================================================================
*/

#if defined ( __CC_ARM )

    #define PRAGMA_END _Pragma("pop")

    #define PRAGMA_SUPPRESS_UNUSED_PARAMETER_BEGIN     _Pragma("push")

    #define PRAGMA_SUPPRESS_STATEMENT_UNREACHABLE_BEGIN _Pragma("push") _Pragma("diag_suppress 1300") _Pragma("diag_suppress 111")

    #define PRAGMA_SUPPRESS_POINTLESS_COMPARISON_BEGIN  _Pragma("push") _Pragma("diag_suppress 186")

    #define PRAGMA_SUPPRESS_NON_TEMPLATE_FRIEND_BEGIN  _Pragma("push") _Pragma("diag_suppress 1457")

    #define PRAGMA_SUPPRESS_ALL_WARNINGS_BEGIN _Pragma("push")\
                                             _Pragma("diag_suppress 111")\
                                             _Pragma("diag_suppress 1300")\
                                             _Pragma("diag_suppress 1293")\

#elif defined ( __GNUC__ )

    #define PRAGMA_END _Pragma("GCC diagnostic pop")

    #define PRAGMA_SUPPRESS_STATEMENT_UNREACHABLE_BEGIN  _Pragma("GCC diagnostic push")

    #define PRAGMA_SUPPRESS_UNUSED_PARAMETER_BEGIN     _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")

    #define PRAGMA_SUPPRESS_POINTLESS_COMPARISON_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")

    #define PRAGMA_SUPPRESS_NON_TEMPLATE_FRIEND_BEGIN  _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wnon-template-friend\"")

    #define PRAGMA_SUPPRESS_ALL_WARNINGS_BEGIN   _Pragma("GCC diagnostic push")\
                                               _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")\
                                               _Pragma("GCC diagnostic ignored \"-Weffc++\"")\
                                               _Pragma("GCC diagnostic ignored \"-Wreturn-type\"")\
                                               _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")\
                                               _Pragma("GCC diagnostic ignored \"-Wall\"")

#endif



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

#if defined __CC_ARM

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
                                                     \
    int toInt() const { return (int)t_; }            \
    void fromInt(int arg) { t_ = (Type)arg; }        \
                                                     \
private:                                             \
                                                     \
    template<typename T>                             \
    operator T () const;                             \
}
#else

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
