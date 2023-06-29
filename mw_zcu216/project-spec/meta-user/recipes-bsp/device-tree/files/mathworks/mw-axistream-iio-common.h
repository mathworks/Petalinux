#ifndef __MW_AXISTREAM_COMMON__
#define __MW_AXISTREAM_COMMON__

#if !defined(MW_DATAWIDTH_SELECT)
	#define MW_DATAWIDTH_SELECT 64
#endif

#if defined(MW_DATAWIDTH_SELECT) && MW_DATAWIDTH_SELECT == 64
	#define MW_MM2S_DATAFMT "u64/64>>0"
	#define MW_S2MM_DATAFMT "u64/64>>0"
#elif defined(MW_DATAWIDTH_SELECT) && MW_DATAWIDTH_SELECT == 128
	#define MW_MM2S_DATAFMT "u128/128>>0"
	#define MW_S2MM_DATAFMT "u128/128>>0"
#endif

#if !defined(MW_SAMPLECNT_REG)
        #define MW_SAMPLECNT_REG 8
#endif

#endif /* __MW_AXISTREAM_COMMON__ */
