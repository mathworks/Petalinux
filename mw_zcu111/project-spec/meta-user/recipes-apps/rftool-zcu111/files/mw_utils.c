#include "cmd_interface.h"
#include "data_interface.h"
#include "error_interface.h"
#include "rfdc_interface.h"
#include "tcp_interface.h"
#include "xrfdc.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <math.h>

/************************** Constant Definitions *****************************/
#define RFDC_DEVICE_ID 0
#define DESIGN_TYPE_REG 0xB0005054
#define MAX_ADC_CHANNELS 8
#define MAX_DAC_CHANNELS 8
#define LMK04208_count 26 
#define LMX2594_A_count 113
#define XIIC_BUS 11 

double adc_sampling_rate_g;
int active_adc_tiles_g ; 
int active_dac_tiles_g; 
config_t cfg;
config_t *cf;
char *multiTileSync = "off";
char *versionInfo = NULL;
char *adcRealtimePorts = "off";
char *dacRealTimeNCOPort = "off";
char *adcRealTimeNCOPort = "off";
char *adcThreshold1Mode[8];
char *adcThreshold2Mode[8];

int adcThreshold1AvgVal[8];
int adcThreshold2AvgVal[8];
int adcThreshold1Min[8];
int adcThreshold1Max[8];
int adcThreshold2Min[8];
int adcThreshold2Max[8];        
char *adcMixerFrequency[MAX_ADC_CHANNELS]={};
char *dacMixerFrequency[MAX_ADC_CHANNELS]={};
double adcNCOFrequency[8];
double adcNCOPhase[8];
double dacNCOFrequency[8];
double dacNCOPhase[8] ;  
extern LMX2592_A[DAC_MAX][LMX2594_A_count]
extern LMK04208_CKin[LMK04208_MAX][LMK04208_count] ;
int nyquistZoneADC = 1;
int rfdcConfigVersion;
int isExternalPLLEnabled = 0; 
int nyquistZoneDAC = 1;


int mw_readIntegerParam(config_t *cfg, char *param, int *val)
{
    if (config_lookup_int(cfg, param , val) ==0 )
    {   
        printf("%s is not defined\n",param );
        return -1;
    }
    return 0;
}
int mw_readStringParam(config_t *cfg, char *param, const char **val )
{
    if (config_lookup_string(cfg, param , val) == 0 )
    {   
        printf("%s is not defined\n",param );
        return -1;
    }
    return 0;
}
int mw_readDoubleParam(config_t *cfg, char * param, double  *val)
{
    if (config_lookup_float(cfg, param , val) == 0 )
    {
        printf("%s is not defined\n",param );
        return -1;
    }
    return 0; 
}

int readStringArrayFromConfig(config_t *config , char *param , char **stringData)
{
    const config_setting_t *retries;
    int  count ; 
    retries = config_lookup(config , param );
    count = config_setting_length(retries);
    for (int n = 0; n < count; n++) {
        stringData[n] = config_setting_get_string_elem(retries, n);
        //printf("\t#%d. %s\n", n + 1,  config_setting_get_string_elem(retries, n));
    }
}

void readFloatArrayFromConfig(config_t *cf,  char *paramName , double  *floatData)
{
    const config_setting_t *retries;
    int  count ; 
    retries = config_lookup(cf, paramName );
    count = config_setting_length(retries);
    for (int n = 0; n < count; n++) {
        floatData[n] = config_setting_get_float_elem(retries, n);
        //printf("\t#%d. %f\n", n + 1,config_setting_get_float_elem(retries, n));
    }
}

void readIntegerArrayFromConfig( config_t *cf , char *paramName , int *intData )
{
    const config_setting_t *retries;
    int  count ; 
    retries = config_lookup(cf, paramName);
    count = config_setting_length(retries);
    int n =0;
    for( n = 0 ; n < count  ; n++) {
        intData[n] =  config_setting_get_int_elem(retries, n);
        //printf("\t#%d. %d\n" , n + 1 , config_setting_get_int_elem(retries, n));
    }
}




int mapThresholdMode( char *type )
{
    if(strcmp(type,"Sticky over") == 0 ){
            return XRFDC_TRSHD_STICKY_OVER  ;
    }
    else if(strcmp(type ,"Sticky under") == 0 ){
        return XRFDC_TRSHD_STICKY_UNDER  ;
    }
    else if(strcmp(type ,"Hysteresis") == 0){
        return XRFDC_TRSHD_HYSTERISIS;
    } 
}
int mapMixerType( char *type )
{
    if(strcmp(type,"Bypassed") == 0 )
        return XRFDC_MIXER_TYPE_OFF ;
    else if(strcmp(type ,"Coarse") == 0 )
        return XRFDC_MIXER_TYPE_COARSE ;
    else if(strcmp(type ,"Fine") == 0)
        return XRFDC_MIXER_TYPE_FINE; 
}

int mapMixerMode( char *mode)
{
    if(strcmp(mode ,"Real->Real") ==0 )
        return XRFDC_MIXER_MODE_R2R;
    else if(strcmp(mode, "Real->I/Q") ==0 )
        return XRFDC_MIXER_MODE_R2C ;
    else if(strcmp(mode, "I/Q->Real") ==0 )
        return XRFDC_MIXER_MODE_C2R ;
}

int mapInverseSyncFilter( char *mode)
{
    if(strcmp(mode ,"on") == 0 )
        return 1 ;
    else if(strcmp( mode, "off") == 0 )
        return 0 ;
    else
        return -1 ; 
}

unsigned int mapMixerFrequency( char  *freq)
{
    if(strcmp(freq,"Fs/2") ==0)
        return  XRFDC_COARSE_MIX_SAMPLE_FREQ_BY_TWO ;  
    else if(strcmp(freq,"Fs/4") ==0)
        return  XRFDC_COARSE_MIX_SAMPLE_FREQ_BY_FOUR ;  
    else if(strcmp(freq,"-Fs/4") ==0)
        return  XRFDC_COARSE_MIX_MIN_SAMPLE_FREQ_BY_FOUR; 
}

int updateNyquistZone( int nyquestZone)
{
    if ( nyquestZone % 2 == 0 )
        nyquestZone = 1 ;
    else
        nyquestZone = 2 ;

    return nyquestZone ;
}
int calculate_NyquistZone( double PLLSampleRate, double  NCOFrequency)
{
    int NyquistZone = 0;
    if ( NCOFrequency == 0) 
        NyquistZone = 1 ;
    else if (NCOFrequency > 0 )
        NyquistZone = ceil(NCOFrequency/(PLLSampleRate/2));
    else if( NCOFrequency < 0)
        NyquistZone = floor(NCOFrequency/(PLLSampleRate/2));
    return NyquistZone ;
}
int calculate_ADC_Calibration_Mode(double  PLLSampleRate, double eff_NCO_freq)
{
    /*  Selects between different calibration optimization schemes depending on the features of the input signals.
     * Mode 1 is optimal for input frequencies {PLLSampleRate/2(Nyquist) +/-10%}. Otherwise, use Mode 2.
     * Ref: RF Data Converter Interface User Guide UG1309 (v1.2) August 16, 2019 www.xilinx.com
     *      Fig. 15, page 20 */
    int cal_mode = 2;
    if(  abs(PLLSampleRate/2 - abs(eff_NCO_freq)) < ( 0.1*(PLLSampleRate/2)) )
        cal_mode = 1;

    return cal_mode;
}

double calculate_effective_NCO_freq(double PLLSampleRate, double NCOFrequency , short isDAC)
{
    /* This helper function folds NCOFrequency (range -10 GHz to 10 GHz, as defined in PG269) into range +/- PLLSampleRate/2 to create the 'effective NCO Frequency'.

     * this  is done for the purpose of displaying the effective NCO Frequency in the digital domain (-Fs/2 ... Fs/2) as a convenience for the user.
     * Note: xrfdc_mixer.c driver code would automatically perform this check when it receives the absolute Fc in the analog domain within (-10 GHz to 10 GHz)
     * Ref: Xilinx PG269 (v2.1) May 22, 2019, pages 47-48

     * Input arguments:
     *   NCOFrequency :
     *       For the DAC, the 'NCOFrequency' requested by the user indicates the analog domain frequency Fc where a desired image should appear at the output of the DAC
     *       Note: In a practical RF front-end (ex: www.avnet.com\rfsockit), a bandpass filter at the output of the DAC serves as the 're-construction' filter to isolate the desired signal at Fc.
     *
     *       For the ADC, the 'NCOFrequency' requested by the user as -Fc, where +Fc indicates the frequency in the analog domain at the ADC input where lies the signal of interest (-10 GHz to 10 GHz, as defined in PG269)
     *       In other words, the assumption is always that the user wishes to frequency-shift the desired analog input signal at the ADC to baseband in the digital domain.
     *
     *   chanidx: channel index (ones-based)

     * Output results:
     *   eff_NCO_freq: "effective" NCO frequency
     *   For ADC: the mixing frequency in the digital domain folded into range [-Fs/2 ... Fs/2] to shift a digital alias of the original analog signal at Fc, back to DC.

     * n Avnet RFSoC Explorer GUI, effective NCO Frequency is displayed in the semi-circular gauge.

     * Note: Fc determines the ADC calibration Mode (see Xilinx UG1309)

     * excerpt https://github.com/Xilinx/embeddedsw/blob/master/XilinxProcessorIPLib/drivers/rfdc/src/xrfdc_mixer.c (033f4c363031f7a1dc580dc4fd00bf8830239359)
     * reproduce behavior of XRFDC driver */

     double eff_NCO_freq = NCOFrequency;
     int NyquistZone, calibrationMode , nyquistZoneADCTmp;
    if ((eff_NCO_freq < -(PLLSampleRate / 2.0)) || (eff_NCO_freq > (PLLSampleRate / 2.0)))
    {
        while( abs(eff_NCO_freq) > (PLLSampleRate / 2.0))
        {
            if (eff_NCO_freq < -(PLLSampleRate / 2.0))
            {
                eff_NCO_freq =  eff_NCO_freq + PLLSampleRate ;   //Shift towards positive frequencies until reach Nyquist zone -1
            }        
            if (eff_NCO_freq > (PLLSampleRate / 2.0))
            {
                eff_NCO_freq = eff_NCO_freq - PLLSampleRate;    //Shift towards negative frequencies until reach Nyquist zone 1
            }
        }
    }
    printf("Effective NCO Freq is %lf \n",eff_NCO_freq);
    if(isDAC )
    {
        nyquistZoneDAC  = calculate_NyquistZone( PLLSampleRate, NCOFrequency);  // NCOFrequency = desired Fc at DAC analog output
    } else
    {
        /* For the ADC, the calculated effective NCO frequency will be negative if the analog carrier frequency (Fc, aka property 'NCOFrequency') is in 2nd Nyquist,
         * ... such that the alias in -1 Nyquist zone gets pulled towards the + frequencies to baseband
         * Hence the following explanation in Xilinx PG269 (v2.1) May 22, 2019, pages 47-48 for the ADC NCO frequency:
         * The DDC, in general, shifts the carrier from Fc to DC, so with an absolute NCO range (-10 GHz to 10 GHz),
         * setting the NCO to -Fc always shifts the desired signal Fc at DC;
         * setting the NCO to  Fc always shifts the inverse signal to DC, no matter which Nyquist band the signal is located in.
         * When setting the NCO within the effective NCO range (-Fs/2 to Fs/2), to convert the carrier to DC, the NCO frequency should be positive when the signal
         * is at even Nyquist bands, and negative when the signal is at odd Nyquist bands.*/

        nyquistZoneADCTmp  = calculate_NyquistZone(PLLSampleRate, -NCOFrequency);  // -NCOFrequency = NCO Frequency to shift the desired analog signal at the ADC input from carrier Fc (= +NCOFrequency) to DC
        nyquistZoneADC = nyquistZoneADCTmp - 1*(nyquistZoneADCTmp < 0 ) ; 
        calibrationMode = calculate_ADC_Calibration_Mode(PLLSampleRate, eff_NCO_freq);
    }

  return eff_NCO_freq;
}
XRFdc_Config  *mw_get_sysinfo_from_config_file()
{
    XRFdc_Config *rfdcConfigPtr= NULL;
    const config_setting_t *retries;
    const char *base = NULL;
    cf = &cfg;
    const char *adcMixerType[MAX_ADC_CHANNELS]={} , *adcMixerMode[MAX_ADC_CHANNELS] = {} ;

    char *dacMixerType[MAX_DAC_CHANNELS]= {} , *dacMixerMode[MAX_DAC_CHANNELS] = {};
    double adcSampleRate[8],  dacSampleRate[8],streamClkFreq  ; 
    int  adcDecimationMode[8], adcSamplesPerCycle[8] ,   dacInterpolationMode[8],  dacSamplesPerCycle[8] ;        
    char *dacPLLEnabled=NULL, *adcPLLEnabled=NULL, *dacInvertSyncFilter[8] , *externalPLL = NULL ;  
    int enabledADC[8], enabledADCCnt, enabledDAC[8], enabledDACCnt; 

    active_adc_tiles_g  = 4 ; 
    active_dac_tiles_g  = 2 ; 
    char *adc_mixer_mode , *adc_mixer_type , *dac_mixer_mode ; 
    char *tmp=NULL;
    int count, n ;
    int numADCs ,numDACs  ;
    const config_setting_t *adc_NCO_freq , *adc_NCO_phase ;
    config_init(cf);
    if (!config_read_file(cf, "/mnt/rfdc-configuration.cfg")) {
        fprintf(stderr, "%s:%d - %s\n",
                config_error_file(cf),
                config_error_line(cf),
                config_error_text(cf));
        config_destroy(cf);
        exit(1) ;// return( EXIT_FAILURE);
    }
    rfdcConfigPtr = (XRFdc_Config * )malloc(sizeof(XRFdc_Config)) ;
    if(rfdcConfigPtr == NULL)
    {
        printf("Failed to allocate memory for XRFDC configuration \n");
        exit(1) ;
    }
    int status ; //= mw_readIntegerParam(cf, "numADCs", &numADCs );
    status  = mw_readStringParam(cf, "version" , &versionInfo);
    //printf("RFDC config version :%d \n", rfdcConfigVersion);
    readFloatArrayFromConfig(cf , "dacNCOFrequency", dacNCOFrequency );
    readFloatArrayFromConfig(cf , "dacNCOPhase", dacNCOPhase );
    readFloatArrayFromConfig(cf , "adcNCOFrequency", adcNCOFrequency );
    readIntegerArrayFromConfig(cf , "adcNCOPhase", adcNCOPhase );
    if(status < 0 )
    {
        printf("Reading Old Configuraiton \n"); 
        mw_readStringParam(cf, "adcMixerFrequency", adcMixerFrequency);
        mw_readStringParam(cf, "dacMixerFrequency", dacMixerFrequency);
        printf("Reading Old Configuraiton \n"); 
        mw_readIntegerParam(cf, "numADCs", &numADCs );
        mw_readIntegerParam(cf, "adcDecimationMode",&adcDecimationMode[0]);
        mw_readIntegerParam(cf, "dacInterpolationMode",&dacInterpolationMode[0]);
        mw_readIntegerParam(cf, "numDACs" , &numDACs);
        mw_readStringParam(cf, "multiTileSync" , &multiTileSync);
        mw_readStringParam(cf, "adcMixerType", &adcMixerType[0]);
        mw_readStringParam(cf, "adcMixerMode", &adcMixerMode[0]);
        mw_readStringParam(cf, "dacMixerType", &dacMixerType[0]);
        mw_readStringParam(cf , "dacPLLEnabled", &dacPLLEnabled);
        mw_readStringParam(cf , "adcPLLEnabled", &adcPLLEnabled);
        mw_readStringParam(cf, "dacMixerMode", dacMixerMode);
        mw_readStringParam(cf, "dacInvertSyncFilter", &dacInvertSyncFilter[0]);
        mw_readDoubleParam(cf, "adcSampleRate", &adcSampleRate[0]) ;
        mw_readDoubleParam(cf, "dacSampleRate", &dacSampleRate[0]);

        printf("num ADCs  : %d \n ", numADCs);
        printf("num DACs  : %d \n ", numDACs);
        printf("adcMixerFrequency  =  : %s \n ", adcMixerFrequency[0]);
        switch(numADCs)
        {
            case 1 :
                active_adc_tiles_g  = 1 ; 
                active_dac_tiles_g  = 1 ; 
                break;
            case 2 :
                active_adc_tiles_g  = 1 ; 
                active_dac_tiles_g  = 1 ; 
                break;
            case 4:
                active_adc_tiles_g  = 2 ; 
                active_dac_tiles_g  = 1 ; 
                break;
            case 8:
                active_adc_tiles_g  = 4 ; 
                active_dac_tiles_g  = 2 ; 
                break;
        }
        for (int i=1 ;i < MAX_ADC_CHANNELS ; i++)
        {
            adcSampleRate[i] = adcSampleRate[0]; 
            dacInterpolationMode[i] = dacInterpolationMode[0];
            adcSamplesPerCycle[i]  = adcSamplesPerCycle[0];
            adcMixerMode[i] = adcMixerMode[0];
            adcMixerType[i] = adcMixerType[0];
            dacMixerMode[i] = dacMixerMode[0];
            dacMixerType[i] = dacMixerType[0];
            adcMixerFrequency[i] = adcMixerFrequency[0] ;
            dacMixerFrequency[i] = dacMixerFrequency[0];
        }
        for (int tile=0;tile< MAX_ADC_TILE ;tile++)
        {    
            if (tile < active_adc_tiles_g)
            {
                rfdcConfigPtr-> ADCTile_Config[tile].Enable =   1     ;
                rfdcConfigPtr-> ADCTile_Config[tile].SamplingRate = adcSampleRate[0]  ;
                rfdcConfigPtr-> ADCTile_Config[tile].RefClkFreq =  0.0 ;
                rfdcConfigPtr-> ADCTile_Config[tile].FabClkFreq =  0.0 ;
                rfdcConfigPtr-> ADCTile_Config[tile].PLLEnable = strcmp(&adcPLLEnabled[0],"on") ? 0 :1 ; 
                for (int block =0; block < MAX_ADC_PER_TILE ; block++)
                {
                    //Analog config
                    rfdcConfigPtr-> ADCTile_Config[tile].ADCBlock_Analog_Config[block].BlockAvailable = 1 ; 
                    rfdcConfigPtr-> ADCTile_Config[tile].ADCBlock_Analog_Config[block].MixMode= mapMixerMode(adcMixerMode[0]) ; 
                    //Digital config
                    rfdcConfigPtr->ADCTile_Config[tile].ADCBlock_Digital_Config[block].DecimationMode = adcDecimationMode[0]  ; 
                    rfdcConfigPtr-> ADCTile_Config[tile].ADCBlock_Digital_Config[block].MixerType= mapMixerType(adcMixerType[0]) ;
                    rfdcConfigPtr-> ADCTile_Config[tile].ADCBlock_Digital_Config[block].DataWidth = 32 ; 
                }
            }else       {
                //DIsabling all the seetings for these tiles
                rfdcConfigPtr-> ADCTile_Config[tile].Enable =  0;
                rfdcConfigPtr-> ADCTile_Config[tile].PLLEnable = 0 ; 
            }
        }

        for (int tile=0;tile< MAX_DAC_TILE  ;tile++)
        {    
            if( tile < active_dac_tiles_g)
            {
                rfdcConfigPtr-> DACTile_Config[tile].Enable =  1  ;
                rfdcConfigPtr-> DACTile_Config[tile].PLLEnable = strcmp(&dacPLLEnabled[0],"on")? 0 :1 ; 
                rfdcConfigPtr-> DACTile_Config[tile].SamplingRate  = dacSampleRate[0]  ;
                rfdcConfigPtr-> DACTile_Config[tile].RefClkFreq =  0.0 ;
                rfdcConfigPtr-> DACTile_Config[tile].FabClkFreq =  0.0 ;
                for (int block =0; block < MAX_DAC_PER_TILE  ;block++)
                {
                    //Analog config 
                    rfdcConfigPtr->DACTile_Config[tile].DACBlock_Analog_Config[block].BlockAvailable = 1 ; 
                    rfdcConfigPtr-> DACTile_Config[tile].DACBlock_Analog_Config[block].MixMode = mapMixerMode(dacMixerMode[0] ) ;  
                    rfdcConfigPtr->DACTile_Config[tile].DACBlock_Analog_Config[block].InvSyncEnable = mapInverseSyncFilter(dacInvertSyncFilter[0]) ; 
                    //Digital config 
                    rfdcConfigPtr->DACTile_Config[tile].DACBlock_Digital_Config[block].MixerType = mapMixerType(dacMixerType[0])   ;  
                    rfdcConfigPtr->DACTile_Config[tile].DACBlock_Digital_Config[block].DataWidth = 32 ; 
                    rfdcConfigPtr->DACTile_Config[tile].DACBlock_Digital_Config[block].InterpolationMode = dacInterpolationMode[0];   
                }
            }else       {
                //DIsabling all the seetings for these tiles
                rfdcConfigPtr-> DACTile_Config[tile].Enable =  0;
                rfdcConfigPtr-> DACTile_Config[tile].PLLEnable = 0 ; 

            }

        }
    }else
    {
    printf("Checking version info:%f: \n", atof(versionInfo) );
        if (atof(versionInfo) >= 1.2 )
        {
            mw_readStringParam(cf, "externalPLL" , &externalPLL);
            if (strcmp(externalPLL,"on") == 0)
            {
                isExternalPLLEnabled = 1;
                printf("External PLL is Enabled \n ");
            } 
        }
        if (atof(versionInfo) > 1.2 )
        {
            mw_readStringParam(cf, "dacRealTimeNCOPortsEnable" , &dacRealTimeNCOPort);
            mw_readStringParam(cf, "adcRealTimeNCOPortsEnable" , &adcRealTimeNCOPort);
            mw_readStringParam(cf, "adcRealTimePortsEnable" , &adcRealtimePorts);
            readStringArrayFromConfig(cf , "adcThreshold1Mode", adcThreshold1Mode );
            readStringArrayFromConfig(cf , "adcThreshold2Mode", adcThreshold2Mode );
            readIntegerArrayFromConfig(cf , "adcThreshold1AvgVal", adcThreshold1AvgVal);
            readIntegerArrayFromConfig(cf , "adcThreshold2AvgVal", adcThreshold2AvgVal);
            readIntegerArrayFromConfig(cf , "adcThreshold1Min", adcThreshold1Min);
            readIntegerArrayFromConfig(cf , "adcThreshold1Max", adcThreshold1Max);
            readIntegerArrayFromConfig(cf , "adcThreshold2Min", adcThreshold2Min);
            readIntegerArrayFromConfig(cf , "adcThreshold2Max", adcThreshold2Max);
        }
        readFloatArrayFromConfig(cf , "adcSampleRate", adcSampleRate);
        readIntegerArrayFromConfig(cf , "adcDecimationMode", adcDecimationMode);
        readIntegerArrayFromConfig(cf , "adcSamplesPerCycle", adcSamplesPerCycle);
        readStringArrayFromConfig(cf , "adcMixerType",adcMixerType );
        readStringArrayFromConfig(cf , "adcMixerMode",adcMixerMode );
        readStringArrayFromConfig(cf , "adcMixerFrequency",adcMixerFrequency  );

        readFloatArrayFromConfig (cf , "dacSampleRate", dacSampleRate  );
        readIntegerArrayFromConfig( cf,"dacInterpolationMode",dacInterpolationMode  );
        readIntegerArrayFromConfig(cf , "dacSamplesPerCycle",dacSamplesPerCycle );
        readStringArrayFromConfig(cf  , "dacMixerType", dacMixerType );
        readStringArrayFromConfig(cf , "dacMixerMode",  dacMixerMode );
        readStringArrayFromConfig(cf , "dacMixerFrequency", dacMixerFrequency );
        readStringArrayFromConfig(cf , "dacInvertSyncFilter", dacInvertSyncFilter );

        mw_readStringParam(cf , "dacPLLEnabled", &dacPLLEnabled);
        mw_readStringParam(cf , "adcPLLEnabled", &adcPLLEnabled);
        mw_readDoubleParam(cf , "streamClkFreq", &streamClkFreq );
        readIntegerArrayFromConfig(cf , "enabledADC", enabledADC );
        mw_readIntegerParam(cf , "enabledADCCnt", &enabledADCCnt );
        readIntegerArrayFromConfig(cf , "enabledDAC", enabledDAC);
        mw_readIntegerParam(cf , "enabledDACCnt", &enabledDACCnt );
        mw_readStringParam(cf , "multiTileSync", &multiTileSync);

        //printf("Multi tile sync is disabled : num of ADC tiles %d , num of DAC tiles %d \n", active_adc_tiles_g , active_dac_tiles_g);  
        for (int tile=0;tile< MAX_ADC_TILE ;tile++)
        {    
            rfdcConfigPtr-> ADCTile_Config[tile].Enable =   enabledADC[tile*MAX_ADC_PER_TILE ] | enabledADC[tile*MAX_ADC_PER_TILE + 1 ]      ;
            rfdcConfigPtr-> ADCTile_Config[tile].SamplingRate = adcSampleRate[tile]  ;
            rfdcConfigPtr-> ADCTile_Config[tile].RefClkFreq =  0.0 ;
            rfdcConfigPtr-> ADCTile_Config[tile].FabClkFreq =  0.0 ;
            rfdcConfigPtr-> ADCTile_Config[tile].PLLEnable = strcmp(&adcPLLEnabled[tile],"on") ? 0 :1 ; 

            for (int block =0; block < MAX_ADC_PER_TILE ; block++)
            {
                //Analog config
                rfdcConfigPtr-> ADCTile_Config[tile].ADCBlock_Analog_Config[block].BlockAvailable = enabledADC[tile*MAX_ADC_PER_TILE + block ] ; 
                rfdcConfigPtr-> ADCTile_Config[tile].ADCBlock_Analog_Config[block].MixMode= mapMixerMode(adcMixerMode[tile*MAX_ADC_PER_TILE + block]); 
                //Digital config
                rfdcConfigPtr->ADCTile_Config[tile].ADCBlock_Digital_Config[block].DecimationMode = adcDecimationMode[tile*MAX_ADC_PER_TILE + block]  ; 
                rfdcConfigPtr-> ADCTile_Config[tile].ADCBlock_Digital_Config[block].MixerType= mapMixerType(adcMixerType[ tile*MAX_ADC_PER_TILE + block]) ;
                rfdcConfigPtr-> ADCTile_Config[tile].ADCBlock_Digital_Config[block].DataWidth = 32 ; 
            }
        }

        for (int tile=0;tile< MAX_DAC_TILE  ;tile++)
        {    
            rfdcConfigPtr-> DACTile_Config[tile].Enable =  (enabledDAC[tile*MAX_DAC_PER_TILE ]) | (enabledDAC[tile*MAX_DAC_PER_TILE + 1] )  | (enabledDAC[tile*MAX_DAC_PER_TILE + 2 ]) | (enabledDAC[tile*MAX_DAC_PER_TILE +  3 ]   ) ;
            rfdcConfigPtr-> DACTile_Config[tile].PLLEnable = strcmp(&dacPLLEnabled[tile],"on")? 0 :1 ; 
            rfdcConfigPtr-> DACTile_Config[tile].SamplingRate  = dacSampleRate[tile]  ;
            rfdcConfigPtr-> DACTile_Config[tile].RefClkFreq =  0.0 ;
            rfdcConfigPtr-> DACTile_Config[tile].FabClkFreq =  0.0 ;
            for (int block =0; block < MAX_DAC_PER_TILE  ;block++)
            {
                //Analog config 
                rfdcConfigPtr->DACTile_Config[tile].DACBlock_Analog_Config[block].BlockAvailable = enabledDAC[tile*MAX_DAC_PER_TILE + block ] ; 
                rfdcConfigPtr-> DACTile_Config[tile].DACBlock_Analog_Config[block].MixMode = mapMixerMode(dacMixerMode[tile*MAX_DAC_PER_TILE + block ] ) ;  
                rfdcConfigPtr->DACTile_Config[tile].DACBlock_Analog_Config[block].InvSyncEnable = mapInverseSyncFilter(dacInvertSyncFilter[tile*MAX_DAC_PER_TILE + block]) ; 
                //Digital config 
                rfdcConfigPtr->DACTile_Config[tile].DACBlock_Digital_Config[block].MixerType = mapMixerType(dacMixerType[tile*MAX_DAC_PER_TILE + block])   ;  
                rfdcConfigPtr->DACTile_Config[tile].DACBlock_Digital_Config[block].DataWidth = 32 ; 
                rfdcConfigPtr->DACTile_Config[tile].DACBlock_Digital_Config[block].InterpolationMode = dacInterpolationMode[tile*MAX_DAC_PER_TILE + block];   
            }
        }

    }
    return rfdcConfigPtr;
}

void  mw_dump_configuration_info(XRFdc_Config *mwConfigPtr)
{
    for (int tile=0;tile< MAX_ADC_TILE ;tile++)
    {    
        printf("\nADC tile %d Configuraiton... ", tile );
        printf("\n\t  Tile Enable: %d ",  mwConfigPtr-> ADCTile_Config[tile].Enable) ;
        printf("\n\t  Samplerate :%f ",mwConfigPtr-> ADCTile_Config[tile].SamplingRate) ; 
        printf("\n\t  PLL enabled :%d ",mwConfigPtr-> ADCTile_Config[tile].PLLEnable);  

        for (int block =0; block < MAX_ADC_PER_TILE ; block++)
        {
            //Analog config
            printf("\n\t #### ADC tile:%d  block:%d Configuration #### " , tile, block ); 
            printf("\n\t\t  Block enable:%d ", mwConfigPtr-> ADCTile_Config[tile].ADCBlock_Analog_Config[block].BlockAvailable);
            printf("\n\t\t  Mixer mode :%d ",mwConfigPtr-> ADCTile_Config[tile].ADCBlock_Analog_Config[block].MixMode) ; 

            //Digital config
            printf("\n\t\t  Decimation mode :%d ", mwConfigPtr->ADCTile_Config[tile].ADCBlock_Digital_Config[block].DecimationMode);
            printf("\n\t\t  MixerType  :%d \n ",  mwConfigPtr-> ADCTile_Config[tile].ADCBlock_Digital_Config[block].MixerType) ; 
        }
    }

    for (int tile=0;tile< MAX_DAC_TILE  ;tile++)
    {    
        printf("\nDAC tile %d Configuraiton... ", tile );
        printf("\n\t DAC Enable:%d  ",mwConfigPtr-> DACTile_Config[tile].Enable);
        printf("\n\t DAC PLL enable:%d  ",mwConfigPtr-> DACTile_Config[tile].PLLEnable);
        printf("\n\t DAC SamplingRate :%f  ",mwConfigPtr-> DACTile_Config[tile].SamplingRate);
        for (int block =0; block < MAX_DAC_PER_TILE  ;block++)
        {
            //Analog config 
            printf("\n\t #### DAC tile:%d  block:%d Configuration #### " , tile, block ); 
            printf("\n\t\t  Block enable:%d ", mwConfigPtr->DACTile_Config[tile].DACBlock_Analog_Config[block].BlockAvailable) ;  
            printf("\n\t\t  Mixer mode :%d ", mwConfigPtr-> DACTile_Config[tile].DACBlock_Analog_Config[block].MixMode) ;   
            printf("\n\t\t  Inverse sync filer enabled :%d ", mwConfigPtr->DACTile_Config[tile].DACBlock_Analog_Config[block].InvSyncEnable);  

            //Digital config 
            printf("\n\t\t  Mixer type:  %d \n",  mwConfigPtr->DACTile_Config[tile].DACBlock_Digital_Config[block].MixerType); 
        }
    }

}

void mw_check_fpga_config_with_ps_config(XRFdc_Config *mw_config_ptr)
{
    int *status;
    XRFdc_BlockStatus BlockStatus;
    int Tile_Id;
    char Response[BUF_MAX_LEN]={0};
    XRFdc_IPStatus ipStatus;

    /* calling this function gets the status of the IP */
    *status = XRFdc_GetIPStatus(&RFdcInst, &ipStatus);
    for (Tile_Id = 0; Tile_Id <= 3; Tile_Id++) {
        sprintf(Response," %d %d %d %d %d %d %d %d %d %d ",
                ipStatus.DACTileStatus[Tile_Id].IsEnabled,ipStatus.DACTileStatus[Tile_Id].BlockStatusMask,
                ipStatus.DACTileStatus[Tile_Id].TileState,ipStatus.DACTileStatus[Tile_Id].PowerUpState,
                ipStatus.DACTileStatus[Tile_Id].PLLState,
                ipStatus.ADCTileStatus[Tile_Id].IsEnabled,ipStatus.ADCTileStatus[Tile_Id].BlockStatusMask,
                ipStatus.ADCTileStatus[Tile_Id].TileState,ipStatus.ADCTileStatus[Tile_Id].PowerUpState,
                ipStatus.ADCTileStatus[Tile_Id].PLLState);
        //strncat (txstrPtr, Response,BUF_MAX_LEN);
    }

    sprintf(Response," %d ", ipStatus.State);
    //strncat (txstrPtr, Response,BUF_MAX_LEN);

    if (*status != SUCCESS) {
        printf("\nXRFdc_GetIPStatus failed\r\n");
    } else {
        printf("    **********XRFdc_GetIPStatus*********\r\n");
        printf("IP Status State: %d\r\n",ipStatus.State);
        for (Tile_Id = 0; Tile_Id <= 3; Tile_Id++) {
            /* DAC */
            printf("Tile: %d DAC Enabled= %d \r\n", Tile_Id,ipStatus.DACTileStatus[Tile_Id].IsEnabled);
            printf("  BlockStatus:  0x%x\r\n",   ipStatus.DACTileStatus[Tile_Id].BlockStatusMask);
            printf("  TileState:    0x%08x\r\n", ipStatus.DACTileStatus[Tile_Id].TileState);
            printf("  PowerUpState: 0x%08x\r\n", ipStatus.DACTileStatus[Tile_Id].PowerUpState);
            printf("  PLLState:     0x%08x\r\n", ipStatus.DACTileStatus[Tile_Id].PLLState);

            /* ADC */
            printf("Tile: %d ADC Enabled= %d \r\n", Tile_Id,ipStatus.ADCTileStatus[Tile_Id].IsEnabled);
            printf("  BlockStatus:  0x%x\r\n", ipStatus.ADCTileStatus[Tile_Id].BlockStatusMask);
            printf("  TileState:    0x%08x\r\n", ipStatus.ADCTileStatus[Tile_Id].TileState);
            printf("  PowerUpState: 0x%08x\r\n", ipStatus.ADCTileStatus[Tile_Id].PowerUpState);
            printf("  PLLState:     0x%08x\r\n", ipStatus.ADCTileStatus[Tile_Id].PLLState);
            printf("    *********************************\r\n");

        }
    }

    for(int Tile_Id =0 ; Tile_Id < MAX_DAC_TILE ;Tile_Id++)
    {
        for(int Block_Id = 0;Block_Id < 4  ;Block_Id++)
        {
            *status = XRFdc_GetBlockStatus(&RFdcInst, DAC, Tile_Id, Block_Id, &BlockStatus);
            if (*status != SUCCESS)
            {
                printf("XRFdc_GetBlockStatus() failed\n\r");
            } else {

                printf("    **********XRFdc_GetBlockStatus***********\r\n");
                printf("    Tile_Id:         %d\r\n", Tile_Id);
                printf("    Block_Id:        %d\r\n", Block_Id);
                printf("    SamplingFreq:                         %f\r\n",  BlockStatus.SamplingFreq);
                printf("    AnalogDataPathStatus:                %d\r\n",  BlockStatus.AnalogDataPathStatus);
                printf("    DigitalDataPathStatus:               %d\r\n", BlockStatus.DigitalDataPathStatus);
                printf("    DataPathClocksStatus:                %d\r\n", BlockStatus.DataPathClocksStatus);
                printf("    IsFIFOFlagsAsserted:                 %d\r\n", BlockStatus.IsFIFOFlagsAsserted);
                printf("    IsFIFOFlagsEnabled:                  %d\r\n", BlockStatus.IsFIFOFlagsEnabled);

            }
        }
    }
}

int getPLLRefClkIndex( double refClock)
{
    double refPLLlist[] = { 102.40625, 204.8, 245.76, 409.6, 491.52, 737.28, 1474.56, 1966.08, 2048.00, 
			    2457.6, 2949.12, 3072.00, 3194.88, 3276.8, 3686.4, 3932.16,4096, 4423.68, 
			    4669.44, 4915.2, 5734.4,5898.24,6144.00 ,6389.76, 6400.00, 6553.6} ; 
    int i = 0;
    int clkIndex;
    for (i = 0; i < sizeof(refPLLlist); i++) {
        if( refPLLlist[i] == refClock ) {
            clkIndex = i;
            return clkIndex; 
        }
    }
    return clkIndex; 
} 
