/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: OnMainPanel */
#define  PANEL_SWITCH                     2       /* control type: binary, callback function: OnSwitch */
#define  PANEL_FILTRED_DATA               3       /* control type: graph, callback function: (none) */
#define  PANEL_RAW_DATA                   4       /* control type: graph, callback function: (none) */
#define  PANEL_LOAD_BUTTON                5       /* control type: command, callback function: OnLoad */
#define  PANEL_TRECERI_ZERO               6       /* control type: numeric, callback function: (none) */
#define  PANEL_MIN_INDEX                  7       /* control type: numeric, callback function: (none) */
#define  PANEL_MAX_INDEX                  8       /* control type: numeric, callback function: (none) */
#define  PANEL_MINIM                      9       /* control type: numeric, callback function: (none) */
#define  PANEL_MAXIM                      10      /* control type: numeric, callback function: (none) */
#define  PANEL_DISPERSIE                  11      /* control type: numeric, callback function: (none) */
#define  PANEL_MEDIANA                    12      /* control type: numeric, callback function: (none) */
#define  PANEL_MEDIE                      13      /* control type: numeric, callback function: (none) */
#define  PANEL_HISTO                      14      /* control type: graph, callback function: (none) */
#define  PANEL_APLICA                     15      /* control type: command, callback function: OnAplica */
#define  PANEL_OPTIUNI_FILTER             16      /* control type: ring, callback function: (none) */
#define  PANEL_ALPHA                      17      /* control type: numeric, callback function: (none) */
#define  PANEL_KURTOSIS                   18      /* control type: numeric, callback function: (none) */
#define  PANEL_SKEWNESS                   19      /* control type: numeric, callback function: (none) */
#define  PANEL_EXIT                       20      /* control type: command, callback function: OnExit */
#define  PANEL_STOP                       21      /* control type: numeric, callback function: (none) */
#define  PANEL_START                      22      /* control type: numeric, callback function: (none) */
#define  PANEL_PREV                       23      /* control type: command, callback function: OnPrev */
#define  PANEL_NEXT                       24      /* control type: command, callback function: OnNext */
#define  PANEL_STARTT                     25      /* control type: command, callback function: OnStart */
#define  PANEL_DERIVATA_2                 26      /* control type: command, callback function: OnDerivata2 */
#define  PANEL_ANVELOPA_2                 27      /* control type: command, callback function: OnAnvelopa2 */
#define  PANEL_ANVELOPA                   28      /* control type: command, callback function: OnAnvelopa */
#define  PANEL_DERIVATA                   29      /* control type: command, callback function: OnDerivata */
#define  PANEL_INTERVAL                   30      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION                 31      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_3               32      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_2               33      /* control type: deco, callback function: (none) */

#define  PANEL_FREQ                       2       /* callback function: OnPanelFreq */
#define  PANEL_FREQ_SWITCH                2       /* control type: binary, callback function: OnSwitch */
#define  PANEL_FREQ_DATAF                 3       /* control type: graph, callback function: (none) */
#define  PANEL_FREQ_SPECTRU               4       /* control type: graph, callback function: (none) */
#define  PANEL_FREQ_WINDOW_2              5       /* control type: graph, callback function: (none) */
#define  PANEL_FREQ_WINDOW                6       /* control type: graph, callback function: (none) */
#define  PANEL_FREQ_EXIT                  7       /* control type: command, callback function: OnExit */
#define  PANEL_FREQ_TIP_DATA              8       /* control type: ring, callback function: (none) */
#define  PANEL_FREQ_WINDOW_TIP            9       /* control type: ring, callback function: (none) */
#define  PANEL_FREQ_OPTIUNI_FILTER        10      /* control type: ring, callback function: OnOptiuni */
#define  PANEL_FREQ_POWPEAK               11      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_FREQPEAK              12      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_TIMERSWITCH           13      /* control type: binary, callback function: OnTimerSwitch */
#define  PANEL_FREQ_APLICAFILTER          14      /* control type: command, callback function: OnFilterAplica */
#define  PANEL_FREQ_FiILTREDDATA          15      /* control type: radioButton, callback function: OnFiltredData */
#define  PANEL_FREQ_RAWDATA               16      /* control type: radioButton, callback function: OnRawData */
#define  PANEL_FREQ_DECORATION_2          17      /* control type: deco, callback function: (none) */
#define  PANEL_FREQ_CUTFREQ               18      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_DECORATION            19      /* control type: deco, callback function: (none) */
#define  PANEL_FREQ_BETA                  20      /* control type: ring, callback function: (none) */
#define  PANEL_FREQ_NUMBER                21      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQ_TIMER                 22      /* control type: timer, callback function: OnTimer */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnAnvelopa(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAnvelopa2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAplica(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDerivata(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDerivata2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFilterAplica(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFiltredData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoad(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNext(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnOptiuni(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelFreq(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrev(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnRawData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStart(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSwitch(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimerSwitch(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
