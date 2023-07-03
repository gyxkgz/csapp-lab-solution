/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i,j, k,l,t0,t1,t2,t3,t4,t5,t6,t7;

    for(l=0;l<N/8;l++){
        for(k=0;k<M/8;k++)
        {
            for (i = 0; i < 4; i++) {                 
                t0= A[l*8+i][k*8];
                t1= A[l*8+i][k*8+1];
                t2= A[l*8+i][k*8+2];
                t3= A[l*8+i][k*8+3];
                t4= A[l*8+i][k*8+4];
                t5= A[l*8+i][k*8+5];
                t6= A[l*8+i][k*8+6];
                t7= A[l*8+i][k*8+7];
                B[k*8][l*8+i] = t0;
                B[k*8+1][l*8+i]=t1;
                B[k*8+2][l*8+i]=t2;
                B[k*8+3][l*8+i]=t3;
                B[k*8][l*8+i+4] = t4;
                B[k*8+1][l*8+i+4]=t5;
                B[k*8+2][l*8+i+4]=t6;
                B[k*8+3][l*8+i+4]=t7;
            }
            for (i = 0; i < 4; i++) {   
                t0= B[k*8+i][l*8+4];
                t1= B[k*8+i][l*8+5];
                t2= B[k*8+i][l*8+6];
                t3= B[k*8+i][l*8+7];
                t4= A[l*8+4][k*8+i];
                t5= A[l*8+5][k*8+i];
                t6= A[l*8+6][k*8+i];
                t7= A[l*8+7][k*8+i];
                B[k*8+i][l*8+4] = t4;
                B[k*8+i][l*8+5]=t5;
                B[k*8+i][l*8+6]=t6;
                B[k*8+i][l*8+7]=t7;
                t4= A[l*8+4][k*8+i+4];
                t5= A[l*8+5][k*8+i+4];
                t6= A[l*8+6][k*8+i+4];
                t7= A[l*8+7][k*8+i+4];

                B[k*8+i+4][l*8] =t0;
                B[k*8+i+4][l*8+1]=t1;
                B[k*8+i+4][l*8+2]=t2;
                B[k*8+i+4][l*8+3]=t3;  
                B[k*8+i+4][l*8+4]=t4;
                B[k*8+i+4][l*8+5]=t5;
                B[k*8+i+4][l*8+6]=t6;
                B[k*8+i+4][l*8+7]=t7;  
            }
        } 
        for(j=k*8;j<M;j++)
        {
            t0 = A[l*8][j];
            t1 = A[l*8+1][j];
            t2 = A[l*8+2][j];
            t3 = A[l*8+3][j];
            t4 = A[l*8+4][j];
            t5 = A[l*8+5][j];
            t6 = A[l*8+6][j];
            t7 = A[l*8+7][j];
   
            B[j][l*8] = t0;
            B[j][l*8+1] = t1;
            B[j][l*8+2] = t2;
            B[j][l*8+3] = t3;
            B[j][l*8+4] = t4;
            B[j][l*8+5] = t5;
            B[j][l*8+6] = t6;
            B[j][l*8+7] = t7;
        }      
    }
    if(l*8<N)
    {
        for(j=0;j<M;j++)
        {
            for(i=l*8;i<N;i++)
            {
                t0 = A[i][j];
                B[j][i] = t0;
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

char trans1_desc[] = "trans1";
void trans1(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k,l,t0,t1,t2,t3,t4,t5,t6,t7;
    int lines_per_loop=32/sizeof(N),colum_per_loop=32/sizeof(M);
    int line_loop_cnt=N/lines_per_loop,colum_loop_cnt=M/colum_per_loop;

    for(l=0;l<line_loop_cnt;l++)
        for(k=0;k<colum_loop_cnt;k++)
        {
            for (i = 0; i < lines_per_loop; i++) {
                    t0 = A[i+l*lines_per_loop][k*colum_per_loop];
                    t1 = A[i+l*lines_per_loop][1+k*colum_per_loop];
                    t2 = A[i+l*lines_per_loop][2+k*colum_per_loop];
                    t3 = A[i+l*lines_per_loop][3+k*colum_per_loop];
                    t4 = A[i+l*lines_per_loop][4+k*colum_per_loop];
                    t5 = A[i+l*lines_per_loop][5+k*colum_per_loop];
                    t6 = A[i+l*lines_per_loop][6+k*colum_per_loop];
                    t7 = A[i+l*lines_per_loop][7+k*colum_per_loop];
                    B[0+k*colum_per_loop][i+l*lines_per_loop] = t0;
                    B[1+k*colum_per_loop][i+l*lines_per_loop] = t1;
                    B[2+k*colum_per_loop][i+l*lines_per_loop] = t2;
                    B[3+k*colum_per_loop][i+l*lines_per_loop] = t3;
                    B[4+k*colum_per_loop][i+l*lines_per_loop] = t4;
                    B[5+k*colum_per_loop][i+l*lines_per_loop] = t5;
                    B[6+k*colum_per_loop][i+l*lines_per_loop] = t6;
                    B[7+k*colum_per_loop][i+l*lines_per_loop] = t7;
            }
        } 
    
     for (i = 0; i < N; i++) {
        if(i<line_loop_cnt*lines_per_loop)
        {
            for (j = colum_loop_cnt*colum_per_loop; j < M; j++) {
                t1 = A[i][j];
                B[j][i] = t1;
             }       
        }
        else
        {
            for (j = 0; j < M; j++) {
                t1 = A[i][j];
                B[j][i] = t1;
             }   
        }

    }  

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc); 
    // registerTransFunction(trans1, trans1_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

