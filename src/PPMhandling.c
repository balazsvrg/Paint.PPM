#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "../include/updates.h"
#include "../include/PPMhandling.h"

bool IsPPM(FILE *imgfile){ /*jelez, ha rossz formátumú képet próbálnánk megnyitni (igen, tényleg Magic Number a szakszerű megfogalmazása, ez lehet csak nekem új)*/
    char magicNumber;
    return (fscanf(imgfile, "P%c\n", &magicNumber) == 1 && magicNumber == '6');
}

Info PPM_GetInfo(FILE *imgfile){ //visszatér a PPM méreteivel, és a maximális színértékkel
    Info imageInfo;
    fscanf(imgfile, "%d%d%d\n", &imageInfo.width, &imageInfo.height, &imageInfo.maxColorVal);
    imageInfo.size = imageInfo.width * imageInfo.height;
    return imageInfo;
}

void PPM_LoadImageToArray(FILE *imgfile, Info imgInfo, Pixel *img){ /*feltölti a kép pixeleinek értékével a kapott tömböt*/
    for (int i = 0; i < imgInfo.width; ++i){
        for (int j = 0; j < imgInfo.height; ++j){
            int index = i * imgInfo.height + j; /*egydimenziós tömbben van tárolva azért ilyen az alakja*/
            img[index].r = fgetc(imgfile);
            img[index].g = fgetc(imgfile);
            img[index].b = fgetc(imgfile);
        }
    }
}

void PPM_NextStep(Info imgInfo, int currStep, Pixel **undoBuffer){ /*következő képfeldolgozó művelet inicializálása*/
    undoBuffer[currStep] = (Pixel *) malloc(sizeof(Pixel) * imgInfo.width * imgInfo.height);
    if (undoBuffer[currStep] == NULL){
        pushmsg("couldn't allocate memory");
        exit(1);
    }
    for (int i = 0; i < imgInfo.width; ++i) /*átmásolja az előző állapotot az új buffer-elembe*/
        for (int j = 0; j < imgInfo.height; ++j){
            int index = i * imgInfo.height + j; 
            undoBuffer[currStep][index] = undoBuffer[currStep-1][index];
        }
}

void PPM_Darken(Pixel *img, int amount, Info imgInfo){ /* minden RGB-értéket csökkent amount mennyiséggel*/
    for (int i = 0; i < imgInfo.height; i++){
        for (int j = 0; j < imgInfo.width; ++j){
            int index = i * imgInfo.width + j;
            img[index].r = img[index].r >= amount ? img[index].r - amount : 0; /* ha minuszba menne az érték akkor lenullázza*/
            img[index].g = img[index].g >= amount ? img[index].g - amount : 0;
            img[index].b = img[index].b >= amount ? img[index].b - amount : 0;
        }
    }
}

void PPM_Lighten(Pixel *img, int amount, Info imgInfo){ /* minden RGB-értéket növel amount mennyiséggel*/
    for (int i = 0; i < imgInfo.height; i++){
        for (int j = 0; j < imgInfo.width; ++j){
            int index = i * imgInfo.width + j;
            img[index].r = img[index].r <= 255 - amount ? img[index].r + amount : 255; /*ha nem engedi hogy 255 fölé menjen az érték*/
            img[index].g = img[index].g <= 255 - amount ? img[index].g + amount : 255;
            img[index].b = img[index].b <= 255 - amount ? img[index].b + amount : 255;
        }
    }
}

void PPM_Invert(Pixel *img, Info imgInfo){ /*negálja az RGB értékeket*/
    for (int i = 0; i < imgInfo.height; i++){
        for (int j = 0; j < imgInfo.width; ++j){
            int index = i * imgInfo.width + j;
            img[index].r = 255 - img[index].r;
            img[index].g = 255 - img[index].g;
            img[index].b = 255 - img[index].b;
        }
    }
}

void PPM_Greyscale(Pixel *img, Info imgInfo){ /*egyenlővé téve az R G éb B értékeket, a kép fekete-fehérnek tűnik*/
    for (int i = 0; i < imgInfo.height; i++){
        for (int j = 0; j < imgInfo.width; ++j){
            int index = i * imgInfo.width + j;
            int greyValue = (img[index].r + img[index].g + img[index].b) / 3; /* valamiért szerintem jobb a sima átlagolás mint a  0.2989, 0.5870, 0.1140 súlyozás szóval maradt ez*/
            img[index].r = img[index].g = img[index].b = greyValue;
        }
    }
}

/* Ha abszolút minimumot és maximumot vennénk a pixelértékekből akkor ha*/
/* a képen lenne egyetlen feket és fehér pixel, az algoritmus azt hinné */
/* hogy a kép telhesen kihasználja a 0-255ös színspektrumot. Ezt kiküszöbölendő*/
/* létrehozunk egy histogram tömböt amiben eltároljuk hogy hány pixel veszi fel*/
/* a képen az adott értéket, és a likógó értékeket így ignorálni tudjuk*/
void FillHistogram(Histogram *tofill, Pixel *img, Info imgInfo){ 
                                                                 
    for (int i = 0; i < 255; i++){ /*nullára inicializáljuk a histogram változót... just in case*/                              
        tofill->histR[i] = 0;                                    
        tofill->histG[i] = 0;                                    
        tofill->histB[i] = 0;
    }

   for (int i = 0; i < imgInfo.size; i++){ /* img[i].<RGB> mindig a histogram egyik indexének felel meg, és ezt növeljük ha találunk egy pixelt ezzel az értékkel*/
        tofill->histR[img[i].r] += 1;
        tofill->histG[img[i].g] += 1;
        tofill->histB[img[i].b] += 1;
    }
}

/*A kilógó értékek ignorálásához "deriválunk". két pixelérték között a Histogram*/
/*meredeksége magától értetődően a két érték különbsége. Ha ez egy bizonyos értéknél*/
/*nagyobb, akkor azt jogosan nézhetjük annak, hogy itt kezdődik a releváns adathalmaz,*/
/*ugyanis kilógó értékekből nem jellemző hogy sok található*/
int FindRelMin(Histogram histogram, Info imgInfo, double relval){                                           
    int min = 0;                                                                                            
    while ((((histogram.histR[min +1] + histogram.histG[min +1] + histogram.histB[min +1])/3) -             
        (histogram.histR[min] + histogram.histG[min] + histogram.histB[min])/3)  < imgInfo.size * relval)   
        min++;
    return min;
}

int FindRelMax(Histogram histogram, Info imgInfo, double relval){
    int max = 255;
    while ((((histogram.histR[max -1] + histogram.histG[max -1] + histogram.histB[max -1])/3) -
        (histogram.histR[max] + histogram.histG[max] + histogram.histB[max])/3)  < (imgInfo.width * imgInfo.height) * relval)   
        max--;
    return max;
}

void PPM_Contrast(Pixel *img, Info imgInfo){ 
    int min;                /*ezt állitjuk majd relativ minimum értékre*/
    int max;                /*ezt meg relatív maximumra*/
    double relval = 0.0001; /*egy pár kisérlet után úgy láttam hogy ez az érték hozza a legjobb eredményeket*/
    Histogram histogram;

    FillHistogram(&histogram, img, imgInfo);
    min = FindRelMin(histogram, imgInfo, relval);
    max = FindRelMax(histogram, imgInfo, relval);

    if (max - min != 0){ /*ne osszunk nullával, nem kellemes élmény*/
        for (int i = 0; i < imgInfo.size; i++){
            int redVal =  255 * (img[i].r - min) / (max - min); /*ez a legelterjedtebb kontraszt képlet amit láttam*/
            int bluVal = 255 * (img[i].g - min) / (max - min);  /*A matlab fórumokon többféle van mint hely egy schönherzes lifben*/
            int greVal = 255 * (img[i].b - min) / (max - min);

            if (redVal < 0)        img[i].r = 0;
            else if (redVal > 255) img[i].r = 255;
            else                   img[i].r = redVal;

            if (bluVal < 0)        img[i].g = 0;
            else if (bluVal > 255) img[i].g = 255;
            else                   img[i].g = bluVal;

            if (greVal < 0)        img[i].b = 0;
            else if (greVal > 255) img[i].b = 255;
            else                   img[i].b = greVal;
        }
    }
}

/*azért, hogy a már homályosított értékű pixelek ne hassanak ki azokra aminek éppen az értékét
 *számoljuk, készítünk egy másolatot a kéről és ezen végezzük a vltoztatásokat*/
void CopyToTemp(Pixel **temp, Info imgInfo, Pixel *from){   
    for (int i = 0; i < imgInfo.height; i++){
        for (int j = 0; j < imgInfo.width; j++){
            temp[i][j] = from[imgInfo.height * i + j];
        }
    }
}


/*egy képet úgy homályosítunk, hogy átlagoljuk a pixeleit körülvevő terület RGB értékeit
 *Ez a funkció jelenleg hibásan működik! Próbálkoztam Gauss-kerneles súlyozással, de kivettem
 *mert egyenlőre egy debughoz átláthatóbb ha nincs ott. Sajnos nem tudtam megállapítani mi a pontos hiba
 *néhány képen teljesen jó, valamelyiken meg értékelhetetlenül működik*/
Pixel AvgConvMtx(int step, Pixel **tmp, int x, int y, Info imageInfo){

    Pixel ToReturn;

    int rval = 0;
    int gval = 0;
    int bval = 0;

    for (int i = -step; i <= step; i++){
        for (int j = -step; j <= step; j++){                /*összeadom egy a pixel körli step sugarú négyzet pixel értékeit*/
            if (y + i >= 0 && y + i < imageInfo.height){    /* ne lógjon ki semelyik oldalon se*/
                if (x + j >= 0 && x + j < imageInfo.width){
                    rval += (tmp[y + i][x + j].r);
                    gval += (tmp[y + i][x + j].g);
                    bval += (tmp[y + i][x + j].b);
                }
            }
        }
    }

    ToReturn.r = rval / ((step * 2 + 1) * (step * 2 + 1));   /*elosztom a pixelérték-összegeket a terület*/
    ToReturn.g = gval / ((step * 2 + 1) * (step * 2 + 1));   /*összes pixelénk számával*/
    ToReturn.b = bval / ((step * 2 + 1) * (step * 2 + 1));

    return ToReturn;
}

void PPM_Blur(int size, Pixel *img, Info imgInfo){
    Pixel **temp = (Pixel **) malloc(sizeof(Pixel *) * imgInfo.height); /*az átmeneti kép, a könnyebb navigáció érdekében kétdimenziós tömbben van tárolva*/
    if (temp == NULL){
        pushmsg("couldn't allocate memory");
        exit(1);
    }
    
    for (int i = 0; i < imgInfo.height; i++){
        temp[i] = malloc(sizeof(Pixel) * imgInfo.width);
        if (temp[i] == NULL){
            pushmsg("couldn't allocate memory");
            exit(1);
        }
    }

    CopyToTemp(temp, imgInfo, img);
    for (int y = 0; y < imgInfo.height; y++){ /*egyenként változtatom a kép pixeleit a homályosított értékre*/
        for (int x = 0; x < imgInfo.width; x++){
            img[y * imgInfo.height + x] = AvgConvMtx(size, temp, x, y, imgInfo);
        }
    }

    for (int i = 0; i < imgInfo.height; i++)
        free(temp[i]);

    free(temp);
}


