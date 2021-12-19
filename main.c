#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <string.h>

int K,d,n;

int* Get_Init_Cent_Idx(int K, int n) {
    srand(time(NULL));
    int count=1,v=0;
    int* index=(int*)calloc(K,sizeof(int));
    while (count!=K) {
        v=0;
        int a=rand()%n;
        for (int i=0;i<count;i++) {
            if (index[i]==a) {
                v=1;
                break;
            }
        }
        if (!v) {
            index[count]=a;
            count++;
        } else continue;
    }
    return index;
}

float Get_Distance(float data1[], float data2[]) {
    float dist=0;
    int* arr=(int*)malloc(sizeof(int)*d);
    for (int i=0;i<d;i++) {
        arr[i]=data1[i]-data2[i];
        dist+=pow(arr[i],2);
    }
    free(arr);
    return sqrt(dist);
}

void Classify(float*** data, float*** Centers, int*** Clusters, int** count) {
    float* dist=(float*)malloc(sizeof(float)*K);
    for (int i=0;i<n;i++) {
        memset(dist,0,sizeof(float)*K);
        for (int j=0;j<K;j++) {
            dist[j]=Get_Distance((*data)[i],(*Centers)[j]);
        }
        float min=FLT_MAX;
        int idx=-1;
        for (int j=0;j<K;j++) {
            if (dist[j]<min) {
                min=dist[j];
                idx=j;
            }
        }
        (*Clusters)[idx][(*count)[idx]]=i; //! USE STACK?
        (*count)[idx]++;
    }
    free(dist);
}

void Get_Centers(int*** Clusters, float*** data, float*** Centers, int count[]) {
    for (int i=0;i<K;i++) {
        for (int j=0;j<d;j++) {
            float mean=0;
            for (int k=0;k<count[i];k++) {
                mean+=(*data)[(*Clusters)[i][k]][j];
            }
            mean/=count[i];
            (*Centers)[i][j]=mean;
        }
    }
}

void Write(int*** Clusters, int itr, int count[]) {
    FILE* f_out=fopen("output.txt","a");
    fprintf(f_out,"Itr %d\n",itr+1);
    for (int i=0;i<K;i++) {
        for (int j=0;j<count[i];j++) {
            fprintf(f_out,"%d ",(*Clusters)[i][j]+1);
        } fputc('\n',f_out);
    }
    fclose(f_out);
}

int Stop(float** Centers_before, float** Centers) {
    for (int i=0;i<K;i++) {
        for (int j=0;j<d;j++) {
            if (fabs(Centers[i][j]-Centers_before[i][j])>0.0001f) {
                return 0;
            }
        }
    }
    return 1;
}

void Reset() {
    FILE* f=fopen("output.txt","w");
    fclose(f);
}

int main() {
    FILE* f_in=fopen("input_1.txt","rt");
    fscanf(f_in,"%d %d %d",&K,&d,&n);
    Reset();

    float** data=(float**)malloc(sizeof(float*)*n);
    for (int i=0;i<n;i++) {
       data[i]=(float*)malloc(sizeof(float)*d);
       for (int j=0;j<d;j++) fscanf(f_in,"%f",&data[i][j]);
    }

    int* idx=Get_Init_Cent_Idx(K,n);
    float** Centers=(float**)malloc(sizeof(float*)*K);
    for (int i=0;i<K;i++) Centers[i]=(float*)malloc(sizeof(float)*d);
    for (int i=0;i<K;i++) memcpy(Centers[i],data[idx[i]],sizeof(float)*d);

    int** Clusters=(int**)malloc(sizeof(int*)*K);
    for (int i=0;i<K;i++) Clusters[i]=(int*)malloc(sizeof(int)*n);

    int* count=(int*)calloc(K,sizeof(int));
    int itr=0;    

    float** Centers_before=(float**)malloc(sizeof(float*)*K);
    for (int i=0;i<K;i++) Centers_before[i]=(float*)malloc(sizeof(float)*d);

    do {
        memset(count,0,sizeof(int)*K);
        for (int i=0;i<K;i++) memcpy(Centers_before[i],Centers[i],sizeof(float)*d);        
        Classify(&data,&Centers,&Clusters,&count);
        Write(&Clusters,itr,count);
        Get_Centers(&Clusters,&data,&Centers,count);
        itr++;
    } while (!Stop(Centers_before,Centers));
  
    fclose(f_in);
    for (int i=0;i<n;i++) free(data[i]);
    free(data);
    for (int i=0;i<K;i++) {
        free(Centers[i]);
        free(Centers_before[i]);
        free(Clusters[i]);
    }
    free(Centers);
    free(Centers_before);
    free(Clusters);
    free(idx);
    free(count);
}