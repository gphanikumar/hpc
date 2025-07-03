#include <bits/stdc++.h>

class WRITER
{
private:
  int data_offset ;
  int data_y_range ;
  int data_x_range ;

public:

  WRITER(int offset, int y_range, int x_range){
    data_offset = offset;
    data_y_range = y_range;
    data_x_range = x_range;
  };

  template<typename T>
  void WRITE(T ARRAY, std::ofstream &file) {
    file.seekp(data_offset) ;
    for (int j = 1 ; j < data_y_range; ++j)
      for (int i = 1; i < data_x_range; ++i){
	file << std::scientific << (float) ARRAY[j][i];
	if (i == data_x_range - 1) file << "\n";
	else file << ",";
      }
  }

};
