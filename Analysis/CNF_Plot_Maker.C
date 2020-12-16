#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TString.h"
#include "TAttFill.h"
#include "TStyle.h"
#include <cmath>
#include <TAttText.h>
#include <TAttLine.h>
#include <TAttMarker.h>
#include <TPad.h>
#include <TAxis.h>
#include <TGraph.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>


using namespace std;

void CNF_Plot_Maker(){//makes histogram of energy vs counts

  double height;//max range on y axis
  double rangemax;//max x range
  string Infi = "";
  //string Bgrnd = "";

  //get user files and plot ranges
  cout << "Sample Data File Name: ";
  cin >> Infi;
  
  //open input and output file
  ifstream in;
  in.open(Infi);
  if(in.fail()){cout << "File does not exist, please try again" << endl; exit(1);}

  /*
  //get user background file
  cout << "Background File Name: ";
  cin >> Bgrnd;
  
  //open input and output file
  ifstream bgd;
  bgd.open(Bgrnd);
  if(bgd.fail()){cout << "File does not exist, please try again" << endl; exit(1);}
  */
  
  int xdata = -1;
  int ydata = -1;
  int num_t = 0;
  int xvariable = -1;
  int yvariable = -1;
  bool possiblevar = false;
  
  while(possiblevar == false){
    cout << "X axis Data? (0=channel, 1=energy): ";
    cin >> xdata;
  
    if(xdata == 0){xvariable = 0; possiblevar = true;}
    else if(xdata == 1){xvariable = 1; possiblevar = true;}
    else{cout << "not a valid data to plot, please try again" << endl;}
  }
  
  possiblevar = false;
  
  while(possiblevar == false){
    cout << "Y axis Data? (0=counts, 1=rate): ";
    cin >> ydata;
  
    if(ydata == 0){yvariable = 2; possiblevar = true;}
    else if(ydata == 1){yvariable = 3; possiblevar = true;}
    else{cout << "not a valid data to plot, please try again" << endl;}
  }
  
  cout << "Max Bin Value (0 for full spectrum): " << endl;
  cin >> rangemax;

  cout << "Max height (0 for full height): " << endl;
  cin >> height;
  
  string line;
  char check;
  
  char bit_c;
  char bit_e;
  
  string count;
  double Act_Count;
  string energy;
  double Engy;
  
  int iter = 0;

  string title = "X Ray Spectrum of ";
  title += Infi;
  TGraph* plot = new TGraph();
  plot->SetTitle((title).c_str());
  if(ydata == 0){plot->GetYaxis()->SetTitle("Counts");}
  if(ydata == 1){plot->GetYaxis()->SetTitle("Rate [1/s]");}
  if(xdata == 0){plot->GetXaxis()->SetTitle("Channel");}
  if(xdata == 1){plot->GetXaxis()->SetTitle("Energy [keV]");}
  
  //loop to obtain x and y values
  while(getline(in, line, '\n')){//look line by line
    if(line[0] == '#'){continue;}//skip over the beginning # section
    cout << line << endl;
    
    for(uint i = 0; i < line.length(); i++){//look at each entry in the table in the line
      check = line[i];
      if(check == '\t'){num_t++;}//count number of tabs
      
      if(num_t == yvariable){//checks number after yvariable number of tabs 
	bit_c = line[i+1];//take the value after this tab
	if(bit_c == '\t'){continue;}
	count += bit_c;//write out the whole value before the next tab
      }
      
      if(num_t == xvariable){//same thing but for x data (determined in switch statement)
	if(xvariable == 0){bit_e = line[i];}
	else{bit_e = line[i+1];}
	if(bit_e == '\t'){continue;}
	energy += bit_e;
      }
    }
    //reset repeated variables used for each line
    num_t = 0;
    bit_c = 0;
    bit_e = 0;
    //cout << "B" << endl;
    double Act_Count = stod(count);
    //cout << "A" << endl;
    double Engy = stod(energy);//change string from file to double for graph
    
    plot->SetPoint(iter, Engy, Act_Count);//plot!
    iter++;
    
    count = "";//reset values for next line
    energy = "";

    if(rangemax != 0){//as long as the energy range is !=0, the while loop will stop reading after rangemax
      if(Engy >= rangemax){break;}
    }
  }

  /*
  //now for background file:
  string BGline;
  char BGcheck;
  
  string BGcount;
  double BGAct_Count;
  string BGenergy;
  double BGEngy;
  
  int BGiter = 0;

  TGraph* BGplot = new TGraph();
  
  //loop to obtain x and y values
  while(getline(bgd, BGline, '\n')){//look line by line
    if(BGline[0] == '#'){continue;}//skip over the beginning # section
    cout << BGline << endl;
    
    for(uint i = 0; i < BGline.length(); i++){//look at each entry in the table in the line
      BGcheck = BGline[i];
      if(BGcheck == '\t'){num_t++;}//count number of tabs
      
      if(num_t == yvariable){//checks number after yvariable number of tabs 
	bit_c = BGline[i+1];//take the value after this tab
	if(bit_c == '\t'){continue;}
	BGcount += bit_c;//write out the whole value before the next tab
      }
      
      if(num_t == xvariable){//same thing but for x data (determined in switch statement)
	if(xvariable == 0){bit_e = BGline[i];}
	else{bit_e = BGline[i+1];}
	if(bit_e == '\t'){continue;}
	BGenergy += bit_e;
      }
    }
    //reset repeated variables used for each line
    num_t = 0;
    bit_c = 0;
    bit_e = 0;
    //    cout << "B" << endl;
    double BGAct_Count = stod(BGcount);
    //cout << "A" << endl;
    double BGEngy = stod(BGenergy);//change string from file to double for graph
    
    BGplot->SetPoint(BGiter, BGEngy, BGAct_Count);//plot!
    BGiter++;
    
    BGcount = "";//reset values for next line
    BGenergy = "";

    if(rangemax != 0){//as long as the energy range is !=0, the while loop will stop reading after rangemax
      if(BGEngy >= rangemax){break;}
    }
  }

  TGraph* finalplot = (TGraph*)plot->Clone("FP");
  finalplot->Add(BGplot, -1);
  
  if(height != 0){finalplot->GetYaxis()->SetRangeUser(0,height);}//change height of y axis if height != 0
  finalplot->Draw();//draw!
  */

   if(height != 0){plot->GetYaxis()->SetRangeUser(0,height);}//change height of y axis if height != 0
   //if(rangemax != 0){plot->GetXaxis()->SetRangeUser(rangemin,rangemax);}//change height of y axis if height != 0
  
  plot->Draw();//draw!
  in.close();//close input file
}
    
