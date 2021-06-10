#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "bmplib.h"


/**
 * Функция устанавливает яркость изображения на определённый уровень от яркости оригинала.
 * Использует YCbCr
 *
 * bitmap - массив пикселей
 * rows   - кол-во строк
 * cols   - кол-во столбцов
 * lvl    - уровень яркости [0, 1]
 */
int decrease_brightness_ycbcr(PIXEL *bitmap, PIXEL **new, int rows, int cols, float lvl)
{
    if (rows <= 0 || cols <= 0) return -1;
    if (lvl < 0 || lvl > 1) return -1;

    *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            PIXEL *p = bitmap + col + row * cols;
            PIXEL *n = *new + col + row * cols;

            float r = (float) p->r / 255;
            float g = (float) p->g / 255;
            float b = (float) p->b / 255;

            float Kr = 0.299;
            float Kg = 0.587;
            float Kb = 0.114;
            float Y = (Kr * r) + (Kg * g) + (Kb * b);
            float Cb = (-0.5 * Kr / (1 - Kb) * r) + (-0.5 * Kg / (1 - Kb) * g) + (0.5 * b);
            float Cr = (0.5 * r) + (-0.5 * Kg / (1 - Kr) * g) + (-0.5 * Kb / (1 - Kr) * b);

            Y *= lvl;

            r = Y + (2 - 2 * Kr) * Cr;
            g = Y + (-Kb/Kg * (2 - 2 * Kb) * Cb) + (-Kr/Kg * (2 - 2 * Kr) * Cr);
            b = Y + (2 - 2 * Kb) * Cb;

            if (r < 0) { r = 0; }
            if (g < 0) { g = 0; }
            if (b < 0) { b = 0; }
            if (r > 1) { r = 1; }
            if (g > 1) { g = 1; }
            if (b > 1) { b = 1; }

            n->r = 255 * r;
            n->g = 255 * g;
            n->b = 255 * b;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int r, c;
    PIXEL *b, *nb;


    int g;
    extern int optind;
    extern char *optarg;

    //Возможные опции для программы
    int oFlag = 0;

    //Место для хранения названия файла
    char inputFileName[25];
    char outputFileName[25];


    char usage[] = "Usage: bmp_down_brightness_tool [-o output_File] [input_file]";


    while((g = getopt(argc, argv, " o:f")) != -1){
        switch(g){
        //Все возможные опции
        case 'o':
            oFlag = 1;
            strncpy(outputFileName, optarg, 25);
            break;
        default:
            break;
        }
    }

    if((oFlag ==0)){
        printf("Please, choose the file!\n%s \n", usage);
        exit(1);
    }


    //Чтение BMP файла
    if(optind == argc){
        //Входной файл не указан
        readFile(NULL, &r, &c, &b);
    }else{
        strncpy(inputFileName, argv[optind], 25);
        readFile(inputFileName, &r, &c, &b);
    }

    // Устанавливаем яркость в 0.5 от оригинала
    decrease_brightness_ycbcr(b, &nb, r, c, 0.5);
    printf("Good!\n");


    //Запись BMP файла
    if(oFlag == 0){
        //Выходной файл не указан
        writeFile(NULL, r, c, nb);
    }else if(oFlag == 1){
        writeFile(outputFileName, r, c, nb);
    }

    //Освобождение выделенной памяти
    free(b);
    free(nb);
  return 0;
}
