#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class CSVWrite{
  private:
    std::ofstream* CSVFile;

  public:
    CSVWrite(std::string filename){
      //printf("\nWriting to CSV: ", filename.c_str());
      std::ofstream myFile(filename);
      this->CSVFile = &myFile;
    }

    ~CSVWrite(){
      this->CSVFile->close();
    }

    void CSVClose(){
      this->CSVFile->close();  
    }

    //Writes 2d vector data to CSV
    void WriteVector(std::vector<std::vector<double>>* dataset){
      for(int i = 0; i < dataset->size(); i++){
        for(int j = 0; j < dataset->at(i).size(); j++){
          //Write data
          *this->CSVFile << dataset->at(i)[j];
          if(j != dataset->size() - 1) *this->CSVFile << ","; // No comma at end of line
        }
      *this->CSVFile << "\n";
      }
    }
};

void CSVTest(){
  printf("\nTesting CSV write functionality\n");
  //fill 3 x 3 testVector with 1-9 square
  std::vector<std::vector<double>> testVector = {{1,2,3}, {4,5,6}, {7,8,9}};
  CSVWrite testCSV(std::string("test_csv.csv"));
  testCSV.WriteVector(&testVector);
  //testCSV.CSVClose();
}
