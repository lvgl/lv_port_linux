#include "schedule_data.h"
#include <time.h>

static lesson_t lessons[] = {
    {"Лабораторные занятия", "Искусственные интеллект и нейронные сети", "Сурина Альфия Адгамовна", 9, 45, 11, 20},
    {"Практические занятия и семинары", "Теория оптимизации", "Кунгурцева Алла Васильевна", 11, 30, 13, 05},
    {"Лекции", "Компьютерная графика", "Алексеева Елена Юрьевна", 13, 35, 15, 10},
    {"Лекции", "Основы компьютерного зрения", "Сурин Владимир Анатольевич", 15, 20, 16, 55},
    {"Лекции", "111111111121111111111211111111111111111111121Основы компьютерного зрения", "Сурин Владимир Анатольевич", 17, 05, 18, 40 }
};

static int lesson_count = sizeof(lessons) / sizeof(lessons[0]);

lesson_t get_lesson(int index)
{
    if (index >= 0 && index < lesson_count)
    {
        return lessons[index];
    }
    return lessons[0]; // Fallback to first lesson
}

int get_lesson_count(void)
{
    return lesson_count;
}

int get_lesson_count_for_date(struct tm* date)
{
    // Return lesson count for date (e.g., check database or array)
    return get_lesson_count(); // Fallback
}

lesson_t get_lesson_for_date(struct tm* date, int index)
{
    // Return lesson for date and index
    return get_lesson(index); // Fallback
}
