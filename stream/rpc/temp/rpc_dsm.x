struct para_1 {
  char buff[1024];
  int len;
  int node;
};
struct para_2 {
  int node;
};
struct para_3 {
  int node;
};
struct para_4 {
  char buff[2048];
  int node;
  int num;
  int length;
  int first;
  int second;
  int third;
};
program TESTPROG {
   version VERSION {
     int WRITE_DATA(para_1) = 1;
     int READ_DATA(para_2) = 2;
     int DELETE_DATA(para_3) = 3;
     int RECOVER_DATA(para_4) = 4;
   } = 1;
} = 87654321;

