#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//could do proper OOP but this class is relatively small
class CSVWrite{
  private:
    std::ofstream* CSVFile;

  public:
    CSVWrite(std::string filename){
      std::cout << "Initialising csv" << std::endl;
      CSVFile = new std::ofstream(filename, std::ios::out);
    }

    ~CSVWrite(){
      //CSVFile->close();
      std::cout << "CSV deconstructor accessed" << std::endl;
    }

    void CSVClose(){
      CSVFile->close();  
      std::cout << "Successfully wrote to CSV" << std::endl;
    }

    //Writes 2d vector data to CSV
    void WriteVector(std::vector<std::vector<double>>* dataset){
      std::cout << "Writing to csv" << std::endl;
      for(int i = 0; i < dataset->at(0).size(); ++i){
        for(int j = 0; j < dataset->size(); ++j){
          //vectors may be different lengths, assume first is largest
          //can fix later
          if(i < dataset->at(j).size()) {
            //Write data
            *CSVFile << dataset->at(j).at(i);
          }
          else *CSVFile << ","; //if no data after end of vector
          //EOL comma
          if(j != dataset->size() - 1) *CSVFile << ",";
        }
      *CSVFile << std::endl;
      }
    }
};

//fix includes by commenting this out
/*
void CSVTest(){
  std::cout << "Testing CSV write functionality" <<std::endl;

  //fill 3 x 3 testVector with 1-9 square
  std::vector<double> col1(10,1.0);
  std::vector<double> col2(10,2.0);
  std::vector<double> col3(10,3.0);
  std::vector<std::vector<double>> testVector = {col1, col2, col3};

  auto testCSV = new CSVWrite(std::string("test_csv.csv"));
  testCSV->WriteVector(&testVector);
  testCSV->CSVClose();
  
  std::cout << "Test completed successfully" <<std::endl;
}
*/
