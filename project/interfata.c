#include <analysis.h>
#include <ansi_c.h>
#include <utility.h>
#include <formatio.h>
#include <cvirte.h>		
#include <userint.h>
#include "interfata.h"

static int panelHandle;

//CONSTANTE
#define SAMPLE_RATE 0
#define NPOINTS 1
#define INTERVAL 12
#define PI 3.14159265358979323846
#define HAMMING 2
#define BLKMAN 5

//GLOBAL VARIABLES
int waveInfo[2]; //waveInfo[0] = sampleRate
				 //waveInfo[1] = number of elements
double sampleRate = 0.0;
int npoints = 0;
double *waveData = 0;
double *filtre = 0;
double *anvelopa = 0;
double *derivata = 0;
double start, stop;
char fileName[55];
int bitmapID=0;
int anv, der; //plotHandle pt anvelopa si derivata
int freqHandle;

double *filtruF = 0; 
double ksr[323];
int lungf = 323;
int filt,wd; //plotHandle pt filtre
int win,n,tip; //valorile pt ferestruire
double *wincoef;
double start = 0; //startul timerului pt numarare

//FUNCTII FILTRARE
void filtru(double alpha);
void filtru_mediere(int maxi);

//FUNCTIE DERIVARE
void derivare();

//FUNCTIE COEFICIENTI
double* WindowCoef(int win, int numElem);

//FUNCTIE SPECTRU
int ComputePowerSpectrum(double inputArray[],unsigned int startp, unsigned int numberOfElements, double samplingRate, double convertedSpectrum[], double *frequencyInterval, double *powerPeak, double *freqPeak);


int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "interfata.uir", PANEL)) < 0)
		return -1;
	
	if ((freqHandle = LoadPanel (0, "interfata.uir", PANEL_FREQ)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK OnMainPanel (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			free(waveData);
			free(filtre);
			if(der!=0)
			{
				free(derivata);
			}
			if(anv!=0)
			{
				free(anvelopa);
			}
			if(filt!=0)
			{
				free(filtruF);
			}
			free(wincoef);
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK OnLoad (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			//creez culori pentru filtru		
			int visiniu = MakeColor(153, 0, 76);
			
			//variabile locale
			double medie, mediana, dis, maxim, minim, kurtosis, skewness;
			ssize_t maxIndex, minIndex;
			int nrzero=0;
			
			//alocare memorie pentru histograma
			ssize_t* histogram = (ssize_t *) calloc(INTERVAL, sizeof(ssize_t));
			double* xValue = (double *) calloc(INTERVAL, sizeof(double));
			
			//executa script python pentru conversia unui fisierului .wav in .txt
			LaunchExecutable("python main.py");
			
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar
			Delay(2);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			
			//alocare memorie pentru numarul de puncte
			waveData = (double *) calloc(npoints, sizeof(double));
			
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			//stergere pe graf
			DeleteGraphPlot(panel,PANEL_RAW_DATA,-1,VAL_IMMEDIATE_DRAW);
			
			//afisare pe grapf
			PlotY(panel, PANEL_RAW_DATA, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, visiniu);
			
			//facem butoanele vizibile
			SetCtrlAttribute(panel,PANEL_MEDIE,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_MEDIANA,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_DISPERSIE,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_MAXIM,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_MINIM,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_TRECERI_ZERO,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_MIN_INDEX,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_MAX_INDEX,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_HISTO,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_APLICA,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_OPTIUNI_FILTER,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_SKEWNESS,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_KURTOSIS,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_INTERVAL,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_START,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_STOP,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_STARTT,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_ALPHA,ATTR_VISIBLE,1);
			SetCtrlAttribute(panel,PANEL_DERIVATA,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_ANVELOPA,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_DERIVATA_2,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_ANVELOPA_2,ATTR_DIMMED,0);
			
			
			//calculam medie, mediana,dispersie,min,max si indecsii 
			Mean(waveData,npoints,&medie);
			Median (waveData, npoints, &mediana);
			Variance(waveData,npoints,&medie,&dis);
			MaxMin1D(waveData,npoints,&maxim,&maxIndex,&minim,&minIndex);
			
			//calculam treceri prin zero
			for(int i=0;i<npoints-1;i++)
			{
				if(waveData[i]==0)
					nrzero++;
				if(waveData[i]<0 && waveData[i+1]>0)
					nrzero++;
				if(waveData[i]>0 && waveData[i+1]<0)
					nrzero++;
			}
			
			//calculez asimetria (skewness) si aplatizarea (kurtosis)
			Moment(waveData,256,3,&skewness);
			Moment(waveData,256,4,&kurtosis);
			
			//HISTOGRAMA
			Histogram(waveData,npoints,minim,maxim,histogram,xValue,INTERVAL);
			
			//afisare histograma
			PlotXY(panel,PANEL_HISTO,xValue,histogram,INTERVAL,VAL_DOUBLE,VAL_INTEGER,VAL_VERTICAL_BAR,VAL_SMALL_SOLID_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_MAGENTA);
			
			//afisare valori min,max,med,dispersie,mediana,treceri prin zero, skewness, kurtosis
			SetCtrlVal(panel,PANEL_DISPERSIE,dis);
			SetCtrlVal(panel,PANEL_MEDIANA,mediana);
			SetCtrlVal(panel,PANEL_MEDIE,medie);
			SetCtrlVal(panel,PANEL_MINIM,minim);
			SetCtrlVal(panel,PANEL_MAXIM,maxim);
			SetCtrlVal(panel,PANEL_MAX_INDEX,maxIndex);
			SetCtrlVal(panel,PANEL_MIN_INDEX,minIndex);
			SetCtrlVal(panel,PANEL_TRECERI_ZERO,nrzero);
			SetCtrlVal(panel,PANEL_SKEWNESS,skewness);
			SetCtrlVal(panel,PANEL_KURTOSIS,kurtosis);
			
			break;
	}
	return 0;
}

 int CVICALLBACK OnAplica (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			//citesc optiunea filtrului
			int optiune;
			GetCtrlVal(panel,PANEL_OPTIUNI_FILTER,&optiune);
			
			SetCtrlAttribute(panel,PANEL_FILTRED_DATA,ATTR_DIMMED,0);
			
			//sterg graful existent
			DeleteGraphPlot(panel,PANEL_FILTRED_DATA,-1,VAL_IMMEDIATE_DRAW);
		
			switch(optiune)
			{
				//filtru de ordin I	
				case 0:
					double alpha;
					GetCtrlVal(panel,PANEL_ALPHA,&alpha);
					filtru(alpha);
					PlotY(panel,PANEL_FILTRED_DATA,filtre,npoints,VAL_DOUBLE,VAL_THIN_LINE,VAL_ASTERISK,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_RED);
					break;
					
				//filtru de mediere 16 elemente
				case 1:
					filtru_mediere(16);
					PlotY(panel,PANEL_FILTRED_DATA,filtre,npoints,VAL_DOUBLE,VAL_THIN_LINE,VAL_ASTERISK,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_RED);
					break;
					
				//filtru de mediere 32 elemente
				case 2:
					filtru_mediere(32);
					PlotY(panel,PANEL_FILTRED_DATA,filtre,npoints,VAL_DOUBLE,VAL_THIN_LINE,VAL_ASTERISK,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_RED);
					break;
			}
			
			break;
	}
	return 0;
}

void filtru(double alpha)
{
	//alocare memorie pentru numarul de puncte
	filtre = (double *) calloc(npoints, sizeof(double));
	
	filtre[0]=waveData[0];
	
	for(int i=1;i<npoints;i++)
	{
		filtre[i]=(1-alpha)*filtre[i-1]+alpha*waveData[i];
	}
}

void filtru_mediere(int maxi)
{			   
	//alocare memorie pentru numarul de puncte
	filtre = (double *) calloc(npoints, sizeof(double));
	
	filtre[0]=waveData[0];
	double sum=filtre[0];
	
	//calculez pentru primele maxi puncte
	for( int i=1;i<maxi;i++)
	{
		sum=sum+waveData[i];
		filtre[i]=sum/(i+1);
	}
	
	//calculez pentru restul de puncte
	for(int i=maxi;i<npoints;i++)
	{
		sum=sum-waveData[i-maxi]+waveData[i];
		filtre[i]=sum/maxi;
	}
}

int CVICALLBACK OnExit (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			free(waveData);
			free(filtre);
			if(der!=0)
			{
				free(derivata);
			}
			if(anv!=0)
			{
				free(anvelopa);
			}
			if(filt!=0)
			{
				free(filtruF);
			}
			free(wincoef);
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK OnPrev (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//vedem pe ce interval suntem
			GetCtrlVal(panel,PANEL_START,&start);
			GetCtrlVal(panel,PANEL_STOP,&stop);
			
			//scadem sa mergem la intervalul anterior
			start = start - 1;
			stop = stop - 1;  
			
			//verificam sa nu iesim din maxime
			if(start<0)
			{
				start=0;
				stop=1;
			}
			if(stop>6)
			{
				stop=6;
				start=5;
			}
					
			//setam noile valori de interval
			SetCtrlVal(panel,PANEL_START,start);
			SetCtrlVal(panel,PANEL_STOP,stop);
			
			//marim afisarea pe graf 
			SetAxisScalingMode(panel,PANEL_RAW_DATA,VAL_BOTTOM_XAXIS,VAL_MANUAL,start*sampleRate,stop*sampleRate);
			SetAxisScalingMode(panel,PANEL_FILTRED_DATA,VAL_BOTTOM_XAXIS,VAL_MANUAL,start*sampleRate,stop*sampleRate);
			
			//salvam graful cu datele nefiltrate
			sprintf(fileName,"Imagini\\graficRaw_%.0f_%.0f.jpg",start,stop);
			GetCtrlDisplayBitmap(panel,PANEL_RAW_DATA,1,&bitmapID);
			SaveBitmapToJPEGFile(bitmapID,fileName,JPEG_PROGRESSIVE,100);
			DiscardBitmap(bitmapID);
			
			//salvam graful cu datele filtrate
			sprintf(fileName,"Imagini\\graficFiltred_%.0f_%.0f.jpg",start,stop);
			GetCtrlDisplayBitmap(panel,PANEL_FILTRED_DATA,1,&bitmapID);
			SaveBitmapToJPEGFile(bitmapID,fileName,JPEG_PROGRESSIVE,100);
			DiscardBitmap(bitmapID); 
			break;
	}
	return 0;
}

int CVICALLBACK OnNext (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//vedem pe ce interval suntem
			GetCtrlVal(panel,PANEL_START,&start);
			GetCtrlVal(panel,PANEL_STOP,&stop);
			
			//scadem sa mergem la intervalul urmator
			start = start + 1;
			stop = stop + 1;
			
			//verificam sa nu iesim din maxime
			if(start<0)
			{
				start=0;
				stop=1;
			}
			if(stop>6)
			{
				start=5;
				stop=6;
			}
			
			//setam noile valori de interval
			SetCtrlVal(panel,PANEL_START,start);
			SetCtrlVal(panel,PANEL_STOP,stop);
			
			//marim afisarea pe graf
			SetAxisScalingMode(panel,PANEL_RAW_DATA,VAL_BOTTOM_XAXIS,VAL_MANUAL,start*sampleRate,stop*sampleRate);
			SetAxisScalingMode(panel,PANEL_FILTRED_DATA,VAL_BOTTOM_XAXIS,VAL_MANUAL,start*sampleRate,stop*sampleRate);
			
			//afisez butonul de prev
			SetCtrlAttribute(panel,PANEL_PREV,ATTR_VISIBLE,1); 
			
			//salvam graful cu datele nefiltrate
			sprintf(fileName,"Imagini\\graficRaw_%.0f_%.0f.jpg",start,stop);
			GetCtrlDisplayBitmap(panel,PANEL_RAW_DATA,1,&bitmapID);
			SaveBitmapToJPEGFile(bitmapID,fileName,JPEG_PROGRESSIVE,100);
			
			//salvam graful cu datele filtrate
			sprintf(fileName,"Imagini\\graficFiltred_%.0f_%.0f.jpg",start,stop);
			GetCtrlDisplayBitmap(panel,PANEL_FILTRED_DATA,1,&bitmapID);
			SaveBitmapToJPEGFile(bitmapID,fileName,JPEG_PROGRESSIVE,100);
			break;
	}
	return 0;
}

int CVICALLBACK OnStart (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//vedem pe ce interval suntem
			GetCtrlVal(panel,PANEL_START,&start);
			GetCtrlVal(panel,PANEL_STOP,&stop);
			
			//marim afisarea pe graf
			SetAxisScalingMode(panel,PANEL_RAW_DATA,VAL_BOTTOM_XAXIS,VAL_MANUAL,start*sampleRate,stop*sampleRate);
			SetAxisScalingMode(panel,PANEL_FILTRED_DATA,VAL_BOTTOM_XAXIS,VAL_MANUAL,start*sampleRate,stop*sampleRate);
			
			//afisez butonul next
			SetCtrlAttribute(panel,PANEL_NEXT,ATTR_VISIBLE,1);
			break;
	}
	return 0;
}

int CVICALLBACK OnDerivata (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//daca e desenata derivata o sterg
			if(der)
			{
				free(derivata);
				DeleteGraphPlot(panel,PANEL_RAW_DATA,der,VAL_IMMEDIATE_DRAW);
				der=0;
			}
			
			//derivez semnalul
			derivare();
			
			//afisez derivata pe graf
			der=PlotY(panel,PANEL_RAW_DATA,derivata,npoints,VAL_DOUBLE,VAL_THIN_LINE,VAL_ASTERISK,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_GREEN);
			break;
	}
	return 0;
}

void derivare()
{
	//alocare memorie pentru numarul de puncte
	derivata = (double *) calloc(npoints, sizeof(double));
	
	for(int i=0;i<npoints-1;i++)
	{
		derivata[i]= (waveData[i+1] - waveData[i])/(i+1-i);
	}
}

int CVICALLBACK OnAnvelopa (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//creez culori pentru filtru
			int pink = MakeColor(255, 192, 203);
		
			//daca e desenata anvelopa o sterg
			if(anv)
			{
				free(anvelopa);
				DeleteGraphPlot(panel,PANEL_RAW_DATA,anv,VAL_IMMEDIATE_DRAW);
				anv=0;
			}
			
			//alocare memorie pentru numarul de puncte
			anvelopa = (double *) calloc(npoints, sizeof(double));
		
		
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("anvelopa.txt", anvelopa, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
		
			//afisare pe grapf
			anv=PlotY(panel, PANEL_RAW_DATA, anvelopa, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, pink);
			break;
	}
	return 0;
}

int CVICALLBACK OnAnvelopa2 (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//daca e desenata anvelopa o sterg
			if(anv)
			{
				free(anvelopa);
				DeleteGraphPlot(panel,PANEL_RAW_DATA,anv,VAL_IMMEDIATE_DRAW);
				anv=0;
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnDerivata2 (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//daca e desenata derivata o sterg
			if(der)
			{
				free(derivata);
				DeleteGraphPlot(panel,PANEL_RAW_DATA,der,VAL_IMMEDIATE_DRAW);
				der=0;
			}
			break;
	}
	return 0;
}

//--------------FREQUNECY---------------


int CVICALLBACK OnSwitch (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel == panelHandle)
			{
				SetCtrlVal(freqHandle,PANEL_SWITCH,1);
				DisplayPanel(freqHandle);
				HidePanel(panelHandle);
				
				//fac vizibile butoanele
				SetCtrlAttribute(freqHandle,PANEL_FREQ_SWITCH,ATTR_DIMMED,0); 
				SetCtrlAttribute(freqHandle,PANEL_FREQ_WINDOW_TIP,ATTR_DIMMED,0); 
				SetCtrlAttribute(freqHandle,PANEL_FREQ_NUMBER,ATTR_DIMMED,0); 
				
				//stergere pe graf
				DeleteGraphPlot(freqHandle,PANEL_FREQ_DATAF,-1,VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot(freqHandle,PANEL_FREQ_WINDOW,-1,VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot(freqHandle,PANEL_FREQ_WINDOW_2,-1,VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot(freqHandle,PANEL_FREQ_SPECTRU,-1,VAL_IMMEDIATE_DRAW);
				
				//
				filt = 0;
				
				//fac butoanele inactive
				SetCtrlAttribute(freqHandle,PANEL_FREQ_RAWDATA,ATTR_CTRL_VAL,0);
				SetCtrlAttribute(freqHandle,PANEL_FREQ_FiILTREDDATA,ATTR_CTRL_VAL,0);
			}else{
				SetCtrlVal(panelHandle,PANEL_SWITCH,0);
				DisplayPanel(panelHandle);
				HidePanel(freqHandle);
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnPanelFreq (int panel, int event, void *callbackData,int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			free(waveData);
			free(filtre);
			if(der!=0)
			{
				free(derivata);
			}
			if(anv!=0)
			{
				free(anvelopa);
			}
			if(filt!=0)
			{
				free(filtruF);
			}
			free(wincoef);
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnOptiuni (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//citesc optiunea filtrului
			int optiune;
			GetCtrlVal(freqHandle,PANEL_FREQ_OPTIUNI_FILTER,&optiune);
		
			switch(optiune)
			{
				//FIR Ksr_HPF cu fpass=1200 Hz si fstop=1450 Hz 	
				case 0:
					SetCtrlAttribute(freqHandle,PANEL_FREQ_CUTFREQ,ATTR_VISIBLE,1);
					SetCtrlAttribute(freqHandle,PANEL_FREQ_BETA,ATTR_VISIBLE,1);
					break;
					
				//Chebyshev I trece jos cu fpass=1200 Hz si fstop=1450 Hz
				case 1:
					SetCtrlAttribute(freqHandle,PANEL_FREQ_CUTFREQ,ATTR_VISIBLE,1);
					SetCtrlAttribute(freqHandle,PANEL_FREQ_BETA,ATTR_VISIBLE,0);
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnFilterAplica (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//citesc optiunea filtrului
			int optiune;
			GetCtrlVal(freqHandle,PANEL_FREQ_OPTIUNI_FILTER,&optiune);
		
			if(filt!=0)
			{
				free(filtruF);
				DeleteGraphPlot(freqHandle,PANEL_FREQ_DATAF,filt,VAL_IMMEDIATE_DRAW);
				filt=0;
			}
			
			//citire frecventa de cutoff 
			int cutofffreq; 
			GetCtrlVal(freqHandle,PANEL_FREQ_CUTFREQ,&cutofffreq); 
			
			switch(optiune)
			{
				//FIR Ksr_HPF cu fpass=1200 Hz si fstop=1450 Hz
				case 0:
					
					//citire beta
					double beta;
					GetCtrlVal(freqHandle,PANEL_FREQ_BETA,&beta);
					
					//alocare memorie pentru numarul de puncte
					filtruF = (double *) calloc(npoints+lungf-1, sizeof(double));
					
					//calculare coefiicienti si a filtrului
					Ksr_HPF(sampleRate,cutofffreq,lungf,ksr,beta);
					Convolve(waveData,npoints,ksr,lungf,filtruF);
					
					//afisare filtru
					filt = PlotY(freqHandle,PANEL_FREQ_DATAF,filtruF,npoints+lungf-1,VAL_DOUBLE,VAL_THIN_LINE,VAL_SOLID_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_DK_GRAY);
					break;
					
				//Chebyshev I trece jos cu fpass=1200 Hz si fstop=1450 Hz 
				case 1:
					SetCtrlAttribute(freqHandle,PANEL_FREQ_CUTFREQ,ATTR_VISIBLE,1);
					SetCtrlAttribute(freqHandle,PANEL_FREQ_BETA,ATTR_VISIBLE,0);
					
					//alocare memorie pentru numarul de puncte
					filtruF = (double *) calloc(npoints, sizeof(double));
					
					//calcularea filtrului
					Ch_LPF(waveData,npoints,sampleRate,cutofffreq,0.1,17,filtruF);
					
					//afisare filtru
					filt = PlotY(freqHandle,PANEL_FREQ_DATAF,filtruF,npoints,VAL_DOUBLE,VAL_THIN_LINE,VAL_SOLID_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS, VAL_DK_GRAY);
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnRawData (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int on;
			GetCtrlVal(freqHandle,PANEL_FREQ_RAWDATA,&on);
			switch(on)
			{
				case 0: 
					//stergere pe graf
					if(wd)
					{
						DeleteGraphPlot(freqHandle,PANEL_FREQ_DATAF,wd,VAL_IMMEDIATE_DRAW);
						wd=0;
					}
					break;
				case 1:
					//creez culori pentru filtru		
					int visiniu = MakeColor(153, 0, 76);
					
					//afisare pe grapf
					wd = PlotY(freqHandle, PANEL_FREQ_DATAF, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, visiniu);
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnFiltredData (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int on;
			GetCtrlVal(freqHandle,PANEL_FREQ_FiILTREDDATA,&on);
			switch(on)
			{
				case 1:
					SetCtrlAttribute(freqHandle,PANEL_FREQ_APLICAFILTER,ATTR_DIMMED,0);
					SetCtrlAttribute(freqHandle,PANEL_FREQ_OPTIUNI_FILTER,ATTR_DIMMED,0); 
					SetCtrlAttribute(freqHandle,PANEL_FREQ_TIP_DATA,ATTR_DIMMED,0);
					break;
				case 0:
					if(filt!=0)
					{
						free(filtruF);
						DeleteGraphPlot(freqHandle,PANEL_FREQ_DATAF,filt,VAL_IMMEDIATE_DRAW);
						filt=0;
					}
					SetCtrlAttribute(freqHandle,PANEL_FREQ_APLICAFILTER,ATTR_DIMMED,1);
					SetCtrlAttribute(freqHandle,PANEL_FREQ_OPTIUNI_FILTER,ATTR_DIMMED,1);
					SetCtrlAttribute(freqHandle,PANEL_FREQ_TIP_DATA,ATTR_DIMMED,1); 
					SetCtrlAttribute(freqHandle,PANEL_FREQ_CUTFREQ,ATTR_VISIBLE,0);
					SetCtrlAttribute(freqHandle,PANEL_FREQ_BETA,ATTR_VISIBLE,0);
					break;
			}
			 
			break;
	}
	return 0;
}


int CVICALLBACK OnTimerSwitch (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//citim valoarea switch timer
			int timer;   	
			GetCtrlVal(freqHandle, PANEL_FREQ_TIMERSWITCH,&timer);
	
			if(timer==1)
			{
				//facem vizibile grafurile si afisajele
				SetCtrlAttribute(freqHandle,PANEL_FREQ_POWPEAK,ATTR_DIMMED,0);
				SetCtrlAttribute(freqHandle,PANEL_FREQ_FREQPEAK,ATTR_DIMMED,0);
				SetCtrlAttribute(freqHandle,PANEL_FREQ_WINDOW,ATTR_DIMMED,0);
				SetCtrlAttribute(freqHandle,PANEL_FREQ_WINDOW_2,ATTR_DIMMED,0);
				SetCtrlAttribute(freqHandle,PANEL_FREQ_SPECTRU,ATTR_DIMMED,0);
		
				//citim valorile 
				GetCtrlVal(freqHandle,PANEL_FREQ_WINDOW_TIP,&win);
				GetCtrlVal(freqHandle,PANEL_FREQ_NUMBER,&n);
				GetCtrlVal(freqHandle,PANEL_FREQ_TIP_DATA,&tip);
		
				//calculam coeficientii ferestrei
				wincoef = WindowCoef(win, n);
				
				//afisam fereastra
				DeleteGraphPlot(freqHandle,PANEL_FREQ_WINDOW,-1,VAL_IMMEDIATE_DRAW);
				PlotY(freqHandle,PANEL_FREQ_WINDOW,wincoef,n,VAL_DOUBLE,VAL_CONNECTED_POINTS,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_DK_RED);
				
				//pornim timer
				SetCtrlAttribute(freqHandle,PANEL_FREQ_TIMER,ATTR_ENABLED,1);
			}
			else
			{
				SetCtrlVal(freqHandle,PANEL_FREQ_POWPEAK,0.0);
				SetCtrlVal(freqHandle,PANEL_FREQ_FREQPEAK,0.0);
				SetCtrlAttribute(freqHandle,PANEL_FREQ_POWPEAK,ATTR_DIMMED,1);
				SetCtrlAttribute(freqHandle,PANEL_FREQ_FREQPEAK,ATTR_DIMMED,1);
		
				//oprim timer
				SetCtrlAttribute(freqHandle,PANEL_FREQ_TIMER,ATTR_ENABLED,0);
				
				//resetam start
				start=0;
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnTimer (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{   
	//vector ce contine spectrul semnalului convertit la volti 
	double *convertedSpectrum;
	//frecventa estimata pentru spectrul de putere (maxim) din vectorul autoSpectrum
	double powerPeak = 0.0;
	//valoarea maxima din spectru de putere (din autoSpectrum)
	double freqPeak = 0.0;
	//variabila ce reprezinta pasul in domeniul frecventei
	double df = 0.0;
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			if(start + n <= npoints)
			{
				//crop semnal wave data N puncte
				SetAxisRange(freqHandle,PANEL_FREQ_DATAF,VAL_MANUAL,start,start + n,VAL_AUTOSCALE,0,0);
				
				//aloc memorie
				convertedSpectrum = (double*)calloc(n,sizeof(double));
				
				//calculeaza spectrul de putere 
				switch(tip)
				{
					//s-a ales sa se faca spectru pentru waveData
					case 0:
						ComputePowerSpectrum(waveData, start, n, sampleRate, convertedSpectrum, &df, &powerPeak, &freqPeak);
						break;
						
					//s-a ales sa se faca spectru pentru filtredData
					case 1:
						ComputePowerSpectrum(filtruF, start, n, sampleRate, convertedSpectrum, &df, &powerPeak, &freqPeak);
						break;
				}
				
				//afisez pe interfata valorile determinate
				SetCtrlVal( freqHandle, PANEL_FREQ_FREQPEAK, freqPeak);
				SetCtrlVal( freqHandle, PANEL_FREQ_POWPEAK, powerPeak);
				
				//sterg graph-ul unde urmeaza sa plotez spectrul semnalului
				DeleteGraphPlot (freqHandle, PANEL_FREQ_SPECTRU, -1, VAL_IMMEDIATE_DRAW);
				
				//plotez spectrul semnalului
			    PlotWaveform( freqHandle, PANEL_FREQ_SPECTRU, convertedSpectrum, n/2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df,
			               VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_DK_GREEN);
				
				//salvam graful cu datele filtrate
				sprintf(fileName,"Imagini\\graficFiltre_%.0f_%.0f.jpg",start,start+n);
				GetCtrlDisplayBitmap(freqHandle,PANEL_FREQ_DATAF,1,&bitmapID);
				SaveBitmapToJPEGFile(bitmapID,fileName,JPEG_PROGRESSIVE,100);
				DiscardBitmap(bitmapID);
				
				//salvam graful cu datele ferestruite
				sprintf(fileName,"Imagini\\graficFerestruire_%.0f_%.0f.jpg",start,start+n);
				GetCtrlDisplayBitmap(freqHandle,PANEL_FREQ_WINDOW_2,1,&bitmapID);
				SaveBitmapToJPEGFile(bitmapID,fileName,JPEG_PROGRESSIVE,100);
				DiscardBitmap(bitmapID);
				
				//salvam graful cu spectru
				sprintf(fileName,"Imagini\\graficSpectru_%.0f_%.0f.jpg",start,start+n);
				GetCtrlDisplayBitmap(freqHandle,PANEL_FREQ_SPECTRU,1,&bitmapID);
				SaveBitmapToJPEGFile(bitmapID,fileName,JPEG_PROGRESSIVE,100);
				DiscardBitmap(bitmapID);
				
				//actualizam valoarea de start
				start += n;
			}
			else
			{
				start = 0;
			}
				
			break;
	}
	
	return 0;
}

int ComputePowerSpectrum(double inputArray[],unsigned int startp, unsigned int numberOfElements, double samplingRate, double convertedSpectrum[], double *frequencyInterval, double *powerPeak, double *freqPeak)
{
	char unit[32]="V";
	double dt = 1.0/samplingRate;
	WindowConst winConst;
	
	double *windowSignal = 0;
	
	//afisam semnalul ferestruit
	windowSignal = (double*)calloc(numberOfElements,sizeof(double));
	for(int i = 0; i < numberOfElements; ++i)
	{
		windowSignal[i] = wincoef[i] * inputArray[i+startp];
	}
	DeleteGraphPlot(freqHandle,PANEL_FREQ_WINDOW_2,-1,VAL_IMMEDIATE_DRAW);
	PlotY(freqHandle,PANEL_FREQ_WINDOW_2,windowSignal,n,VAL_DOUBLE,VAL_CONNECTED_POINTS,VAL_SIMPLE_DOT,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_DK_RED);
	
	//calculam spectru
	ScaledWindowEx(inputArray,numberOfElements,win,0,&winConst);
	AutoPowerSpectrum(inputArray + startp, numberOfElements,dt,convertedSpectrum,frequencyInterval);
	PowerFrequencyEstimate(convertedSpectrum,numberOfElements/2,-1.0,winConst,*frequencyInterval,7,freqPeak,powerPeak);
	SpectrumUnitConversion(convertedSpectrum,numberOfElements/2,SPECTRUM_POWER,SCALING_MODE_LINEAR,DISPLAY_UNIT_VRMS,*frequencyInterval,winConst,convertedSpectrum,unit);
	
	free(windowSignal);
	windowSignal = 0;
	
	return 0;
}

double* WindowCoef(int win, int numElem)
{
	double *coef = 0;
	int i;
	
	coef = (double*)calloc(numElem,sizeof(double));
	
	//calculam coeficientii ferestrei Hamming
	if(win == HAMMING)
	{
		for(i=1; i <= numElem; ++i)
		{
			coef[i-1] = 0.54 - 0.46 * cos(2 * PI * i/(numElem-1));
		}
	}
	//calculam coeficientii ferestrei Blackman
	else if(win == BLKMAN)
	{
		for(i=1; i <= numElem; ++i)
		{
			coef[i-1] = 0.42 - 0.5 * cos(2 * PI * i/(numElem - 1)) + 0.08 * cos(4 * PI * i/(numElem - 1));
		}
	}
	
	return coef;
}
