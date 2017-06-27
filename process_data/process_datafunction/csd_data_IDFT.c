#include "../../headers/process_data.h"
#include "../../headers/process.h"
#include "../../headers/globalVarINIT.h"
#include "../../headers/commonStructure.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
float SymbDuration=12.8;
void csd_data_IDFT(complex32 **csd_data, complex32 **trans_data, int N_SYM)
{
    int i;
    int16 temp;
    int Ntx = N_STS;
    int SymbLen = (int)(SymbDuration*SampRate);
    int DftSize = SampRate/(Band/4)*subcar;
    //double coeff = DftSize / sqrt(N_tone*Ntx);
    complex32 *X_freq = (complex32 *)malloc(sizeof(complex32)*subcar);
    complex32 *X_freq_pad = (complex32 *)malloc(sizeof(complex32)*DftSize);
    //complex32 *x_Field = (complex32 *)malloc(sizeof(complex32)*DftSize);
    MKSUREENMEM(X_freq);
    MKSUREENMEM(X_freq_pad);
    //MKSUREENMEM(x_Field);
    memset(X_freq,0,sizeof(complex32)*subcar);
    memset(X_freq_pad,0,sizeof(complex32)*DftSize);
    //memset(x_Field,0,sizeof(complex32)*DftSize);

    complex32 *symbol_data[N_STS];
    for(i=0; i<N_STS; i++)
    {
        symbol_data[i] = (complex32 *)malloc(sizeof(complex32)*SymbLen);
        MKSUREENMEM(symbol_data[i]);
        memset(symbol_data[i],0,sizeof(complex32)*SymbLen);
    }

    int symbol,tx;
    //complex32 *test0;
    //complex32 *test1;
    //complex32 *test2;
    //test0=*csd_data;
    for(symbol=0; symbol<N_SYM; symbol++)
    {
        for(tx=0; tx<Ntx; tx++)
        {
           // test1=csd_data[tx];
           // test2=&(csd_data[tx][subcar*symbol]);
            memcpy(X_freq,&(csd_data[tx][subcar*symbol]),sizeof(complex32)*subcar);
          //FILE* q=fopen("X_freq_C_2.txt", "w");
          //printStreamToFile(X_freq,256,q);
          //fclose(q);

            ///预处理
            switch(Band)
            {
                case 40:
                    for(i=64; i<subcar; i++){
                        temp = X_freq[i].real;
                        X_freq[i].real = -1*X_freq[i].imag;
                        X_freq[i].imag = temp;
                    }
                    break;
                case 80:
                    for(i=64; i<subcar; i++){
                        X_freq[i].imag = -1*X_freq[i].imag;
                        X_freq[i].real = -1*X_freq[i].real;
                    }
                    break;
                case 160:
                    for(i=64; i<256; i++){
                        X_freq[i].imag = -1*X_freq[i].imag;
                        X_freq[i].real = -1*X_freq[i].real;
                    }
                    for(i=320; i<subcar; i++){
                        X_freq[i].imag = -1*X_freq[i].imag;
                        X_freq[i].real = -1*X_freq[i].real;
                    }
                    break;
            }

            ///带宽与采样率不同时，IDFT大小与子载波数目不等，需将数据放在中间
            ///这里Band指ac的标准，ax除以4
            if(SampRate>(Band/4)){
                int n_pad =  DftSize - subcar;
                //for(i=0;i<n_pad/2;i++)
                //    X_freq_pad[i] = 0;
                for(i=n_pad/2;i<n_pad/2+subcar;i++)
                    X_freq_pad[i] = X_freq[i-n_pad/2];
                //for(i=n_pad/2+subcar;i<n_pad+subcar;i++)
                //    X_freq_pad[i] = 0;
            }
            ifftShiftandIFFTData(X_freq_pad,symbol_data[tx]);
            //FILE* e=fopen("X_freq_pad_C_2.txt", "w");
            //printStreamToFile(X_freq_pad,512,e);
            //fclose(e);
            //   FILE* w=fopen("symbol_data[1]_C.txt", "w");
            //printStreamToFile(symbol_data[0],512,w);
            //fclose(w);
        }

        //FILE *s=fopen("second_symbol_C.txt", "w");
       // for(i=0;i<N_TX;i++) printStreamToFile_float(symbol_data[i],512,s);
       //fclose(s);

        for(tx=0; tx<Ntx; tx++){
           addCPforData(symbol_data[tx],trans_data[tx],N_SYM,symbol);
        }
    //FILE *s=fopen("trans_data_first_symbol_C.txt", "w");
    //  for(i=0;i<N_TX;i++) printStreamToFile_float(trans_data[i],640,s);
    //   fclose(s);


    }
    free(X_freq);
    free(X_freq_pad);
    for(i=0;i<N_STS;i++) {
     free(symbol_data[i]);
     }   // FILE *t=fopen("trans_data.txt", "w");
   // for(i=0;i<N_TX;i++) printStreamToFile_float(trans_data[i],1280,t);
   // fclose(t);

}
