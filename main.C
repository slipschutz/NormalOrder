
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

int nStates=8;
int nParticles=4;
int HFIterations=100;


class Operator {

public:
  Operator();
  //  virtual double Apply(vector<bool>& ){cout<<"Nope"<<endl;}
  virtual string GetSubScript(){return theSubScript;}
  virtual string GetType()=0;
  virtual void Print(){cout<<"nope"<<endl;}
  virtual void PrintLatex(){cout<<"nope"<<endl;}
  string theSubScript;
  int Phase;
private:
};

Operator::Operator() 
{Phase=1;}

class Raise : public Operator{
public:
  Raise();
  Raise(string s){theSubScript=s;}
  Raise(string s,int p){theSubScript=s;Phase=p;}
  void Print(){cout<<"R_"<<theSubScript<<" ";}
  void PrintLatex(){cout<<"\\RO{"<<theSubScript<<"} ";}

  string GetType(){return "raise";}
  
};

class Lower : public Operator{
public:
  Lower();
  Lower(string s){theSubScript=s;}
  Lower(string s,int p){theSubScript=s;Phase=p;}
  void Print(){cout<<"L_"<<theSubScript<<" ";}
  void PrintLatex(){cout<<"\\LO{"<<theSubScript<<"} ";}
  string GetType(){return "lower";}
};

Lower::Lower()
{}

Raise::Raise()
{}


void PrintVector(vector <Operator*> v){
  if (v.size()>0){
    cout<<v[0]->Phase;
    for (int i=0;i<v.size();i++){
      v[i]->Print();
    }
    cout<<endl;
  } else {
    cout<<"vector has no size"<<endl;
  }
}

void PrintVector(vector <vector<Operator*> > v){

  for (int i=0;i<v.size();i++){
    PrintVector(v[i]);
    cout<<"_________"<<endl;
  }
  cout<<"*******************"<<endl;

}
void PrintVectorLatex(vector <vector<Operator*> > v){
  cout<<"\\begin{equation}"<<endl;
  cout<<"\\begin{aligned}"<<endl;
  for (int i=0;i<v.size();i++){
    if (v[i][0]->Phase == 1){
      if (i!=0){
	cout<<"+";
      }
    }else {
      cout<<"-";
    }

    for (int j=0;j<v[i].size();j++){
      v[i][j]->PrintLatex();
    }
    cout<<"\\\\"<<endl;
  }
  cout<<"\\end{aligned}"<<endl;
  cout<<"\\end{equation}"<<endl;
  cout<<"\\\\\\\\"<<endl;

}


vector <Operator*> ApplyDelta( vector <Operator*> theDeltaTerm,string subScript1,string subScript2){
  //make subscript2 into subscript1
  //Need to make a new instance to ensure that things don't get changed
  vector <Operator*> ReturnDeltaTerm(theDeltaTerm.size());
  for (int i=0;i<theDeltaTerm.size();i++){
    if (theDeltaTerm[i]->GetType() == "raise"){
      ReturnDeltaTerm[i]=new Raise(theDeltaTerm[i]->GetSubScript());//make copy of term
    }
    if (theDeltaTerm[i]->GetType() == "lower"){
      ReturnDeltaTerm[i]=new Lower(theDeltaTerm[i]->GetSubScript());//make copy of term
    }
    ReturnDeltaTerm[i]->Phase = theDeltaTerm[i]->Phase;
  }
  int numThingsReplaced=0;
  for (int i=0;i<theDeltaTerm.size();i++){
    if (ReturnDeltaTerm[i]->GetSubScript() == subScript2){
      ReturnDeltaTerm[i]->theSubScript=subScript1;
      numThingsReplaced++;
    }
  }
  //  if (numThingsReplaced==0
  return ReturnDeltaTerm;
}

vector< vector<Operator*> > Permute(vector <Operator*> theList,int first,int second){
  
  vector< vector <Operator*> > theReturn(2);//Everytime you permute you get another term
  
  vector <Operator*> theLeftOfSpot(theList.begin(),theList.begin()+first);
  vector <Operator*> theRightOfSpot(theList.begin()+second+1,theList.end());
  
  
  vector <Operator*> theSwitchedTerm(theList.size());
  vector <Operator*> theDeltaTerm(theList.size()-2);//Delta term should have two less operators

  
  for (int i=0;i<first;i++){
    theSwitchedTerm[i]=theLeftOfSpot[i]; 
    theDeltaTerm[i]=theLeftOfSpot[i];
  }

  theSwitchedTerm[first]=theList[second];
  theSwitchedTerm[second]=theList[first];
  for (int i=second+1;i<theList.size();i++){
    theSwitchedTerm[i]=theList[i];
    theDeltaTerm[i-2]=theList[i];
  }
  vector <Operator*> retDelta=ApplyDelta(theDeltaTerm,theList[first]->GetSubScript(),theList[second]->GetSubScript());

  for (int i=0;i<theSwitchedTerm.size();i++){
    theSwitchedTerm[i]->Phase=theSwitchedTerm[i]->Phase*-1;//Store phase in first operator  
  }


  /*
  PrintVector(theSwitchedTerm);
  PrintVector(theDeltaTerm);
  PrintVector(retDelta);
  */
  
  
  theReturn[0]=theSwitchedTerm;
  theReturn[1]=retDelta;

  
  return theReturn;
}


void LoadOperators(string fileName,  vector < vector <Operator*> >& theList){

  ifstream inFile;
  inFile.open(fileName.c_str());
  if (!inFile.is_open()){
    cout<<"FILE NOT FOUND "<<endl;
    throw 1;
  }
  string var1,var2,var3;
  vector <Operator*> temp;
  while (true){
    inFile>>var1>>var2>>var3;
    if(inFile.eof()){break;}
    if (var2 == "R"){
      temp.push_back(new Raise(var3,atoi(var1.c_str())));
    }
    if (var2 == "L"){
      temp.push_back(new Lower(var3,atoi(var1.c_str())));
    }
    if (atoi(var1.c_str())==9 &&atoi(var2.c_str())==9){

      theList.push_back(temp);
      temp.clear();
    }

  }
  

}
int main(int argc, char**argv){

  if (argc !=2){
    cout<<"Give Input file"<<endl;
    return -1;
  }
  string fileName =string(argv[1]);
  vector < vector <Operator*> > theList;
  try {
    LoadOperators(fileName,theList);
  } catch (...){
    cout<<"Program exited..."<<endl;
    return -9;
  }

  bool KeepLooping=true;
  
  while (KeepLooping==true){
    PrintVectorLatex(theList);
    


    bool allGood=true;
    for (int j=0;j<theList.size();j++){
      for (int i=0;i<theList[j].size()-1;i++){
	if (theList[j][i]->GetType() =="lower" &&
	    theList[j][i+1]->GetType()=="raise"){
	  //There is an incorrect ordering 
	  allGood=false;
	  
	  vector < vector <Operator*> > temp=Permute(theList[j],i,i+1);
	  //first remove the unpermuted term
	  theList.erase(theList.begin()+j);
	  theList.push_back(temp[0]);
	  theList.push_back(temp[1]);
	  i=1000;
	}
      }
      if (allGood==false){
	j=1000;
      }
    }//end of for j

    if (allGood){

      KeepLooping=false;
    }

  }

  
  return 0;



}
