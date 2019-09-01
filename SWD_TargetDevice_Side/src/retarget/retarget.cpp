/***************************************************************************************************
                              Это файл, который затыкает собой щели.
 
 В нем лежит atexit, assert_failed, обвязка для printf'a и всякая веселуха
 
***************************************************************************************************/

#include "project_config.h"

/**************************************************************************************************
 Это мы так запихиваем по фиксированному адресу время компиляции и хеш коммита
 Строчка с define COMMIT_DATA генерируется скриптом update_commit_hash.bat, который нужно
 вызывать до билда. Чтобы от изменения этого файла не изменялась рабочая копия, после билда нужно
 вызывать скрипт remove_commit_hash.bat.
 
 Для этого в меню project->options->user нужно выбрать соответствующие скрипты (по относительному пути)
 в пунктах Before build/rebuild и After build/rebuild.
 
 Для использования этого скрипта нужен bash от гита. Чтобы помочь скрипту его найти, создайте
 переменную окружения GIT_BASH_PATH (95% что она будет равна "C:\Program Files (x86)\Git\usr\bin\" 
 
 Строчка текста с инфой будет лежать в памяти. Если вы хотите положить ее по какому-то определнному 
 адресу, создайте дефайн RETARGET_BUILD_INFO_ADDRESS, равный этому адресу.

 Эта функциональность включена по-умолчанию. 
 Для отключеная - создайте дефайн RETARGET_DISABLE_BUILD_INFO.
 
 Все дефайны можно класть в project_config.h.
 

**************************************************************************************************/

// эти дефайны можно выставлять в project_config, а не на весь проект
#ifndef UMBA_RETARGET_DISABLE_BUILD_INFO

    // типовые костыли для слияния макроса со строкой
    // нужны для clang-аттрибута
    #define RETARGET_BUILD_INFO_ADDRESS_CLANG_2( x )  ".ARM.__at_" #x
    #define RETARGET_BUILD_INFO_ADDRESS_CLANG_1( x )   RETARGET_BUILD_INFO_ADDRESS_CLANG_2( x )
    #define RETARGET_BUILD_INFO_ADDRESS_CLANG          RETARGET_BUILD_INFO_ADDRESS_CLANG_1( RETARGET_BUILD_INFO_ADDRESS )

    // опциональное размещение по фиксированному адресу
    #ifdef RETARGET_BUILD_INFO_ADDRESS
        
        // keil armcc
        #if defined ( __CC_ARM )
        
            #define RETARGET_BUILD_INFO_ADDRESS_ATTRIB __attribute__((at(RETARGET_BUILD_INFO_ADDRESS)))
        
        // keil-clang
        #elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)

            #define RETARGET_BUILD_INFO_ADDRESS_ATTRIB __attribute__((section(RETARGET_BUILD_INFO_ADDRESS_CLANG)))

        #endif
        
    // размещение где-нибудь
    #else
    
        // keil armcc
        #if defined ( __CC_ARM )
        
            #define RETARGET_BUILD_INFO_ADDRESS_ATTRIB
            
        // keil-clang
        #elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
            #error "Your compiler is not supported yet."
        
        // чтобы gcc был счастлив
        #else
            #define RETARGET_BUILD_INFO_ADDRESS_ATTRIB
        #endif
        
        
    #endif
        
    #define COMMIT_DATA "no data"

    const char date_time_commit[] __attribute__((used)) RETARGET_BUILD_INFO_ADDRESS_ATTRIB = 
    {
        "compilation time: " __DATE__ ", " __TIME__ " commit_data: " COMMIT_DATA 
    };

#endif        

extern "C" 
{

    /**************************************************************************************************
    Описание:  Эта функция вызывается в конструкторе каждого объекта со статическим временем жизни,
               чтобы "зарегистрировать" в динамически выделяемом списке. После выхода из main, вызовется
               _sys_exit, который для всех этих объектов вызывает деструкторы. Поскольку у нас main 
               никогда не завершается, все это абсолютно бессмысленно и только зря память жрет.
               ARM официально разрешает переопределить эту функцию.
    Аргументы: Какие-то есть, но игнорируются.
    Возврат:   Положительное число означает, что все хорошо.
    Замечания: Гуглите __aeabi_atexit и читайте официальную доку от ARM, если хотите узнать больше.
    **************************************************************************************************/
    int __aeabi_atexit(void)
    {
        return 1;
    }

    /**************************************************************************************************
    Описание:  Эта функция вызывается, если ассерт в периферийной библиотеке не смог.
    Аргументы: file - имя файла, в котором ассерт сработал, line - номер строки
    Возврат:   -
    Замечания: Вызывает обычный UMBA_ASSERT и повисает.
    **************************************************************************************************/

    // классический ассерт для STM32
    #ifdef USE_FULL_ASSERT
        void assert_failed(uint8_t * file, uint32_t line)
        { 
            /* User can add his own implementation to report the file name and line number,
             ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
             
            (void)file;
            (void)line;

            UMBA_ASSERT_FAIL();
        }
    #endif

    // варианты для миландра 
    #if (USE_ASSERT_INFO == 1)    
        void assert_failed(uint32_t file_id, uint32_t line)
        {
            (void)file_id;
            (void)line;
        
            UMBA_ASSERT_FAIL();
        }
    #elif (USE_ASSERT_INFO == 2)

        void assert_failed(uint32_t file_id, uint32_t line, const uint8_t * expr)
        {
            (void)file_id;
            (void)line;
            (void)expr;
        
            UMBA_ASSERT_FAIL();
        }
    #endif 

}

/***************************************************************************************************
  Следующий блок функций делает возможным использования printf в симуляторе keil'a.
  Чтобы выключить - объявите символ UMBA_DONT_USE_RETARGET.
  
  Данная реализация является минимальной и не будет работать вне симулятора.
  
***************************************************************************************************/
#ifndef UMBA_DONT_USE_RETARGET 

// если компилятор - armcc или keil-clang
#if __CC_ARM || ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

    #if ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

        asm(".global __use_no_semihosting_swi\n");
        
    #elif __CC_ARM
    
        #pragma import(__use_no_semihosting_swi)
        
        namespace std { struct __FILE { int handle;} ; }
    
    #endif
    
    #include <stdio.h>
    #include <rt_sys.h>
    #include <rt_misc.h>


    std::FILE std::__stdout;
    std::FILE std::__stdin;
    std::FILE std::__stderr;
    
    extern "C"
    { 
        int fputc(int c, FILE *f)
        {
            return ITM_SendChar(c);
        }

        int fgetc(FILE *f)
        {
            char ch = 0;

            return((int)ch);
        }

        int ferror(FILE *f)
        {
            /* Your implementation of ferror */
            return EOF;
        }

        void _ttywrch(int ch)
        {
            ITM_SendChar(ch);            
        }
        
        char *_sys_command_string(char *cmd, int len)
        {
            return NULL;
        }
        
        // вызывается после main
        void _sys_exit(int return_code) 
        {
            while(1);
        }        
    }
#endif

#endif
