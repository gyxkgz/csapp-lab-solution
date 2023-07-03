/*

缓存行结构

|--63位标记--|-1位有效位|--n字节块--|

*/
#include "cachelab.h"
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<stdint.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
extern char *optarg;

/*
命令行参数
*/
int verbose_flag=0,sets = 0,lines = 1,block_offset_bits = 0;
char * input_filep=NULL;
//缓存性能统计
int hits=0,misses=0,evictions=0;

void show_usage()
{
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\n");
    printf("Examples:\n");
    printf("linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}
void parse_arg(int argc ,char *argv[])
{
    int opt;
    if(argc<2)
    {
        printf("./csim: Missing required command line argument\n");
        show_usage();
        exit(0);
    }
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch(opt){
            case 'h':
                show_usage();
                exit(0);
            case 'v':
                verbose_flag=1;
                break;
            case 's':
                sets = atoi(optarg);
                break;
            case 'E':
                lines = atoi(optarg);
                break;
            case 'b':
                block_offset_bits = atoi(optarg);
                break;
            case 't':
                input_filep = optarg;
                break;
            default:
                show_usage();               
                exit(0);
        }
    }
    if(!input_filep)
    {
        printf("./csim: Missing required command line argument\n");
        show_usage();
        exit(0);
    }
}
char * init_cache(int size)
{
    return (char *)malloc(size);
}

void free_cache(void * cachep)
{
    free(cachep);
}

void replay_trace(char * cache)
{
    char buf[20];                           //存储指令字符串
    int blk_size = 1<<block_offset_bits;    //块大小
    int set_size = ((1<<block_offset_bits)+8)*lines;    //组大小
    int set_index = 0;                      //组索引
    int64_t tag = 0;                        //标记
    int64_t addr = 0;                       //地址
    int bytes = 0;
    
    char * setp;//*str;                     //组指针
    int i=0,invalid_cnt=0,lru_index;
    int hit_flag=0,miss_flag=0,evict_flag=0;

    FILE *fp = fopen(input_filep,"r");

    char *lru_cnt = (char *)calloc(lines*(1<<sets),1); //lru变量，替换最小值
    while(fgets(buf,20,fp)!=NULL)           //读取指令
    {
        hit_flag=0;
        miss_flag=0;
        evict_flag=0;
        invalid_cnt = 0;
        buf[strlen(buf)-1]=0;
        if(buf[0]=='I')
            continue;
        sscanf(buf+3,"%lx,%d",&addr,&bytes);                    //获取地址和字节
        set_index = (addr>>block_offset_bits)&((1<<sets)-1);    //获得组索引
        tag = addr>>(block_offset_bits+sets);                   //获得标记
        setp = &cache[set_index*set_size];                      //根据组索引得到组的缓存地址
        for(i=0;i<lines;i++)
        {
            char cvalid = setp[i*(blk_size+8)]&0x1;
            int64_t ctag =*(int64_t * )(setp+i*(blk_size+8))>>1;
            if(cvalid)
            {   if(ctag==tag)
                {
                    if(bytes<=blk_size)
                    {  
                        hits++;
                        hit_flag=1;
                        for(int j=0;j<lines;j++)
                        {
                            if(lru_cnt[i+lines*set_index]<lru_cnt[j+lines*set_index])   
                                lru_cnt[j+lines*set_index]--;
                        }
                        lru_cnt[i+lines*set_index]=lines-1;

                        break;
                    }                     
                }
            }
            else 
            {
                invalid_cnt++;
            }
        }
        if(i==lines)
        {
            misses++;
            miss_flag=1;
            if(!invalid_cnt)
            {
                evictions++;
                evict_flag=1;
                {
                    lru_index=0;
                    for(i=1;i<lines;i++)
                    {
                        if(lru_cnt[lru_index+lines*set_index]>=lru_cnt[i+lines*set_index])
                        {
                            lru_cnt[lru_index+lines*set_index]--;
                            lru_index = i;
                        }
                        else
                        {
                            lru_cnt[i+lines*set_index]--;
                        }
                    }
                    lru_cnt[lru_index+lines*set_index]=lines-1;
                    *(int64_t *)(setp+lru_index*(blk_size+8))=(tag<<1)|1;
                }                
            }
            else
            {
                for(i=0;i<lines;i++)
                {
                    char cvalid = setp[i*(blk_size+8)]&0x1;
                    if(!cvalid)
                    {
                        *(int64_t *)(setp+i*(blk_size+8))=(tag<<1)|1;
                        for(int j=0;j<lines;j++)
                        {
                            if(lru_cnt[i+lines*set_index]<lru_cnt[j+lines*set_index])
                                lru_cnt[j+lines*set_index]--;
                        }
                        lru_cnt[i+lines*set_index]=lines-1;
                        break;
                    }
                }
            }
        }
        if(buf[1]=='M')
        {
            hits++;
        }
        if(verbose_flag)
        {
            printf("%c %lx,%d ",buf[1],addr,bytes);
            if(miss_flag)
            printf("miss ");
            if(evict_flag)
            printf("eviction ");
            if(hit_flag)
            printf("hit ");
            if(buf[1]=='M')
            printf("hit ");
            printf("\n");
        }
    }
    free(lru_cnt);
    fclose(fp);
}

int main(int argc ,char *argv[])
{

    parse_arg(argc ,argv);
    int size =((1<<block_offset_bits)+8)*lines*(1<<sets);
    char * cp = init_cache(size);
    replay_trace(cp);
    free_cache(cp);
    printSummary(hits, misses, evictions);
    return 0;
}
