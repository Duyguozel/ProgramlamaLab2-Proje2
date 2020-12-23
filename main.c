///LZ77 Algoritmasi https://ysar.net/algoritma/lz77.html sitesinden uyarlanmistir.
///HUFFMAN Kodlamasi https://gist.github.com/yasar11732/30d2fc9c1c404d776218424e5e3ca795 sitesinden uyarlanmistir.

#include <stddef.h> // NULL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define OFFSETBITS 5
#define LENGHTBITS (8-OFFSETBITS)

#define OFFSETMASK ((1<<(OFFSETBITS))-1)
#define LENGTHMASK ((1<<(LENGHTBITS))-1)

#define GETOFFSET(x) (x>> LENGHTBITS)
#define GETLENGTH(x) (x & LENGHTMASK)
#define OFFSETLENGTH(x,y) (x << LENGHTBITS | y)

#ifdef _MSC_VER

__pragma(pack(push, 1))
struct huffmancode
{
    uint16_t code;
    uint8_t len;
};
__pragma(pack(pop))
#elif defined(__GNUC__)
struct __attribute__((packed)) huffmancode
{
    uint16_t code;
    uint8_t len;
};
#endif
FILE *deflateCikti,*cikti;
float deflatePerformance;
float lz77Performance;
// her bir code için uzunluk ve kod değerlerini
// tuttuğumuz array
struct huffmancode tree[0X100];

// her uzunlukta kaç adet kod
// olduğunu saymak için
uint8_t bl_count[0x10];

// her uzunluktaki kodlama için
// atanacak kodu tutar
uint16_t next_code[0x10];


typedef struct _huffman
{
    char c;
    int freq;
    struct _huffman *left;
    struct _huffman *right;
} HUFFMANTREE;

typedef struct _huffman_array
{
    int cap;
    int size;
    HUFFMANTREE **items;
} HUFFMANARRAY;


struct token
{
    uint8_t benzeyenSayisi;
    char c;
};
int benzeyenSayisibul(char *s1, char *s2, int limit)
{
    int len=0;
    while(*s1++ == *s2++ && len<limit)
    {
        len++;
    }
    return len;
}
void lz77memcpy(char *s1,char *s2,int size)
{
    while(size--)
        *s1++=*s2++;
}
struct token* lz77(char *text,int limit,int *tokenSayisi)
{

    cikti=fopen("LZ77cikti.txt","w");
    struct token t;
    char *aramaTamponu, *ileriTampon;
    int cap=1<<3;
    int tSayisi=0,max_len,len,a;
    struct token *encoded=malloc(cap*sizeof(struct token));
    char buf[limit];
    int i=0,n=0;



    printf("TOKEN LISTESI: \n");
    fprintf(cikti,"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nTOKEN LISTESI: \n");
    for(ileriTampon=text; ileriTampon<text+limit ; ileriTampon++)
    {
        aramaTamponu=ileriTampon-OFFSETMASK;
        if(aramaTamponu<text)
        {
            aramaTamponu=text;
        }
        max_len=0;

        char *max_cumle=ileriTampon;

        for(; aramaTamponu<ileriTampon; aramaTamponu++)
        {
            len=benzeyenSayisibul(aramaTamponu,ileriTampon,LENGTHMASK);
            if(len>max_len)
            {
                max_len=len;
                max_cumle=aramaTamponu;
            }
        }
        if(ileriTampon+max_len>= text+limit)
        {
            max_len=text+limit-ileriTampon-1;
        }
        t.benzeyenSayisi=OFFSETLENGTH(ileriTampon-max_cumle,max_len);
        a=benzeyenSayisibul(ileriTampon,aramaTamponu,max_len);
        ileriTampon+=max_len;
        t.c=*ileriTampon;
        printf("<%d %d %c>\n",t.benzeyenSayisi/8,a,t.c);
        fprintf(cikti,"<%d %d %c>\n",t.benzeyenSayisi/8,a,t.c);
        buf[i]=t.c;
        i++;
        n++;
        if(tSayisi+1>cap)
        {
            cap=cap<<1;
            encoded=realloc(encoded,cap*sizeof(struct token));
        }
        encoded[tSayisi++]=t;
    }
    if(tokenSayisi)
        *tokenSayisi=tSayisi;

    fprintf(cikti,"\n");
    fprintf(cikti,"METNIN ONCEKI BOYUTU : %d",limit);
    fprintf(cikti,"\n\n");
    fprintf(cikti,"SIKISTIRILMIS METNIN BOYUTU : %d",(tSayisi*sizeof(struct token))/2);
    printf("SIKISTIRILMIS METNIN BOYUTU : %d",(tSayisi*sizeof(struct token))/2);
    fprintf(cikti,"\n\n");
    fprintf(cikti,"METNIN ENCODED BOYUTU : %d",tSayisi*sizeof(struct token));
    fprintf(cikti,"\n\n");
    fprintf(cikti,"METNIN SIKISTIRILMIS HALI=>");
    printf("\nMETNIN LZ77 ILE SIKISTIRILMIS HALI =>");
    for(int k=0; k<n; k++)
    {
        fprintf(cikti,"%c",buf[k]);
        printf("%c",buf[k]);
    }
    printf("\n\n");
    fprintf(cikti,"\n");

    lz77Performance = (float)limit / (float) ((tSayisi*sizeof(struct token))/2);
    float lz77yuzde=(float)100/lz77Performance;
    printf("LZ77 Algoritmasi ile sikistirilmis metin ,orjinal metnin %%%.2f kadaridir.\n",lz77yuzde);
    fprintf(cikti,"LZ77 Algoritmasi ile sikistirilmis metin , orjinal metnin %%%.2f kadaridir.\n",lz77yuzde);
    fclose(cikti);
    return encoded;
}

HUFFMANTREE *huffmantree_new(char c, int freq)
{
    HUFFMANTREE *t = malloc(sizeof(HUFFMANTREE));
    t->c = c;
    t->freq = freq;
    t->left = NULL;
    t->right = NULL;
    return t;
}

/*
* Selection sort, büyükten küçüðe
*/

void huffman_array_sort(HUFFMANARRAY *arr)
{
    int i, k;
    int max_index, max_value;
    for (i = 0; i < arr->size - 1; i++)
    {
        max_index = i;
        max_value = arr->items[i]->freq;

        for (k = i + 1; k < arr->size; k++)
        {
            if (arr->items[k]->freq > max_value)
            {
                max_value = arr->items[k]->freq;
                max_index = k;
            }
        }
        if (i != max_index)
        {
            HUFFMANTREE *_tmp = arr->items[i];
            arr->items[i] = arr->items[max_index];
            arr->items[max_index] = _tmp;
        }
    }
}

HUFFMANTREE *huffman_array_pop(HUFFMANARRAY *arr)
{
    return arr->items[--arr->size];
}
void *huffman_array_add(HUFFMANARRAY *arr, HUFFMANTREE *t)
{
    if (arr->size + 1 == arr->cap)
    {
        arr->cap *= 2;
        arr->items = realloc(arr->items, arr->cap * sizeof(HUFFMANTREE *));
    }
    arr->items[arr->size++] = t;
}

HUFFMANARRAY *huffman_array_new()
{
    HUFFMANARRAY *arr = malloc(sizeof(HUFFMANARRAY));
    arr->cap = 8;
    arr->size = 0;
    arr->items = malloc(arr->cap * sizeof(HUFFMANTREE *));
    return arr;
}

void huffmantree_yazdir(HUFFMANTREE *t, char *prefix, int size_prefix)
{
    if (t->left == NULL && t->right == NULL)
    {
        prefix[size_prefix] = 0;
        printf("%c: %s\n", t->c, prefix);
        return;
    }
    if (t->left)
    {
        prefix[size_prefix++] = '0';
        huffmantree_yazdir(t->left, prefix, size_prefix);
        size_prefix--;
    }
    if (t->right)
    {
        prefix[size_prefix++] = '1';
        huffmantree_yazdir(t->right, prefix, size_prefix);
        size_prefix--;
    }
}

void load_canonical_codes_from_tree(HUFFMANTREE *t, int uzunluk)
{
    if (!t)
        return;
    if (t->c != 0)
    {
        tree[t->c].len = uzunluk;
    }

    load_canonical_codes_from_tree(t->left, uzunluk + 1);
    load_canonical_codes_from_tree(t->right, uzunluk + 1);
}

char *code_to_binary(struct huffmancode h)
{
    char *b = malloc(h.len + 1); // +1 null
    int i;
    for (i = 0; i < h.len; i++)
    {
        b[i] = h.code & (1 << (h.len - i - 1)) ? '1' : '0';
    }
    b[h.len] = 0;
    return b;
}

void huffman(char metin[500])
{
    int n=strlen(metin);
    FILE *f=fopen("metin.txt","r");
    char c[200];
    int toplamBoyut=0;
    while(fgets(c,500,f)!=NULL)
    {
        toplamBoyut+=strlen(c);
    }
    //deflateCikti=fopen("deflateCikti.txt","w");
    unsigned long frequencies[0xFF];
    char *pcTemp;
    int i;
    unsigned long tekrar[0xFF];
    HUFFMANARRAY *arr = huffman_array_new();
    HUFFMANARRAY *arr2 = huffman_array_new();
    memset(&frequencies[0], 0, sizeof(frequencies));
    memset(&tekrar[0], 0, sizeof(tekrar));

    for (pcTemp = &metin[0]; *pcTemp != 0; pcTemp++)
    {
        frequencies[(int)*pcTemp]++;
    }
    for (i = 0; i < 255; i++)
    {
        if (frequencies[i] > 0)
        {
            huffman_array_add(arr, huffmantree_new(i, frequencies[i]));

        }
    }
    while (arr->size > 1)
    {
        huffman_array_sort(arr);
        HUFFMANTREE *t1 = huffman_array_pop(arr);
        HUFFMANTREE *t2 = huffman_array_pop(arr);
        HUFFMANTREE *t3 = calloc(1, sizeof(HUFFMANTREE));
        t3->left = t1;
        t3->right = t2;
        t3->freq = t1->freq + t2->freq;
        huffman_array_add(arr, t3);
    }
    memset(tree, 0, sizeof(tree));
    memset(bl_count, 0, sizeof(bl_count));
    memset(next_code, 0, sizeof(next_code));

    load_canonical_codes_from_tree(huffman_array_pop(arr), 0);

    for (i = 0; i < 256; i++)
    {
        bl_count[tree[i].len]++;
    }
    int code = 0;
    bl_count[0] = 0;

    for (i = 1; i < 0x10; i++)
    {
        code = (code + bl_count[i - 1]) << 1;
        next_code[i] = code;
    }

    for (i = 0; i < 0x100; i++)
    {
        int len = tree[i].len;
        if (len)
        {
            tree[i].code = next_code[len];
            next_code[len]++;
        }
    }

    int len;

    for (i = 0; i < 0x100; i++)
    {
        len = tree[i].len;
        if (len)
        {
            printf("%c: %s\n", i, code_to_binary(tree[i]));
            fprintf(deflateCikti,"%c: %s\n", i, code_to_binary(tree[i]));
        }
    }

    printf("%s => ",metin);
    fprintf(deflateCikti,"%s => ",metin);
    for(int j=0 ; j<n ; j++)
    {
        for(int k=0 ; k<0x100 ; k++)
        {
            len=tree[i].len;
            if(metin[j]==k)
            {
                printf("%s",code_to_binary(tree[k]));
                fprintf(deflateCikti,"%s",code_to_binary(tree[k]));
            }
        }
    }
    printf("\n\n");
    fprintf(deflateCikti,"\n\n");
    int toplam2=toplamBoyut*8;
    printf("ORJINAL METNIN TOPLAM BIT SAYISI=%d\n\n",toplam2);
    fprintf(deflateCikti,"\nORJINAL METNIN TOPLAM BIT SAYISI=%d\n\n",toplam2);
    int toplam=0;
    for (i = 0; i < 0x100; i++)
    {
        int len = tree[i].len;
        if (len)
        {
            printf("%c : %d*%d=>%d\n",i,frequencies[i],len,len*frequencies[i]);
            toplam+=len*frequencies[i];
        }
    }
    printf("\nSIKISMIS METNIN TOPLAM BIT SAYISI=%d\n\n",toplam);
    fprintf(deflateCikti,"SIKISMIS METNIN TOPLAM BIT SAYISI=%d\n\n",toplam);

    deflatePerformance = (float)toplam2 / (float)toplam;
    float defalteYuzde=(float)100/deflatePerformance;
    printf("DEFLATE Algoritmasi ile sikistirilmis metin ,orjinal metnin %%%.2f kadaridir.\n",defalteYuzde);
    fprintf(deflateCikti,"DEFLATE Algoritmasi ile sikistirilmis metin , orjinal metnin %%%.2f kadaridir.\n",defalteYuzde);

    fclose(f);

}

void deflate(char *text,int limit, int *tokenSayisi)
{
    deflateCikti=fopen("deflateCikti.txt","w");
    struct token t;
    char *aramaTamponu, *ileriTampon;
    int cap=1<<3;
    int tSayisi=0,max_len,len,a;
    struct token *encoded=malloc(cap*sizeof(struct token));
    char buf[500];
    int i=0;

    printf("Huffman Agacina Alinacak Cumle: ");
    fprintf(deflateCikti,"Huffman Agacina Alinan Cumle: ");
    for(ileriTampon=text; ileriTampon<text+limit ; ileriTampon++)
    {
        aramaTamponu=ileriTampon-OFFSETMASK;
        if(aramaTamponu<text)
        {
            aramaTamponu=text;
        }
        max_len=0;

        char *max_cumle=ileriTampon;

        for(; aramaTamponu<ileriTampon; aramaTamponu++)
        {
            len=benzeyenSayisibul(aramaTamponu,ileriTampon,LENGTHMASK);
            if(len>max_len)
            {
                max_len=len;
                max_cumle=aramaTamponu;
            }
        }
        if(ileriTampon+max_len>= text+limit)
        {
            max_len=text+limit-ileriTampon-1;
        }
        t.benzeyenSayisi=OFFSETLENGTH(ileriTampon-max_cumle,max_len);
        a=benzeyenSayisibul(ileriTampon,aramaTamponu,max_len);
        ileriTampon+=max_len;
        t.c=*ileriTampon;
        printf("%c",t.c);
        fprintf(deflateCikti,"%c",t.c);
        //printf("%d %d %c\n",t.benzeyenSayisi/8,a,t.c);
        buf[i]=t.c;
        i++;
        if(tSayisi+1>cap)
        {
            cap=cap<<1;
            encoded=realloc(encoded,cap*sizeof(struct token));
        }


        encoded[tSayisi++]=t;

        if(tokenSayisi)
            *tokenSayisi=tSayisi;
    }
    //printf("%s\n",buf);
    printf("\n");
    fprintf(deflateCikti,"\n");
    fprintf(deflateCikti,"\n");
    fprintf(deflateCikti,"METNIN DEFLATE SIKISTIRILMIS HALI : ");
    fprintf(deflateCikti,"\n");
    huffman(buf);

    fclose(deflateCikti);
}
char *file_read(FILE *f, int *size)
{
    char *content;
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    content = malloc(*size);
    fseek(f, 0, SEEK_SET);
    fread(content, 1, *size, f);
    return content;
}
int main()
{
    FILE *dosya=fopen("metin.txt","rb");
    FILE *dosya2 = fopen("metin.txt","rb");
    cikti=fopen("LZ77cikti.txt","w");
    char *cumle;
    char cumle2[500];
    int *boyut;
    int *tokenSayisi,i=0;
    struct token *encodedMetin;
    if(dosya=fopen("metin.txt","rb"))
    {
        cumle=file_read(dosya,&boyut);
        fclose(dosya);
    }
    printf("DOSYADAN ALINAN CUMLE: \n");
    fprintf(cikti,"CUMLE:");

    while(fgets(cumle2,500,dosya2)!=NULL)
    {
        printf("%s",cumle2);
        fprintf(cikti,"%s",cumle2);
    }
    printf("\n");


    printf("Metnin  ilk boyutu => %d\n\n",boyut);
    printf("\n\t\t------ LZ77 ------\n\n");
    encodedMetin=lz77(cumle,boyut,&tokenSayisi);
    printf("\n\n\t\t------ DEFLATE ------\n\n");
    deflate(cumle, boyut,&tokenSayisi);

    printf("\n");
    printf("\n--------------------------\n\n");

    if(deflatePerformance>lz77Performance)
    {
        printf("DEFLATE ALGORITMASININ PERFORMANSI DAHA YUKSEKTIR.\n\n");
    }
    else if(lz77Performance>deflatePerformance)
    {
        printf("LZ77 ALGORITMASININ PERFORMANSI DAHA YUKSEKTIR.\n\n");
    }
    printf("--------------------------\n\n");
    fclose(dosya);

    fclose(dosya2);


    return 0;
}

