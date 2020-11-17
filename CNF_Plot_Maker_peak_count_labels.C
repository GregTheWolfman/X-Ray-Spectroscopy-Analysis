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
#include <TText.h>


using namespace std;

void CNF_Plot_Maker_peak_count_labels(){//makes histogram of energy vs counts

  double height = 200;//max range on y axis
  double rangemax = 1000;//max x range
  string Infi = "";

  //get user files and plot ranges
  cout << "Data File Name: ";
  cin >> Infi;
  
  //open input and output file
  ifstream in;
  in.open(Infi);
  if(in.fail()){cout << "File does not exist, please try again" << endl; exit(1);}
  
  int xdata = 1;
  int ydata = 0;
  int num_t = 0;
  int xvariable = 1;
  int yvariable = 2;
  int num_ydata = 0;
  bool possiblevar = false;

  /*
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
  */
  
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

  float xrange[16384];//number of total x datum
  float yrange[16384];//number of total x datum
  int uiter = 0;
  
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

    if(Act_Count != 0){num_ydata++;}//count number of channels without 0 counts/rate
    
    xrange[uiter] = Engy;//collect all of the x datum;
    yrange[uiter] = Act_Count;//collect all of the y datum
    uiter++;
    
    count = "";//reset values for next line
    energy = "";

    if(rangemax != 0){//as long as the energy range is !=0, the while loop will stop reading after rangemax
      if(Engy >= rangemax){break;}
    }
  }
  in.clear();
  in.seekg(0);
  
  if(height != 0){plot->GetYaxis()->SetRangeUser(0,height);}//change height of y axis if height != 0
  plot->Draw();//draw!

  //time to add labels to the graphs
  int inner_bounds = 1;//bounds for a better peak approximation at low energies
  int inner_buffer = 8;
  int bounds = 2;
  int buffer = 6;
  float low_cut_off = 200;//low energy cut off for analysis
  
  for(uint k = 0; k < num_ydata; k++){ //cycle through the y values
    float sum = 0;
    float average = 0;
    if(xrange[k] >= 20){
      if(xrange[k] < low_cut_off){ //for lower x values, the staistics are changed bc the peaks are different
	for(uint q = k-inner_bounds; q < k+inner_bounds; q++){//moving average around the current bin
	  sum+=yrange[q];
	}
	average = sum/(2*inner_bounds);
	if(yrange[k] >= average+inner_buffer){//if the y value is over the threshold
	cout << "Peak at: " << xrange[k] << endl;

	stringstream inner_label;

	double error = trunc(sqrt(yrange[k])*100)/100;
	inner_label << "(" << xrange[k] << "): " << yrange[k] << " +/- " << error;
	TLatex Tl_iner;//make the label = to the y value
	
	Tl_iner.SetTextSize(0.03);
	Tl_iner.SetTextAngle(45);
	
	Tl_iner.DrawLatex(xrange[k], yrange[k]+0.05*height, inner_label.str().c_str()); //draw it
      }
	
      }
      if(xrange[k] >= low_cut_off){//same thing but for high energy region
	for(uint y = k-bounds; y < k+bounds; y++){
	  sum+=yrange[y];
	}
	average = sum/(2*bounds);
	if(yrange[k] >= average+buffer){
	  cout << "Peak at: " << xrange[k] << endl;

	  stringstream label;
	  double error_out = trunc(sqrt(yrange[k])*100)/100;
	  label << "(" << xrange[k] << "): " << yrange[k] << " +/- " << error_out;
	  TLatex Tl;
	  
	  Tl.SetTextSize(0.03);
	  Tl.SetTextAngle(45);
	  
	  Tl.DrawLatex(xrange[k], yrange[k]+0.05*height, label.str().c_str());
	}
      }
    }
  }
  /*
  stringstream outplot;
  outplot << Infi;
  outplot << "_Plot.pdf";
  //outplot.str().c_str()
  plot->SaveAs("Plots.pdf");	       
  */
  in.close();//close input file
}
    
