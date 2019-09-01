#pragma once

#include "project_config.h"
#include <stdint.h>
#include <limits>
#include <string.h>


namespace common_functions
{
    uint8_t xorshiftRandomByte(void);
  
    int64_t linearRescale(int32_t val, int32_t val_min, int32_t val_max, int32_t ret_min, int32_t ret_max );
    
    /**************************************************************************************************
    Описание:  Возвращает uint32 собранный из байтов
    Аргументы: first - самый младший байт
               ...
               fourth - самый старший байт
    Возврат:   собранный uint32
    Замечания: endianness на ваш выбор
    **************************************************************************************************/
    inline uint32_t bytesToUint32(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
    {
        uint32_t result = 0;

        result |= first  << 0;
        result |= second << 8;
        result |= third  << 16;
        result |= fourth  << 24;

        return result;
    }
    
    /**************************************************************************************************
    Описание:  Возвращает uint16 собранный из байтов
    Аргументы: first - самый младший байт
               second - самый старший байт
    Возврат:   собранный uint16
    Замечания: endianness на ваш выбор
    **************************************************************************************************/
    inline uint16_t bytesToUint16(uint8_t first, uint8_t second)
    {
        uint16_t result = 0;

        result |= first  << 0;
        result |= second << 8;
        
        return result;
    }

    /**************************************************************************************************
    Описание:  Возвращает float собранный из байтов
    Аргументы: first - самый младший байт
               ...
               fourth - самый старший байт
    Возврат:   собранный float
    Замечания: endianness на ваш выбор
    **************************************************************************************************/
    inline float bytesToFloat(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
    {
        uint32_t fourBytes = 0;
        float result;

        fourBytes |= first  << 0;
        fourBytes |= second << 8;
        fourBytes |= third  << 16;
        fourBytes |= fourth  << 24;

        memcpy(&result, &fourBytes, 4);

        return result;
    }
    
    /**************************************************************************************************
    Описание:  Разбирает uint32 на байты
    Аргументы: first - ссылка для хранения самого младшего байта
               ...
               fourth - ссылка для хранения самого старшего байта
    Возврат:   -
    Замечания: endianness на ваш выбор. volatile, чтобы приемники могли быть volatile
    **************************************************************************************************/
    inline void uint32ToBytes(uint32_t integer, volatile uint8_t & first, volatile uint8_t & second, 
                              volatile uint8_t & third, volatile uint8_t & fourth)
    {
        first =  (integer >> 0)  & 0xFF;
        second = (integer >> 8)  & 0xFF;
        third =  (integer >> 16) & 0xFF;
        fourth =  (integer >> 24) & 0xFF;
    }
    
    /**************************************************************************************************
    Описание:  Разбирает uint16 на байты
    Аргументы: first - ссылка для хранения самого младшего байта
               second - ссылка для хранения самого старшего байта
    Возврат:   -
    Замечания: endianness на ваш выбор. volatile, чтобы приемники могли быть volatile
    **************************************************************************************************/
    inline void uint16ToBytes(uint16_t integer, volatile uint8_t & first, volatile uint8_t & second)
    {
        first =  (integer >> 0)  & 0xFF;
        second = (integer >> 8)  & 0xFF;
    }

    /**************************************************************************************************
    Описание:  Разбирает float на байты
    Аргументы: first - ссылка для хранения самого младшего байта
               ...
               fourth - ссылка для хранения самого старшего байта
    Возврат:   -
    Замечания: endianness на ваш выбор. volatile, чтобы приемники могли быть volatile
    **************************************************************************************************/
    inline void floatToBytes(float floatValue, volatile uint8_t & first, volatile uint8_t & second,
                              volatile uint8_t & third, volatile uint8_t & fourth)
    {
        uint32_t fourBytes = 0;

        memcpy(&fourBytes, &floatValue, 4);

        first =  (fourBytes >> 0)  & 0xFF;
        second = (fourBytes >> 8)  & 0xFF;
        third =  (fourBytes >> 16) & 0xFF;
        fourth =  (fourBytes >> 24) & 0xFF;
    }

    /**************************************************************************************************
    Описание:  Возвращает знак числа
    Аргументы: value - значение
               min - минимум
               max - максимум
    Возврат:   -
    Замечания: Явное задание параметров шаблона не требуется: int a = 19; if( signum(a) )...
    **************************************************************************************************/
    template<typename T>
    int8_t signum(T val)
    {
        if (val > 0) return 1;
        if (val < 0) return -1;
        return 0;
    }
    
    /**************************************************************************************************
    Описание:  Функция ограничения значения сверху и снизу
    Аргументы: value - значение
               min - минимум
               max - максимум
    Возврат:   Ссылка на value, чтобы можно было делать цепочки и использовать clamp в вызовах других функций
    Замечания: Если min >= max - UMBA_ASSERT
               Явное задание параметров шаблона не требуется: int a = 19; clamp(a, 0, 10);
               Ассерт времени компиляции, если сочетание типов может привести к неверной смене знака.
    **************************************************************************************************/
    template <typename T, typename U, typename V>
    T & clamp(T & value, U min, V max)
    {
        // проверка на не очень удачное сочетание типов для min и max
        // защита от неявного приведения с неверной сменой знака
        // суть проверки: если один аргумент unsigned int или больше и беззнаковый, а другой меньше и знаковый
        // то меньший аргумент будет неявно преобразован к беззнаковому типу; 
        // при этом может произойти неверная смена знака
        
        #define CHECK_SIGN_CORRECTNESS( typeA, typeB )  UMBA_STATIC_ASSERT( !( std::numeric_limits<typeA>::digits >= std::numeric_limits<int>::digits &&   \
                                                                               std::numeric_limits<typeB>::digits < std::numeric_limits<typeA>::digits &&  \
                                                                               std::numeric_limits<typeA>::is_signed == false &&                           \
                                                                               std::numeric_limits<typeB>::is_signed == true ) )                        
        
        CHECK_SIGN_CORRECTNESS( U, V );
        CHECK_SIGN_CORRECTNESS( V, U );
        
        CHECK_SIGN_CORRECTNESS( T, V );
        CHECK_SIGN_CORRECTNESS( V, T );
        
        CHECK_SIGN_CORRECTNESS( T, U );
        CHECK_SIGN_CORRECTNESS( U, T );        
                               
        UMBA_ASSERT( min <= max );

        if( value > max )
            value = max;

        else if( value < min )
            value = min;
            
        return value;
        
        #undef CHECK_SIGN_CORRECTNESS
    }
    
   
    /**************************************************************************************************
    Описание:  Функция для округления целочисленных значений
    Аргументы: value - значение
               roundStep - шаг округления (должен быть положительным)
               assertOnOverflow - выбор поведения при переполнении - ассерт или утыкание в максимум/минимум
    Возврат:   ссылка на полученное значение для использования round как аргумента функции
    Замечания: Для float и double будет ошибка компиляции.
               Для неположительных шагов округления - UMBA_ASSERT.
               Если при округлении происходит переполнение - UMBA_ASSERT.
               Явное задание параметров шаблона не требуется: int a = 19; round(a, 5);               
    **************************************************************************************************/
    template <typename T, typename U>
    T & round(T & value, U roundStep, bool assertOnOverflow = true)
    {   
        UMBA_ASSERT(roundStep > 0);
        
        UMBA_STATIC_ASSERT( std::numeric_limits<T>::is_integer );
        UMBA_STATIC_ASSERT( std::numeric_limits<U>::is_integer );
        
        T quotient = value / roundStep;
        T remainder = value - quotient*roundStep;
        
        if(remainder < roundStep/2)
        {
            value = quotient*roundStep;
        }
        else
        {
            T temp = (quotient+1)*roundStep;
            
            // переполнение
            if( assertOnOverflow )
            {
                UMBA_ASSERT( temp > value );                
                value = temp;
            }
            else
            {
                if( temp < value )
                {
                    value = std::numeric_limits<T>::max();
                }
                else
                {
                    value = temp;
                }
            }
        }
        
        return value;
    }
    
    /**************************************************************************************************
    Описание:  Функция-затычка "ничего не делать"
    Аргументы: -
    Возврат:   -
    Замечания: -
    **************************************************************************************************/
    inline void doNothing(void)
    {}

}
