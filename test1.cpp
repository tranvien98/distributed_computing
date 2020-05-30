#include <iostream>
#include <bits/stdc++.h>

using namespace std;
int main()
{

  int n_col = 6;
  int myArray[n_col] = {};
  double a = 1.0;
  int loss = 0;
  #pragma omp parallel for private(a) shared(loss) reduction(+:myArray[:n_col])
  for (int i=0; i<50; ++i)
  {
    // Or something non-trivial justifying the parallelism...
    a = 2.0;
    loss += 1;
    for (int n = 0; n<6; ++n)
    {
      myArray[n] += a;
    }
  }
  // Print the array elements to see them summed   
  for (int n = 0; n<6; ++n)
  {
    std::cout << myArray[n] << " " << std::endl;
  } 
  std::cout << "loss: " << loss << " " << std::endl;
}